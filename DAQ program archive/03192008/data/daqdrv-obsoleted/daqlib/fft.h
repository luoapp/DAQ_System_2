/* fft.h
*/

#ifndef _FFT_H_
#define _FFT_H_

typedef struct fftpars {
  double *window;
  int    n;
  int    *ip;
  double *w;
} FFT;

void fftFree( FFT *f );

double *fftInitialize( FFT *f, int nwindow );

double *fftRaw( FFT *f, unsigned int *udata, int ndata, int ncols, int nchan, float vfs );

double fftPowerFreq( FFT *fft, float f );

void fftDump( FFT *fft );

#endif
