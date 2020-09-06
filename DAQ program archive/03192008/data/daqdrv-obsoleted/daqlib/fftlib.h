/* fftlib.c

  Copyright 2006
  
    Dr. Mitchell C. Nelson
    
      NOTE:
        
*/

#ifndef FFTLIB_H_
#define FFTLIB_H_


// FFT working space

typedef struct fftarray {
  double *data;
  int ndata;
  int nrows;
  int ncols;
} FFTArray;

typedef struct fftipw {
  int *ip;
  double *w;
  int state;
} FFTIPW;


/* =================================================
   FFT working space
 */

int fftipwFree( FFTIPW *p );

int fftipwSetup( FFTIPW *p, int ndata );

void *fftFree( FFTArray *a );

double *fftMalloc( FFTArray *a, int nrows, int ncols );

/* =================================================
   FFT load from column data
 */

int fftLoadFloat( FFTArray *a, float *data, int ndata, int ncols );

int fftLoad( FFTArray *a, double *data, int ndata, int ncols );

/* =================================================
   Peak find and fit
 */

int ipeak( double *data, int ndata );

double fpeak( double *data, int ndata, int peakwidth, double dx, double *py0 );

/* =================================================
   FFT processing
 */

void parzenwindow( double *data, int ndata );
void parzenwindows( double *data, int ndata, int ncols );

double baseline( double *data, int ndata );
void baselines( double *data, int nrows, int ncols );

int fftbare( double *data, int ndata, FFTIPW *ipw );
int fftbares( double *data, int nrows, int ncols, FFTIPW *ipw );

int fftbareinverse( double *data, int ndata, FFTIPW *ipw );
int fftbareinverses( double *data, int nrows, int ncols, FFTIPW *ipw );

int fftvector( double *data, int ndata, FFTIPW *ipw );
int fftvectors( double *data, int nrows, int ncols, FFTIPW *ipw );

/* =================================================
   PSD processing
 */

int psdfft( double *psd, double *fftdata, int ndata );

int psdvector( double *psd, double *data, int ndata, FFTIPW *ipw );

double psdpeak( double *psd, int ndata, int peakwidth, double dx, double *py0 );

double fftpeak( double *scratch, double *fftdata, int ndata, int peakwidth, double dx, double *py0 );

// ============================================================

int fftcorrelation( double *corr, FFTIPW *ipw, double *ffta, double *fftb, int ndata );

double correlationpeak( double *correlation, int ndata, int peakwidth, double dx, double *py0 );

// ============================================================

int printfftpeaks( FILE *fd, 
		   double *scratch, double *fftdata, int nrows, int ncols, double samplerate );

int printfftlags( FILE *fd, 
		  double *scratch, FFTIPW *ipw, 
		  double *fftdata, int nrows, int ncols, double samplerate );

int printfftpeaksandlags( FILE *fd, 
			  double *scratch, FFTIPW *ipw, double *fftdata, int nrows, int ncols,
			  double samplerate );

int printffts( FILE *fd, double *fftdata, int nrows, int ncols, double samplerate );

int printfftpsds( FILE *fd, double *fftdata, int nrows, int ncols, double samplerate );

// ================================================

#endif
