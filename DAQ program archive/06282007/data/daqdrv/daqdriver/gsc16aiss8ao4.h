/* daqdrv

   gsc daq card driver

   Dr. Mitchell C. Nelson

*/

#ifndef _GSC16AISS8AO4_H_
#define _GSC16AISS8AO4_H_

#include <asm/io.h>
#include <linux/sched.h>

#include "gsc16aiss8ao4regs.h"

void plxInterruptEnable( void *plxbase );
void plxInterruptDisable( void *plxbase );

void plxLocalInterruptEnable( void *plxbase );
void plxLocalInterruptDisable( void *plxbase );

void plxDmaInterruptEnable( void *plxbase );
void plxDmaInterruptDisable( void *plxbase );

void plxDma0InterruptEnable( void *plxbase );
void plxDma0InterruptDisable( void *plxbase );

void plxDma1InterruptEnable( void *plxbase );
void plxDma1InterruptDisable( void *plxbase );

void daqInputInterruptEnable( void *iobase );
void daqInputInterruptDisable( void *iobase );

void daqOutputInterruptEnable( void *iobase );
void daqOutputInterruptDisable( void *iobase );

void plxDma0Load( void *plxbase, dma_addr_t addr, unsigned int len );
void plxDma0Setup( void *plxbase );
void plxDma0Start( void *plxbase );
void plxDma0Stop( void *plxbase );

void plxDma1Load( void *plxbase, dma_addr_t addr, unsigned int len );
void plxDma1Setup( void *plxbase );
void plxDma1Start( void *plxbase );
void plxDma1Stop( void *plxbase );

int daqDma0Next( DAQDRV_Dev *dev );
int daqDma0Start(  DAQDRV_Dev *dev );
int daqDma0Cancel(  DAQDRV_Dev *dev );

int daqDma1Next( DAQDRV_Dev *dev );
int daqDma1Start(  DAQDRV_Dev *dev );
int daqDma1Cancel(  DAQDRV_Dev *dev );

int daqInterruptHandler( void *iobase, void *p );

int daqReadFifo_Pio( void *iobase, unsigned int *uaddr, int ndata );

int daqWriteFifo_Pio( void *iobase, unsigned int *uaddr, int ndata );

void daqReset( void *iobase );

void daqReadCtls( void *iobase, DAQDRV_Ctl *b );
void daqWriteCtls( void *iobase, DAQDRV_Ctl *b );
void daqInputTrigger( void *iobase );
void daqOutputTrigger( void *iobase );
void daqInputOutputTrigger( void *iobase );

#endif
