/* daqdrv

  gsclib

   Copyright 2006, Mitchell C. Nelson, Ph.D.

   This software is confidential and proprietary. This software is provided
   only subject to specific written agreements. No rights whatsoever are
   hereby provided.

   No part of this software may be copied, stored, reproduced or transmitted in
   any form or by any means without the written permission of the author.

   No warranty or representation is provided of suitability for any purpose
   whatsoever.

  NOTE:
  
    For now, we are going to pretend/assume that this will only be
    accessed from one thread at a time (i.e. not reentrant).

*/

#ifndef _GSCLIB_H_
#define _GSCLIB_H_

#define BOARDCLOCKDEFAULT (40.320E6)

#define RAWTOSHORT(a) ((short) (0xffff & (a)))
#define RAWTOFLOAT(a,vfs) ( (vfs / 32768.F) * RAWTOSHORT(a) )


/* Clock values
 */

float gscClockRate( float clockHz, int ndiv );

int gscClockDivider( float clockHz, float samplerate );

/* Voltage Scale translations
*/

int gscRange( float vfs );

float gscVfs( int range );

int countbits( unsigned int uval );

// -----------------------------------------------------

int gscInit_( int fd );

// -----------------------------------------------------

int gscOpen_(int nbrd );

void gscClose_( int fd );

// -----------------------------------------------------

int gscDioReset_( int fd );

/* byte 0 or 1;
 */

int gscDioSet_( int fd, int byte, unsigned int val );

int gscDioClear_( int fd, int byte, unsigned int val );

int gscDioRead_( int fd, unsigned int *uval );

int gscDioToggleHi_( int fd, int byte, unsigned int uval, int msec );

int gscDioToggleLO_( int fd, int byte, unsigned int uval, int msec );

// -----------------------------------------------------

int gscSwInputTrigger_( int fd );

int gscHwTrigger_( int fd );

int gscSwOutputTrigger_( int fd );

// -----------------------------------------------------

int gscSetCircularOutput_( int fd );

// -----------------------------------------------------

int gscChkOverflow( int fd );

int gscChkRaw( unsigned int *raw, int ndata, int ncols );

// -----------------------------------------------------

inline
unsigned int raw_float( float r, float vfs );

inline
float float_raw( unsigned int u, float vfs );

inline
double double_raw( unsigned int u, double vfs );

inline
float float_ushort( unsigned short u, float vfs );

inline
unsigned int uint_raw( unsigned int u );

inline
short short_raw( unsigned int u );

// -----------------------------------------------------

int gscRawChanneltoDouble( double *out, int nout, unsigned int *raw, int nraw, 
			   int nchannels, int nchan, float vfs );

void gscRawtoFloat( float *f, unsigned int *u, int ndata, float vfs );

void gscRawtoFloatTranspose( float *f, unsigned int *u, int ndata, int ncols, float vfs );

void gscRawtoDoubleTranspose( double *d, unsigned int *u, int ndata, int ncols, float vfs );


void gscUshorttoFloat( float *f, unsigned short *u, int ndata, float vfs );

// -----------------------------------------------------

void gscMarkEndofFrame( unsigned int *udata, int ndata );

void gscFloattoRaw( unsigned int *u, float *f, int ndata, float vfs );

void gscFloattoRawTranspose( unsigned int *u, float *f, int ndata, int ncols, float vfs );

// -----------------------------------------------------

int gscDumpRegisters( unsigned int fd );

#endif
