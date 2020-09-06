/* daqdrv 

daqthreshm.h

Copyright 2006
  
Dr. Mitchell C. Nelson
    
NOTE:
        
*/

#ifndef _DAQTHRESHM_H_
#define _DAQTHRESHM_H_

#include "fftrt.h"

/* Canned trigger functions
 */

typedef struct daqtresholdlevelargm {
  int   channel;
  unsigned int level;
  int   direction;
} DaqThresholdLevelArgM;

typedef struct daqtresholdtoneargm {
  int   channel;
  float level;
  float f;
  FFTRT fft;
  float vfs;
} DaqThresholdToneArgM;

int daqReadThresholdStopM_( int nboards );

int daqReadThresholdStopM( );

int daqReadThresholdStartM( int nlatent, int npretrig );

inline
int daqMuxBuffer_( unsigned int *out, int nout, int nchans, unsigned int *in, int nin, int inchans );

int daqMuxRing_( unsigned int *udata, int ndata, int nchans, DaqBuff *b0, DaqBuff *b1, int inchans );

int daqCopyInputRingBufferM( unsigned int *udata, int ndata, int nchans );

int daqCopyInputRingM( unsigned int *udata, int ndata, int nchans );

int daqWaitInputRingBufferM( );

int daqThresholdM( int (*thresholdfuncM)( void *, int, int, int, void * ), void *thresholdarg );

int daqReadThresholdWaitM( unsigned int *udata, int ndata, int nchans,
			   int (*thresholdfuncM)( void *, int, int, int, void * ),
			   void *thresholdargM );

int daqReadThresholdM( unsigned int *udata,  int ndata,
		       int (*readyfunc)( void * ), void *readyarg,
		       int (*thresholdfuncM)( void *, int, int, int, void * ),
		       void *thresholdargM, int nlatent, int npretrig );

/* Threshold level function
 */

int daqThresholdLevelMf( unsigned int *data, int ndata, int nchan0, int nchannels,
			 DaqThresholdLevelArgM *a );

int daqReadThresholdLevelM( unsigned int *udata,  int ndata,
			    int (*readyfunc)( void * ), void *readyarg,
			    int channel, float level, int direction, int nlatency, int npretrig );

/* Edge level function
 */

int daqThresholdEdgeMf( unsigned int *data, int ndata, int nchan0, int nchannels,
			DaqThresholdLevelArgM *a );

int daqReadThresholdEdgeM( unsigned int *udata,  int ndata,
			   int (*readyfunc)( void * ), void *readyarg,
			   int channel, float level, int direction, int nlatency, int npretrig );

/* Tone level function
 */

int daqThresholdToneMf( unsigned int *data, int ndata, int nchan0, int nchannels, 
			DaqThresholdToneArgM *a );

int daqReadThresholdToneM( unsigned int *udata,  int ndata,
			   int (*readyfunc)( void * ), void *readyarg,
			   int channel, float level, float f, int nlatency, int npretrig );

#endif
