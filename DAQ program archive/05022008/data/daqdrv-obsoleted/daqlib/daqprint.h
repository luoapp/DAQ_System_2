/* daqutil

  Copyright 2006

  Dr. Mitchell C. Nelson

  NOTE:
  
    For now, we are going to pretend/assume that this will only be
    accessed from one thread at a time (i.e. not reentrant).

*/

#ifndef _DAQPRINT_H_
#define _DAQPRINT_H_

#include <stdio.h>

int printerror( char *fmt, ... );

int printRawFlt( FILE *fout, unsigned int *data, int ndata, int ncols, float vfs );

int printRawInt( FILE *fout, unsigned int *data, int ndata, int ncols );

int printRawUInt( FILE *fout, unsigned int *data, int ndata, int ncols );

int printRawHex( FILE *fout, unsigned int *data, int ndata, int ncols );

int printTransposeFlt( FILE *fout, float *data, int ndata, int ncols );

int printArrayFlt( FILE *fout, float *data, int ndata, int ncols );

int printArrayHex( FILE *fout, unsigned int *data, int ndata, int ncols );

#endif