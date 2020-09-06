/*
  readburst.c

  Sample code for libdaq and daqdrv driver
  Copyright 2006, Mitchell C. Nelson, Ph.D.
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
  int retv = 0;
  int inrate=0;

  FILE * outputfile;

  retv = daqOpen( &board, 0 );
  if ( retv ) { perror( "daqOpen"); exit( 1 ); }

  // set input sampling rate to 5 kHz , internal clock
  inrate = 5E3;
  board.ctl.ictl.ndiv	= gscClockDivider( board.ctl.cfg.clock, inrate );
  board.ctl.ictl.clkmaster  = 1;

  // set single ended input, range +/- 5 Volts
  board.ctl.ictl.nmode	= 1;
  board.ctl.ictl.nrange	= gscRange( 5.f );	// VFS = 5 volts

  // set 8 input channels, burst length 10 seconds
  board.ctl.ictl.nchans	= 8;
  board.ctl.ictl.nburst	= 1 * inrate;

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

  outputfile = fopen("r1.txt","w");

  // dump the data as real voltage values 
  printRawFlt( outputfile, buffer, board.ctl.ictl.ndata, board.ctl.ictl.nchans,
	       board.ctl.ictl.vfs );

  free( buffer );

  fclose( outputfile );

  exit( 0 );
}
