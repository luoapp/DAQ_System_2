/* daqdrv 

  daqiodblbuf.c

  Copyright 2006
  
    Dr. Mitchell C. Nelson
    
      NOTE:
        
*/

#ifndef _DAQIODBLBUF_H_
#define _DAQIODBLBUF_H_

int daqReadDblBufStop( DaqBoard *b );

int daqReadDblBufStart( DaqBoard *b, 
			int (*datafunc) ( void *, int, void * ), void *dataarg,
			int ndata_per_buffer );

int daqReadDblBuf( DaqBoard *b, 
		   int (*readyfunc)( void * ), void *readyarg,
		   int (*datafunc) ( void *, int, void * ), void *dataarg,
		   int ndata_per_buffer );

// =============================================================================

int daqWriteDblBufStop( DaqBoard *b );

int daqWriteDblBufStart( DaqBoard *b, 
			int (*datafunc) ( void *, int, void * ), void *dataarg,
			 int ndata_per_buffer );

int daqWriteDblBuf( DaqBoard *b, 
		   int (*readyfunc)( void * ), void *readyarg,
		   int (*datafunc) ( void *, int, void * ), void *dataarg,
		    int ndata_per_buffer );

// ==========================================================

int daqWriteDblBufReadDblBuf( DaqBoard *b, 
			      int (*readyfunc)( void * ), void *readyarg,
			      int (*outfunc) ( void *, int, void * ), void *outarg,
			      int (*infunc) ( void *, int, void * ), void *inarg,
			      int ndata_per_buffer );

// =============================================================================

int daqBigRead( DaqBoard *b, unsigned int *data, int ndata,
		int (*readyfunc)( void * ), void *readyarg );

int daqBigWrite( DaqBoard *b, unsigned int *data, int ndata,
		 int (*readyfunc)( void * ), void *readyarg );

int daqBigWriteBigRead( DaqBoard *b,
			unsigned int *out, int nout,
			unsigned int *in, int nin,
			int (*readyfunc)( void * ), void *readyarg );

// =============================================================================

int daqReadtoFile( DaqBoard *b, FILE *fd, char *comments, int ndatatotal,
		   int (*readyfunc)( void * ), void *readyarg );

int daqWriteReadtoFile( DaqBoard *b, unsigned int *uwrite, int nwrite,
			FILE *fd, char *comments, int ndatatotal,
			int (*readyfunc)( void * ), void *readyarg );

// =============================================================================

int daqReadtoTransposed( DaqBoard *b, double *data, int ndatatotal,
			 int (*readyfunc)( void * ), void *readyarg );

int daqWriteReadtoTransposed( DaqBoard *b, unsigned int *uwrite, int nwrite,
			      double *d, int ndatatotal,
			      int (*readyfunc)( void * ), void *readyarg );

#endif
