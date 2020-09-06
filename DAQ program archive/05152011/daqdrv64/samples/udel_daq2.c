
#include "daqutil.h"
#include "daqio.h"
#include "daqregs.h"
#include "daqprint.h"
#include "gsclib.h"
#include "udel_daq.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <X11/Xlib.h>
#include <unistd.h>	

float udelraw2float ( unsigned int x, float vfs)
{ 
  /*float t1;
  gscRawtoFloat( &t1, &x, 1, vfs );
  return t1;*/
  return (( (int)( ( x  ) & 0x0000ffff) - 32768.0)) / 32768.0 * vfs ;  
}

//========================================

void udeltime( char * p )
{
  time_t rawtime;
  time ( &rawtime );
  //printf( "UTC time=%s\n", asctime( gmtime( &rawtime ) ) );
  sprintf( p, "UTC=%s\n", asctime( gmtime( &rawtime ) ) );
}

//=========================================
FILE * udeldaqfileinit( DaqBoard * pboard, unsigned int bx1 )
{
  DaqBoard board;
  board = *pboard;
  UDELdaqheader fileheader = { { 0 } };  
  strcpy(fileheader.udel_sig, UDELsignature);
    
  struct tm * ptm;
  time_t rawtime;
  time ( &rawtime );
  ptm = gmtime ( &rawtime );
  char daqfilename[256];

  sprintf ( fileheader.UTCtime,"%04d%02d%02dT%02d%02d%02d",
    ( ptm -> tm_year + 1900 ), ptm -> tm_mon+1, ptm -> tm_mday,
    ptm -> tm_hour, ptm -> tm_min, ptm -> tm_sec );

  sprintf ( daqfilename,"/data/sata1/%s.daq", fileheader.UTCtime );
  printf( "%s\n", daqfilename);

  //printf( "fileheader time = %s\n", fileheader.UTCtime );
  fileheader.board = board;
  fileheader.sample_per_block = UDELdaqblocksize;
  fileheader.block_requested = bx1;
  //strcpy( daqfilename, "/data/sata1/test.daq" );

  FILE * pdaqfile;
  pdaqfile = fopen(daqfilename,"wb+");
  
  fwrite( &fileheader, 1, sizeof( UDELdaqheader ), pdaqfile );
  //fclose( pdaqfile );
  return pdaqfile;
}

//=================================================
int udeldaqfileclose( FILE * pdaqfile, DaqBoard * pboard )
{
  DaqBoard board = *pboard;
  UDELdaqheader fileheader = { { 0 } };  
  strcpy(fileheader.udel_sig, UDELsignature);
    
  struct tm * ptm;
  time_t rawtime;
  time ( &rawtime );
  ptm = gmtime ( &rawtime );
  sprintf ( fileheader.UTCtime,"%04d%02d%02dT%02d%02d%02d",
    ( ptm -> tm_year + 1900 ), ptm -> tm_mon+1, ptm -> tm_mday,
    ptm -> tm_hour, ptm -> tm_min, ptm -> tm_sec );
  //printf( "%s\n", daqfilename);

/*  sprintf ( fileheader.UTCtime,"%04d%02d%02dT%02d%02d%02d",
    ( ptm -> tm_year + 1900 ), ptm -> tm_mon+1, ptm -> tm_mday,
    ptm -> tm_hour, ptm -> tm_min, ptm -> tm_sec );
*/
  fileheader.board = board;

  fwrite( &fileheader, 1, sizeof( UDELdaqheader ), pdaqfile );

  int fs = fclose( pdaqfile );
  
  return 0;
}
//=================================================

