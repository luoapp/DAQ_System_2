/*
  readburst.c

  Sample code for libdaq and daqdrv driver
  Copyright 2006, Mitchell C. Nelson, Ph.D.

  All rights reserved.

*/


#include "daqutil.h"
#include "daqio.h"
#include "daqregs.h"
#include "daqprint.h"
#include "gsclib.h"
#include "udeldaq.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NDATA UDELdaqblocksize

float avgvalue( float *rdata, int ndata )
{
  float ravg = 0.f;
  int n;

  for( n = 0; n < ndata; n++ ) ravg += rdata[n];
  ravg /= ndata;
  
  return( ravg );
}

int main( )
{
  DaqBoard board = { { 0 } };
  unsigned int *buffa = NULL;
  unsigned int *buffb = NULL;
  unsigned int *p;
  UDELdaqheader fileheader;

  float rdata[NDATA];
  float ravg = 0.f;

  int n = 0;

  int retv = 0;



  retv = daqOpen( &board, 0 );
  if ( retv ) { perror( "daqOpen"); exit( 1 ); }

  /* Set input sampling rate, internal clock.
     We will be printing buffer by buffer.
   */
  board.ctl.ictl.ndiv	= gscClockDivider( board.ctl.cfg.clock, 100.E3 );
  board.ctl.ictl.clkmaster  = 1;

  /* Set single ended input, range +/- 5 Volts
   */
  board.ctl.ictl.nmode	= 1;
  board.ctl.ictl.nrange	= gscRange( 5.f );	// VFS = 5 volts

  /* Set 8 input channels, burst length 100,000 samples
   */
  board.ctl.ictl.nchans	= 8;
  board.ctl.ictl.nburst	= 0;

  /* Internal software trigger
   */
  board.ctl.trigmaster	= 1;

  /* Load the controls
   */
  retv = daqWriteControls( &board );
  if ( retv ) { perror( "daqWriteControls"); exit( 1 ); }

  /* Malloc two buffers (alignment is optional)
   */
  buffa = daqMallocAligned( buffa,  NDATA*sizeof(unsigned int) );
  if ( !buffa ) { printf( "unable to malloc buffa\n" ); exit( 1 ); }

  buffb = daqMallocAligned( buffb,  NDATA*sizeof(unsigned int) );
  if ( !buffb ) { printf( "unable to malloc buffb\n" ); exit( 1 ); }

  /* Add the buffers to the input dma engineinbuffer
   */
  retv = daqAddInputBuffer( &board, buffa, NDATA );
  if ( retv ) { perror( "add buffa"); exit( 1 ); }

  retv = daqAddInputBuffer( &board, buffb, NDATA );
  if ( retv ) { perror( "add buffb"); goto release; }

  /* Start the DMAinbuffer
   */
  printf( " buffa 0x%x  buffb 0x%x\n", (unsigned int) buffa, (unsigned int) buffb );

  retv = daqStartInputDma( &board );
  if ( retv ) { perror( "start dma" ); goto release; }

  /* Start the input
   */
  retv = gscSwInputTrigger_( board.fd );
  if ( retv ) { perror( "sw trigger" ); goto cancel; }

  /* Wait for the data
   */
  n = 0;


  time_t rawtime;
  time ( &rawtime );
  strcpy(fileheader.UTCtime,  asctime ( gmtime(&rawtime) ) );
  printf(  "Current UTC time is: %s", asctime ( gmtime(&rawtime) ));
  fileheader.ctl = board.ctl;
  
  char daqfilename[]="/data/sata1/1.daq";
  FILE * pdaqfile;
  pdaqfile = fopen(daqfilename,"wb+");
//  fwrite( &fileheader, 1, sizeof( UDELdaqheader ), pdaqfile );
//  fread( &fileheader, 1, sizeof( UDELdaqheader ), pdaqfile );
//  fclose(pdaqfile);
/*
  printf(  "recorded UTC time is: %s", fileheader.UTCtime);
  printf( "ver = %d, inchans = %d, outchans = %d, clock = %f\n",fileheader.ctl.cfg.version,fileheader.ctl.cfg.inchans,
fileheader.ctl.cfg.outchans,fileheader.ctl.cfg.clock);
*/
  
  
  while( n < 1024 )
    {
      p = daqWaitInputBuffer( &board );
      if ( !p ) break;
      gscRawtoFloat( rdata, p, NDATA, board.ctl.ictl.vfs );
		
      ravg = avgvalue( rdata, NDATA );
		//printRawFlt( pdaqfile, p, 1sizeof (unsigned int), board.ctl.ictl.nchans,
	   //   board.ctl.ictl.vfs );
		//fprintf( pdaqfile, "%f\n", rdata[0] );
		fwrite( p, NDATA, sizeof (unsigned int), pdaqfile);


//      printf( " => addr 0x%x avg %.4f s=%d", (unsigned int) p, ravg,  NDATA);

/*      p = daqWaitInputBuffer( &board );
      if ( !p ) break;

      gscRawtoFloat( rdata, p, NDATA, board.ctl.ictl.vfs );
      ravg = avgvalue( rdata, NDATA );

      printf( ", addr 0x%x avg %.4f\n", (unsigned int) p, ravg );
*/
      n++;
    }

  /* Stop the input (disable rate-a generator)
   */
  daqRegisterAnd( board.fd, 0, ~(1<<26) );

  /* Stop the DMA
   */
 cancel:
  daqCancelInputDma( &board );

  /* Release the buffers
   */
 release:
  daqReleaseInputBuffers( &board );

  /* Free the buffers
   */
  free( buffa );
  free( buffb );
  fclose(pdaqfile);
  exit( 0 );
}
