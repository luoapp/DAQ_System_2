/* daqdrv 

  daqio.c

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

#ifndef _DAQIO_H_
#define _DAQIO_H_

#include "daqlib.h"

/* ===============================================================
   Note that the control settings have to be written to the cards 
   before calling any of these routines.
*/

int daqReadRawStart( DaqBoard *b, unsigned int *udata,  int ndata );

int daqReadRawStop( DaqBoard *b );

int daqReadWait( DaqBoard *b );

int daqReadRaw( DaqBoard *b, unsigned int *udata,  int ndata,
		int (*readyfunc)( void * ), void *readyarg );

// ===============================================================

int daqWriteRawStart( DaqBoard *b, unsigned int *udata,  int ndata );

int daqWriteRawStop( DaqBoard *b );

int daqWriteWait( DaqBoard *b );

int daqWriteRaw( DaqBoard *b, unsigned int *udata,  int ndata,
		 int (*readyfunc)( void * ), void *readyarg );

// ===============================================================

int daqWriteReadRaw( DaqBoard *b, unsigned int *uwrite, int nwrite,
		     unsigned int *uread, int nread,
		     int (*readyfunc)( void * ), void *readyarg );


#endif
