/*
  readleveltrigger.c

  Sample code for libdaq and daqdrv driver
  Copyright 2006, Mitchell C. Nelson, Ph.D.

  All rights reserved.

*/


#include "daqutil.h"
#include "daqio.h"
#include "daqthreshold.h"
#include "daqregs.h"
#include "daqprint.h"
#include "gsclib.h"
#include <stdio.h>
#include <stdlib.h>

#define NCHANS   8
#define NDATA    (10000 * NCHANS)
#define NPRETRIG (200)
#define NLATENCY (100)

#define CHANNEL (0)
#define LEVEL (0.055f)
#define VFS  (5.f)

#define SAMPLERATE (100.E3)


int main( )
{
  DaqBoard board = { { 0 } };

  unsigned int udata[NDATA];

  int retv = 0;

  retv = daqOpen( &board, 0 );
  if ( retv ) { perror( "daqOpen"); exit( 1 ); }

  /* Set input sampling rate internal clock.
   */
  board.ctl.ictl.ndiv	= gscClockDivider( board.ctl.cfg.clock, SAMPLERATE );
  board.ctl.ictl.clkmaster  = 1;

  /* Set single ended input, range +/- VFS Volts
   */
  board.ctl.ictl.nmode	= 1;
  board.ctl.ictl.nrange	= gscRange( VFS );

  /* Set 8 input channels
   */
  board.ctl.ictl.nchans	= NCHANS;
  board.ctl.ictl.nburst	= 0;

  /* Internal software trigger
   */
  board.ctl.trigmaster	= 1;

  /* Load the controls
   */
  retv = daqWriteControls( &board );
  if ( retv ) { perror( "daqWriteControls"); exit( 1 ); }

  /* read with threshold
   */
  retv = daqReadThresholdLevel( &board, udata, NDATA,
				(int(*)(void*))daqTrigger, &board,
				CHANNEL, LEVEL, 1, NLATENCY, NPRETRIG );

  //  printf( "# daqReadThreshold => %d\n", retv );

  if ( !retv )
    {
      // dump the data as real voltage values 
      printRawFlt( stdout, udata, NDATA, board.ctl.ictl.nchans, board.ctl.ictl.vfs );
    }

  return( retv );  
}

