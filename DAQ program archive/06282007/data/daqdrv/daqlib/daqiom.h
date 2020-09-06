/* daqdrv 

daqiom.c

Copyright 2006
  
Dr. Mitchell C. Nelson
    
NOTE:
        
*/

#ifndef _DAQIOM_H_
#define _DAQIOM_H_

#include "daqdrv.h"
#include "daqlib.h"

extern DaqBoard daqboardlist[];
extern int daqboardcount;

extern int daqboardreading;
extern int daqboardwriting;



int daqOpenM( DAQDRV_Ctl *ctl );

void daqCloseM( );

int daqReadControlsM( DAQDRV_Ctl *ctl );

int daqWriteControlsM( DAQDRV_Ctl *ctl );

int daqInitM( DAQDRV_Ctl *ctl );

void daqDumpM( );

void daqDumpFormattedM( );

/* ===============================================================
   Note that the control settings have to be written to the cards 
   before calling any of the daq routines.
*/

void demuxdata( unsigned int *out, int outcols,
		unsigned int *in, int incols, int nrows );

void remuxdata( unsigned int *out, int outcols,
		unsigned int *in, int incols, int nrows );

// ---------------------------------------------------

int daqCountInputChannelsM( int nboards );

void daqFreeInputBuffersM( int nboards );

int daqMallocInputBuffersM( int nboards );

// ---------------------------------------------------

int daqCancelInputDmaM( int nboards );

int daqStartInputDmaM( int nboards );

int daqWaitInputM( int nboards );

// ---------------------------------------------------

int daqReadRawStartM( );

int daqReadRawStopM( );

int daqReadWaitM( unsigned int *buffer, int ndata );

int daqReadRawM( unsigned int *udata,  int ndata,
		 int (*readyfunc)( void * ), void *readyarg );

// ---------------------------------------------------

int daqCountOutputChannelsM( int nboards );

void daqFreeOutputBuffersM( int nboards );

int daqMallocOutputBuffersM( int nboards, int nrows );

// ---------------------------------------------------

int daqCancelOutputDmaM( int nboards );

int daqStartOutputDmaM( int nboards );

int daqWaitOutputM( int nboards );

// ---------------------------------------------------

int daqWriteRawStartM( unsigned int *buffer, int ndata );

int daqWriteRawStopM( );

int daqWriteWaitM( );

int daqWriteRawM( unsigned int *udata,  int ndata,
		  int (*readyfunc)( void * ), void *readyarg );

// ---------------------------------------------------

int daqWriteReadRawM( unsigned int *uwrite, int nwrite,
		      unsigned int *uread, int nread,
		      int (*readyfunc)( void * ), void *readyarg );

#endif
