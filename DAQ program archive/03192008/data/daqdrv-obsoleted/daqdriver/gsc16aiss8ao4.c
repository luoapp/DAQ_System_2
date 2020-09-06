/* daqdrv

   daqdrv component for gsc daq card

   Dr. Mitchell C. Nelson

*/

#include <asm/uaccess.h>
#include <linux/sched.h>

#include "plx_regs.h"

#include "daqdrvmain.h"
#include "daqdrv.h"

#include "gsc16aiss8ao4regs.h"
#include "gsc16aiss8ao4.h"

#define LIMIT( a, max, min ) ( a > max ? max : ( a < min ? min : a) )

// --------------------------------------------

void plxInterruptEnable( void *plxbase )
{
  unsigned int uval;

  uval = ioread32( (plxbase+INT_CTRL_STATUS) );

  uval |= (IRQ_PCI_ENABLE | IRQ_DMA_0_ENABLE | IRQ_DMA_1_ENABLE | IRQ_LOCAL_PCI_ENABLE);

  iowrite32( uval, (plxbase+INT_CTRL_STATUS) ); 
}

void plxInterruptDisable( void *plxbase )
{
  unsigned int uval;

  uval = ioread32( (plxbase+INT_CTRL_STATUS) );

  uval &= ~(IRQ_PCI_ENABLE | IRQ_DMA_0_ENABLE | IRQ_DMA_1_ENABLE | IRQ_LOCAL_PCI_ENABLE);

  iowrite32( uval, (plxbase+INT_CTRL_STATUS) ); 
}

// --------------------------------------------

void plxLocalInterruptEnable( void *plxbase )
{
  unsigned int uval;

  uval = ioread32( (plxbase+INT_CTRL_STATUS) );

  uval |= (IRQ_PCI_ENABLE | IRQ_LOCAL_PCI_ENABLE);

  iowrite32( uval, (plxbase+INT_CTRL_STATUS) ); 
}

void plxLocalInterruptDisable( void *plxbase )
{
  unsigned int uval;

  uval = ioread32( (plxbase+INT_CTRL_STATUS) );

  uval &= ~(IRQ_LOCAL_PCI_ENABLE);

  iowrite32( uval, (plxbase+INT_CTRL_STATUS) ); 
}

// --------------------------------------------

void plxDmaInterruptEnable( void *plxbase )
{
  unsigned int uval;

  uval = ioread32( (plxbase+INT_CTRL_STATUS) );

  uval |= (IRQ_PCI_ENABLE | IRQ_DMA_0_ENABLE | IRQ_DMA_1_ENABLE);

  iowrite32( uval, (plxbase+INT_CTRL_STATUS) ); 
}

void plxDmaInterruptDisable( void *plxbase )
{
  unsigned int uval;

  uval = ioread32( (plxbase+INT_CTRL_STATUS) );

  uval &= ~(IRQ_DMA_0_ENABLE|IRQ_DMA_1_ENABLE);

  iowrite32( uval, (plxbase+INT_CTRL_STATUS) ); 
}

// --------------------------------------------

void plxDma0InterruptEnable( void *plxbase )
{
  unsigned int uval;

  uval = ioread32( (plxbase+INT_CTRL_STATUS) );

  uval |= (IRQ_PCI_ENABLE | IRQ_DMA_0_ENABLE);

  iowrite32( uval, (plxbase+INT_CTRL_STATUS) ); 
}

void plxDma0InterruptDisable( void *plxbase )
{
  unsigned int uval;

  uval = ioread32( (plxbase+INT_CTRL_STATUS) );

  uval &= ~(IRQ_DMA_0_ENABLE);

  iowrite32( uval, (plxbase+INT_CTRL_STATUS) ); 
}

// --------------------------------------------

void plxDma1InterruptEnable( void *plxbase )
{
  unsigned int uval;

  uval = ioread32( (plxbase+INT_CTRL_STATUS) );

  uval |= (IRQ_PCI_ENABLE | IRQ_DMA_1_ENABLE);

  iowrite32( uval, (plxbase+INT_CTRL_STATUS) ); 
}

void plxDma1InterruptDisable( void *plxbase )
{
  unsigned int uval;

  uval = ioread32( (plxbase+INT_CTRL_STATUS) );

  uval &= ~(IRQ_DMA_1_ENABLE);

  iowrite32( uval, (plxbase+INT_CTRL_STATUS) ); 
}

// --------------------------------------------

void plxDma0Load( void *plxbase, dma_addr_t addr, unsigned int len )
{
  DEBUGMSG( ">>plxDma0Load\n" );

  iowrite32( addr, (plxbase+DMA_CH_0_PCI_ADDR) );
  iowrite32( len, (plxbase+DMA_CH_0_TRANS_BYTE_CNT) );
}

