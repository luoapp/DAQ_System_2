/* daqdrv 

  daqio.c

  Copyright 2006
  
    Dr. Mitchell C. Nelson
    
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
