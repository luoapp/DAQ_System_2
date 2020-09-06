/* daqdrv

   daqdrv component for gsc daq card

   Copyright 2006, Mitchell C. Nelson, Ph.D.

   No representation is provided of suitability for any purpose whatsoever.

*/

#include <linux/version.h>

#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19))
#include <linux/config.h>
#endif

#include <asm/uaccess.h>
#include <linux/sched.h>

#include <linux/interrupt.h>

#include "plx_regs.h"

// debug for this file only
//#define DEBUGISR
//#define DAQDRV_DEBUG


// debugs
#ifdef DAQDRV_DEBUG
#ifndef DEBUGISR
#define DEBUGISR
#endif
#endif

#ifdef DEBUGISR
#define DEBUGISRMSG(fmt, args...) printk( KERN_NOTICE "daqdrv: " fmt, ## args )
#else
#define DEBUGISRMSG(fmt, args...)
#endif


#include "daqdrvmain.h"
#include "daqdrv.h"

#include "gsc16aiss8ao4regs.h"
#include "gsc16aiss8ao4.h"

#ifdef DAQDRV_KERNELFPU
#include <asm/i387.h>
#endif

#define LIMIT( a, max, min ) ( a > max ? max : ( a < min ? min : a) )


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
  DEBUGMSG( "  plxDma0Load\n" );

  iowrite32( addr, (plxbase+DMA_CH_0_PCI_ADDR) );
  iowrite32( len, (plxbase+DMA_CH_0_TRANS_BYTE_CNT) );
}

void plxDma0Setup( void *plxbase )
{
  unsigned mode = 0x00020D43; // 32 bit, TA/READY input, Done-Int, Constant Address, PCI-Int.

  DEBUGMSG( "  plxDma0Setup\n" );

  iowrite32( mode, (plxbase+DMA_CH_0_MODE) );
  iowrite32( AIN_DATA, (plxbase+DMA_CH_0_LOCAL_ADDR) );
  iowrite32( 0x8, (plxbase+DMA_CH_0_DESC_PTR) );
}

void plxDma0Start( void *plxbase )
{
  unsigned int uval;

  DEBUGMSG( "  plxDma0Start\n" );

  uval = ioread32( (plxbase+DMA_CMD_STATUS) );
  uval = 0x03 | (uval & 0x0101);                 // enable and start
  iowrite32( uval, (plxbase+DMA_CMD_STATUS) );
}

void plxDma0Stop( void *plxbase )
{
  unsigned int uval;
  
  DEBUGMSG( "  plxDma0Stop\n" );

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
  DEBUGMSG( "  plxDma1Load\n" );

  iowrite32( addr, (plxbase+DMA_CH_1_PCI_ADDR) );
  iowrite32( len, (plxbase+DMA_CH_1_TRANS_BYTE_CNT) );
}

void plxDma1Setup( void *plxbase )
{ 
  unsigned mode = 0x00020D43; // 32 bit, TA/READY input, Done-Int, Constant Address, PCI-Int.

  DEBUGMSG( "  plxDma1Setup\n" );

  iowrite32( mode, (plxbase+DMA_CH_1_MODE) );
  iowrite32( AOUT_DATA, (plxbase+DMA_CH_1_LOCAL_ADDR) );
  iowrite32( 0x0, (plxbase+DMA_CH_1_DESC_PTR) );
}

void plxDma1Start( void *plxbase )
{
  unsigned int uval;

  DEBUGMSG( "  plxDma1Start\n" );

  uval = ioread32( (plxbase+DMA_CMD_STATUS) );
  uval = 0x0300 | (uval & 0x0101);                 // enable and start
  iowrite32( uval, (plxbase+DMA_CMD_STATUS) );
}

