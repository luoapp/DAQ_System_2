/* daqwaveforms

  Copyright 2006

  Dr. Mitchell C. Nelson

*/


#ifndef DAQWAVEFORMS_H_
#define DAQWAVEFORMS_H_

#include <stdio.h>

#include "daqdrv.h"
#include "daqfile.h"

typedef float (*ValueFunc)(float, float, float, float, int);
typedef int  (*OperatorFunc)(float *, int, int, float, float, float, ValueFunc);

typedef struct wavepars {
  ValueFunc func;
  float a;
  float f;
  float p0;
  float offset;
  float duty;
  float cycles;
  char  name[16];
} WAVEPARS;

typedef struct wavespec {
  struct wavepars carrier;
  struct wavepars modulation;
} WAVESPEC;

typedef struct wavedatastruct {
  float *data;
  int ndata;
  int ncols;
  int nrows;
  float clockrate;
  float vfs;
} WAVEDATA;

// =====================================================================

float decayvalue( float a, float f, float p0, float fdummy, int n );

float pulsevalue( float a, float f, float p0, float fduty, int n );

float sinevalue( float a, float f, float p0, float fdummy, int n );

float squarevalue( float a, float f, float p0, float fduty, int n );

int channelspec( char *s, int *chan );

// =====================================================================

void clearwavespecs( WAVESPEC *spec, int maxchans );

int parsewavepars( WAVEPARS *spec, char *s );

int parsewavespec( WAVESPEC *spec, int maxchans, char *s );

int sprintwavespecs( char *s, WAVESPEC *spec, int maxchans );

int printwavespecs( FILE *fd, WAVESPEC *spec, int maxchans );

void printwavehelp( );

// =====================================================================

int wavedataCountChannels_( WAVESPEC *spec, int maxchans );

int wavedataMaxRows_( WAVESPEC *spec, int maxchans, float clockrate );

int wavedataFillChannels_( WAVESPEC *spec, int maxchans );

int wavedataMalloc_( WAVEDATA *w, WAVESPEC *spec, int maxchans, float clockrate );

int wavedataPopulate_( WAVEDATA *w, WAVESPEC *spec );

int wavedataGenerate( WAVEDATA *w, WAVESPEC *spec, int maxchans, float clockrate );

void wavedataFree( WAVEDATA *w );

// =====================================================================

int daqWriteWaveFrame( FILE *fd, WAVEDATA *w );

int daqReadWaveHeader( FILE *fd, WAVEDATA *w );

float *daqReadWaveFrame( FILE *fd, WAVEDATA *w, DaqAioCtl *octl );


#endif