void plxDma0Setup( void *plxbase )
{
  unsigned mode = 0x00020D43; // 32 bit, TA/READY input, Done-Int, Constant Address, PCI-Int.

  DEBUGMSG( ">>plxDma0Setup\n" );

  iowrite32( mode, (plxbase+DMA_CH_0_MODE) );
  iowrite32( AIN_DATA, (plxbase+DMA_CH_0_LOCAL_ADDR) );
  iowrite32( 0x8, (plxbase+DMA_CH_0_DESC_PTR) );
}

void plxDma0Start( void *plxbase )
{
  unsigned int uval;

  DEBUGMSG( ">>plxDma0Start\n" );

  uval = ioread32( (plxbase+DMA_CMD_STATUS) );
  uval = 0x03 | (uval & 0x0101);                 // enable and start
  iowrite32( uval, (plxbase+DMA_CMD_STATUS) );
}

void plxDma0Stop( void *plxbase )
{
  unsigned int uval;
  
  DEBUGMSG( ">>plxDma0Stop\n" );

  // abort
  uval = ioread32( (plxbase+DMA_CMD_STATUS) );
  uval = 0x04 | (uval & 0x0100);
  iowrite32( uval, (plxbase+DMA_CMD_STATUS) );

  // wait for done
  uval = ioread32( (plxbase+DMA_CMD_STATUS) );
  while( !(uval & 0x10) )
    {
      schedule();
      uval = ioread32( (plxbase+DMA_CMD_STATUS) );
    }

  // clear the interrupt
  uval = 0x08 | (uval & 0x0100);
  iowrite32( uval, (plxbase+DMA_CMD_STATUS) );
}

// --------------------------------------------

void plxDma1Load( void *plxbase, dma_addr_t addr, unsigned int len )
{
  DEBUGMSG( ">>plxDma1Load\n" );

  iowrite32( addr, (plxbase+DMA_CH_1_PCI_ADDR) );
  iowrite32( len, (plxbase+DMA_CH_1_TRANS_BYTE_CNT) );
}

void plxDma1Setup( void *plxbase )
{ 
  unsigned mode = 0x00020D43; // 32 bit, TA/READY input, Done-Int, Constant Address, PCI-Int.

  DEBUGMSG( ">>plxDma1Setup\n" );

  iowrite32( mode, (plxbase+DMA_CH_1_MODE) );
  iowrite32( AOUT_DATA, (plxbase+DMA_CH_1_LOCAL_ADDR) );
  iowrite32( 0x0, (plxbase+DMA_CH_1_DESC_PTR) );
}

void plxDma1Start( void *plxbase )
{
  unsigned int uval;

  DEBUGMSG( ">>plxDma1Start\n" );

  uval = ioread32( (plxbase+DMA_CMD_STATUS) );
  uval = 0x0300 | (uval & 0x0101);                 // enable and start
  iowrite32( uval, (plxbase+DMA_CMD_STATUS) );
}

void plxDma1Stop( void *plxbase )
{
  unsigned int uval;
  
  DEBUGMSG( ">>plxDma1Stop\n" );

  // abort
  uval = ioread32( (plxbase+DMA_CMD_STATUS) );
  uval = 0x0400 | (uval & 0x01);
  iowrite32( uval, (plxbase+DMA_CMD_STATUS) );

  // wait for done
  uval = ioread32( (plxbase+DMA_CMD_STATUS) );
  while( !(uval & 0x1000) )
    {
      schedule();
      uval = ioread32( (plxbase+DMA_CMD_STATUS) );
    }

  // clear the interrupt
  uval = 0x0800 | (uval & 0x01);
  iowrite32( uval, (plxbase+DMA_CMD_STATUS) );
}

// ===================================================================

void daqInputInterruptEnable( void *iobase )
{
  unsigned int uval;

  uval = ioread32( (iobase+ICR) );

  uval |= (ICR_INPUT_LOW_TO_HIGH_SELECT | ICR_INPUT_OVERFLOW_SELECT);

  iowrite32( uval, (iobase+ICR) ); 
}

void daqInputInterruptDisable( void *iobase )
{
  unsigned int uval;

  uval = ioread32( (iobase+ICR) );

  uval &= ~(ICR_INPUT_LOW_TO_HIGH_SELECT | ICR_INPUT_OVERFLOW_SELECT);

  iowrite32( uval, (iobase+ICR) ); 
}

// ===================================================================

void daqOutputInterruptEnable( void *iobase )
{
  unsigned int uval;

  uval = ioread32( (iobase+ICR) );

  uval |= (ICR_OUTPUT_HIGH_TO_LOW_SELECT | ICR_OUTPUT_OVERFLOW_SELECT);

  iowrite32( uval, (iobase+ICR) ); 
}