void plxDma1Stop( void *plxbase )
{
  unsigned int uval;
  
  DEBUGMSG( "  plxDma1Stop\n" );

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

void daqInterruptDisableClear( void *iobase )
{
  iowrite32( 0, (iobase+ICR) ); 
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

int daqDma0Cancel(  DAQDRV_Dev *dev )
{
  int retv = 0;

  DEBUGMSG( "  daqDma0Cancel\n" );

  // mark disabled
  dev->dma0.enabled = 0;

  // disable the fifo interrupt
  daqInputInterruptDisable( dev->iobase );
  
  // disable dma interrupt
  plxDma0InterruptDisable( dev->plxbase );

  // stop the dma
  plxDma0Stop( dev->plxbase );

  DEBUGMSG( "    daqDma0Cancel done\n" );

  return( retv );
}

int daqDma0Start(  DAQDRV_Dev *dev )
{
  DAQDRV_Dma    *dma = &dev->dma0;
  DAQDRV_DmaSeg *seg = dma->seghead;

  unsigned int uval;

  int retv = 0;

  DEBUGMSG( "  daqDma0Start, dev 0x%lx\n", (unsigned long) dev );

  // restart the segment list and counters, disabled
  daqdrv_dma_reset( dev, 0 );

  // check the first segment
  if ( !seg )
    {
      DAQDRVMSG( "daqDma0Start with NULL seg!!\n");
    }
  else if ( (seg->controls & SEGIDMASK) != SEGID ) 
    {
      DAQDRVMSG( "daqDma0Start with segid mismatch!!\n");
    }
  else if ( !seg->len  )
    {
      DAQDRVMSG( "daqDma0Start with seg len = 0!!\n");
    }
  else
    {
      DEBUGISRMSG( "daqDma0Start, starting (wait for fifo)\n");

      /* load the current user page addres, this is how the user
	 finds out know which user buffer is completed
      */
      dma->user = dma->userhead;

      // enable the dma interrupt handler
      dma->seg     = seg;
      dma->enabled = 1;

      // setup the DMA
      plxDma0Setup( dev->plxbase );

      // load the dma registers
      plxDma0Load( dev->plxbase, seg->addr, seg->len );

      // enable dma interrupt
      plxDma0InterruptEnable( dev->plxbase );

      // clear the fifo threshold
      iowrite32( 0x40000, (dev->iobase+AIN_BUFF_THRSHLD) );

      // enable fifo interrupt
      plxLocalInterruptEnable( dev->plxbase );
      daqInputInterruptEnable( dev->iobase );

      /* set the threshold, this will force it if
	 it was alread set.
      */
      uval = seg->len/4 - 1;
      iowrite32( uval, (dev->iobase+AIN_BUFF_THRSHLD) );

      retv = seg->len;
    }

  DEBUGMSG( "    daqDma0Start done, retv = %d\n", retv );
 
  return( retv );
}

// ===================================================================

int daqDma1Cancel(  DAQDRV_Dev *dev )
{
  int retv = 0;

  DEBUGMSG( "  daqDma1Cancel\n" );

  // mark disabled
  dev->dma1.enabled = 0;

  // disable the fifo interrupt
  daqOutputInterruptDisable( dev->iobase );
  
  // disable dma interrupt
  plxDma1InterruptDisable( dev->plxbase );

  // stop the dma
  plxDma1Stop( dev->plxbase );

  DEBUGMSG( "  daqDma1Cancel done\n" );

  return( retv );
}

int daqDma1Start(  DAQDRV_Dev *dev )
{
  DAQDRV_Dma    *dma = &dev->dma1;
  DAQDRV_DmaSeg *seg = dma->seghead;

  int retv = 0;

  DEBUGMSG( "  daqDma1Start dev 0x%lx\n", (unsigned long) dev );

  // restart the segment list and counters
  daqdrv_dma_reset( dev, 1 );

  // check the first segment
  if ( !seg )
    {
      DAQDRVMSG( "daqDma1Start with NULL seg!!\n");
    }
  else if ( (seg->controls & SEGIDMASK) != SEGID ) 
    {
      DAQDRVMSG( "daqDma1Start with segid mismatch!!\n");
    }
  else if ( !seg->len  )
    {
      DAQDRVMSG( "daqDma1Start with seg len = 0!!\n");
    }
  else
    {
      DEBUGISRMSG( "daqDma1Start, starting\n");

      /* load the current user page addres, this is how the user
	 finds out know which user buffer is completed
      */
      dma->user = dma->userhead;

      // enable the dma interrupt handler
      dma->seg     = seg;
      dma->enabled = 1;

      // setup the DMA
      plxDma1Setup( dev->plxbase );

      // load the next address and length
      plxDma1Load( dev->plxbase, seg->addr, seg->len );

      // enable dma interrupt
      plxDma1InterruptEnable( dev->plxbase );

      // clear the threshold
      iowrite32( 0, (dev->iobase+AOUT_BUFF_THRSHLD) );

      // enable fifo interrupt
      plxLocalInterruptEnable( dev->plxbase );
      daqOutputInterruptEnable( dev->iobase );

      // start
      plxDma1Start( dev->plxbase );

      retv = seg->len;
    }

  DEBUGMSG( "    daqDma1Start done, retv = %d\n", retv );
 
  return( retv );
}


// ===================================================================

void daqdebuginfo( DAQDRV_Dev *dev )
{
  void *iobase  = dev->iobase;
  void *plxbase = dev->plxbase;

  unsigned int plxcsr, daqicr, ithresh, ibuff, othresh, obuff;

  plxcsr  = ioread32( (plxbase+INT_CTRL_STATUS) );
  daqicr  = ioread32( (iobase+ICR) );

  ithresh = ioread32( (iobase+AIN_BUFF_THRSHLD) );
  ibuff   = ioread32( (iobase+AIN_BUFF_SIZE) );
  othresh = ioread32( (iobase+AOUT_BUFF_THRSHLD) );
  obuff   = ioread32( (iobase+AOUT_BUFF_SIZE) );

  DAQDRVMSG( "  plxcsr 0x%x, daqicr 0x%x in thresh 0x%x size 0x%x out thresh 0x%x size 0x%x\n",
	     plxcsr, daqicr, ithresh, ibuff, othresh, obuff );

}

// ===================================================================

#define BITMATCH( a, b ) ( (a & (b)) == b )

static
void dmaupdate( DAQDRV_Dma *dma )
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
}

int daqInterruptHandler( void *iobase, void *p )
{
  DAQDRV_Dev *dev = p;
  DAQDRV_Dma *dma;
  DAQDRV_DmaSeg *seg;

  void *plxbase = dev->plxbase;

  unsigned int plxcsr = 0;
  unsigned int dmacsr = 0;
  unsigned int daqicr = 0;

  // debuggery
  unsigned int ithresh = 0;
  unsigned int othresh = 0;
  unsigned int ibuff = 0;
  unsigned int obuff = 0;

  unsigned int uval;

  int retv = 0;

  int siglist[8];
  int nsigs = 0;
  int n = 0;

  /* check if it could be our interrupt
   */
  plxcsr = ioread32( (plxbase+INT_CTRL_STATUS) );
  if ( !(plxcsr & IRQ_PCI_ENABLE) ) return( 0 );
  if ( !(plxcsr & ( (1<<13)|(1<<14)|(1<<15)|(1<<20)|(1<<21)|(1<<22)|(1<<23)) ) ) return( 0 );

  // get the dma and fifo status registers
  dmacsr = ioread32( (plxbase+DMA_CMD_STATUS) );
  daqicr = ioread32( (iobase+ICR) );

  ithresh = ioread32( (iobase+AIN_BUFF_THRSHLD) );
  ibuff   = ioread32( (iobase+AIN_BUFF_SIZE) );
  othresh = ioread32( (iobase+AOUT_BUFF_THRSHLD) );
  obuff   = ioread32( (iobase+AOUT_BUFF_SIZE) );

  // first level debuggery
  DEBUGISRMSG( ">>isr top, plxcsr 0x%8.8x dmacsr 0x%8.8x daqicr 0x%8.8x"
	       " i thr 0x%x siz 0x%x o thr 0x%x siz 0x%x\n",
	       plxcsr, dmacsr, daqicr, ithresh, ibuff, othresh, obuff );

  // save the plxcsr and daqicr at interrupt
  dev->plxstatus = plxcsr;
  dev->intstatus = daqicr;

  // clear all of the message bits;
  dev->status &= 0xF;
  dev->intstatus = 0;


  /* -------------------------------------------------------------------------------
     Clear only the DMA interrupts that we detected. The same interrupt might be set
     again during the isr.
  */

  uval = 0;

  if ( BITMATCH( plxcsr , (IRQ_DMA_0_ENABLE | IRQ_DMA_0_ACTIVE) ) )
    uval |= DMA0_CLR_INT;

  if ( BITMATCH( plxcsr , (IRQ_DMA_1_ENABLE | IRQ_DMA_1_ACTIVE) ) )
    uval |= DMA1_CLR_INT;

  if ( uval )
    {
      uval |= dmacsr & ( DMA0_ENABLE | DMA1_ENABLE );
      iowrite32( uval, plxbase+DMA_CMD_STATUS );

      // We cleared something, Mazel Tov!
      retv = 1;
    }

  /* -------------------------------------------------------------------------------
     Check, save, clear, and handle the DAQ FIFO interrupts. Same issues as above.
   */

  if ( BITMATCH( plxcsr , (IRQ_LOCAL_ENABLE | IRQ_LOCAL_ACTIVE) ) )
    {
      /* If input overflowed, stop the input.
       */
      if ( BITMATCH( daqicr, (ICR_INPUT_OVERFLOW | ICR_INPUT_OVERFLOW_SELECT ) ) )
	{
	  DEBUGISRMSG( "  isr, daq input overflow, disabling dma0, signalling POLL_ERR\n" );

	  // Clear the response bit(s)
	  uval = daqicr & ~(ICR_INPUT_OVERFLOW|ICR_INPUT_LOW_TO_HIGH);
	  iowrite32( uval, (iobase+ICR) );

	  // We cleared something, Mazel Tov!
	  retv = 1;
						
	  // Disable.
	  dev->dma0.seg     = NULL;
	  dev->dma0.enabled = 0;

	  // save status and increment error counter
	  dev->status |= INPUTOVERFLOW;
	  dev->dma0.overflow = (dev->dma0.overflow < 0xFFFFFFFF) ? (dev->dma0.overflow + 1) : 0;

	  // add a signal to the list to be done at the bottom
	  siglist[nsigs] = POLL_ERR;
	  nsigs++;

	  /* this is a workaround for a possible firmware bug. if the bit is cleared as the
	     other channel passes its threshold, the response bit isn't set. so we force it.
	  */
	  if ( (daqicr & ICR_OUTPUT_HIGH_TO_LOW_SELECT) && !(daqicr & ICR_OUTPUT_HIGH_TO_LOW) )
	    {
	      uval = ioread32( (iobase+AOUT_BUFF_THRSHLD) );
	      uval &= 0x7FFFF;

	      if ( uval != 0x0 )
		{
		  iowrite32( 0x0, (iobase+AOUT_BUFF_THRSHLD) );
		  iowrite32( uval, (iobase+AOUT_BUFF_THRSHLD) );
		}
	    }
	}

      /* Fifo ready interrupt, start next or done
       */
      else if ( BITMATCH( daqicr, (ICR_INPUT_LOW_TO_HIGH | ICR_INPUT_LOW_TO_HIGH_SELECT) ) )
	{
	  // Clear the response bit
	  uval = daqicr & ~(ICR_INPUT_LOW_TO_HIGH);
	  iowrite32( uval, (iobase+ICR) );

	  // We cleared something, Mazel Tov!
	  retv = 1;

	  if ( dev->dma0.enabled )
	    {
	      DEBUGISRMSG( "  isr, daq input threshold, enabled (continuing), starting dma0\n" );

	      /* Clear the Threshold First(upwards).
		 Fifo level is the integral of the input rate minus the DMA rate. The DMA rate
		 is subject to bus contention, for example from the other DMA channel. In short
		 time scales, the FIFO level can move up or down even though in the long run
		 the DMA wins.  Threshold at max, means no fifo interrupt until overflow.
	      */
	      iowrite32( 0x40000, (dev->iobase+AIN_BUFF_THRSHLD) );

	      // start the dma engine
	      plxDma0Start( dev->plxbase );
	    }

	  else
	    {
	      DEBUGISRMSG( "  isr, daq input threshold, not enabled (done), signalling POLL_IN\n" );

	      // this is like a data ready, with no dma
	      siglist[nsigs] = POLL_IN;
	      nsigs++;
	    }


	  /* this is a workaround for a possible firmware bug. if the bit is cleared as the
	     other channel passes its threshold, the response bit isn't set. so we force it.
	  */
	  if ( (daqicr & ICR_OUTPUT_HIGH_TO_LOW_SELECT) && !(daqicr & ICR_OUTPUT_HIGH_TO_LOW) )
	    {
	      uval = ioread32( (iobase+AOUT_BUFF_THRSHLD) );
	      uval &= 0x7FFFF;

	      if ( uval != 0x0 )
		{
		  iowrite32( 0x0, (iobase+AOUT_BUFF_THRSHLD) );
		  iowrite32( uval, (iobase+AOUT_BUFF_THRSHLD) );
		}
	    }
	}

      /* If output overflowed, stop the output (should never happen).
       */
      if ( BITMATCH( daqicr, (ICR_OUTPUT_OVERFLOW | ICR_OUTPUT_OVERFLOW_SELECT ) ) )
	{
	  DEBUGISRMSG( "  isr, daq output overflow, disabling dma1, signalling POLL_PRI\n" );

	  // Clear the response bit
	  uval = daqicr & ~(ICR_OUTPUT_OVERFLOW|ICR_OUTPUT_HIGH_TO_LOW);
	  iowrite32( uval, (iobase+ICR) );

	  // We cleared something, Mazel Tov!
	  retv = 1;

	  // Disable.
	  dev->dma1.seg     = NULL;
	  dev->dma1.enabled = 0;

	  // save status and increment error counter
	  dev->status |= OUTPUTOVERFLOW;
	  dev->dma1.overflow = (dev->dma1.overflow < 0xFFFFFFFF) ? (dev->dma1.overflow + 1) : 0;

	  // add the error signal to the list
	  siglist[nsigs] = POLL_PRI;
	  nsigs++;

	  /* this is a workaround for a possible firmware bug. if the bit is cleared as the
	     other channel passes its threshold, the response bit isn't set. so we force it.
	  */
	  if ( (daqicr & ICR_INPUT_LOW_TO_HIGH_SELECT) && !(daqicr & ICR_INPUT_LOW_TO_HIGH) )
	    {
	      uval = ioread32( (iobase+AIN_BUFF_THRSHLD) );
	      uval &= 0x7FFFF;

	      if ( uval != 0x40000 )
		{
		  iowrite32( 0x40000, (iobase+AIN_BUFF_THRSHLD) );
		  iowrite32( uval, (iobase+AIN_BUFF_THRSHLD) );
		}
	    }
	}

      /* Fifo high-to-low interrupt, start next or done
       */
      else if ( BITMATCH( daqicr, (ICR_OUTPUT_HIGH_TO_LOW | ICR_OUTPUT_HIGH_TO_LOW_SELECT) ) )
	{
	  // Clear the response but
	  uval = daqicr & ~(ICR_OUTPUT_HIGH_TO_LOW);
	  iowrite32( uval, (iobase+ICR) );

	  // We cleared something, Mazel Tov!
	  retv = 1;

	  if ( dev->dma1.enabled )
	    {
	      /* Check for underflow
	       */
	      uval = ioread32( (dev->iobase+AOUT_BUFF_SIZE) );

	      if ( (uval & 0x7FFFF) )
		{
		  DEBUGISRMSG( "  isr, daq output threshold, dma enabled (continuing), starting dma1\n" );
		  
		  /* Clear the Threshold (downwards) before starting the DMA.
		     Fifo level is the integral of the DMA rate minus the output rate. The DMA rate
		     is subject to bus contention, for example from the other DMA channel. In short
		     time scales, the FIFO level can move up or down even though in the long run
		     the DMA wins. Setting the threshold to 0, means no fifo interrupt until empty.
		  */

		  iowrite32( 0, (dev->iobase+AOUT_BUFF_THRSHLD) );
		  plxDma1Start( dev->plxbase );
		}

	      else
		{
		  DEBUGISRMSG( "  isr, daq output underflow, stopping dma1 signalling POLL_PRI\n" );

		  // Disable.
		  dev->dma1.seg     = NULL;
		  dev->dma1.enabled = 0;

		  // save status and increment error counter
		  dev->status |= OUTPUTUNDERFLOW;
		  dev->dma1.overflow = (dev->dma1.overflow < 0xFFFFFFFF) ? (dev->dma1.overflow + 1) : 0;

		  // add the error signal to the list
		  siglist[nsigs] = POLL_PRI;
		  nsigs++;
		}
	    }

	  else
	    {
	      DEBUGISRMSG( "  isr, output threshold, dma not enabled, signalling POLL_OUT\n" );
	      
	      // We're updating
	      dmaupdate( &dev->dma1 );

	      // We're signalling
	      siglist[nsigs] = POLL_OUT;
	      nsigs++;
	    }

	  /* this is a workaround for a possible firmware bug. if the bit is cleared as the
	     other channel passes its threshold, the response bit isn't set. so we force it.
	  */
	  if ( (daqicr & ICR_INPUT_LOW_TO_HIGH_SELECT) && !(daqicr & ICR_INPUT_LOW_TO_HIGH) )
	    {
	      uval = ioread32( (iobase+AIN_BUFF_THRSHLD) );
	      uval &= 0x7FFFF;

	      if ( uval != 0x40000 )
		{
		  iowrite32( 0x40000, (iobase+AIN_BUFF_THRSHLD) );
		  iowrite32( uval, (iobase+AIN_BUFF_THRSHLD) );
		}
	    }
	}
    }
      
  if ( BITMATCH( plxcsr , (IRQ_DMA_0_ENABLE | IRQ_DMA_0_ACTIVE) ) )
    {
      dma = &dev->dma0;
      seg = dma->seg;

      /* Check for valid DMA state
       */
      if ( !(dmacsr & DMA0_DONE) )
	{
	  DAQDRVMSG( "  isr, dma0 done, no DMA_DONE bit!!\n" );

	  // make sure we're disabled
	  dma->seg     = NULL;
	  dma->enabled = 0;

	  // add the error signal to the list
	  siglist[nsigs] = POLL_MSG;
	  nsigs++;
	}

      else if ( !dma->enabled )
	{
	  DAQDRVMSG( "  isr, dma0 done, dma0 not enabled!!\n" );

	  // make sure we're disabled
	  dma->seg     = NULL;
	  dma->enabled = 0;

	  // add the error signal to the list
	  siglist[nsigs] = POLL_MSG;
	  nsigs++;
	}

      /* Check for valid segment completed
       */
      else if ( !seg )
	{
	  DAQDRVMSG( "  isr, dma0 done, no seg!!\n" );

	  // make sure we're disabled
	  dma->seg     = NULL;
	  dma->enabled = 0;

	  // add the error signal to the list
	  siglist[nsigs] = POLL_MSG;
	  nsigs++;
	}

      else if ( (seg->controls & SEGIDMASK) != SEGID ) 
	{
	  DAQDRVMSG( "isr, dma0 done, segid mismatch!!\n");
	  
	  // Disable.
	  dev->dma0.seg     = NULL;
	  dev->dma0.enabled = 0;

	  // add the error signal to the list
	  siglist[nsigs] = POLL_MSG;
	  nsigs++;
	}

      else if ( !seg->len  )
	{
	  DAQDRVMSG( "isr, dma0 done, seg len = 0!!\n");
	  
	  // Disable.
	  dev->dma0.seg     = NULL;
	  dev->dma0.enabled = 0;

	  // add the error signal to the list
	  siglist[nsigs] = POLL_MSG;
	  nsigs++;
	}

      /* Check for done segment list
       */
      else if ( !(seg=seg->next) )
	{
	  DEBUGMSG( "  isr, dma0 done, no next, signalling POLL_IN\n" );
	  
	  // We're disabling the DMA
	  dma->seg     = NULL;
	  dma->enabled = 0;

	  // We're updating
	  dmaupdate( dma );

	  // We're signalling
	  siglist[nsigs] = POLL_IN;
	  nsigs++;
	}

      /* Okay, we're onto the next
       */
      else
	{
	  /* Did we just finish a buffer?
	   */
	  if ( (dma->seg->controls & SEGINTBIT) )
	    {
	      DEBUGISRMSG( "  isr, dma0 done, previous SEGINTBIT set, signalling POLL_IN\n" );

	      dmaupdate( dma );

	      siglist[nsigs] = POLL_IN;
	      nsigs++;
	    }

	  /* Now the error checking, is the next segment valid?
	   */
	  if ( (seg->controls & SEGIDMASK) != SEGID ) 
	    {
	      DAQDRVMSG( "isr, dma0 done, next segid mismatch!!\n");
	  
	      // Disable.
	      dma->seg     = NULL;
	      dma->enabled = 0;

	      // add the error signal to the list
	      siglist[nsigs] = POLL_MSG;
	      nsigs++;
	    }

	  else if ( !seg->len  )
	    {
	      DAQDRVMSG( "isr, dma0 done, next seg len = 0!!\n");
	  
	      // Disable.
	      dma->seg     = NULL;
	      dma->enabled = 0;

	      // add the error signal to the list
	      siglist[nsigs] = POLL_MSG;
	      nsigs++;
	    }

	  /* Do we have data ready in the buffer?
	   */
	  else if ( (ibuff & 0x7FFFF) > seg->len/4 )
	    {
	      DEBUGISRMSG( "  isr, dma0 done, buff 0x%x len 0x%x, starting\n",
			   ibuff, seg->len/4 );

	      // Enable.
	      dma->seg     = seg;
	      dma->enabled = 1;

	      // clear the threshold
	      iowrite32( 0x40000, (dev->iobase+AIN_BUFF_THRSHLD) );

	      // load the next address and length
	      plxDma0Load( dev->plxbase, seg->addr, seg->len );

	      // start
	      plxDma0Start( dev->plxbase );
	    }

	  else
	    {
	      DEBUGISRMSG( "  isr, dma0 done, buff 0x%x len 0x%x, waiting\n",
			   ibuff, seg->len/4 );
    
	      // Enable.
	      dma->seg     = seg;
	      dma->enabled = 1;

	      // clear the threshold
	      iowrite32( 0x40000, (dev->iobase+AIN_BUFF_THRSHLD) );

	      // load the next address and length
	      plxDma0Load( dev->plxbase, seg->addr, seg->len );

	      // wait
	      uval = seg->len/4 - 1;
	      iowrite32( uval, (dev->iobase+AIN_BUFF_THRSHLD) );
	    }
	}
    }

  if ( BITMATCH( plxcsr , (IRQ_DMA_1_ENABLE | IRQ_DMA_1_ACTIVE) ) )
    {
      dma = &dev->dma1;
      seg = dma->seg;

      /* Check for valid DMA state
       */
      if ( !(dmacsr & DMA1_DONE) )
	{
	  DAQDRVMSG( "  isr, dma1 done, no DMA_DONE bit!!\n" );

	  // make sure we're disabled
	  dma->seg     = NULL;
	  dma->enabled = 0;

	  // add the error signal to the list
	  siglist[nsigs] = POLL_MSG;
	  nsigs++;
	}

      else if ( !dma->enabled )
	{
	  DAQDRVMSG( "  isr, dma1 done, dma1 not enabled!!\n" );

	  // make sure we're disabled
	  dma->seg     = NULL;
	  dma->enabled = 0;

	  // add the error signal to the list
	  siglist[nsigs] = POLL_MSG;
	  nsigs++;
	}

      /* Check for valid segment completed
       */
      else if ( !seg )
	{
	  DAQDRVMSG( "  isr, dma1 done, no seg!!\n" );

	  // make sure we're disabled
	  dma->seg     = NULL;
	  dma->enabled = 0;

	  // add the error signal to the list
	  siglist[nsigs] = POLL_MSG;
	  nsigs++;
	}

      else if ( (seg->controls & SEGIDMASK) != SEGID ) 
	{
	  DAQDRVMSG( "isr, dma1 done, segid mismatch!!\n");
	  
	  // Disable.
	  dev->dma0.seg     = NULL;
	  dev->dma0.enabled = 0;

	  // add the error signal to the list
	  siglist[nsigs] = POLL_MSG;
	  nsigs++;
	}

      else if ( !seg->len  )
	{
	  DAQDRVMSG( "isr, dma1 done, seg len = 0!!\n");
	  
	  // Disable.
	  dev->dma0.seg     = NULL;
	  dev->dma0.enabled = 0;

	  // add the error signal to the list
	  siglist[nsigs] = POLL_MSG;
	  nsigs++;
	}

      /* Check for done segment list
       */
      else if ( !(seg=seg->next) )
	{
	  DEBUGISRMSG( "  isr, dma1 done, no next\n" );
	  
	  // We're disabling the DMA
	  dma->seg     = NULL;
	  dma->enabled = 0;

	  // We're updating
	  dmaupdate( dma );

	  // We're not signalling, fifo empty does it
	  // siglist[nsigs] = POLL_OUT;
	  // nsigs++;
	}

      /* Okay, we're onto the next
       */
      else
	{
	  /* Did we just finish a buffer?
	   */
	  if ( (dma->seg->controls & SEGINTBIT) )
	    {
	      DEBUGISRMSG( "  isr, dma1 done, previous SEGINTBIT set, signalling POLL_OUT\n" );

	      dmaupdate( dma );

	      siglist[nsigs] = POLL_OUT;
	      nsigs++;
	    }

	  /* Now the error checking, is the next segment valid?
	   */
	  if ( (seg->controls & SEGIDMASK) != SEGID ) 
	    {
	      DAQDRVMSG( "isr, dma1 done, next segid mismatch!!\n");
	  
	      // Disable.
	      dma->seg     = NULL;
	      dma->enabled = 0;

	      // add the error signal to the list
	      siglist[nsigs] = POLL_MSG;
	      nsigs++;
	    }

	  else if ( !seg->len  )
	    {
	      DAQDRVMSG( "isr, dma1 done, next seg len = 0!!\n");
	  
	      // Disable.
	      dma->seg     = NULL;
	      dma->enabled = 0;

	      // Add the error signal to the list
	      siglist[nsigs] = POLL_MSG;
	      nsigs++;
	    }

	  /* Do we have room for more data ready in the buffer?
	   */
	  else if ( (FIFOSIZE - (obuff & 0x7FFFF)) > seg->len/4 )
	    {
	      DEBUGISRMSG( "  isr, dma1 done, buff 0x%x len 0x%x, starting\n",
			   obuff, seg->len/4 );

	      // Enable.
	      dma->seg     = seg;
	      dma->enabled = 1;

	      // clear the threshold
	      iowrite32( 0x0, (dev->iobase+AOUT_BUFF_THRSHLD) );

	      // load the next address and length
	      plxDma1Load( dev->plxbase, seg->addr, seg->len );

	      // start
	      plxDma1Start( dev->plxbase );
	    }

	  else
	    {
	      DEBUGISRMSG( "  isr, dma1 done, buff 0x%x len 0x%x, waiting\n",
			   obuff, seg->len/4 );
    
	      // Enable.
	      dma->seg     = seg;
	      dma->enabled = 1;

	      // clear the threshold
	      iowrite32( 0x0, (dev->iobase+AOUT_BUFF_THRSHLD) );

	      // load the next address and length
	      plxDma1Load( dev->plxbase, seg->addr, seg->len );

	      // wait
	      uval = FIFOSIZE - seg->len/4;
	      iowrite32( uval, (dev->iobase+AOUT_BUFF_THRSHLD) );
	    }
	}
    }
       
  /* update timers
   */
  if ( retv )
    {
      dev->jiffie = jiffies;
      dev->cycle  = get_cycles();
    }
  DEBUGMSG( ">>isr done at %ld jiffies, queueing %d signals\n", dev->jiffie, nsigs );

  // The user might be running on another processor, so we send the signals
  // after we are completely done accessing the hardware.

  if ( dev->async_queue )
    {
#ifdef DEBUGISR
      for ( n = 0; n < nsigs; n++ )
	{
	  switch( siglist[n] )
	    {
	    case POLL_IN:
	      DAQDRVMSG( ">>isr signalling POLL_IN %d\n", siglist[n] );
	      break;
	    case POLL_OUT:
	      DAQDRVMSG( ">>isr signalling POLL_OUT %d\n", siglist[n] );
	      break;
	    case POLL_ERR:
	      DAQDRVMSG( ">>isr signalling POLL_ERR %d\n", siglist[n] );
	      break;
	    case POLL_PRI:
	      DAQDRVMSG( ">>isr signalling POLL_PRI %d\n", siglist[n] );
	      break;
	    default:
	      DAQDRVMSG( ">>isr signalling UNKNOWN %d\n", siglist[n] );
	    }
	}
#endif

      for ( n = 0; n < nsigs; n++ )
	{
	  kill_fasync( &dev->async_queue, SIGRTMIN+DAQRTSIG, siglist[n] );
	}
    }

  else if ( nsigs )
    {
      DAQDRVMSG( "  isr, signals pending, no async signal queue!!\n" );
    }

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

/*
#ifdef DAQDRV_KERNELFPU
static
float gscVfs( int range )
{
  float vfs[3] = { 2.5f, 5.f, 10.f };
  
  if ( range > 2 ) range = 2;
  else if ( range < 0 ) range = 0;
  
  return( vfs[range] );
}
#endif
*/

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

#ifdef DAQDRV_KERNELFPU
  kernel_fpu_begin();
  b->cfg.clock    = BOARDCLOCKDEFAULT_16AISS8AO4 ;
  kernel_fpu_end();
#endif

  b->cfg.id       = DAQDRV_SUBSYSTEM_ID_16AISS8AO4;

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

  ictl->clkmaster = (uval >> 24) & 1;

  octl->nrange = (uval >> 16) & 3;
  octl->nburst  = 0;
  octl->nmode   = 0;

  b->trigmaster = (uval >> 21) & 1;

#ifdef DAQDRV_KERNELFPU
  /* set floating point VFS
   */

  kernel_fpu_begin();

  switch( ictl->nrange )
    {
    case 2:
      ictl->vfs = 10.f;
      break;
    case 1:
      ictl->vfs = 5.f;
      break;
    case 0:
      ictl->vfs = 2.5f;
      break;
    default:
      ictl->vfs = 10.f;	
    }

  switch( octl->nrange )
    {
    case 2:
      octl->vfs = 10.f;
      break;
    case 1:
      octl->vfs = 5.f;
      break;
    case 0:
      octl->vfs = 2.5f;
      break;
    default:
      octl->vfs = 10.f;	
    }

  kernel_fpu_end();
#endif

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

  DEBUGMSG( "**daqInputTrigger\n" );

  uval  = ioread32( (iobase+BCR) );
  uval |= (1<<11);

  iowrite32( uval, (iobase+BCR) ); 
}

void daqOutputTrigger( void *iobase )
{
  unsigned int uval;

  DEBUGMSG( "**daqOutputTrigger\n" );

  uval  = ioread32( (iobase+BUFF_OUT_OPS) );
  uval |= (1<<20);

  iowrite32( uval, (iobase+BUFF_OUT_OPS) ); 
}

void daqInputOutputTrigger( void *iobase )
{
  unsigned int uval1, uval2;

  DEBUGMSG( "**daqInputOutputTrigger\n" );

  uval1  = ioread32( (iobase+BCR) );
  uval1 |= (1<<11) | (1<<23);

  uval2  = ioread32( (iobase+BUFF_OUT_OPS) );
  uval2 |= (1<<20);

  // this is as good as it gets for this card
  iowrite32( uval1, (iobase+BCR) ); 
  iowrite32( uval2, (iobase+BUFF_OUT_OPS) ); 
}
