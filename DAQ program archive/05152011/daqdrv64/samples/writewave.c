/*
  writewave.c

  Sample code for libdaq and daqdrv driver
  Copyright 2006, Mitchell C. Nelson, Ph.D.

  All rights reserved.

*/


#include "daqutil.h"
#include "daqio.h"
#include "daqprint.h"
#include "gsclib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
	  waveforms[i] = a * sin( (f*n + ((f*m)/nchans)) );
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

int main( )
{
  DaqBoard board = { { 0 } };
  unsigned int *buffer = NULL;
  int retv = 0;

  float samplerate = 100.e3;
  float vfs        = 5.f;
  int nsamples     = 200000;
  int nchans       = 2;
  int ndata        = nsamples * nchans;

  float freq = 0.1f;
  float ampl = 1.f;

  retv = daqOpen( &board, 0 );
  if ( retv ) { perror( "daqOpen"); exit( 1 ); }

  // set output rate, internal clock
  board.ctl.octl.ndiv	= gscClockDivider( board.ctl.cfg.clock, samplerate );
  board.ctl.octl.clkmaster  = 1;

  // set  range = +/- vfs
  board.ctl.octl.nrange	= gscRange( vfs );

  // set number of channels,
  board.ctl.octl.nchans	= nchans;

  // internal software trigger
  board.ctl.trigmaster	= 1;

  // load the controls
  retv = daqWriteControls( &board );
  if ( retv ) { perror( "daqWriteControls"); exit( 1 ); }

  // allocate a data buffer (alignment is optional)
  buffer = daqMallocAligned( buffer, ndata*sizeof(unsigned int) );
  if ( !buffer ) { printf( "malloc failed\n");  exit( 1 ); }

  // generate a test pattern
  if ( !genwaveforms( buffer, freq, ampl, vfs, nsamples, nchans ) )
    {
      printf( "unable to create the waveforms\n" );
      exit( 1 );
    }  

  // Start the DMA,  trigger the output and wait for completion
  retv = daqWriteRaw( &board, buffer, ndata,
		     (int(*)(void*))daqTrigger, &board );
	
  if ( retv ) { perror("daqWriteRaw"); exit( 1 ); }


  free( buffer );
  exit( 0 );
}
