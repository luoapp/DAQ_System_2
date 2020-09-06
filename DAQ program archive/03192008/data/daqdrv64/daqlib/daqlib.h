/* daqdrv 

  daqlib.c

   Copyright 2006, Mitchell C. Nelson, Ph.D.

   This software is confidential and proprietary. This software is provided
   only subject to specific written agreements. No rights whatsoever are
   hereby provided.

   No part of this software may be copied, stored, reproduced or transmitted in
   any form or by any means without the written permission of the author.

   No warranty or representation is provided of suitability for any purpose
   whatsoever.


      NOTE:
        
*/

#ifndef _DAQLIB_H_
#define _DAQLIB_H_

#include <semaphore.h>
#include <signal.h>

#include "gsc16aiss8ao4regs.h"
#include "gsc16aiss16ao2regs.h"
#include "daqdrv.h"

typedef struct daqbuff {
  unsigned int *p;
  int  n;
  void *next;
} DaqBuff;

typedef struct daqring {
  DaqBuff *b0;
  DaqBuff *b1;
  int count;
} DaqRing;

typedef struct daqboard {

  DAQDRV_Status  status;

  DAQDRV_Ctl     ctl;

  sem_t          isem;
  int            iwait;
  unsigned int   istatus;
  unsigned int   icounter;
  unsigned int   ioverflow;

  DaqBuff        *ibufferlist;
  DaqBuff        *ibuffertail;
  int            ibuffers;

  sem_t          osem;
  int            owait;
  unsigned int   ostatus;
  unsigned int   ocounter;
  unsigned int   ooverflow;

  DaqBuff        *obufferlist;
  DaqBuff        *obuffertail;
  int            obuffers;

  // for ring buffering
  DaqBuff        *iring0;
  DaqBuff        *iring1;

  DaqBuff        *oring0;
  DaqBuff        *oring1;

  int fd;

  int           (*ifunc)( void * );
  void           *ifuncarg;

  int           (*ofunc)( void * );
  void           *ofuncarg;

} DaqBoard;

#define DAQDONE 1
#define DAQOVERFLOW 2

// ====================================================================

int daqTestSigIn( int fd );

int daqTestSigOut( int fd );

int daqTestSigMsg( int fd );

int daqTestSigErr( int fd );

int daqTestSigPri( int fd );

int daqTestDma( int fd );

int daqTestInputBuffer( int fd );

int daqTestOutputBuffer( int fd );

// ====================================================================

int daqReadStatus( DaqBoard *b );

int daqReadInputSize( DaqBoard *b );

int daqReadOutputSize( DaqBoard *b );

// ====================================================================

int daqReadControls( DaqBoard *b );

int daqWriteControls( DaqBoard *b );

// ====================================================================

int daqAddInputBuffer_( int fd, unsigned int *data, int ndata );

int daqAddInputBuffer( DaqBoard *b, unsigned int *data, int ndata );

// ====================================================================

int daqReleaseInputBuffers_( int fd );

int daqReleaseInputBuffers( DaqBoard *b );

// ====================================================================

unsigned int *daqMallocInputBuffer( DaqBoard *b, int ndata );

int daqFreeInputBuffers( DaqBoard *b );

// ====================================================================

int daqStartInputDma_( int fd );

int daqStartInputDma( DaqBoard *b );

// ====================================================================

int daqCancelInputDma_( int fd );

int daqCancelInputDma( DaqBoard *b );

// ====================================================================

int daqWaitInput( DaqBoard *b );

void *daqWaitInputBuffer(  DaqBoard *b );

// ====================================================================

int daqAddOutputBuffer_( int fd, unsigned int *data, int ndata );

int daqAddOutputBuffer( DaqBoard *b, unsigned int *data, int ndata );

// ====================================================================

int daqReleaseOutputBuffers_( int fd );

int daqReleaseOutputBuffers( DaqBoard *b );

// ====================================================================

unsigned int *daqMallocOutputBuffer( DaqBoard *b, int ndata );

int daqFreeOutputBuffers( DaqBoard *b );

// ====================================================================

int daqStartOutputDma_( int fd );

int daqStartOutputDma( DaqBoard *b );

// ====================================================================

int daqCancelOutputDma_( int fd );

int daqCancelOutputDma( DaqBoard *b );

// ====================================================================

int daqWaitOutput( DaqBoard *b );

void *daqWaitOutputBuffer(  DaqBoard *b );

// ====================================================================

int daqTriggerInput_( int fd );

int daqTriggerOutput_( int fd );

int daqTriggerInputOutput_( int fd );

int daqTrigger( DaqBoard *b );

// ====================================================================

int daqStopInput( DaqBoard *b );

int daqStopOutput( DaqBoard *b );

int daqStop( DaqBoard *b );

// ====================================================================

void daqFreeInputHandler( DaqBoard *b );

int daqSetInputHandler( DaqBoard *b, 
			 int (*func) ( void *, int, void * ),
			void *arg );

// ====================================================================

void daqFreeOutputHandler( DaqBoard *b );

int daqSetOutputHandler( DaqBoard *b, 
			 int (*func) ( void *, int, void * ),
			void *arg );

// ====================================================================

int daqInit( DaqBoard *b );

// ====================================================================
// void daqsighandler( int signum, siginfo_t *info, void *context );

int daqStatusHandler_( DaqBoard *b );

void daqSigHandler( int signum, siginfo_t *info, void *context );

int daqEnableAsynch( DaqBoard *b, void (*handler)(int, siginfo_t *, void *) );

// ====================================================================

int daqClearUser( DaqBoard *b );

int daqSetUser( DaqBoard *b );

DaqBoard *daqGetUser( int fd );

// ====================================================================

void daqClose( DaqBoard *b );

int daqOpen( DaqBoard *b, int n );

#endif
