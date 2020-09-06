/* daqdrv

   daq card driver

   Copyright 2006, Mitchell C. Nelson, Ph.D.

   No representation is provided of suitability for any purpose whatsoever.

*/

#include <linux/version.h>

#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19))
#include <linux/config.h>
#endif

// debug for this file only
/*
#ifndef DAQDRV_DEBUG
#define DAQDRV_DEBUG
#endif
*/

#include "daqdrvmain.h"

#include "daqdrv.h"

#include "gsc16aiss8ao4.h"
#include "gsc16aiss16ao2.h"

static DAQDRV_Dev devlist[MAXDAQDEV+1] = { { 0 } };

static int daqdrv_major = 0;
static int daqdrv_minor = 0;

static unsigned long cpucyclesperjiffie = 1;
static unsigned long cpucyclespersecond = 1;

#define PCI_DEVICE_ID_PLX_9056 0x9056

extern int daqInterruptHandler( void *iobase, void *p );

// 16AISS8AO4
extern void daqReadCtls( void *iobase, DAQDRV_Ctl *b );
extern void daqWriteCtls( void *iobase, DAQDRV_Ctl *b );
extern void daqInputTrigger( void *iobase );
extern void daqOutputTrigger( void *iobase );
extern void daqInputOutputTrigger( void *iobase );

// 16AISS16AO4
extern void daqReadCtls_16aiss16ao2( void *iobase, DAQDRV_Ctl *b );
extern void daqWriteCtls_16aiss16ao2( void *iobase, DAQDRV_Ctl *b );
extern void daqInputTrigger_16aiss16ao2( void *iobase );
extern void daqOutputTrigger_16aiss16ao2( void *iobase );
extern void daqInputOutputTrigger_16aiss16ao2( void *iobase );

/* =====================================================================

   dma user space functions functions

   don't forget to use cpu_to_le32( paddr) to pass the physical
   address to the dma device

interesting user calls:

   #include <unistd.h>
   getpagesize()

   #define _XOPEN_SOURCE 600
   #include <stdlib.h>
   posix_memalign( void **memptr, size_t alignment, size_t size )

          memory is allocated aligned, can be freed with free()
          returns 0 on success

 these are not needed because the driver does the locking

   #include <sys/mman.h>
   mlock( void *addr, size_t size(bytes) );
   munlock( void *addr, size_t size );

Kernel references

 <asm/page.h>
   PAGE_SIZE     page size in bytes (usually 4096)

   PAGE_SHIFT    number of bits to shift an address to get the page frame number (PFN)

*/


// channel 0 reads from the device, channel 1 writes to the device

void daqdrv_testsglist( struct scatterlist *sg, int nsg )
{
  int n = 0;
  int m = 0;

  void *p;
  unsigned int *u;

  DEBUGMSG( "testsglist, sg=0x%lx nsg=%d\n", (unsigned long) sg, nsg );

  for( n = 0; n < nsg; n++ )
    {
      DEBUGMSG( "writing sg[%d].page offset=%d length=%d\n",
	      n, sg[n].offset, sg[n].length );

      // map the page to a kernel virtual address
      p = kmap( sg[n].page );

      if ( p )
	{
	  // fill the page with a test pattern

	  p += sg[n].offset;
	  u = p;

	  for ( m = 0; m < sg[n].length/4; m++ )
	    {
	      u[m] = ((n & 0xFFFF) << 16) | (m & 0xFFFF) | 0x8000;
	    }
	}

      // unmap it.
      kunmap( sg[n].page );
    }
}

// ============================================================================

void daqdrv_dma_release_userpages_( DAQDRV_Dev *daqdev, DAQDRV_DmaUser *dmauser, int channel )
{
  int n = 0;

  DEBUGMSG( "**dma_release_userpages_ dmauser 0x%lx channel %d useraddress=0x%lx, bytes=%d\n",
	    (unsigned long) dmauser, channel, dmauser->uaddr, dmauser->usize );

  if ( dmauser->seg )
    {
      DEBUGMSG( "  dma_release_userpages_ kfree seg 0x%lx\n", (unsigned long) dmauser->seg );

      kfree( dmauser->seg );
      dmauser->seg  = NULL;
    }
  dmauser->nseg = 0;

  if ( dmauser->sg )
    {
      DEBUGMSG( "  dma_release_userpages_ unmap and kfree sg 0x%lx\n", (unsigned long) dmauser->sg );

      dma_unmap_sg( &daqdev->pcidev->dev, dmauser->sg, dmauser->nsg,
		    ( (channel == 0) ? DMA_FROM_DEVICE : DMA_TO_DEVICE) );

      kfree( dmauser->sg );
      dmauser->sg  = NULL;
    }
  dmauser->nsg = 0;

  if ( dmauser->pages )
    {
      DEBUGMSG( "  dma_release_userpages_ dirty and release %d pages 0x%lx\n",
		dmauser->npages, (unsigned long) dmauser->pages );

      // if they were gotten, unget then.
      for( n = 0; n < dmauser->npages; n++ )
	{
	  if ( !PageReserved(dmauser->pages[n]) ) SetPageDirty(dmauser->pages[n]);
	  page_cache_release(dmauser->pages[n]);
	}

      kfree( dmauser->pages );
      dmauser->pages = NULL;
    }
  dmauser->npages = 0;

  dmauser->uaddr = 0;
  dmauser->usize = 0;

  DEBUGMSG( "==dma_release_userpages_ channel %d done\n", channel );
}

void daqdrv_dma_release_( DAQDRV_Dev *daqdev, int channel )
{
  DAQDRV_Dma *dma;

  DAQDRV_DmaUser *dmauser;

  DEBUGMSG( "**dma_release_ channel %d\n", channel );

  dma = (channel == 0) ? &(daqdev->dma0) : &(daqdev->dma1);

  dma->seghead = NULL;
  dma->segtail = NULL;
  dma->seg     = NULL;
  dma->enabled = 0;

  dma->user    = NULL;
  dma->uaddr   = 0;
  dma->usize   = 0;

  // make sure the list is not circular
  if ( dma->usertail ) dma->usertail->next = NULL;

  // setp thru the list
  while( dma->userhead )
    {
      // remove one from the head of the list
      dmauser       = dma->userhead;
      dma->userhead = dmauser->next;

      // release its pages
      DEBUGMSG( "  dma_release_ channel %d dmauser 0x%lx\n", channel, (unsigned long) dmauser );

      daqdrv_dma_release_userpages_( daqdev, dmauser, channel );

      kfree( dmauser );
    }
  dma->usertail = NULL;


  DEBUGMSG( "==dma_release_ channel %d done\n", channel );
}