void udelprintinfo ( DAQDRV_Ctl	*pctl )
{
  DAQDRV_Ctl ctl = *pctl;
  printf( "\n=====Daq board info=====\n");
  printf( "daq board firmware version: %d\nAnalog input channel: %d \nAualog output channel: %d \n", ctl.cfg.version, ctl.cfg.inchans, ctl.cfg.outchans );
  printf( "Master clock rate: %fHz\n", ctl.cfg.clock);
 
  DaqAioCtl ioctl = ctl.ictl;
  
  printf( "\n \n=====Analog input controls=====\n");
  printf( "Volts full scale: %fv\nSample rate/channel: %fHz \nclock divider: %d\n", ioctl.vfs, ioctl.clockrate, ioctl.ndiv );
  printf( "clockmaster: ");
  if ( ioctl.clkmaster == 1 ) printf( "internal clock" );
    else printf( "external clock" );
  printf( "\ninput mode: ");
  switch ( ioctl.nmode )
  {
    case 1:
      printf( "single ended (%d)\n",ioctl.nmode);
      break;
    case 0:
      printf( "differential (%d)\n",ioctl.nmode);
      break;
    default:
      printf( "Others (%d)\n",ioctl.nmode);
  }

  printf( "input range: ");
 
  switch ( ioctl.nrange )
  {
    case 0:
      printf( "2.5v\n");
      break;
    case 1:
      printf( "5v\n");
      break;
    case 2:
      printf( "10v\n");
      break;
    default:
      printf( "unknown (%d)\n",ioctl.nrange);
  }
  
  if ( ioctl.nburst == 0 ) 
  {
     printf( "Continuous mode\n");
     printf( "  number of active channels: %d\n", ioctl.nchans );
     printf( "  fifo threshold level: %d\n", ioctl.nthreshold );
  }
  else
  {
     printf( "Burst mode\n");
     printf( "  number of active channels: %d\nburst length (samples/channel): %d\n", ioctl.nchans, ioctl.nburst);
     printf( "  fifo threshold level: %d\ndata requirement ( nchans * nburst ):%d\n",ioctl.nthreshold, ioctl.ndata);
  }


  ioctl = ctl.octl;
  
  printf( "\n \n=====Analog output controls=====\n");
  printf( "Volts full scale: %fv\nSample rate/channel: %fHz \nclock divider: %d\n", ioctl.vfs, ioctl.clockrate, ioctl.ndiv );
  printf( "clockmaster: ");
  if ( ioctl.clkmaster == 1 ) printf( "internal clock" );
    else printf( "external clock" );
  
  if ( ioctl.nburst == 0 ) 
  {
     printf( "\nContinuous mode\n");
     printf( "  number of active channels: %d\n", ioctl.nchans );
     printf( "  fifo threshold level: %d\n", ioctl.nthreshold );
  }
  else
  {
     printf( "Burst mode\n");
     printf( "  number of active channels: %d\nburst length (samples/channel): %d\n", ioctl.nchans, ioctl.nburst);
     printf( "  fifo threshold level: %d\ndata requirement ( nchans * nburst ):%d\n",ioctl.nthreshold, ioctl.ndata);
  }

  printf("\n\n");
}


//===========================================================

void udelprintstatus ( DAQDRV_Status *p )
{
  printf( "\n=====Daq board status=====\n");
  printf( "high level driver status: 0x%X\nPrimary status register: 0x%X\n\n", p->status, p->intstatus);
  printf( "completed input transfers: %u\ninput FIFO overflows: %u\nlast completed input buffer: 0x%X\nsize in bytes, input buffer: %u\n\n", p->dma0counter, p->dma0overflow, (unsigned int)p->useraddr0, (unsigned int)p->usersize0 );
  printf( "completed output transfers: %u\noutput FIFO overflows: %u\nlast completed output buffer: 0x%X\nsize in bytes, output buffer: %u\n\n", p->dma1counter, p->dma1overflow, (unsigned int)p->useraddr1, (unsigned int) p->usersize1 );
  printf( "points to DaqBoard for this fd: 0x%X\n", (unsigned int)p->userdata );
}

//===========================================================

Window
create_simple_window(Display* display, int width, int height, int x, int y)
{
  int screen_num = DefaultScreen(display);
  int win_border_width = 2;
  Window win;

  /* create a simple window, as a direct child of the screen's */
  /* root window. Use the screen's black and white colors as   */
  /* the foreground and background colors of the window,       */
  /* respectively. Place the new window's top-left corner at   */
  /* the given 'x,y' coordinates.                              */
  win = XCreateSimpleWindow(display, RootWindow(display, screen_num),
                            x, y, width, height, win_border_width,
                            BlackPixel(display, screen_num),
                            WhitePixel(display, screen_num));

  /* make the window actually appear on the screen. */
  XMapWindow(display, win);

  /* flush all pending requests to the X server. */
  XFlush(display);

  return win;
}

GC
create_gc(Display* display, Window win, int reverse_video)
{
  GC gc;				/* handle of newly created GC.  */
  unsigned long valuemask = 0;		/* which values in 'values' to  */
					/* check when creating the GC.  */
  XGCValues values;			/* initial values for the GC.   */
  unsigned int line_width = 2;		/* line width for the GC.       */
  int line_style = LineSolid;		/* style for lines drawing and  */
  int cap_style = CapButt;		/* style of the line's edje and */
  int join_style = JoinBevel;		/*  joined lines.		*/
  int screen_num = DefaultScreen(display);

  gc = XCreateGC(display, win, valuemask, &values);
  if (gc < 0) {
	fprintf(stderr, "XCreateGC: \n");
  }

  /* allocate foreground and background colors for this GC. */
  if (reverse_video) {
    XSetForeground(display, gc, WhitePixel(display, screen_num));
    XSetBackground(display, gc, BlackPixel(display, screen_num));
  }
  else {
    XSetForeground(display, gc, BlackPixel(display, screen_num));
    XSetBackground(display, gc, WhitePixel(display, screen_num));
  }

  /* define the style of lines that will be drawn using this GC. */
  XSetLineAttributes(display, gc,
                     line_width, line_style, cap_style, join_style);

  /* define the fill style for the GC. to be 'solid filling'. */
  XSetFillStyle(display, gc, FillSolid);

  return gc;
}


//===========================================================