void daqOutputInterruptDisable( void *iobase )
{
  unsigned int uval;

  uval = ioread32( (iobase+ICR) );

  uval &= ~(ICR_OUTPUT_HIGH_TO_LOW_SELECT | ICR_OUTPUT_OVERFLOW_SELECT);

  iowrite32( uval, (iobase+ICR) ); 
}

// ===================================================================

int daqDma0StartSeg( DAQDRV_Dev *dev, DAQDRV_DmaSeg *seg )
{
  unsigned uval;
  int retv = 0;

  DEBUGMSG( "**daqDma0StartSeg seg = 0x%x\n", (unsigned int) seg );

  if ( seg && seg->len && ( (seg->controls & SEGIDMASK) == SEGID ) ) 
    {
      // enable the dma interrupt handler
      dev->dma0.seg = seg;
      dev->dma0.enabled = 1;

      // load the dma registers
      iowrite32( seg->addr, (dev->plxbase+DMA_CH_0_PCI_ADDR) );
      iowrite32( seg->len, (dev->plxbase+DMA_CH_0_TRANS_BYTE_CNT) );

      // clear the threshold
      iowrite32( 0x40000, (dev->iobase+AIN_BUFF_THRSHLD) );

      // can we read it now?
      uval = ioread32( (dev->iobase+AIN_BUFF_SIZE) );

      DEBUGMSG( "  daqDma0StartSeg: dma addr 0x%x len %d bytes, current buff %d words\n",
		seg->addr, seg->len, uval );

      if ( seg->len/4 <= uval )
	{
	  DEBUGMSG( "  daqDma0StartSeg: starting dma\n" );

	  // start
	  plxDma0Start( dev->plxbase );
	}

      else
	{
	  DEBUGMSG( "  daqDma0StartSeg: wait for data in fifo\n" );

	  // set the threshold
	  uval = seg->len/4 - 1;
	  iowrite32( uval, (dev->iobase+AIN_BUFF_THRSHLD) );
	}

      retv = seg->len;
    }

  else
    {
      dev->dma0.seg = NULL;
      dev->dma0.enabled = 0;

      // debuggery

      uval = ioread32( (dev->iobase+AIN_BUFF_SIZE) );

      if ( !seg )
	DEBUGMSG( "  daqDma0StartSeg: no segment, buffsize %d\n", uval );

      else if ( !seg->len )
	DEBUGMSG( "  daqDma0StartSeg: segment null length, buffsize %d\n", uval );

      if ( seg && ((seg->controls & SEGIDMASK) != SEGID) )
	DEBUGMSG( "  daqDma0StartSeg: segment null length, buffsize %d\n", uval );

    }

  DEBUGMSG( "==daqDma0StartSeg => retv %d\n", retv );

  return( retv );
}

int daqDma0Start(  DAQDRV_Dev *dev )
{
  int retv = 0;

  DEBUGMSG( "**daqDma0Start => dev 0x%x\n", (unsigned int) dev );

  // disable dma interrupt
  plxDma0InterruptDisable( dev->plxbase );

  // disable fifo interrupt
  daqInputInterruptDisable( dev->iobase );

  // make sure we're stopped
  plxDma0Stop( dev->plxbase );

  // setup
  plxDma0Setup( dev->plxbase );

  // restart the segment list and counters
  daqdrv_dma_reset( dev, 0 );

  // load the current user page map
  // this is how we know which user buffer is completed
  dev->dma0.user = dev->dma0.userhead;

  // start the DMA, includes seg load, start as needed
  retv = daqDma0StartSeg( dev, dev->dma0.seghead );

  // enable dma interrupt
  plxDma0InterruptEnable( dev->plxbase );

  // enable fifo interrupt
  daqInputInterruptEnable( dev->iobase );

  DEBUGMSG( "==daqDma0Start => retv = %d\n", retv );
 
  return( retv );
}

int daqDma0Cancel(  DAQDRV_Dev *dev )
{
  int retv = 0;

  DEBUGMSG( "**daqDma0Cancel\n" );

  dev->dma0.enabled = 0;

  // disable the fifo interrupt

  daqInputInterruptDisable( dev->iobase );
  
  // disable dma interrupt

  plxDma0InterruptDisable( dev->plxbase );

  // stop the dma

  plxDma0Stop( dev->plxbase );

  DEBUGMSG( "==daqDma0Cancel  done\n" );

  return( retv );
}

// ===================================================================

