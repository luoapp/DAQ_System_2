/*
  readusertrigger.c

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
#define NDATA    (1000 * NCHANS)
#define NPRETRIG (100)
#define NLATENCY (10)

#define LEVEL (0.054f)
#define VFS  (5.f)

#define SAMPLERATE (100.E3)

/* Trigger level function
 */
int thresholdlevelf( unsigned int *data, int ndata, DaqThresholdLevelArg *a )
{
  int n;
  int retv = 0;

  /*
  printf( "thresholdf ndata %d channel %d channels %d level %d",
	  ndata, a->channel, a->nchannels, a->level );
  */

  //  printRawUInt( stdout, data, ndata, a->nchannels );

  if ( a->direction >= 0 )
    {
      n = a->channel;

      while ( n < ndata )
	{
	  if ( (data[n] & 0xFFFF) > a->level ) 
	    {
	      retv = n+1;
	      break;
	    }

	  n += a->nchannels;
	}
    }

  else
    {
      n = a->channel;

      while ( n < ndata )
	{
	  if ( (data[n] & 0xFFFF) < a->level ) 
	    {
	      retv = n+1;
	      break;
	    }

	  n += a->nchannels;
	}
    }

  //  printf( "\n" );

  return( retv );
}

// ============================================================

int main( )
{
  DaqBoard board = { { 0 } };

  DaqThresholdLevelArg dtla = { 0 };

  unsigned int udata[NDATA];

  int n = 0;

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

  /* setup for the threshold function
   */
  dtla.level     = raw_float( LEVEL, VFS );
  dtla.direction = 1;
  dtla.channel   = 0;
  dtla.nchannels = NCHANS;

  //  printf( "dtla.level %d <= %.3f / %.3f\n", dtla.level, LEVEL, VFS );

  n = daqReadThreshold( &board, udata, NDATA,
			(int(*)(void*))daqTrigger, &board,
			(int(*)(void*,int,void*))thresholdlevelf, &dtla,
			NLATENCY, NPRETRIG );

  retv = n > 0 ? 0 : 1;

  //  printf( "# daqReadThreshold => %d\n", n );

  if ( n >= 0 )
    {
      // dump the data as real voltage values 
      printRawFlt( stdout, udata, NDATA, board.ctl.ictl.nchans, board.ctl.ictl.vfs );
    }

  return( retv );  
}

