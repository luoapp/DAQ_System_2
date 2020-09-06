/*
  echoburst.c

  Sample code for libdaq and daqdrv driver
  Copyright 2006, Mitchell C. Nelson, Ph.D.

  All rights reserved.

  Note:

  For this routine, the first digital I/O line should be connected to
  the trigger input, or a true external trigger should be supplied.

  The output waveforms can be read by the inputs, by connecting an
  output to the input hi of any of the configured channels, and also,
  the output return to the input return.

*/


#include "daqutil.h"
#include "daqio.h"
#include "daqprint.h"
#include "gsclib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int genwaveforms( unsigned int *raw, float f, float a, float vfs, int nsamples, int nchans )
{
  float *waveforms = NULL;
  int ndata = nsamples * nchans;
  int n, m, i;

  waveforms = malloc( ndata*sizeof( float ) );
  if ( !waveforms ) return( 0 );

  // leave room at the end to set the outputs to zero
  for( n = 0, i = 0; n < nsamples-1; n++ )
    {
      for( m = 0; m < nchans; m++, i++ )
	{
	  /* Frequency f is scaled to the sample clock
	     Each channel differs by a phase shift
	  */
	  waveforms[i] = a * sin( 2.f * M_PI * (f*n + ((f*m)/nchans)) );
	}
    }

  // leave the outputs at 0.
  for( m = 0; m < nchans; m++, i++ )
    {
      waveforms[i] = 0.f;
    }

  gscFloattoRaw( raw, waveforms, ndata, vfs );

  free( waveforms );

  return( ndata );
}

float averagecolumn( unsigned int *raw, int ndata, int ncols, float vfs )
{
  int n = 0;
  float avg = 0.f;

  while( n < ndata )
    {
      avg += float_raw( raw[n], vfs );
      n += ncols;
    }
  avg = avg/n;

  return( avg );
}

float pkpkcolumn( unsigned int *raw, int ndata, int ncols, float vfs )
{
  int n = 0;
  float rmax;
  float rmin;
  float r;

  rmax = float_raw( raw[0], vfs );
  rmin = float_raw( raw[0], vfs );

  n += ncols;
  while( n < ndata )
    {
      r = float_raw( raw[n], vfs );

      if ( r > rmax ) rmax = r;
      else if ( r < rmin ) rmin = r;

      n += ncols;
    }

  return( (rmax-rmin) );
}

int main( )
{
  DaqBoard board = { { 0 } };
  unsigned int *inbuffer = NULL;
  unsigned int *outbuffer = NULL;
  int retv = 0;

  float outrate      = 100.e3;
  float outvfs       = 5.f;
  int outsamples     = 500000;
  int outchans;
  int outdata;

  float inrate       = 100.e3;
  float invfs        = 5.f;
  int insamples      = 300000;
  int inchans;
  int indata;

  float freq = 0.01f;
  float ampl = 1.f;

  int knt = 1000;
  int m = 0;

  time_t t;
  time_t t0;

  int n;
  float avg;

  retv = daqOpen( &board, 0 );
  if ( retv ) { perror( "daqOpen"); exit( 1 ); }

  // set output controls
  board.ctl.octl.ndiv	    = gscClockDivider( board.ctl.cfg.clock, outrate );
  board.ctl.octl.clkmaster  = 1;
  board.ctl.octl.nrange	    = gscRange( outvfs );
  board.ctl.octl.nchans	    = board.ctl.cfg.outchans;

  outchans = board.ctl.octl.nchans;
  outdata  = outchans * outsamples;

  // set input controls
  board.ctl.ictl.ndiv	    = gscClockDivider( board.ctl.cfg.clock, inrate );
  board.ctl.ictl.clkmaster  = 1;
  board.ctl.ictl.nmode	    = 1;
  board.ctl.ictl.nrange	    = gscRange( invfs );
  board.ctl.ictl.nchans	    = board.ctl.cfg.inchans;
  board.ctl.ictl.nburst	    = insamples;

  inchans = board.ctl.ictl.nchans;
  indata  = inchans * insamples;

  /* internal software trigger
  */
  board.ctl.trigmaster	= 1;

  // load the controls
  retv = daqWriteControls( &board );
  if ( retv ) { perror( "daqWriteControls"); exit( 1 ); }

  // allocate an output data buffer (alignment is optional)
  outbuffer = daqMallocAligned( outbuffer, outdata*sizeof(unsigned int) );
  if ( !outbuffer ) { printf( "malloc outbuffer failed\n");  exit( 1 ); }

  // generate a test pattern
  if ( !genwaveforms( outbuffer, freq, ampl, outvfs, outsamples, outchans ) )
    {
      printf( "unable to create the waveforms\n" );
      exit( 1 );
    }  

  // allocate an input data buffer (alignment is optional)
  inbuffer = daqMallocAligned( inbuffer, indata*sizeof(unsigned int) );
  if ( !inbuffer ) { printf( "malloc inbuffer failed\n");  exit( 1 ); }

  // first, check overhead
  printf( "overhead measurement, expect %g seconds per cycle\n", ((float)insamples/inrate) );

  t0 = time( NULL );
  for( m = 0; m < 10; m++ )
    {
      printf( "starting daq cycle %d/%d\n", m, 10 );


      // Start the DMA,  trigger the output and wait for completion
      retv = daqWriteReadRaw( &board, outbuffer, outdata,
			      inbuffer, indata,
			      (int(*)(void*))daqTrigger, &board );
	
      if ( retv ) { perror("daqWriteReadRaw"); exit( 1 ); }
    }
  t = time(NULL);

  printf( "elapsed time %g seconds\n", difftime( t, t0 ) );

  for( m = 0; m < knt; m++ )
    {
      printf( "starting daq cycle %d/%d\n", m, knt );

      // Start the DMA,  trigger the output and wait for completion
      retv = daqWriteReadRaw( &board, outbuffer, outdata,
			      inbuffer, indata,
			      (int(*)(void*))daqTrigger, &board );
	
      if ( retv ) { perror("daqWriteReadRaw"); exit( 1 ); }

      printf( "average each channel:\n" );

      // dump the data as real voltage values 
      for( n = 0; n < inchans; n++ )
	{
	  avg = averagecolumn( &inbuffer[n], (indata-n), inchans, invfs );
	  printf( " %.4g", avg );
	}
      printf( "\n" );

      printf( "pk-pk each channel:\n" );

      // dump the data as real voltage values 
      for( n = 0; n < inchans; n++ )
	{
	  avg = pkpkcolumn( &inbuffer[n], (indata-n), inchans, invfs );
	  printf( " %.4g", avg );
	}
      printf( "\n" );
    }

  free( outbuffer );
  free( inbuffer );
  exit( 0 );
}