int daqDma1StartSeg( DAQDRV_Dev *dev, DAQDRV_DmaSeg *seg )
{
  unsigned uval;
  int retv = 0;

  DEBUGMSG( "**daqDma1StartSeg seg = 0x%x\n", (unsigned int) seg );

  if ( seg && seg->len && ( (seg->controls & SEGIDMASK) == SEGID ) )
    {
      dev->dma1.seg = seg;
      dev->dma1.enabled = 1;

      iowrite32( seg->addr, (dev->plxbase+DMA_CH_1_PCI_ADDR) );
      iowrite32( seg->len, (dev->plxbase+DMA_CH_1_TRANS_BYTE_CNT) );

      // clear the threshold
      iowrite32( 0, (dev->iobase+AOUT_BUFF_THRSHLD) );

      // can we write it now?
      uval = ioread32( (dev->iobase+AOUT_BUFF_SIZE) );

      DEBUGMSG( "  daqDma1StartSeg: dma addr 0x%x len %d bytes, current buff %d words\n",
		seg->addr, seg->len, uval );

      if ( seg->len/4 <= FIFOSIZE-uval )
	{
	  DEBUGMSG( "  daqDma1StartSeg: starting dma\n" );

	  // start
	  plxDma1Start( dev->plxbase );
	}

      else
	{
	  DEBUGMSG( "  daqDma1StartSeg: wait for space in fifo\n" );

	  // set the threshold
	  uval = FIFOSIZE - seg->len/4 + 1;
	  iowrite32( uval, (dev->iobase+AOUT_BUFF_THRSHLD) );
	}

      retv = seg->len;
    }
  else
    {
      uval = ioread32( (dev->iobase+AOUT_BUFF_SIZE) );

      if ( !seg )
	DEBUGMSG( "  daqDma1StartSeg: no segment, buffsize %d\n", uval );

      else if ( !seg->len )
	DEBUGMSG( "  daqDma1StartSeg: segment null length, buffsize %d\n", uval );

      if ( seg && (seg->controls & SEGIDMASK) != SEGID )
	DEBUGMSG( "  daqDma1StartSeg: segment null length, buffsize %d\n", uval );

      dev->dma1.seg = NULL;
      dev->dma1.enabled = 0;
    }

  DEBUGMSG( "==daqDma1StartSeg => retv %d\n", retv );

  return( retv );
}

int daqDma1Start(  DAQDRV_Dev *dev )
{
  int retv = 0;

  DEBUGMSG( "**daqDma1Start => dev 0x%x\n", (unsigned int) dev );

  // disable dma interrupt
  plxDma1InterruptDisable( dev->plxbase );

  // disable fifo interrupt
  daqOutputInterruptDisable( dev->iobase );

  // make sure we're stopped
  plxDma1Stop( dev->plxbase );

  // setup
  plxDma1Setup( dev->plxbase );

  // restart the segment list and counters
  daqdrv_dma_reset( dev, 1 );

  // load the current user page map
  // this is how we know which user buffer is completed
  dev->dma1.user = dev->dma1.userhead;

  // includes load, start as needed
  retv = daqDma1StartSeg( dev, dev->dma1.seghead );

  // enable dma interrupt
  plxDma1InterruptEnable( dev->plxbase );

  // enable fifo interrupt
  daqOutputInterruptEnable( dev->iobase );

  DEBUGMSG( "==daqDma1Start => retv = %d\n", retv );
 
  return( retv );
}

int daqDma1Cancel(  DAQDRV_Dev *dev )
{
  int retv = 0;

  DEBUGMSG( "**daqDma1Cancel\n" );

  dev->dma1.enabled = 0;

  // disable the fifo interrupt

  daqOutputInterruptDisable( dev->iobase );
  
  // disable dma interrupt

  plxDma1InterruptDisable( dev->plxbase );

  // stop the dma

  plxDma1Stop( dev->plxbase );

  DEBUGMSG( "==daqDma1Cancel  done\n" );

  return( retv );
}

// ===================================================================