// ============================================================================

int daqdrv_dma_add_( DAQDRV_Dev *daqdev, unsigned long useraddress, int lenbytes, int channel )
{
  int n = 0;
  int n0,nl;
  int m;

  int lenpages;

  DAQDRV_Dma *dma;
  DAQDRV_DmaUser *dmauser;

  dma = (channel == 0) ? &(daqdev->dma0) : &(daqdev->dma1);

  DEBUGMSG( "**dma_add channel %d useraddress=0x%lx, bytes=%d\n",
	    channel, useraddress, lenbytes );

  // -----------------------------------------------------------------------

  /* Allocate the user page management structure
   */
  dmauser = kmalloc( sizeof( DAQDRV_DmaUser ), GFP_KERNEL );
  if ( !dmauser ) {
    DEBUGMSG( "dma_add kmalloc dmauser failed\n" );
    return( 0 );
  }
  memset( dmauser, 0, sizeof( DAQDRV_DmaUser ) );


  // attach it to the device dma engine
  if ( !dma->usertail )
    {
      dma->userhead = dmauser;
      dma->usertail = dmauser;
    }
  else
    {
      // point to the beggining of the list, circular list
      dmauser->next = dma->userhead;

      // add it to the end of the list
      dma->usertail->next = dmauser;
      dma->usertail = dmauser;
    }

  // for diagnostics
  dmauser->uaddr = useraddress;
  dmauser->usize = lenbytes;

  // -----------------------------------------------------------------------

  // calculate maximum number of pages anticipated, save offset in first page
  n0 = (useraddress & (PAGE_SIZE-1));
  nl = lenbytes - (PAGE_SIZE-n0);
  if ( nl < 0 ) {
    lenpages = 1;
  }
  else {
    lenpages = 1 + nl/PAGE_SIZE + (nl&(PAGE_SIZE-1)?1:0);
  }

  // malloc everything up front
  dmauser->pages = kmalloc( lenpages * sizeof(struct page *), GFP_KERNEL );
  if ( !dmauser->pages ) {
    DEBUGMSG( "dma_add kmalloc %d page pointers failed\n", lenpages );
    daqdrv_dma_release_userpages_( daqdev, dmauser, channel );
    return( 0 );
  }

  dmauser->sg = kmalloc( lenpages * sizeof(struct scatterlist), GFP_KERNEL );
  if( !dmauser->sg ) {
    DEBUGMSG( "dma_add kmalloc %d scatterlist failed\n", lenpages );
    daqdrv_dma_release_userpages_( daqdev, dmauser, channel );
    return( 0 );
  }

  // this is the segment list that will be added to the dma engine
  dmauser->seg = kmalloc( lenpages * sizeof( DAQDRV_DmaSeg ), GFP_KERNEL );
  if( !dmauser->seg ) {
    DEBUGMSG( "dma_add kmalloc %d segment list failed\n", lenpages );
    daqdrv_dma_release_userpages_( daqdev, dmauser, channel );
    return( 0 );
  }

  // -----------------------------------------------------------------------

  // get the user pages
  down_read( &current->mm->mmap_sem );

  dmauser->npages =
    get_user_pages( current, current->mm, useraddress, lenpages, 1, 0, dmauser->pages, NULL );

  up_read( &current->mm->mmap_sem );

  if ( !dmauser->npages )
    {
      DEBUGMSG( "dma_add get user pages 0x%lx failed\n", (unsigned long) useraddress );
      daqdrv_dma_release_userpages_( daqdev, dmauser, channel );
      return( -1 );
    }

  // setup the scatter gather list
  n0 = (useraddress & (PAGE_SIZE-1));
  nl = lenbytes;
  n = 0;

  dmauser->sg[0].page = dmauser->pages[0];
  dmauser->sg[0].offset = n0;
  dmauser->sg[0].length = n0+nl > PAGE_SIZE ? (PAGE_SIZE-n0) : nl;
  nl -= dmauser->sg[0].length;
  n++;

  while ( nl )
    {
      dmauser->sg[n].page = dmauser->pages[n];
      dmauser->sg[n].offset = 0;
      dmauser->sg[n].length = (nl > PAGE_SIZE)? PAGE_SIZE : nl;
      nl -= dmauser->sg[n].length;
      n++;
    }
  dmauser->nsg = n;

  // remap the scatter gather list
  dmauser->nseg = dma_map_sg( &daqdev->pcidev->dev, dmauser->sg, dmauser->nsg,
			      (channel==0 ? DMA_TO_DEVICE : DMA_FROM_DEVICE) );

  // -----------------------------------------------------------------------

  /* copy the scatter list to the local segment list
   */
  for( n = 0, m = 0; n < dmauser->nseg; n++, m++ )
    {
      dmauser->seg[m].addr = sg_dma_address( &dmauser->sg[n] );
      dmauser->seg[m].len  = sg_dma_len( &dmauser->sg[n] );

      /* make sure all the pieces are within half a fifo (to avoid timeouts/underflows)
       */
      while( dmauser->seg[m].len >= FIFOSIZE/2 )
	{
	  dmauser->seg[m+1].addr = dmauser->seg[m].addr + FIFOSIZE/2;
	  dmauser->seg[m+1].len  = dmauser->seg[m].len - FIFOSIZE/2;
	  dmauser->seg[m].len    = FIFOSIZE/2;
	  m++;
	}
    }
  dmauser->nseg = m;

  /* link the list segment list
   */
  if ( dmauser->nseg )
    {
      // mark the list
      for( m = 0; m < dmauser->nseg; m++ )
	{
	  dmauser->seg[m].controls = SEGID;      // stamp it
	}
      dmauser->seg[m-1].controls |= SEGINTBIT;   // interrupt

      // link the list
      for( m = 1; m < dmauser->nseg; m++ )
	{
	  dmauser->seg[m-1].next = &dmauser->seg[m];
	}
      dmauser->seg[m-1].next = NULL;

      // add the list to the dma engine
      m = dmauser->nseg;
      if( !dma->seghead )
	{
	  dma->seghead = dmauser->seg;
	  dma->segtail = &dmauser->seg[m-1];
	}
      else
	{
	  dma->segtail->next = dmauser->seg;
	  dma->segtail = &dmauser->seg[m-1];

	  // if this is an extra buffer, its a ring
	  dma->segtail->next = dma->seghead;
	}
    }

#ifdef DAQDRV_DEBUG
  for( m = 0; m < dmauser->nseg; m++ )
    {
      DEBUGMSG( "  dma_add seg 0x%lx  addr 0x%lx len %d next 0x%lx\n",
		(unsigned long) &dmauser->seg[m],
		(unsigned long) dmauser->seg[m].addr,
		dmauser->seg[m].len,
		(unsigned long) dmauser->seg[m].next );
    }
  DEBUGMSG( "  dma_add seghead 0x%lx segtail 0x%lx\n",
	    (unsigned long) dma->seghead, (unsigned long) dma->segtail );
#endif

  DEBUGMSG( "==dma_add channel %d  useraddress=0x%lx, bytes=%d nseg=%d done\n",
	    channel, dmauser->uaddr, dmauser->usize, dmauser->nseg );

  return( dmauser->nseg );
}

