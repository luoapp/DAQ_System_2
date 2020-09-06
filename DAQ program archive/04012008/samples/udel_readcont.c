#include "daqutil.h"
#include "daqio.h"
#include "daqregs.h"
#include "daqprint.h"
#include "daqparse.h"
#include "gsclib.h"
#include "udel_daq.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NDATA UDELdaqblocksize


int main( int argc, char **argv )
{
  DaqBoard board = { { 0 } };
  unsigned int *buffa = NULL;
  unsigned int *buffb = NULL;
  unsigned int *p;
  float length_in_sec = 0;
  float nominal_sr = 100.E3;
  int n = 0;

  int retv = 0;



  retv = daqOpen( &board, 0 );
  if ( retv ) { perror( "daqOpen"); exit( 1 ); }


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

  UDELarg uarg[4];

  strcpy( uarg[0].arg, "nmode" );
  uarg[0].ipv = &board.ctl.ictl.nmode;
  uarg[0].fpv = NULL;
  strcpy( uarg[1].arg, "vfs" );
  uarg[1].ipv = NULL;
  uarg[1].fpv = &board.ctl.ictl.vfs;
  strcpy( uarg[2].arg, "length" );
  uarg[2].ipv = NULL;
  uarg[2].fpv = &length_in_sec;
  strcpy( uarg[3].arg, "sr" );
  uarg[3].ipv = NULL;
  uarg[3].fpv = &nominal_sr;

  for ( n = 1; n < argc; n++)
  {
    int ix2;
    for ( ix2 = 0; ix2<4; ix2++)
    if ( strstr( argv[n], uarg[ix2].arg ) != NULL )
    {
      //printf("%d\n",strlen(uarg[ix2].arg));
      if (uarg[ix2].fpv)
      {
        *uarg[ix2].fpv	= (float) atof( argv[n] + (unsigned int )strlen(uarg[ix2].arg) + 1 );
        printf( "%s=%f\n", uarg[ix2].arg, (*uarg[ix2].fpv) );
      }
      else if (uarg[ix2].ipv) 
      {
        *uarg[ix2].ipv	= (int) atof( argv[n] + (unsigned int )strlen(uarg[ix2].arg) + 1 );
        printf( "%s=%d\n", uarg[ix2].arg, (*uarg[ix2].ipv) );
      }
    }
    //else if (
  }
  //printf("%d %f\n",board.ctl.ictl.nmode, nominal_sr);
  //exit(0);

  /* Set input sampling rate, internal clock.
     We will be printing buffer by buffer.
   */
  board.ctl.ictl.ndiv	= gscClockDivider( board.ctl.cfg.clock, nominal_sr );
  board.ctl.ictl.clkmaster  = 1;

  
  /* Load the controls
   */

  retv = daqWriteControls( &board );
  if ( retv ) { perror( "daqWriteControls"); exit( 1 ); }

  int block_num, file_num;
  block_num = (int) length_in_sec *  board.ctl.ictl.nchans * ((float)board.ctl.ictl.clockrate / UDELdaqblocksize) +1 ;
  file_num = (int) block_num * 2 * ((float)UDELdaqblocksize / UDELdaqfilesize ) + 1;
  printf( "cr=%f nch=%d bn=%d fn=%d\n",board.ctl.ictl.clockrate,board.ctl.ictl.nchans, block_num, file_num);
  //system("mv /data/sata1/test.daq /data/sata1/tese1.daq");
  //exit(0);
  

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


  FILE * pdaqfile;
  
  
//  fwrite( &fileheader, 1, sizeof( UDELdaqheader ), pdaqfile );
//  fread( &fileheader, 1, sizeof( UDELdaqheader ), pdaqfile );
//  fclose(pdaqfile);
/*
  printf(  "recorded UTC time is: %s", fileheader.UTCtime);
  printf( "ver = %d, inchans = %d, outchans = %d, clock = %f\n",fileheader.ctl.cfg.version,fileheader.ctl.cfg.inchans,
fileheader.ctl.cfg.outchans,fileheader.ctl.cfg.clock);
*/
  
  char t_start[64];
  udeltime( t_start );
  unsigned short u16buff[NDATA];
  int ix, ix_f, ix_b, bx1;

  for ( ix_f = 0; ix_f <file_num; ix_f++)
  
    {
      if ( ix_f == file_num -1 ) bx1 = (int)(block_num - ix_f * ((float)UDELdaqfilesize / UDELdaqblocksize / 2));
      else bx1 = UDELdaqfilesize / UDELdaqblocksize / 2;
      pdaqfile = udeldaqfileinit( &board, bx1 );
      //printf("%d\n",bx1);continue;
      for ( ix_b = 0; ix_b < bx1; ix_b ++ )
      { 
        p = daqWaitInputBuffer( &board );
        if ( !p ) break;
        //daqStatusPrint( stdout, &board.status );
		//udelprintstatus( &board.status );
		  for ( ix=0; ix < NDATA ; ix++ ) u16buff[ix] = 0xffff & p[ix];
        //fwrite( u16buff, NDATA, sizeof (unsigned short), pdaqfile);
		fwrite( p, NDATA, sizeof (unsigned int), pdaqfile);
		//printf( " board.ioverflow = %u\n", board.ioverflow);
		//udelprintstatus( &board.status );

//      printf( " => addr 0x%x avg %.4f s=%d", (unsigned int) p, ravg,  NDATA);

//      p = daqWaitInputBuffer( &board );
//      if ( !p ) break;

/*      gscRawtoFloat( rdata, p, NDATA, board.ctl.ictl.vfs );
      ravg = avgvalue( rdata, NDATA );

      printf( ", addr 0x%x avg %.4f\n", (unsigned int) p, ravg );
*/
//		daqStatusPrint( stdout, &board.status );
      
      }
    udeldaqfileclose(pdaqfile, &board);

    }
  //daqStatusPrint( stdout, &board.status );
  char t_end[64];
  udeltime( t_end );
  printf( "%s%s",t_start, t_end );
  

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
  exit( 0 );
}