int daqInterruptHandler( void *iobase, void *p )
{
  DAQDRV_Dev *dev = p;
  DAQDRV_Dma *dma;
  DAQDRV_DmaSeg *seg;

  void *plxbase = dev->plxbase;

  unsigned int plxcsr = 0;
  unsigned int daqicr = 0;

  unsigned int uval;

  int retv = 0;

  // clear all of the message bits;
  dev->status &= 0xF;
  dev->intstatus = 0;
  dev->plxstatus = 0;

  /* save plx interrupt and control status
   */
  plxcsr = ioread32( (plxbase+INT_CTRL_STATUS) );
  if ( !(plxcsr & IRQ_PCI_ENABLE) ) return( 0 );
  if ( !(plxcsr & ((1<<13)|(1<<14)|(1<<15)|(1<<20)|(1<<21)|(1<<22)|(1<<23)) ) ) return( 0 );

  // disable all of the plx interrupts
  uval   = plxcsr & ~(IRQ_LOCAL_PCI_ENABLE|IRQ_DMA_0_ENABLE|IRQ_DMA_1_ENABLE);
  iowrite32( uval, (plxbase+INT_CTRL_STATUS) ); 

  DEBUGMSG( ">>isr, top, plxcsr 0x%x\n", plxcsr );

  /* local board interrrupts
   */
  if ( (plxcsr & IRQ_LOCAL_PCI_ENABLE) && (plxcsr & IRQ_LOCAL_ACTIVE) )
    {
      /* Save and clear the daq interrupt status
       */
      daqicr = ioread32( (iobase+ICR) );

      uval  = (~daqicr) & 0xFFFF0000;      // only clear what we read
      uval |= daqicr & 0x0000FFFF;         // save the selects (enables)
      iowrite32( uval, (iobase+ICR) );

      DEBUGMSG( "  isr, local interrupt, icr 0x%x\n", daqicr );

      if (daqicr & 0xFFFF0000)
	{
	  // input overflow
	  if ( daqicr & ICR_INPUT_OVERFLOW )
	    {
	      dev->status |= INPUTOVERFLOW;      // save the status no matter what

	      DEBUGMSG( "  isr, input overflow, status 0x%x\n", dev->status );

	      dev->dma0.overflow = (dev->dma0.overflow < 0xFFFFFFFF) ? (dev->dma0.overflow + 1) : 0;

	      if ( dev->async_queue )
		kill_fasync( &dev->async_queue, SIGRTMIN+DAQRTSIG, POLL_ERR );
	    }

	  // input ready for dma
	  else if ( daqicr & ICR_INPUT_LOW_TO_HIGH ) // input threshold
	    {
	      if ( dev->dma0.enabled )
		{

#ifdef DAQDRV_DEBUG
		  uval = ioread32( (dev->iobase+AIN_BUFF_SIZE) );
		  DEBUGMSG( "  isr, input threshold, buffsize %d calling dma0start\n", uval );
#endif

		  // clear the threshold
		  iowrite32( 0x40000, (dev->iobase+AIN_BUFF_THRSHLD) );

		  plxDma0Start( dev->plxbase );
		}

	      else if ( dev->async_queue )
		{
		  DEBUGMSG( "  isr, input threshold, no dma, signaling async_queue\n" );

		  kill_fasync( &dev->async_queue, SIGRTMIN+DAQRTSIG, POLL_IN );
		}
	      else
		{
		  DEBUGMSG( "  isr, inputput fifo threshold, no dma, no async_queue\n" );
		}
	    }

	  // output overflo
	  if ( daqicr & ICR_OUTPUT_OVERFLOW )
	    {
	      dev->status |= OUTPUTOVERFLOW;      // save the status no matter what

	      DEBUGMSG( "  isr, output overflow status 0x%x\n", dev->status );

	      dev->dma1.overflow = (dev->dma1.overflow < 0xFFFFFFFF) ? (dev->dma1.overflow + 1) : 0;

	      if ( dev->async_queue )
		kill_fasync( &dev->async_queue, SIGRTMIN+DAQRTSIG, POLL_PRI );
	    }

	  // output ready for dma
	  else if ( daqicr & ICR_OUTPUT_HIGH_TO_LOW ) // output threshold
	    {
	      if ( dev->dma1.enabled )
		{

#ifdef DAQDRV_DEBUG
		  uval = ioread32( (dev->iobase+AOUT_BUFF_SIZE) );
		  DEBUGMSG( "  isr, output threshold, buffsize %d calling dma1start\n", uval );
#endif

		  // clear the threshold
		  iowrite32( 0, (dev->iobase+AOUT_BUFF_THRSHLD) );

		  plxDma1Start( dev->plxbase );
		}

	      else if ( dev->async_queue )
		{
		  DEBUGMSG( "  isr, output threshold, no dma, signaling async_queue\n" );

		  kill_fasync( &dev->async_queue, SIGRTMIN+DAQRTSIG, POLL_OUT );
		}
	      else
		{
		  DEBUGMSG( "  isr, outputput fifo threshold, no dma, no async_queue\n" );
		}
	    }

	  dev->intstatus = daqicr;
	}

      retv = 1;
    }

  /* DMA interrupts
   */
  if ( plxcsr & IRQ_DMA_0_ACTIVE )
    {
      uval = ioread32( (plxbase+DMA_CMD_STATUS) );
      iowrite32( (uval|DMA0_CLR_INT), (plxbase+DMA_CMD_STATUS) ); 

      DEBUGMSG( "  isr, DMA0 interrupt  cmdstatus = 0x%x\n", uval );

      /* input done
       */
      if ( uval & DMA0_DONE )
	{
	  dma = &dev->dma0;
	  seg = dma->seg;

	  DEBUGMSG( "    isr, DMA0 done intr, dma 0x%x enabled %d seg 0x%x\n",
		    (unsigned int) dma, dma->enabled, (unsigned int) seg );

	  // start next segment if any
	  if ( !dma->enabled || !seg )
	    {
	      daqInputInterruptDisable( dev->iobase );

#ifdef DAQDRV_DEBUG
	      uval = ioread32( (dev->iobase+AIN_BUFF_SIZE) );
	      DEBUGMSG( "    isr, DMA0 done while not enabled or no seg, buffsize %d\n", uval );
#endif
	    }

	  else if ( !daqDma0StartSeg( dev, seg->next ) )
	    {
	      daqInputInterruptDisable( dev->iobase );

#ifdef DAQDRV_DEBUG
	      if ( seg->next )
		DEBUGMSG( "    isr, DMA0 seg with 0 length!!\n" );
	      else if ( !(seg->controls & SEGINTBIT) )
		DEBUGMSG( "    isr, DMA0 seg endoflist with no intbit!!\n" );
	      else
		DEBUGMSG( "    isr, DMA0 seg done, next segment not loaded\n" );
#endif

	    }

	  else
	    {
	      DEBUGMSG( "    isr, DMA0 seg done, next segment loaded\n" );
	    }
	  
	  // signal if indicated
	  if ( seg && (seg->controls & SEGINTBIT) )
	    {
	      if ( dma->user )
		{
		  dma->uaddr   = dma->user->uaddr;
		  dma->usize   = dma->user->usize;
		  dma->user    = dma->user->next;
		}
	      else
		{
		  dma->uaddr   = 0;
		  dma->usize   = 0;
		}

	      dma->counter = (dma->counter < 0xFFFFFFFF) ? (dma->counter + 1) : 0;

	      if ( dev->async_queue )
		{
		  DEBUGMSG( "    isr, DMA0 done intr, iobase 0x%x, signaling async_queue\n",
			    (unsigned int) dev->iobase );

		  kill_fasync( &dev->async_queue, SIGRTMIN+DAQRTSIG, POLL_IN );
		}
	      else
		{
		  DEBUGMSG( "    isr, DMA0 done intr, no async_queue\n" );
		}
	    }
	}

      dev->plxstatus = plxcsr;
      retv = 1;
    }

  if ( plxcsr & IRQ_DMA_1_ACTIVE )
    {
      uval = ioread32( (plxbase+DMA_CMD_STATUS) );
      iowrite32( (uval|DMA1_CLR_INT), (plxbase+DMA_CMD_STATUS) ); 

      DEBUGMSG( "  isr, DMA1 interrupt  cmdstatus = 0x%x\n", uval );

      // do stuff here
      if ( uval & DMA1_DONE )
	{
	  dma = &dev->dma1;
	  seg = dma->seg;

	  DEBUGMSG( "    isr, DMA1 done intr, dma 0x%x seg 0x%x\n", (unsigned int) dma, (unsigned int) seg );

	  // start next segment if any
	  if ( !dma->enabled || !seg )
	    {
	      daqOutputInterruptDisable( dev->iobase );

#ifdef DAQDRV_DEBUG
	      uval = ioread32( (dev->iobase+AOUT_BUFF_SIZE) );
	      DEBUGMSG( "    isr, DMA1 done while not enabled or no seg, buffsize %d\n", uval );
#endif
	    }

	  else if ( !daqDma1StartSeg( dev, seg->next ) )
	    {
	      daqOutputInterruptDisable( dev->iobase );

#ifdef DAQDRV_DEBUG
	      if ( seg->next )
		  DEBUGMSG( "    isr, DMA1 seg with 0 length!!\n" );
	      else if ( !(seg->controls & SEGINTBIT) )
		  DEBUGMSG( "    isr, DMA1 seg endoflist with no intbit!!\n" );
	      else
		  DEBUGMSG( "    isr, DMA1 seg done, next segment not loaded\n" );
#endif

	    }
	  else
	    {
	      DEBUGMSG( "    isr, DMA1 seg done, next segment loaded\n" );
	    }

	  // signal if indicated
	  if ( seg && (seg->controls & SEGINTBIT) )
	    {
	      if ( dma->user )
		{
		  dma->uaddr   = dma->user->uaddr;
		  dma->usize   = dma->user->usize;
		  dma->user    = dma->user->next;
		}
	      else
		{
		  dma->uaddr   = 0;
		  dma->usize   = 0;
		}

	      dma->counter = (dma->counter < 0xFFFFFFFF) ? (dma->counter + 1) : 0;

	      if ( dev->async_queue )
		{
		  DEBUGMSG( "    isr, DMA1 done intr, iobase 0x%x, signaling async_queue\n",
			    (unsigned int) dev->iobase );

		  kill_fasync( &dev->async_queue, SIGRTMIN+DAQRTSIG, POLL_OUT );
		}
	      else
		{
		  DEBUGMSG( "    isr, DMA1 done intr, no async_queue\n" );
		}
	    }
	}

      dev->plxstatus = plxcsr;

      retv = 1;
    }

  /* update timers
   */
  if ( retv )
    {
      dev->jiffie = jiffies;
      dev->cycle  = get_cycles();
    }


  /* reenable pci interrupts in the plx chips
   */
  iowrite32( plxcsr, (plxbase+INT_CTRL_STATUS) ); 

  return( retv );
}