// ============================================================================

void daqdrv_dma_reset( DAQDRV_Dev *daqdev, int channel )
{
  DAQDRV_Dma *dma;

  DEBUGMSG( "**dma_reset_ channel %d\n", channel );

  dma = (channel == 0) ? &(daqdev->dma0) : &(daqdev->dma1);

  dma->user     = NULL;
  dma->seg      = NULL;

  dma->counter  = 0;
  dma->overflow = 0;

  dma->uaddr    = 0;
  dma->usize    = 0;

}

/* =====================================================================

   dma kernel space allocation functions

   don't forget to use cpu_to_le32( paddr) to pass the physical
   address to the dma device

*/

static
void daqdrv_freedma( DAQDRV_Dev *daqdev )
{
  DAQDRV_DmaMem *dma = &daqdev->dmamem;

  if ( dma->vaddr ) dma_free_coherent( &daqdev->pcidev->dev, dma->size, dma->vaddr, dma->paddr );

  dma->vaddr  = NULL;
  dma->size   = 0;
  dma->paddr  = 0;
  dma->count  = 0;
}

static
void *daqdrv_getdma( DAQDRV_Dev *daqdev, int size )
{
  DAQDRV_DmaMem *dma = &daqdev->dmamem;

  daqdrv_freedma( daqdev );

  dma->vaddr = dma_alloc_coherent( &daqdev->pcidev->dev, size, &dma->paddr, GFP_KERNEL );

  if ( dma->vaddr )
    {
      dma->size = size;
    }

  return( dma->vaddr );
}

void daqDmaTest( DAQDRV_Dev *dev )
{
  DAQDRV_DmaMem *dma = &dev->dmamem;

  plxDma1Setup( dev->plxbase );

  plxDma1Load( dev->plxbase, dma->paddr, dma->size );

  plxDma1Start( dev->plxbase );
}

/* =====================================================================

   irq functions

*/

//irqreturn_t daqdrv_irqhandler( int irq, void *p, struct pt_regs *regs)
//irqreturn_t daqdrv_irqhandler( int irq, void *p )

static
#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19))
irqreturn_t daqdrv_irqhandler( int irq, void *p, struct pt_regs *regs)
#else
irqreturn_t daqdrv_irqhandler(int irq, void *p)
#endif
{
  DAQDRV_Dev *dev = p;

  int intstatus ;

  /*
#ifdef DAQDRV_DEBUG
  unsigned long      njiff;
  njiff = jiffies;
  DEBUGMSG( ">>dma irq handler jiffie %ld\n", njiff );
#endif
  */

  intstatus = (*dev->irqhandler)( dev->iobase, dev->irqarg );

  return( (intstatus ? IRQ_HANDLED : 0) );
}


static
void daqdrv_freeirq( DAQDRV_Dev *dev )
{
  if ( (dev->status & IRQCNCT) )
    {
      free_irq( dev->irq, dev );

      dev->irqarg     = NULL;
      dev->irqhandler = NULL;
      dev->status    &= ~IRQCNCT;
    }
}

static
int daqdrv_connectirq( DAQDRV_Dev *dev, int (*handler)( void *, void * ), void *arg )
{
  int retv;

  daqdrv_freeirq( dev );

  dev->irqarg     = arg;
  dev->irqhandler = handler;

  retv = request_irq( dev->irq, daqdrv_irqhandler, SA_INTERRUPT|SA_SHIRQ, "daqdrv", dev );

  if ( !retv )
    {
      dev->status |= IRQCNCT;
    }
  else
    {
      dev->irqarg     = NULL;
      dev->irqhandler = NULL;
    }

  return( retv );
}

/* =====================================================================
   timed delay
*/

inline
void daqdrv_uwait( int usecs )
{
  unsigned long ltemp;

  ltemp = (usecs > 0) ? 1000000/usecs : 1000000;
  ltemp = HZ/ltemp;
  if ( ltemp < 1 ) ltemp = 1;

  ltemp += jiffies;
  while( ltemp > jiffies ) schedule( );
}

// =====================================================================
// status bit-mask functions

static int testandsetstatus( DAQDRV_Dev *dev, int bitmask )
{
  int status;

  // mutex lock
  down( &dev->sem );

  status = dev->status & bitmask;

  dev->status |= bitmask;

  // mutex unlock
  up( &dev->sem );

  return( status );
}

static void clearstatus(  DAQDRV_Dev *dev, int bitmask )
{
  // mutex lock
  down( &dev->sem );

  dev->status &= ~bitmask;

  // mutex unlock
  up( &dev->sem );
}

// ============================================================
// driver file operations

int daqdrv_open( struct inode *inode, struct file *filp )
{
  unsigned int ndev = 0;

  int ntemp;

  ndev = iminor( inode );

  if ( ndev >= MAXDAQDEV )
    {
      DEBUGMSG( "daqdrv: open error, dev %d >= MAXDAQDEV\n", ndev );
      return -ENXIO;
    }

  if ( !(devlist[ndev].pcidev) )
    {
      DEBUGMSG( "daqdrv: open error, no hardware found for dev %d\n", ndev );
      return -ENXIO;
    }

  if ( testandsetstatus( &devlist[ndev], DEVOPEN ) )
    {
      DEBUGMSG( "open error, already open, dev %d\n", ndev );
      return -EBUSY;
    }

  printk( KERN_NOTICE "daqdrv: open dev %d\n", ndev );

  filp->private_data = &devlist[ndev];

  /* lets make sure interrupts are disabled before we
     start fiddlin around.
  */
  plxInterruptDisable( devlist[ndev].plxbase );

  daqReset( devlist[ndev].iobase );
  daqdrv_uwait( 3000 );

  ntemp = daqdrv_connectirq( &devlist[ndev], daqInterruptHandler, &devlist[ndev] );
  if ( ntemp )
    {
       DEBUGMSG( "open error, request_irq returned %d\n", ntemp );
       return -EBUSY;     
    }

  /* Now we should be connected, we can re-enable interrutps.  At this point,
     pci interrupts are enabled from the card and the plx chip, but no specific
     conditions are enabled
  */
  //  plxLocalInterruptEnable( devlist[ndev].plxbase );

  return( 0 );
}

