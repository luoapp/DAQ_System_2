/*
  echoburst.c

  Sample code for libdaq and daqdrv driver
  Copyright 2006, Mitchell C. Nelson, Ph.D.

  All rights reserved.

  Note:

  For this routine, the first digital I/O line should be connect to the
  trigger input, or a true external trigger should be supplied.

  The output waveforms can be read by the inputs, by connecting the outputs
  to the input hi for each configured channe.


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

int main( )
{
  DaqBoard board = { { 0 } };
  unsigned int *inbuffer = NULL;
  unsigned int *outbuffer = NULL;
  int retv = 0;

  float outrate      = 500.e3;
  float outvfs       = 5.f;
  int outsamples     = 10000;
  int outchans       = 2;
  int outdata        = outsamples * outchans;

  float inrate       = 500.e3;
  float invfs        = 5.f;
  int insamples      = outsamples + 0.0 * inrate; // listen for another 1/2 second
  int inchans        = 2;
  int indata         = insamples * inchans;

  float freq = 0.01f;
  float ampl = 1.f;

  retv = daqOpen( &board, 0 );
  if ( retv ) { perror( "daqOpen"); exit( 1 ); }

  outchans = board.ctl.cfg.outchans;

  inchans = board.ctl.cfg.inchans;
  indata  = insamples * inchans;

  // set output controls
  board.ctl.octl.ndiv	= gscClockDivider( board.ctl.cfg.clock, outrate );
  board.ctl.octl.clkmaster  = 1;
  board.ctl.octl.nrange	= gscRange( outvfs );
  board.ctl.octl.nchans	= outchans;

  // set input controls
  board.ctl.ictl.ndiv	= gscClockDivider( board.ctl.cfg.clock, inrate );
  board.ctl.ictl.clkmaster  = 1;
  board.ctl.ictl.nmode	= 1;
  board.ctl.ictl.nrange	= gscRange( invfs );
  board.ctl.ictl.nchans	= inchans;
  board.ctl.ictl.nburst	= insamples;

  /* 0 = external software trigger (toggles the first dio line,
     the first dio line should be connected to the trigger input)
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

  // Start the DMA,  trigger the output and wait for completion
  retv = daqWriteReadRaw( &board, outbuffer, outdata,
			  inbuffer, indata,
			  (int(*)(void*))daqTrigger, &board );
	
  if ( retv ) { perror("daqWriteReadRaw"); exit( 1 ); }

  // dump the data as real voltage values 
  printRawFlt( stdout, inbuffer, board.ctl.ictl.ndata, board.ctl.ictl.nchans,
	       board.ctl.ictl.vfs );

  free( outbuffer );
  free( inbuffer );
  exit( 0 );
}
