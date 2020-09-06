/*
  readburst.c

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

int main( )
{
  DaqBoard board = { { 0 } };
  unsigned int *buffer = NULL;

  float rsum = 0.f;
  int n;

  int retv = 0;


  retv = daqOpen( &board, 0 );
  if ( retv ) { perror( "daqOpen"); exit( 1 ); }

  // set input sampling rate to 100 kHz , internal clock
  board.ctl.ictl.ndiv	= gscClockDivider( board.ctl.cfg.clock, 100.E3 );
  board.ctl.ictl.clkmaster  = 1;

  // set single ended input, range +/- 5 Volts
  board.ctl.ictl.nmode	= 1;
  board.ctl.ictl.nrange	= gscRange( 5.f );	// VFS = 5 volts

  // set 8 input channels, burst length 100,000 samples
  board.ctl.ictl.nchans	= 8;
  board.ctl.ictl.nburst	= 100000;

  // internal software trigger
  board.ctl.trigmaster	= 1;

  // load the controls
  retv = daqWriteControls( &board );
  if ( retv ) { perror( "daqWriteControls"); exit( 1 ); }

  // allocate a data buffer (alignment is optional)
  buffer = daqMallocAligned( buffer,  board.ctl.ictl.ndata*sizeof(unsigned int) );
  if ( !buffer ) { printf( "malloc failed\n");  exit( 1 ); }

  // Start the DMA,  trigger the input burst and wait for completion
  retv = daqReadRaw( &board, buffer, board.ctl.ictl.ndata,
		     (int(*)(void*))daqTrigger, &board );
	
  if ( retv ) { perror("daqReadRaw"); exit( 1 ); }

  // dump the data as real voltage values 
  for ( n = 0; n < board.ctl.ictl.ndata; n++ )
    rsum += float_raw( buffer[n], board.ctl.ictl.vfs );

  printf( " sum %f avg %f\n", rsum, rsum/board.ctl.ictl.ndata );

  /*
  printRawFlt( stdout, buffer, board.ctl.ictl.ndata, board.ctl.ictl.nchans,
	       board.ctl.ictl.vfs );
  */

  free( buffer );
  exit( 0 );
}