ssize_t daqdrv_read( struct file *filp, char __user *buf, size_t count, loff_t *fpos )
{

  return( 0 );
}

int daqdrv_ioctl( struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg )
{
  int ndev;

  DAQDRV_RegReq regreq;
  DAQDRV_Fifo   fiforeq;
  DAQDRV_Status statusreq;
  DAQDRV_Ctl    ctlreq;

  unsigned int utemp;

  int nsize;

  int n;

  if ( _IOC_TYPE( cmd ) != DAQDRV_IOC_NUMBER ) return -EINVAL;

  ndev = iminor( inode );

  if ( (ndev < MAXDAQDEV) && devlist[ndev].pcidev )
    {

      switch( cmd )
	{
	case DAQDRV_IOC_SUBSYSTEMID:

	  if ( copy_from_user( &regreq, (void *) arg, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, subsystemid copy_from_user error\n" );
	      return -EFAULT;
	    }

	  regreq.value = (devlist[ndev].subvendor << 16) | devlist[ndev].subid;

	  if ( __copy_to_user( (void *) arg, &regreq, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, subsystemid __copy_to_user error\n" );
	      return -EFAULT;
	    }

	  break;

	case DAQDRV_IOC_CONTROLS_READ:

	  DEBUGMSG( "ioctl controls read\n" );

	  if ( copy_from_user( &ctlreq, (void *) arg, sizeof( ctlreq ) ) )
	    {
	      DEBUGMSG( "ioctl, control_read copy_from_user error\n" );
	      return -EFAULT;
	    }

	  //daqReadCtls( devlist[ndev].iobase, &ctlreq );
	  (*devlist[ndev].readcontrols)( devlist[ndev].iobase, &ctlreq );

	  if ( __copy_to_user( (void *) arg, &ctlreq, sizeof( ctlreq ) ) )
	    {
	      DEBUGMSG( "ioctl, control_read __copy_to_user error\n" );
	      return -EFAULT;
	    }

	  break;

	case DAQDRV_IOC_CONTROLS_WRITE:

	  DEBUGMSG( "ioctl controls write\n" );

	  if ( copy_from_user( &ctlreq, (void *) arg, sizeof( ctlreq ) ) )
	    {
	      DEBUGMSG( "ioctl, control_write copy_from_user error\n" );
	      return -EFAULT;
	    }

	  //daqWriteCtls( devlist[ndev].iobase, &ctlreq );
	  (*devlist[ndev].writecontrols)( devlist[ndev].iobase, &ctlreq );

	  if ( __copy_to_user( (void *) arg, &ctlreq, sizeof( ctlreq ) ) )
	    {
	      DEBUGMSG( "ioctl, control_write __copy_to_user error\n" );
	      return -EFAULT;
	    }

	  break;

	case DAQDRV_IOC_TRIGGER_INPUT:

	  (*devlist[ndev].triggerinput)( devlist[ndev].iobase );

	  break;

	case DAQDRV_IOC_TRIGGER_OUTPUT:

	  (*devlist[ndev].triggeroutput)( devlist[ndev].iobase );

	  break;

	case DAQDRV_IOC_TRIGGER_INPUTOUTPUT:

	  (*devlist[ndev].triggerinputoutput)( devlist[ndev].iobase );

	  break;

	case DAQDRV_IOC_SIGINTEST:

	  /*
	  if ( devlist[ndev].async_queue )
	    kill_fasync( &devlist[ndev].async_queue, SIGIO, POLL_IN );
	  */

	  if ( devlist[ndev].async_queue )
	    kill_fasync( &devlist[ndev].async_queue, SIGRTMIN+DAQRTSIG, POLL_IN );

	  break;

	case DAQDRV_IOC_SIGOUTTEST:

	  if ( devlist[ndev].async_queue )
	    kill_fasync( &devlist[ndev].async_queue, SIGRTMIN+DAQRTSIG, POLL_OUT );

	  break;

	case DAQDRV_IOC_SIGMSGTEST:

	  if ( devlist[ndev].async_queue )
	    kill_fasync( &devlist[ndev].async_queue, SIGRTMIN+DAQRTSIG, POLL_MSG );

	  break;

	case DAQDRV_IOC_SIGERRTEST:

	  if ( devlist[ndev].async_queue )
	    kill_fasync( &devlist[ndev].async_queue, SIGRTMIN+DAQRTSIG, POLL_ERR );

	  break;

	case DAQDRV_IOC_SIGPRITEST:

	  if ( devlist[ndev].async_queue )
	    kill_fasync( &devlist[ndev].async_queue, SIGRTMIN+DAQRTSIG, POLL_PRI );

	  break;

	case DAQDRV_IOC_TEST_DMA:

	  daqdrv_getdma( &devlist[ndev], 4096 );

	  daqDmaTest( &devlist[ndev] );

	  break;

	case DAQDRV_IOC_TEST_INPUT_BUFFER:

	  if( devlist[ndev].dma0.userhead && devlist[ndev].dma0.userhead->sg )
	    {
	      daqdrv_testsglist( devlist[ndev].dma0.userhead->sg,
				 devlist[ndev].dma0.userhead->nsg );
	    }

	  break;

	case DAQDRV_IOC_TEST_OUTPUT_BUFFER:

	  if( devlist[ndev].dma1.userhead && devlist[ndev].dma1.userhead->sg )
	    {
	      daqdrv_testsglist( devlist[ndev].dma1.userhead->sg,
				 devlist[ndev].dma1.userhead->nsg );
	    }

	  break;

	case DAQDRV_IOC_STATUS_READ:

	  DEBUGMSG( "ioctl status read\n" );

	  if ( copy_from_user( &statusreq, (void *) arg, sizeof( statusreq ) ) )
	    {
	      DEBUGMSG( "ioctl, statusread copy_from_user error\n" );
	      return -EFAULT;
	    }

	  statusreq.status       = devlist[ndev].status;
	  statusreq.intstatus    = devlist[ndev].intstatus;
	  statusreq.dma0counter  = devlist[ndev].dma0.counter;
	  statusreq.dma1counter  = devlist[ndev].dma1.counter;
	  statusreq.dma0overflow = devlist[ndev].dma0.overflow;
	  statusreq.dma1overflow = devlist[ndev].dma1.overflow;
	  statusreq.userdata     = devlist[ndev].userdata;
	  statusreq.useraddr0    = devlist[ndev].dma0.uaddr;
	  statusreq.useraddr1    = devlist[ndev].dma1.uaddr;
	  statusreq.usersize0    = devlist[ndev].dma0.usize;
	  statusreq.usersize1    = devlist[ndev].dma1.usize;

	  if ( __copy_to_user( (void *) arg, &statusreq, sizeof( statusreq ) ) )
	    {
	      DEBUGMSG( "ioctl, statusread __copy_to_user error\n" );
	      return -EFAULT;
	    }

	  break;

	case DAQDRV_IOC_SET_USERDATA:

	  DEBUGMSG( "ioctl set user data\n" );

	  if ( copy_from_user( &statusreq, (void *) arg, sizeof( statusreq ) ) )
	    {
	      DEBUGMSG( "ioctl, setuserdata copy_from_user error\n" );
	      return -EFAULT;
	    }

	  devlist[ndev].userdata = statusreq.userdata;

	  statusreq.status       = devlist[ndev].status;
	  statusreq.intstatus    = devlist[ndev].intstatus;
	  statusreq.dma0counter  = devlist[ndev].dma0.counter;
	  statusreq.dma1counter  = devlist[ndev].dma1.counter;
	  statusreq.dma0overflow = devlist[ndev].dma0.overflow;
	  statusreq.dma1overflow = devlist[ndev].dma1.overflow;
	  statusreq.useraddr0    = devlist[ndev].dma0.uaddr;
	  statusreq.useraddr1    = devlist[ndev].dma1.uaddr;
	  statusreq.usersize0    = devlist[ndev].dma0.usize;
	  statusreq.usersize1    = devlist[ndev].dma1.usize;

	  if ( __copy_to_user( (void *) arg, &statusreq, sizeof( statusreq ) ) )
	    {
	      DEBUGMSG( "ioctl, setuserdata __copy_to_user error\n" );
	      return -EFAULT;
	    }

	  break;

        case DAQDRV_IOC_ADD_INPUT_BUFFER:

	  DEBUGMSG( "ioctl add input buffer\n" );

	  if ( devlist[ndev].dma0.enabled )
	    return -EBUSY;

	  if ( copy_from_user( &fiforeq, (void *) arg, sizeof( fiforeq ) ) )
	    {
	      DEBUGMSG( "ioctl, add_input_buffer copy_from_user error\n" );
	      return -EFAULT;
	    }  

	  nsize = fiforeq.ndata * DATAWORDSIZE;

	  if ( (fiforeq.ndata <= 0) || !access_ok(VERIFY_WRITE, fiforeq.data, nsize ) )
	    {
	      DEBUGMSG( "ioctl, add_input_buffer invalid buffer 0x%lx %d words\n",
		      (unsigned long) fiforeq.data, fiforeq.ndata );
	      return -EFAULT;
	    }

	  DEBUGMSG( "ioctl, add_int_buffer dma0: 0x%lx %d words\n",
		    (unsigned long) fiforeq.data, fiforeq.ndata );

	  n = daqdrv_dma_add_( &devlist[ndev], (unsigned long) fiforeq.data, nsize, 0 );

	  if ( n <= 0 ) return( -EFAULT );

	  break;

        case DAQDRV_IOC_START_INPUT_DMA:

	  DEBUGMSG( "ioctl enable input dma\n" );

	  if ( devlist[ndev].dma0.enabled )
	    return -EBUSY;
	    
	  if ( !daqDma0Start( &devlist[ndev] ) )
	    return -EFAULT;

	  break;

        case DAQDRV_IOC_CANCEL_INPUT_DMA:

	  DEBUGMSG( "ioctl cancel input dma\n" );

	  daqDma0Cancel(  &devlist[ndev] );

	  break;

        case DAQDRV_IOC_RELEASE_INPUT_BUFFERS:

	  DEBUGMSG( "ioctl release input buffers\n" );

	  if ( devlist[ndev].dma0.enabled )
	    return -EBUSY;

	  daqdrv_dma_release_( &devlist[ndev], 0 );

	  break;

        case DAQDRV_IOC_ADD_OUTPUT_BUFFER:

	  DEBUGMSG( "ioctl add output buffer\n" );

	  if ( devlist[ndev].dma1.enabled )
	    return -EBUSY;

	  if ( copy_from_user( &fiforeq, (void *) arg, sizeof( fiforeq ) ) )
	    {
	      DEBUGMSG( "ioctl, add_output_buffer copy_from_user error\n" );
	      return -EFAULT;
	    }  

	  nsize = fiforeq.ndata * DATAWORDSIZE;

	  if ( (fiforeq.ndata <= 0) || !access_ok(VERIFY_READ, fiforeq.data, nsize ) )
	    {
	      DEBUGMSG( "ioctl, add_output_buffer invalid buffer 0x%lx %d words\n",
		      (unsigned long) fiforeq.data, fiforeq.ndata );
	      return -EFAULT;
	    }

	  DEBUGMSG( "ioctl, add_output_buffer dma1: 0x%lx %d words\n",
		    (unsigned long) fiforeq.data, fiforeq.ndata );

	  n = daqdrv_dma_add_( &devlist[ndev], (unsigned long) fiforeq.data, nsize, 1 );

	  if ( n <= 0 ) return( -EFAULT );

	  break;

        case DAQDRV_IOC_RELEASE_OUTPUT_BUFFERS:

	  DEBUGMSG( "ioctl release output buffers\n" );

	  if ( devlist[ndev].dma1.enabled )
	    return -EBUSY;

	  daqdrv_dma_release_( &devlist[ndev], 1 );

	  break;

        case DAQDRV_IOC_START_OUTPUT_DMA:

	  DEBUGMSG( "ioctl enable output buffer\n" );

	  if ( !daqDma1Start( &devlist[ndev] ) )
	    return-EFAULT;

	  break;

        case DAQDRV_IOC_CANCEL_OUTPUT_DMA:

	  DEBUGMSG( "ioctl cancel output buffer\n" );

	  daqDma1Cancel(  &devlist[ndev] );

	  break;

        case DAQDRV_IOC_FIFO_READPIO:

	  if ( copy_from_user( &fiforeq, (void *) arg, sizeof( fiforeq ) ) )
	    {
	      DEBUGMSG( "ioctl, fiforead copy_from_user error\n" );
	      return -EFAULT;
	    }  

	  nsize = fiforeq.ndata * DATAWORDSIZE;

	  if (!access_ok(VERIFY_WRITE, fiforeq.data, nsize ) )
	    {
	      DEBUGMSG( "ioctl, fiforead VERIFY WRITE failed 0x%lx %d\n",
		      (unsigned long) fiforeq.data, fiforeq.ndata );
	      return -EFAULT;
	    }

	  fiforeq.ndata = daqReadFifo_Pio( devlist[ndev].iobase, fiforeq.data, fiforeq.ndata );

	  if ( __copy_to_user( (void *) arg, &fiforeq, sizeof( fiforeq ) ) )
	    {
	      DEBUGMSG( "ioctl, fiforead __copy_to_user error\n" );
	      return -EFAULT;
	    }

	  break;

        case DAQDRV_IOC_FIFO_WRITEPIO:

	  if ( copy_from_user( &fiforeq, (void *) arg, sizeof( fiforeq ) ) )
	    {
	      DEBUGMSG( "ioctl, fifowrite copy_from_user error\n" );
	      return -EFAULT;
	    }  

	  nsize = fiforeq.ndata * DATAWORDSIZE;

	  if (!access_ok(VERIFY_READ, fiforeq.data, nsize ) )
	    {
	      DEBUGMSG( "ioctl, fifowrite VERIFY WRITE failed 0x%lx %d\n",
		      (unsigned long) fiforeq.data, fiforeq.ndata );
	      return -EFAULT;
	    }

	  fiforeq.ndata = daqWriteFifo_Pio( devlist[ndev].iobase, fiforeq.data, fiforeq.ndata );

	  if ( __copy_to_user( (void *) arg, &fiforeq, sizeof( fiforeq ) ) )
	    {
	      DEBUGMSG( "ioctl, fifowrite __copy_to_user error\n" );
	      return -EFAULT;
	    }

	  break;

	case DAQDRV_IOC_REGISTER_READ:

	  if ( copy_from_user( &regreq, (void *) arg, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, regread copy_from_user error\n" );
	      return -EFAULT;
	    }

	  regreq.value = ioread32( (devlist[ndev].iobase+regreq.offset) );

	  if ( __copy_to_user( (void *) arg, &regreq, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, regread __copy_to_user error\n" );
	      return -EFAULT;
	    }

	  break;

	case DAQDRV_IOC_REGISTER_WRITE:

	  if ( copy_from_user( &regreq, (void *) arg, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, regread invalid user address\n" );
	      return -EFAULT;
	    }

	  iowrite32( regreq.value, (devlist[ndev].iobase+regreq.offset) );

	  break;

	case DAQDRV_IOC_REGISTER_OR:

	  if ( copy_from_user( &regreq, (void *) arg, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, regread invalid user address\n" );
	      return -EFAULT;
	    }

	  regreq.value |= ioread32(  (devlist[ndev].iobase+regreq.offset) );
	  iowrite32( regreq.value, (devlist[ndev].iobase+regreq.offset) );

	  if ( __copy_to_user( (void *) arg, &regreq, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, regread __copy_to_user error\n" );
	      return -EFAULT;
	    }

	  break;

	case DAQDRV_IOC_REGISTER_AND:

	  if ( copy_from_user( &regreq, (void *) arg, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, regread invalid user address\n" );
	      return -EFAULT;
	    }

	  regreq.value &= ioread32(  (devlist[ndev].iobase+regreq.offset) );
	  iowrite32( regreq.value, (devlist[ndev].iobase+regreq.offset) );

	  if ( __copy_to_user( (void *) arg, &regreq, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, regread __copy_to_user error\n" );
	      return -EFAULT;
	    }

	  break;

	case DAQDRV_IOC_REGISTER_TOGGLE:

	  if ( copy_from_user( &regreq, (void *) arg, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, regread invalid user address\n" );
	      return -EFAULT;
	    }

	  utemp = ioread32(  (devlist[ndev].iobase+regreq.offset) );

	  if ( utemp & regreq.value )
	    {
	      // set low
	      utemp &= ~regreq.value;
	      iowrite32( utemp, (devlist[ndev].iobase+regreq.offset) );

	      // wait 10 microseconds
	      daqdrv_uwait( 10 );

	      // set high
	      utemp |= regreq.value;
	      iowrite32( utemp, (devlist[ndev].iobase+regreq.offset) );
	    }
	  else
	    {
	      // set high
	      utemp |= regreq.value;
	      iowrite32( utemp, (devlist[ndev].iobase+regreq.offset) );

	      // wait 10 microseconds
	      daqdrv_uwait( 10 );

	      // set low
	      utemp &= ~regreq.value;
	      iowrite32( utemp, (devlist[ndev].iobase+regreq.offset) );
	    }

	  if ( __copy_to_user( (void *) arg, &regreq, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, regread __copy_to_user error\n" );
	      return -EFAULT;
	    }

	  break;

	case DAQDRV_IOC_PLX_READ:

	  if ( copy_from_user( &regreq, (void *) arg, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, regread copy_from_user error\n" );
	      return -EFAULT;
	    }

	  regreq.value = ioread32( (devlist[ndev].plxbase+regreq.offset) );

	  if ( __copy_to_user( (void *) arg, &regreq, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, regread __copy_to_user error\n" );
	      return -EFAULT;
	    }

	  break;

	case DAQDRV_IOC_PLX_WRITE:

	  if ( copy_from_user( &regreq, (void *) arg, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, regread invalid user address\n" );
	      return -EFAULT;
	    }

	  iowrite32( regreq.value, (devlist[ndev].plxbase+regreq.offset) );

	  break;

	case DAQDRV_IOC_PLX_OR:

	  if ( copy_from_user( &regreq, (void *) arg, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, regread invalid user address\n" );
	      return -EFAULT;
	    }

	  regreq.value |= ioread32(  (devlist[ndev].plxbase+regreq.offset) );
	  iowrite32( regreq.value, (devlist[ndev].plxbase+regreq.offset) );

	  if ( __copy_to_user( (void *) arg, &regreq, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, regread __copy_to_user error\n" );
	      return -EFAULT;
	    }

	  break;

	case DAQDRV_IOC_PLX_AND:

	  if ( copy_from_user( &regreq, (void *) arg, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, regread invalid user address\n" );
	      return -EFAULT;
	    }

	  regreq.value &= ioread32(  (devlist[ndev].plxbase+regreq.offset) );
	  iowrite32( regreq.value, (devlist[ndev].plxbase+regreq.offset) );

	  if ( __copy_to_user( (void *) arg, &regreq, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, regread __copy_to_user error\n" );
	      return -EFAULT;
	    }

	  break;

	case DAQDRV_IOC_PLX_TOGGLE:

	  if ( copy_from_user( &regreq, (void *) arg, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, regread invalid user address\n" );
	      return -EFAULT;
	    }

	  utemp = ioread32(  (devlist[ndev].plxbase+regreq.offset) );

	  if ( utemp & regreq.value )
	    {
	      // set low
	      utemp &= ~regreq.value;
	      iowrite32( utemp, (devlist[ndev].plxbase+regreq.offset) );

	      // wait 10 microseconds
	      daqdrv_uwait( 10 );

	      // set high
	      utemp |= regreq.value;
	      iowrite32( utemp, (devlist[ndev].plxbase+regreq.offset) );
	    }
	  else
	    {
	      // set high
	      utemp |= regreq.value;
	      iowrite32( utemp, (devlist[ndev].plxbase+regreq.offset) );

	      // wait 10 microseconds
	      daqdrv_uwait( 10 );

	      // set low
	      utemp &= ~regreq.value;
	      iowrite32( utemp, (devlist[ndev].plxbase+regreq.offset) );
	    }

	  if ( __copy_to_user( (void *) arg, &regreq, sizeof( regreq ) ) )
	    {
	      DEBUGMSG( "ioctl, regread __copy_to_user error\n" );
	      return -EFAULT;
	    }

	  break;

	default:

	  return -EINVAL;
	}

    }

  else
    {
      DEBUGMSG( "ioctl, no hardware, dev %d\n", ndev );

      return -ENXIO;
    }

  return( 0 );
}

int daqdrv_fasync( int fd, struct file *filep, int mode )
{
  DAQDRV_Dev *dev = filep->private_data;

  return  fasync_helper( fd, filep, mode, &dev->async_queue );
}

int daqdrv_release( struct inode *inode, struct file *filep )
{
  int ndev;

  ndev = iminor( inode );

  // disable PCI interrupts
  plxInterruptDisable( devlist[ndev].plxbase );

#ifdef DAQDRV_DEBUG
  daqdebuginfo( &devlist[ndev] );
#endif

  // reset the DAQ function
  daqReset( devlist[ndev].iobase );
  daqdrv_uwait( 3000 );

  // stop the DMA engines and mark not enabled
  daqDma0Cancel(  &devlist[ndev] );
  daqDma1Cancel(  &devlist[ndev] );

  // free any user pages
  daqdrv_dma_release_( &devlist[ndev], 0 );
  daqdrv_dma_release_( &devlist[ndev], 1 );

  // free any kernel space buffers
  daqdrv_freedma( &devlist[ndev] );

  // release all interrupts
  daqdrv_freeirq( &devlist[ndev] );

  // remove from async notification list
  daqdrv_fasync( -1, filep, 0 );

  // mark device not open
  clearstatus( &devlist[ndev], DEVOPEN );

  printk( KERN_NOTICE "daqdrv: close dev %d, done.\n", ndev );

  return( 0 );
}

struct file_operations daqdrv_fops = {
  .owner   = THIS_MODULE,
  .read    = daqdrv_read,
  .ioctl   = daqdrv_ioctl,
  .fasync  = daqdrv_fasync,
  .open    = daqdrv_open,
  .release = daqdrv_release,
};

// ============================================================
// driver initialization and remove

// id table for supported hardware

static struct pci_device_id ids[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_PLX, PCI_DEVICE_ID_PLX_9056), },
	{ 0, }
};
MODULE_DEVICE_TABLE(pci, ids);

static DAQDRV_Dev *find_daqdrv_pci_dev( struct pci_dev *dev )
{
  DAQDRV_Dev *pdev = NULL;

  int n = 0;

  while( (n < MAXDAQDEV) && (devlist[n].pcidev != dev) ) n++;

  if( n < MAXDAQDEV )
    {
      pdev = &devlist[n];
    }

  return( pdev );
}

static int probe( struct pci_dev *dev, const struct pci_device_id *id)
{
  DAQDRV_Dev *pdev = NULL;

  u16 subvendor;
  u16 subid;

  int nsize;

  printk( KERN_ALERT "daqdrv: probe top, dev 0x%lx  id 0x%lx\n",
	  (unsigned long) dev, (unsigned long) id );

  if ( pci_enable_device(dev) ) {
    printk( KERN_ALERT "daqdrv: pci_enable_device error\n" );
    return -ENODEV;
  }

  pci_read_config_word( dev, PCI_SUBSYSTEM_VENDOR_ID, &subvendor );
  pci_read_config_word( dev, PCI_SUBSYSTEM_ID, &subid );

  printk( KERN_NOTICE "daqdrv: subsystem vendor 0x%x (%d) id 0x%x (%d)\n",
	subvendor, subvendor, subid, subid );

  if ( subvendor != DAQDRV_SUBSYSTEM_VENDOR_GSC )
    {
      printk( KERN_ALERT "daqdrv: not a general standards board.\n" );
      return -ENODEV;
    }

  switch( subid )
    {
    case DAQDRV_SUBSYSTEM_ID_16AISS8AO4:
      printk( KERN_ALERT "daqdrv: general standards corporation 16AISS8AO4.\n" );
      break;
    case DAQDRV_SUBSYSTEM_ID_16AISS16AO2:
      printk( KERN_ALERT "daqdrv: general standards corporation 16AISS16AO2.\n" );
      break;
    default:
      printk( KERN_ALERT "daqdrv: general standards corporation card, model not recognized.\n" );
      return -ENODEV;
    }
  
  /*
  pci_read_config_byte( dev, PCI_REVISION_ID, &u8temp );

  if ( u8temp != 0x14) 
    {
      printk( KERN_ALERT "daqdrv driver, wrong hw revision = 0x%x\n", u8temp );
      return -ENODEV;
    }
  */

  // look for an empty device entry
  pdev = find_daqdrv_pci_dev( NULL );

  // is there room in the table for another device?
  if ( !pdev ) {
    printk( KERN_ALERT "daqdrv: no room in table for device\n" );
    return -ENODEV;
  }

  pdev->pcidev  = dev;

  // board  control registers
  nsize = 64;
  while( nsize < CARDSIZE ) nsize *= 2;

  pdev->phybase = pci_resource_start( dev, 2 );
  pdev->iobase  = ioremap( pdev->phybase, nsize );

  // plx  control registers
  nsize = 64;
  while( nsize < PLXSIZE ) nsize *= 2;

  pdev->plxphy = pci_resource_start( dev, 0 );
  pdev->plxbase  = ioremap( pdev->plxphy, nsize );

  /* obsoleted
     pci_read_config_byte( dev, PCI_INTERRUPT_LINE, &u8temp );
     printk( KERN_NOTICE "daqdrv: config space irq reg = 0x%x (%d)\n", u8temp, u8temp );
  */

  /* this is the new way to do it.
   */
  pdev->irq = dev->irq;
  printk( KERN_NOTICE "daqdrv: assigned irq = 0x%x (%d)\n", pdev->irq, pdev->irq );

  // board type
  pdev->subvendor = subvendor;
  pdev->subid     = subid;

  // boardspecific functions
  if ( pdev->subid == DAQDRV_SUBSYSTEM_ID_16AISS8AO4 )
    {
      pdev->readcontrols         = (void (*)(void*,void*))daqReadCtls;
      pdev->writecontrols        = (void (*)(void*,void*))daqWriteCtls;
      pdev->triggerinput         = daqInputTrigger;
      pdev->triggeroutput        = daqOutputTrigger;
      pdev->triggerinputoutput   = daqInputOutputTrigger;
    }

  else if ( pdev->subid == DAQDRV_SUBSYSTEM_ID_16AISS16AO2 )
    {
      pdev->readcontrols         = (void (*)(void*,void*))daqReadCtls_16aiss16ao2;
      pdev->writecontrols        = (void (*)(void*,void*))daqWriteCtls_16aiss16ao2;
      pdev->triggerinput         = daqInputTrigger_16aiss16ao2;
      pdev->triggeroutput        = daqOutputTrigger_16aiss16ao2;
      pdev->triggerinputoutput   = daqInputOutputTrigger_16aiss16ao2;
    }

  // init the semaphore if this is the first use of this entry
  if ( !(pdev->status & SEMINIT) )
    {
      pdev->status |= SEMINIT;
      init_MUTEX( &pdev->sem );
    }

  // prevent any spurious interrupts
  plxInterruptDisable( pdev->plxbase );

  printk( KERN_NOTICE "daqdrv: added device at slot 0x%x\n", PCI_SLOT( dev->devfn ) );

  return 0;
}

static void remove(struct pci_dev *dev)
{
  DAQDRV_Dev *pdev = NULL;

  pdev = find_daqdrv_pci_dev( dev );

  if ( pdev )
    {
      printk( KERN_NOTICE "daqdrv: removing device at slot 0x%x\n", PCI_SLOT( dev->devfn ) );

      plxInterruptDisable( pdev->plxbase );

      iounmap( pdev->iobase );
      iounmap( pdev->plxbase );

      pdev->pcidev  = NULL;
      pdev->phybase = 0;

      pdev->iobase  = NULL;
      pdev->plxbase  = NULL;
    }
}

static struct pci_driver pci_driver = {
	.name = "daqdrv",
	.id_table = ids,
	.probe = probe,
	.remove = remove,
};

// ============================================================

static unsigned long measurecpuclock( void )
{
  unsigned long ltemp;
  unsigned long long lltemp;

  unsigned long lrate = 0;

  /* first, wait for a fresh jiffie
   */
  ltemp = jiffies;
  while( ltemp == jiffies ) schedule( );

  /* here it is
   */
  ltemp = jiffies;
  lltemp = get_cycles();

  /* next, wait for another fresh jiffie
   */
  while( ltemp == jiffies ) schedule( );

  /* and here's the difference
   */
  ltemp  = jiffies - ltemp;
  lltemp = get_cycles() - lltemp;

  /* return as long (32 bit)
   */
  lrate = (unsigned long) (lltemp&0xffffffff);
  if ( ltemp > 1 ) lrate /= ltemp;

  return( lrate );
}


static int daqdrv_setup_cdev( DAQDRV_Dev *dev, int n )
{
  int retv;
  int devno = MKDEV(daqdrv_major, daqdrv_minor + n);

  cdev_init( &dev->cdev, &daqdrv_fops);

  dev->cdev.owner = THIS_MODULE;
  dev->cdev.ops   = &daqdrv_fops;

  retv = cdev_add( &dev->cdev, devno, 1);

  if( retv ) printk( KERN_NOTICE "daqdrv: cdev_add error 0x%x device %d\n", retv, n );

  return( retv );
}

static int __init daqdrv_init(void)
{
  int result;
  dev_t devn;

  int n;

  printk( KERN_NOTICE "daqdrv (data acquisition card driver)"
	  " Copyright 2006, Mitchell C. Nelson, Ph.D., mcnelson@mindspring.com\n" );


#ifdef CONFIG_PREEMPT
  printk( KERN_NOTICE "daqdrv (data acquisition card driver) - kernel is configured preempt" )
#endif

  // register the pci driver

  result = pci_register_driver(&pci_driver);

  if ( result )
    {
      printk( KERN_ALERT "daqdrv: pci_register_driver returned %d\n", result );
      return( result );
    }

  // allocate node numbers

  result = alloc_chrdev_region( &devn, daqdrv_minor, MAXDAQDEV, "daqdrv");
  daqdrv_major = MAJOR(devn);

  if (result < 0) {
    printk(KERN_WARNING "daqdrv: can't get major, result = %d\n", result);
    return result;
  }

  printk(KERN_NOTICE "daqdrv: major number = %d\n", daqdrv_major );


  // register char devices

  for( n = 0; n < MAXDAQDEV; n++ )
    {
      DEBUGMSG( "setup_cdev %d\n", n );

      daqdrv_setup_cdev( &devlist[n], n );
    }


  // init the spinlocks
  for( n = 0; n < MAXDAQDEV; n++ )
    {
      spin_lock_init( &devlist[n].lock );
    }


  // check jiffies granularity
  cpucyclesperjiffie = measurecpuclock( );
  cpucyclespersecond = cpucyclesperjiffie * HZ;

  printk(KERN_NOTICE "daqdrv: %lu cycles/jiffie @ %u jiffies/second = %lu MHz\n",
	 cpucyclesperjiffie, HZ, (cpucyclespersecond/1000000) );
 
  return 0;
}

static void __exit daqdrv_exit(void)
{
  int n;
  dev_t first;

  // unregister the pci driver

  pci_unregister_driver(&pci_driver);

  // release the node numbers

  first = MKDEV( daqdrv_major, 0 );
  unregister_chrdev_region( first, MAXDAQDEV );

  // remove the char devices

  for( n = 0; n < MAXDAQDEV; n++ )
    {
      DEBUGMSG( "cdev_del %d\n", n );

      cdev_del( &devlist[n].cdev );
    }

  printk( KERN_ALERT "daqdrv exit\n" );
}

MODULE_LICENSE("GPL");


module_init( daqdrv_init );
module_exit( daqdrv_exit );

