/* daqutil

   Copyright 2006, Mitchell C. Nelson, Ph.D.

   This software is confidential and proprietary. This software is provided
   only subject to specific written agreements. No rights whatsoever are
   hereby provided.

   No part of this software may be copied, stored, reproduced or transmitted in
   any form or by any means without the written permission of the author.

   No warranty or representation is provided of suitability for any purpose
   whatsoever.

  NOTE:
  
    For now, we are going to pretend/assume that this will only be
    accessed from one thread at a time ( fd,i.e. not reentrant).

*/

#ifndef _DAQUTIL_H_
#define _DAQUTIL_H_

#include "daqdrv.h"

/* Memory Allocation
*/

typedef struct daqdatabuffer {
  unsigned int *data;
  int  size;
} DaqDataBuffer;

void *daqMalloc( void *p, unsigned int nsize );

void *daqMallocNoFill( void *p, unsigned int nsize );

void *daqMallocAligned( void *p, unsigned int nsize );

unsigned int *daqDataBufferFree( unsigned int **current_buffer, int *current_ndata );

unsigned int *daqDataBufferMalloc( unsigned int *p, int *current_ndata, int new_ndata );

unsigned int *daqDataBufferMallocAligned( unsigned int *p, int *current_ndata, int new_ndata );

unsigned int *daqFifoBufferFree( DAQDRV_Fifo *db );

unsigned int *daqFifoBufferMalloc( DAQDRV_Fifo *db, int new_ndata );

unsigned int *daqFifoBufferMallocAligned( DAQDRV_Fifo *db, int new_ndata );

int daqCountBits( unsigned int u );

int daqLastNonZeroF( float *data, int ndata );

float daqMaxAbs( float *data, int ndata );

#endif
