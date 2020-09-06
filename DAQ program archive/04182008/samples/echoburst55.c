/*
  echoburst.c

  Sample code for libdaq and daqdrv driver
  Copyright 2006, Mitchell C. Nelson, Ph.D.

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
#include "udel_daq.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main( int argc, char **argv )
{
  DaqBoard board = { { 0 } };
  unsigned int *inbuffer = NULL;
  unsigned short *u16buff = NULL;
  unsigned int *outbuffer = NULL;
  int retv = 0;
/*
  float outrate      = 100.e3;
  float outvfs       = 5.f;
  int outsamples     = 100000;
  int outchans       = 1;
  int outdata        = outsamples * outchans;

  float inrate       = outrate;
  float invfs        = 5.f;
  int insamples      = outsamples + 0. * inrate; // listen for another 1/2 second
  int inchans        = 1;
  int indata         = insamples * inchans;
*/

  
  int ix;

  FILE * outputfile;

  retv = daqOpen( &board, 0 );
  if ( retv ) { perror( "daqOpen"); exit( 1 ); }
  board.ctl.octl.nburst = 1000;

  UDELctl uctl;
  udelctlinit( & uctl );
  udelargparser(argc, argv, &board, &uctl);

  /*
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
  */

  /* 0 = external software trigger (toggles the first dio line,
     the first dio line should board.ctl.octl.nburstbe connected to the trigger input)
  */
  //board.ctl.trigmaster	= 1;
  //board.ctl.octl.ndata = board.ctl.octl.nburst * board.ctl.octl.nchans;
  //board.ctl.ictl.ndata = board.ctl.ictl.nburst * board.ctl.ictl.nchans;  
  //udelprintinfo(&board.ctl);
  // allocate an output data buffer (alignment is optional)
  outbuffer = daqMallocAligned( outbuffer, board.ctl.octl.ndata * sizeof(unsigned int) );

  //outbuffer = malloc( board.ctl.octl.nburst * board.ctl.octl.nchans*sizeof(unsigned int) );
  //outbuffer = malloc( 400000);
  if ( !outbuffer ) { printf( "malloc outbuffer failed\n");  exit( 1 ); }
  
  // generate a test pattern
  if ( !genwaveforms( outbuffer, uctl.out_freq, uctl.out_ampl, board.ctl.octl.vfs, board.ctl.octl.nburst, board.ctl.octl.clockrate, board.ctl.octl.nchans ) )
    {
      printf( "unable to create the waveforms\n" );
      exit( 1 );
    }  

 // allocate an input data buffer (alignment is optional)
  inbuffer = daqMallocAligned( inbuffer, board.ctl.ictl.ndata*sizeof(unsigned int) );
  //inbuffer = malloc( board.ctl.ictl.nburst * board.ctl.ictl.nchans*sizeof(unsigned int) );
  u16buff  = malloc( board.ctl.ictl.ndata*sizeof(unsigned short) );
  if ( !inbuffer ) { printf( "malloc inbuffer failed\n");  exit( 1 ); }
  printf("start...\n");
  // load the controls 

  //udelprintinfo(&board.ctl);
  
  retv = daqWriteControls( &board );
  if ( retv ) { perror( "daqWriteControls"); exit( 1 ); }
   
  // Start the DMA,  trigger the output and wait for completion
  retv = daqWriteReadRaw( &board, outbuffer, board.ctl.octl.ndata, 
			  inbuffer, board.ctl.ictl.ndata,
			  (int(*)(void*))daqTrigger, &board );
	
  if ( retv ) { perror("daqWriteReadRaw"); exit( 1 ); }

  udelprintinfo(&board.ctl);
  printf("writing to disk...\n");
  outputfile = udeldaqfileinit(&board, 0);
  if (outputfile)
  {
    for ( ix=0; ix < board.ctl.ictl.ndata ; ix++ ) u16buff[ix] = 0xffff & inbuffer[ix];
    fwrite(u16buff, board.ctl.ictl.nburst, sizeof (unsigned short), outputfile);
    udeldaqfileclose(outputfile, &board);
  }
  free( outbuffer );
  free( inbuffer );
  free( u16buff );
  exit( 0 );
}
