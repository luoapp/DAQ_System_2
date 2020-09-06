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
  int outdata, ix;

  FILE * outputfile;

  retv = daqOpen( &board, 0 );
  if ( retv ) { perror( "daqOpen"); exit( 1 ); }
  board.ctl.octl.nburst = 1000;

  UDELctl uctl;
  udelctlinit( & uctl );
  udelargparser(argc, argv, &board, &uctl);

  FILE * pinfile;
  long int infilelength;
  UDELdaqheader fileheader;
  
  outdata = board.ctl.octl.ndata;

  if (uctl.daqfile[0])
  {
    pinfile = fopen( uctl.daqfile, "rb" );
    if ( pinfile )
    {
      fseek( pinfile, 0, SEEK_END );
      infilelength = ftell( pinfile );
      fseek( pinfile, 0, SEEK_SET );
      fread( &fileheader, sizeof( UDELdaqheader ), 1, pinfile );
      outdata = infilelength - 2 * sizeof( UDELdaqheader );
    }
  }



  //udelprintinfo(&board.ctl);
  outbuffer = daqMallocAligned( outbuffer, outdata * sizeof(unsigned int) );

  if ( !outbuffer ) { printf( "malloc outbuffer failed\n");  exit( 1 ); }
  
  if (pinfile)
  {
    u16buff = malloc( outdata * sizeof (unsigned short) );
    fread( u16buff, sizeof(unsigned short), outdata, pinfile );
    fclose( pinfile );
    for ( ix=0; ix < outdata; ix++) outbuffer[ix] = u16buff[ix];
    free( u16buff );
    board.ctl.octl.nchans = fileheader.board.ctl.octl.nchans;
    board.ctl.octl.vfs = fileheader.board.ctl.octl.vfs;
    //board.ctl.octl.nburst = fileheader.board.ctl.octl.nburst;
    board.ctl.octl.nburst = outdata / board.ctl.octl.nchans;
    board.ctl.octl.clockrate = fileheader.board.ctl.octl.clockrate;
    board.ctl.octl.nrange = fileheader.board.ctl.octl.nrange;
    board.ctl.octl.ndiv = fileheader.board.ctl.octl.ndiv;
    board.ctl.octl.clkmaster = fileheader.board.ctl.octl.clkmaster;
    board.ctl.octl.ndata = outdata;
    printf("%d ", outdata);
  }
  else
  {
    if ( !genwaveforms( outbuffer, uctl.out_freq, uctl.out_ampl, board.ctl.octl.vfs, board.ctl.octl.nburst, board.ctl.octl.clockrate, board.ctl.octl.nchans ) )
    {
      printf( "unable to create the waveforms\n" );
      exit( 1 );
    }  
  }

  inbuffer = daqMallocAligned( inbuffer, board.ctl.ictl.ndata*sizeof(unsigned int) );

  u16buff  = malloc( board.ctl.ictl.ndata*sizeof(unsigned short) );
  if ( !inbuffer ) { printf( "malloc inbuffer failed\n");  exit( 1 ); }
  printf("start...\n");
  // load the controls 

  udelprintinfo(&board.ctl);
  //exit(0);
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
    fwrite(u16buff, board.ctl.ictl.ndata, sizeof (unsigned short), outputfile);
    udeldaqfileclose(outputfile, &board);
  }
  free( outbuffer );
  free( inbuffer );
  free( u16buff );
  exit( 0 );
}
