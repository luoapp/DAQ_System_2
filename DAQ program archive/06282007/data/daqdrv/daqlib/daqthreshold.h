/* daqdrv 

daqthreshold.h

Copyright 2006
  
Dr. Mitchell C. Nelson
    
NOTE:
        
*/

#ifndef _DAQTHRESHOLD_H_
#define _DAQTHRESHOLD_H_

#include "daqio.h"

#include "fftrt.h"

int daqMallocInputRingBuffer( DaqBoard *b, int nlatent, int npretrig );

int daqReadThresholdStart( DaqBoard *b, int nlatent, int npretrig );

int daqReadThresholdStop( DaqBoard *b );

DaqBuff *daqWaitInputRingBuffer( DaqBoard *b );

int daqReadThresholdWait( DaqBoard *b, unsigned int *udata, int ndata,
			  int (*thresholdfunc)( void *, int, void * ),
			  void *thresholdarg );

int daqReadThreshold( DaqBoard *b, unsigned int *udata,  int ndata,
		      int (*readyfunc)( void * ), void *readyarg,
		      int (*thresholdfunc)( void *, int, void * ),
		      void *thresholdarg, int nlatent, int npretrig );


/* Canned trigger functions
 */

typedef struct daqtresholdlevelarg {
  int   channel;
  int   nchannels;
  unsigned int level;
  int   direction;
} DaqThresholdLevelArg;

typedef struct daqtresholdtonearg {
  int   channel;
  int   nchannels;
  float f;
  float level;
  FFTRT fft;
  float vfs;
} DaqThresholdToneArg;

inline
int abovelevel_( unsigned int *data, int ndata, int nchannels, unsigned int level, int *state );

inline
int belowlevel_( unsigned int *data, int ndata, int nchannels, unsigned int level, int *state );

inline
int toneabovelevel_( unsigned int *data, int ndata, int nchannels,
		     int nchan, float f, float level, float vfs, FFTRT *fft );

int daqThresholdLevelf( unsigned int *data, int ndata, DaqThresholdLevelArg *a );

int daqThresholdEdgef( unsigned int *data, int ndata, DaqThresholdLevelArg *a );

int daqThresholdTonef( unsigned int *data, int ndata, DaqThresholdToneArg *a );

int daqReadThresholdLevel( DaqBoard *b, unsigned int *udata,  int ndata,
			   int (*readyfunc)( void * ), void *readyarg,
			   int channel, float level, int direction, int nlatency, int npretrig );

int daqReadThresholdEdge( DaqBoard *b, unsigned int *udata,  int ndata,
			  int (*readyfunc)( void * ), void *readyarg,
			  int channel, float level, int direction, int nlatency, int npretrig );

int daqReadThresholdTone( DaqBoard *b, unsigned int *udata,  int ndata,
			  int (*readyfunc)( void * ), void *readyarg,
			  int channel, float level, float f, int nlatency, int npretrig );

#endif