// ===================================================================

int daqReadFifo_Pio( void *iobase, unsigned int *uaddr, int ndata )
{
  unsigned int utemp;

  int navailable;

  int n = 0;
  int m;

  while( n < ndata )
    {
      navailable = readl( (iobase+AIN_BUFF_SIZE) );

      if ( navailable & FIFOOVFL )
	{
	  DEBUGMSG( "daqReadFifo_Pio overflow 0x%0x\n", navailable );
	  n = -1;
	  break;
	}

      navailable &= FIFOMASK;

      if ( !navailable )
	{
	  schedule( );
	}

      else
	{
	  m = ndata - n;
	  if ( navailable > m ) navailable = m;

	  for( m = 0; m < navailable; m++)
	    {
	      utemp = readl( (iobase+AIN_DATA) );
	      __put_user( utemp, uaddr );
	      uaddr++;
	    }
	  n += navailable;
	}
    }

  return( n );
}


int daqWriteFifo_Pio( void *iobase, unsigned int *uaddr, int ndata )
{
  unsigned int utemp;

  int navailable;

  int n = 0;
  int m;

  while( n < ndata )
    {
      navailable = readl( (iobase+AOUT_BUFF_SIZE) );

      if ( navailable & FIFOOVFL )
	{
	  DEBUGMSG( "daqWriteFifo_Pio overflow 0x%0x\n", navailable );
	  n = -1;
	  break;
	}

      navailable &= FIFOMASK;
      navailable = FIFOSIZE - navailable;

      if ( !navailable )
	{
	  schedule( );
	}

      else
	{
	  m = ndata - n;
	  if ( navailable > m ) navailable = m;

	  for( m = 0; m < navailable; m++)
	    {
	      __get_user( utemp, uaddr );
	      writel(utemp, (iobase+AOUT_DATA) );
	      uaddr++;
	    }
	  n += navailable;
	}
    }

  return( n );
}

