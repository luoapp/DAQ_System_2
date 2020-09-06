

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

#include <X11/Xlib.h>
#include <unistd.h>	

#define NDATA UDELdaqblocksize


int main( int argc, char **argv )
{
  DaqBoard board = { { 0 } };
  unsigned int *buffa = NULL;
  unsigned int *buffb = NULL;
  unsigned int *p;
  UDELctl uctl;
  udelctlinit( & uctl );
  int n = 0;
  int retv = 0;
  retv = daqOpen( &board, 0 );
  if ( retv ) { perror( "daqOpen"); exit( 1 ); }

  udelargparser(argc, argv, &board, &uctl);

//--------------------------------------------------------------------------------- 

  Display* display;		/* pointer to X Display structure.           */
  int screen_num;		/* number of screen to place the window on.  */
  Window win;			/* pointer to the newly created window.      */
  unsigned int display_width,
               display_height;	/* height and width of the X display.        */
  unsigned int win_width,
	       win_height;	/* height and width for the new window.      */
  char *display_name = getenv("DISPLAY");  /* address of the X display.      */
  GC gc;			/* GC (graphics context) used for drawing    */
				/*  in our window.			     */
//  XFontStruct* font_info;       /* Font structure, used for drawing text.    */
//  char* font_name = "*-helvetica-*-12-*"; /* font to use for drawing text.   */
  XColor red,  blue,  green, white, black;
  Colormap screen_colormap;     


  if( uctl.flag_disp ){


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
  //printf("window width - '%d'; height - '%d'\n", win_width, win_height);
 
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
  XSetLineAttributes(display, gc, 1, LineSolid, CapRound, JoinRound); 

  }
//-----------------------------------------------------------------------



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
  board.ctl.ictl.clkmaster  = 1;




  /* Load the controls
   */
  retv = daqWriteControls( &board );
  if ( retv ) { perror( "daqWriteControls"); exit( 1 ); }

  int block_num, file_num;
  block_num = (int) uctl.length_in_sec *  board.ctl.ictl.nchans * ((float)board.ctl.ictl.clockrate / UDELdaqblocksize) +1 ;
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
  char t_start[64];
  udeltime( t_start );
  unsigned short u16buff[NDATA];
  int ix, ix_f, ix_b, bx1, ix2, ix3, ix4;
  float rdata[NDATA], rdata2[NDATA];

  for ( ix_f = 0; ix_f <file_num; ix_f++)
  
    {
      if ( ix_f == file_num -1 ) bx1 = (int)(block_num - ix_f * ((float)UDELdaqfilesize / UDELdaqblocksize / 2));
      else bx1 = UDELdaqfilesize / UDELdaqblocksize / 2;
      if ( uctl.flag_hd )pdaqfile = udeldaqfileinit( &board, bx1 );
      for ( ix_b = 0; ix_b < bx1; ix_b ++ )
      { 
        p = daqWaitInputBuffer( &board );
        if ( !p ) break;
       // daqStatusPrint( stdout, &board.status );
		//udelprintstatus( &board.status );
		  for ( ix=0; ix < NDATA ; ix++ ) u16buff[ix] = 0xffff & p[ix];
        //fwrite( u16buff, NDATA, sizeof (unsigned short), pdaqfile);
      if ( uctl.flag_hd ) retv=fwrite( u16buff, NDATA, sizeof (unsigned short), pdaqfile);
		//printf( " board.ioverflow = %u\n", board.ioverflow);
		//udelprintstatus( &board.status );
      
      if (uctl.flag_disp){ 


      gscRawtoFloat( rdata, p, NDATA, board.ctl.ictl.vfs );
      if (ix_b == 0) memcpy(rdata2, rdata, NDATA*sizeof(float));

      
      
      for(ix2=0; ix2<win_width-1; ix2 ++ )
      {
        XSetForeground(display, gc, white.pixel);
        ix3 = (int)(((float)ix2) / win_width * (NDATA-7) / 8) * 8 + uctl.xwin_chan;
        ix4 = (int)(((float)ix2+1.0) / win_width * (NDATA-7) / 8) * 8 + uctl.xwin_chan;
        XDrawLine(display, win, gc, ix2, (int)(-(rdata2[ix3] - uctl.xwin_offset) * win_height /uctl.xwin_fs  + win_height/2), ix2+1, (int)(-(rdata2[ix4] -uctl.xwin_offset) * win_height /uctl.xwin_fs  + win_height/2) );

        XSetForeground(display, gc, black.pixel);
        XDrawLine(display, win, gc, ix2, (int)(-(rdata[ix3] - uctl.xwin_offset) * win_height /uctl.xwin_fs  + win_height/2), ix2+1, (int)(-(rdata[ix4] -uctl.xwin_offset)* win_height /uctl.xwin_fs  + win_height/2) );
      }
        memcpy(rdata2, rdata, NDATA*sizeof(float));
        XSetForeground(display, gc, red.pixel);
        XDrawLine(display, win, gc, 0, win_height/2, win_width-1, win_height/2);
        XFlush(display);
      
      }
    
   }
   if ( uctl.flag_hd )udeldaqfileclose(pdaqfile, &board);

    
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
  
  if ( uctl.flag_disp) { getchar(); XCloseDisplay(display);}
  exit( 0 );
}
