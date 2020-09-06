/* quadraticlsq.c

   Copyright 2005
   Dr. Mitchell C. Nelson

*/

#ifndef QUADRATICLSQ_H_
#define QUADRATICLSQ_H_

double quadraticlsq( double *y, double *x, int ndata,
		     double *pa, double *pb, double *pc );

double quadraticlsq_int( double *y, int ndata, double *pa, double *pb, double *pc );

double peakfit( double *y, double *x, int ndata, double *py0, double *px0 );

double peakfit_int( double *y, int ndata, double *py0, double *px0 );

#endif