// ===================================================================

void daqReset( void *iobase )
{
  iowrite32( (0x1<<25)|(0x1<<31), (iobase+BCR) );
}

// ===================================================================

static
int countbits( unsigned int uval )
{
  int n = 0;

  while( uval ) {
    n += ( uval & 1 );
    uval = uval >> 1;
  }

  return( n );
}

/* Voltage Scale translations
*/

static
float gscVfs( int range )
{
  float vfs[3] = { 2.5f, 5.f, 10.f };
  
  if ( range > 2 ) range = 2;
  else if ( range < 0 ) range = 0;
  
  return( vfs[range] );
}

static
void gscCheckCtl_( DaqAioCtl *ctl, int maxchannels )
{
  ctl->nchans = LIMIT( ctl->nchans, maxchannels, 0 );

  ctl->nburst = LIMIT( ctl->nburst, 0xFFFFFF, 0 );

  ctl->ndiv   = LIMIT( ctl->ndiv, 0xFFFFFF, 0x14 );

  ctl->nrange = LIMIT( ctl->nrange, 2, 0 );
}

static
void gscCheckCtls( DAQDRV_Ctl *b )
{
  DaqBoardConfig *cfg  = &(b->cfg);
  DaqAioCtl      *ictl = &(b->ictl);
  DaqAioCtl      *octl = &(b->octl);

  octl->nmode   = 0;
  octl->nburst  = 0;

  gscCheckCtl_( ictl, cfg->inchans );
  gscCheckCtl_( octl, cfg->outchans );
}

static
void daqReadBoardSpecs( void *iobase, DAQDRV_Ctl *b )
{
  unsigned int uval;

  uval = ioread32( (iobase+BRD_CONFIG) );
  
  b->cfg.version  = uval & 0xfff;
  b->cfg.inchans  = ((uval >> 16) & 1) ? 4 : 8;
  b->cfg.outchans = ((uval >> 17) & 1) ? 0 : 4;

  b->cfg.clock    = BOARDCLOCKDEFAULT_16AISS8AO4 ;
}

