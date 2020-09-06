

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
#include <pthread.h>
#include <X11/Xlib.h>
#include <unistd.h>	

#define NDATA UDELdaqblocksize

float xwin_buffer1[ xwin_bufferlen ],xwin_buffer2[NDATA];
float xwin_pd = 4, xwin_fs = 10, xwin_fps = 12;
Display* display;	
Window win;			
GC gc;			
XColor red, brown, blue, yellow, green, white, black;
pthread_mutex_t mymutex=PTHREAD_MUTEX_INITIALIZER;
unsigned int win_width,
	       win_height;	/* height and width for the new window.      */
int pt_stop = 0;
int main( int argc, char **argv )
{
  DaqBoard board = { { 0 } };
  unsigned int *buffa = NULL;
  unsigned int *buffb = NULL;
  unsigned int *p;
  float length_in_sec = 0;
  float nominal_sr = 100.E3;
  int n = 0;
  int flag_hd = 0, flag_disp = 1;
  int retv = 0;
  pthread_t mythread;

  
  int t1,t2;
  for (t1 = 0; t1 < xwin_bufferlen; t1 ++)
  {
    xwin_buffer1[t1]=0;
    xwin_buffer2[t1]=0;
  }

  int screen_num;		/* number of screen to place the window on.  */
  unsigned int display_width,
               display_height;	/* height and width of the X display.        */
  char *display_name = getenv("DISPLAY");  /* address of the X display.      */
				/*  in our window.			     */
  XFontStruct* font_info;       /* Font structure, used for drawing text.    */
  char* font_name = "*-helvetica-*-12-*"; /* font to use for drawing text.   */
  Colormap screen_colormap;     

  /* open connection with the X server. */
  display = XOpenDisplay(display_name);
  if (display == NULL) {
    fprintf(stderr, "%s: cannot connect to X server '%s'\n",
            argv[0], display_name);
    exit(1);
  }

  /* get the geometry of the default screen for our display. */
  screen_num = DefaultScreen(display);
  display_width = DisplayWidth(display, screen_num);
  display_height = DisplayHeight(display, screen_num);

  /* make the new window occupy 1/9 of the screen's size. */
  win_width = (display_width / 2);
  win_height = (display_height / 3);
  printf("window width - '%d'; height - '%d'\n", win_width, win_height);

  /* create a simple window, as a direct child of the screen's   */
  /* root window. Use the screen's white color as the background */
  /* color of the window. Place the new window's top-left corner */
  /* at the given 'x,y' coordinates.                             */
  win = create_simple_window(display, win_width, win_height, 0, 0);

  /* allocate a new GC (graphics context) for drawing in the window. */
  gc = create_gc(display, win, 0);
  XSync(display, False);
  screen_colormap = DefaultColormap(display, DefaultScreen(display));
  XAllocNamedColor(display, screen_colormap, "red", &red, &red);
  XAllocNamedColor(display, screen_colormap, "white", &white, &white);
  XAllocNamedColor(display, screen_colormap, "black", &black, &black);
  XAllocNamedColor(display, screen_colormap, "green", &green, &green);
  XAllocNamedColor(display, screen_colormap, "blue", &blue, &blue);
  


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

  UDELarg uarg[8];

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
  strcpy( uarg[4].arg, "hd" );
  uarg[4].ipv = &flag_hd;
  uarg[4].fpv = NULL;
  strcpy( uarg[5].arg, "disp" );
  uarg[5].ipv = &flag_disp;
  uarg[5].fpv = NULL;
  strcpy( uarg[6].arg, "xwin_pd" );
  uarg[6].ipv = NULL;
  uarg[6].fpv = &xwin_pd;
  strcpy( uarg[7].arg, "xwin_fs" );
  uarg[7].ipv = NULL;
  uarg[7].fpv = &xwin_fs;

  for ( n = 1; n < argc; n++)
  {
    int ix2;
    for ( ix2 = 0; ix2<8; ix2++)
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
  
  pthread_create( &mythread, NULL, xwin_drawthread , NULL);
      
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
  int ix, ix_f, ix_b, bx1, ix2, ix3;
  float rdata[NDATA], rdata2[NDATA];

  for ( ix_f = 0; ix_f <file_num; ix_f++)
  
    {
      if ( ix_f == file_num -1 ) bx1 = (int)(block_num - ix_f * ((float)UDELdaqfilesize / UDELdaqblocksize / 2));
      else bx1 = UDELdaqfilesize / UDELdaqblocksize / 2;
      if ( flag_hd )pdaqfile = udeldaqfileinit( &board, bx1 );
      //printf("%d\n",bx1);continue;
      for ( ix_b = 0; ix_b < bx1; ix_b ++ )
      { 
        p = daqWaitInputBuffer( &board );
        if ( !p ) break;
        //daqStatusPrint( stdout, &board.status );
		//udelprintstatus( &board.status );
		  for ( ix=0; ix < NDATA ; ix++ ) u16buff[ix] = 0xffff & p[ix];
        //fwrite( u16buff, NDATA, sizeof (unsigned short), pdaqfile);
		if ( flag_hd ) fwrite( p, NDATA, sizeof (unsigned int), pdaqfile);
		//printf( " board.ioverflow = %u\n", board.ioverflow);
		//udelprintstatus( &board.status );

//      printf( " => addr 0x%x avg %.4f s=%d", (unsigned int) p, ravg,  NDATA);

//      p = daqWaitInputBuffer( &board );
//      if ( !p ) break;
      gscRawtoFloat( rdata, p, NDATA, board.ctl.ictl.vfs );
      //if (ix_b == 0) memcpy(rdata2, rdata, NDATA*sizeof(float));
/*      int tx1=NDATA/board.ctl.ictl.nchans/ (nominal_sr * xwin_pd /win_width );
      memcpy(xwin_buffer1, xwin_buffer2+tx1, sizeof(float)*(xwin_bufferlen-tx1));
      for (ix2=0;ix2< tx1 ; ix2++ )
      {
        ix3 = ix2 * nominal_sr * xwin_pd / win_width;
        xwin_buffer1[xwin_bufferlen-tx1+ix2] = rdata[ix3*8];
      }*/
      
      pthread_mutex_lock(&mymutex);
      memcpy(xwin_buffer2, rdata, NDATA * sizeof (float));
      //printf("\n%f %f %f %f\n",rdata[0],rdata[1],rdata[2],rdata[3]);
      pthread_mutex_unlock(&mymutex);
      //pthread_join ( mythread, NULL ) ;
      
      //for(ix2=0; ix2< (513-win_width); ix2 ++ )
      { 
        //XSetForeground(display, gc, white.pixel);
        //ix3 = (int)(((float)ix2) / win_width * (NDATA-7) / 8) * 8;
        //XDrawPoint(display, win, gc, ix2, (int)(rdata2[ix3] * win_height /xwin_fs  + win_height/2) );
        /*ix3 = (int)(((float)ix2) / win_width * (NDATA-7) / 8) * 8 +2;
        XDrawPoint(display, win, gc, ix2, (int)(rdata2[ix3] * win_height /0.2  + win_height/2) );


        ix3 = (int)(((float)ix2) / win_width * (NDATA-7) / 8) * 8 +4;
        XDrawPoint(display, win, gc, ix2, (int)(rdata2[ix3] * win_height /0.2  + win_height/2) );
        ix3 = (int)(((float)ix2) / win_width * (NDATA-7) / 8) * 8 +6;
        XDrawPoint(display, win, gc, ix2, (int)(rdata2[ix3] * win_height /0.2  + win_height/2) );*/

        //ix3 = (int)(((float)ix2) / win_width * (NDATA-7) / 8) * 8;
        //XSetForeground(display, gc, black.pixel);
        //XDrawPoint(display, win, gc, ix2, (int)(rdata[ix3] * win_height /xwin_fs  + win_height/2) );
 
        /*ix3 = (int)(((float)ix2) / win_width * (NDATA-7) / 8) * 8 +2;
        XSetForeground(display, gc, red.pixel);
        XDrawPoint(display, win, gc, ix2, (int)(rdata[ix3] * win_height /0.2  + win_height/2) );
        ix3 = (int)(((float)ix2) / win_width * (NDATA-7) / 8) * 8 +4;
        XSetForeground(display, gc, green.pixel);
        XDrawPoint(display, win, gc, ix2, (int)(rdata[ix3] * win_height /0.2  + win_height/2) );
        ix3 = (int)(((float)ix2) / win_width * (NDATA-7) / 8) * 8 +6;
        XSetForeground(display, gc, blue.pixel);
        XDrawPoint(display, win, gc, ix2, (int)(rdata[ix3] * win_height /0.2  + win_height/2) );*/
         //printf("%f ",rdata[ix3]);
      }
        //memcpy(rdata2, rdata, NDATA*sizeof(float));
        //XFlush(display);
      
/*      ravg = avgvalue( rdata, NDATA );

      printf( ", addr 0x%x avg %.4f\n", (unsigned int) p, ravg );
*/
//		daqStatusPrint( stdout, &board.status );
      
      }
    if ( flag_hd )udeldaqfileclose(pdaqfile, &board);

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
  getchar();
  pt_stop = 1;
  pthread_join ( xwin_drawthread, NULL );
  XCloseDisplay(display);
  exit( 0 );
}
