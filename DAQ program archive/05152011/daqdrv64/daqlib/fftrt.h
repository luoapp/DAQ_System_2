/* fftrt.h

   Copyright 2007, Mitchell C. Nelson, Ph.D.

   This software is confidential and proprietary. This software is provided
   only subject to specific written agreements. No rights whatsoever are
   hereby provided.

   No part of this software may be copied, stored, reproduced or transmitted in
   any form or by any means without the written permission of the author.

   No warranty or representation is provided of suitability for any purpose
   whatsoever.

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