void daqReadCtls( void *iobase, DAQDRV_Ctl *b )
{
  DaqBoardConfig *cfg  = &(b->cfg);
  DaqAioCtl      *ictl = &(b->ictl);
  DaqAioCtl      *octl = &(b->octl);
  
  unsigned int uval;

  daqReadBoardSpecs( iobase, b );

  uval = ioread32( (iobase+RATE_A) );

  ictl->ndiv   = uval & 0xFFFFFF;
  //  ictl->clockrate = gscClockRate( cfg->clock, ictl->ndiv );

  uval = ioread32( (iobase+AIN_CONFIG) );

  ictl->nchans = countbits( (uval>>24) );
  ictl->nchans = LIMIT( ictl->nchans, cfg->inchans, 0 );

  ictl->nburst =  uval & 0xFFFFFF;
  ictl->ndata  = ictl->nchans * ictl->nburst;

  ictl->nthreshold = ioread32( (iobase+AIN_BUFF_THRSHLD) );

  uval = ioread32( (iobase+RATE_C) );

  octl->ndiv   = uval & 0xFFFFFF;
  //  octl->clockrate = gscClockRate( cfg->clock, octl->ndiv );

  uval = ioread32( (iobase+BUFF_OUT_OPS) );

  octl->nchans = countbits( (uval & 0xF) );
  octl->nchans = LIMIT( octl->nchans, cfg->outchans, 0 );

  octl->clkmaster = (uval >> 4) & 1;

  octl->nthreshold = ioread32( (iobase+AOUT_BUFF_THRSHLD) );

  uval = ioread32( (iobase+BCR) );

  ictl->nmode   = uval & 0x7;
  ictl->nrange  = (uval>>4) & 0x3;
  ictl->vfs     = gscVfs( ictl->nrange );

  ictl->clkmaster = (uval >> 24) & 1;

  octl->nrange = (uval >> 16) & 3;
  octl->vfs    = gscVfs( octl->nrange );

  octl->nburst  = 0;
  octl->nmode   = 0;

  b->trigmaster = (uval >> 21) & 1;
}

void daqWriteCtls( void *iobase, DAQDRV_Ctl *b )
{
  DaqAioCtl      *ictl = &(b->ictl);
  DaqAioCtl      *octl = &(b->octl);

  unsigned int uval;

  daqReadBoardSpecs( iobase, b );

  gscCheckCtls( b );

  iowrite32( ictl->ndiv, (iobase+RATE_A) ); 

  uval = (1<<(ictl->nchans)) - 1;
  uval = ictl->nburst | (uval << 24);
    
  iowrite32( uval, (iobase+AIN_CONFIG) ); 

  if( (ictl->nthreshold >= 0) && (ictl->nthreshold <= 0x40000) )
    {
      iowrite32( ictl->nthreshold , (iobase+AIN_BUFF_THRSHLD) );
    }

  iowrite32( octl->ndiv, (iobase+RATE_C) ); 

  uval = ((1<<(octl->nchans))-1);          // turn on everything
  uval |= octl->clkmaster ? 1<<4 : 0;      // configure clock initiator/target
  uval |= 1<<5;                            // enable output clocking
  uval |= 1<<11;                           // open and clear the output buffer, 
  uval |= 1<<19;                           // enable output burst

  iowrite32( uval, (iobase+BUFF_OUT_OPS) ); 

  if( (octl->nthreshold >= 0) && (octl->nthreshold <= 0x40000) )
    {
      iowrite32( octl->nthreshold , (iobase+AOUT_BUFF_THRSHLD) );
    }
      
  uval = ictl->nmode;                           // input mode select
  uval |= (ictl->nrange<<4);                    // input range A select
  uval |= (ictl->nrange<<6);                    // input range B select
  //  uval |= (ictl->nburst)? (1<<9) : 0;           // input burst enable, if burst
  uval |= 1<<9;                                 // input burst enable, always
  uval |= 3<<12;                                // input buffer enable and clear
  uval |= (octl->nrange<<16);                   // output range
  uval |= 1<<18;                                // simultaneous outputs
  uval |= 1<<19;                                // output fifo buffer enable
  uval |= b->trigmaster ? (1<<21) : 0;          // trigger master
  uval |= (1<<22);                              // Rate-C clock enable
  uval |= ictl->clkmaster ? (1<<24) : 0;        // input clock master
  uval |= (1<<25);                              // offset binary
  uval |= (1<<26);                              // Rate-A clock enable

  iowrite32( uval, (iobase+BCR) ); 

  daqReadCtls( iobase, b );
}

void daqInputTrigger( void *iobase )
{
  unsigned int uval;

  uval  = ioread32( (iobase+BCR) );
  uval |= (1<<11);

  iowrite32( uval, (iobase+BCR) ); 
}

void daqOutputTrigger( void *iobase )
{
  unsigned int uval;

  uval  = ioread32( (iobase+BUFF_OUT_OPS) );
  uval |= (1<<20);

  iowrite32( uval, (iobase+BUFF_OUT_OPS) ); 
}

void daqInputOutputTrigger( void *iobase )
{
  unsigned int uval1, uval2;

  uval1  = ioread32( (iobase+BCR) );
  uval1 |= (1<<11) | (1<<23);

  uval2  = ioread32( (iobase+BUFF_OUT_OPS) );
  uval2 |= (1<<20);

  // this is as good as it gets for this card
  iowrite32( uval1, (iobase+BCR) ); 
  iowrite32( uval2, (iobase+BUFF_OUT_OPS) ); 
}
