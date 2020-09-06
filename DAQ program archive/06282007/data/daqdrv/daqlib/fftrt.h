/* fftrt.h
*/

#ifndef _FFTRT_H_
#define _FFTRT_H_

typedef struct fftpars {
  double *window;
  int    n;
  int    *ip;
  double *w;
} FFTRT;

void fftRtFree( FFTRT *f );

double *fftRtInitialize( FFTRT *f, int nwindow );

double *fftRtRaw( FFTRT *f, unsigned int *udata, int ndata, int ncols, int nchan, float vfs );

double fftRtPowerFreq( FFTRT *fft, float f );

void fftRtDump( FFTRT *fft );

#endif
