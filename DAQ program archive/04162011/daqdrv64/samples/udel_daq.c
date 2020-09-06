
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
#include <math.h>
#include <sys/time.h>
#include <X11/keysymdef.h>


/*
extern float xwin_buffer1[ xwin_bufferlen ], xwin_buffer2[ xwin_bufferlen ];
extern float xwin_pd, xwin_fs, xwin_fps;
extern Display* display;	
extern Window win;			
extern GC gc;			
extern XColor red, brown, blue, yellow, green, white, black;
*/

//========================================

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
//  time ( &rawtime );
//  ptm = gmtime ( &rawtime );
  char daqfilename[256];

  struct timeval tv;
  gettimeofday(&tv, NULL); 
  rawtime=tv.tv_sec;
  ptm = gmtime ( &rawtime );


  sprintf ( fileheader.UTCtime,"%04d%02d%02dT%02d%02d%02d",
    ( ptm -> tm_year + 1900 ), ptm -> tm_mon+1, ptm -> tm_mday,
    ptm -> tm_hour, ptm -> tm_min, ptm -> tm_sec );

  sprintf ( daqfilename,"/data/sata1/%s.daq", fileheader.UTCtime );
  printf( "%s\n", daqfilename);

  sprintf ( fileheader.UTCtime,"%04d%02d%02dT%02d%02d%02d.%06ldGMT",
    ( ptm -> tm_year + 1900 ), ptm -> tm_mon+1, ptm -> tm_mday,
    ptm -> tm_hour, ptm -> tm_min, ptm -> tm_sec, tv.tv_usec );
  //printf( "%s\n", fileheader.UTCtime);
  

  //printf( "fileheader time = %s\n", fileheader.UTCtime );
  fileheader.board = board;
  fileheader.sample_per_block = UDELdaqblocksize;
  fileheader.block_requested = bx1;
  //strcpy( daqfilename, "test.daq" );

  FILE * pdaqfile;
  pdaqfile = fopen(daqfilename,"wb+");
  if (pdaqfile == NULL ) 
  {
    printf("Can not create daq file!\n");
    return 0;
  }
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
  struct timeval tv;
  gettimeofday(&tv, NULL); 
  rawtime=tv.tv_sec;
  ptm = gmtime ( &rawtime );
  sprintf ( fileheader.UTCtime,"%04d%02d%02dT%02d%02d%02d.%06ldGMT",
    ( ptm -> tm_year + 1900 ), ptm -> tm_mon+1, ptm -> tm_mday,
    ptm -> tm_hour, ptm -> tm_min, ptm -> tm_sec, tv.tv_usec );
  //printf( "%s\n", daqfilename);
  //printf("%s\n",fileheader.UTCtime);

  fileheader.board = board;
  fileheader.sample_per_block = UDELdaqblocksize;
  fileheader.block_requested = 0;  

  int fs = fwrite( &fileheader, 1, sizeof( UDELdaqheader ), pdaqfile );
  
  fs = fclose( pdaqfile );
  if (fs > 0) 
  {
    printf("daq file error!\n");
    return fs;
  }
  else return 0;
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
  printf( "Sample rate/channel: %fHz \nclock divider: %d\n",  ioctl.clockrate, ioctl.ndiv );
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
  printf( "Volts full scale: %fv\n",ioctl.vfs);
  printf( "input range (= vfs): ");
 
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
  
   printf( "number of active channels: %d\nburst length (samples/channel): %d\n", ioctl.nchans, ioctl.nburst);
   printf( "fifo threshold level: %d\ndata requirement ( nchans * nburst ):%d\n",ioctl.nthreshold, ioctl.ndata);


  ioctl = ctl.octl;
  
  printf( "\n \n=====Analog output controls=====\n");
  printf( "Sample rate/channel: %fHz \nclock divider: %d\n",  ioctl.clockrate, ioctl.ndiv );
  printf( "clockmaster: ");
  if ( ioctl.clkmaster == 1 ) printf( "internal clock" );
    else printf( "external clock" );
  printf( "\nVolts full scale: %fv\n",ioctl.vfs);

  printf( "output range (= vfs): ");
 
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
   printf( "number of active channels: %d\nburst length (samples/channel): %d\n", ioctl.nchans, ioctl.nburst);
   printf( "fifo threshold level: %d\ndata requirement ( nchans * nburst ):%d\n",ioctl.nthreshold, ioctl.ndata);

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
//====================================================================
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


//==========================================================
int genwaveforms( unsigned int *raw, float f, float a, float vfs, int nsamples, float outrate, int nchans )
{
  float *waveforms = NULL;
  int ndata = nsamples * nchans;
  int n, m, i;
  printf("outrate=%f nsamples=%d nchans=%d\n",outrate,nsamples,nchans);
  waveforms = malloc( ndata*sizeof( float ) );
  if ( !waveforms ) return( 0 );
  // leave room at the end to set the outputs to zero
  for( n = 0, i = 0; n < nsamples-1; n++ )
    {
      for( m = 0; m < nchans; m++, i++ )
	{
	  /* Frequency f is scaled to the sample clock
	     Each channel differs by a phase shift
	  */
    
	 waveforms[i] = a * sin( 2.f * M_PI * (f*n/outrate + ((f*m)/nchans)) );
    //waveforms[i]=0;
     
	}
    }
  
  // leave the outputs at 0.
  for( m = 0; m < nchans; m++, i++ )
    {
      waveforms[i] = 0.f;
    }

  gscFloattoRaw( raw, waveforms, ndata, vfs );

  free( waveforms );

  return( ndata );
}


//========================================
void udelctlinit(UDELctl * puctl)
{

  puctl->xwin_fs=5;
  puctl->xwin_fps=12;
  puctl->xwin_chan=0;
  puctl->daqfile[0]=0;;
  puctl->length_in_sec = 1;
  puctl->input_nominal_sr = 100.E3;
  puctl->output_nominal_sr = 100.E3;
  puctl->flag_hd = 0;
  puctl->flag_disp = 1;
  puctl->xwin_fs = 5;
  puctl->xwin_offset = 0;
  puctl->flag_verbose = 0;
  puctl->out_freq = 50000;
  puctl->out_ampl = 1;
}

/*===============================================*/
void udelargparser(int argc, char **argv, DaqBoard * pb, UDELctl * pu )
{
  #define NARG 22
  UDELarg uarg[NARG];
  int n;
  strcpy( uarg[0].arg, "i.nmode" );
  uarg[0].ipv = & (pb->ctl.ictl.nmode);
  uarg[0].fpv = NULL;
  strcpy( uarg[1].arg, "i.vfs" );
  uarg[1].ipv = NULL;
  uarg[1].fpv = & (pb->ctl.ictl.vfs);
  strcpy( uarg[9].arg, "i.nchans" );
  uarg[9].ipv = & (pb->ctl.ictl.nchans);
  uarg[9].fpv = NULL;
  strcpy( uarg[10].arg, "i.nburst" );
  uarg[10].ipv = & (pb->ctl.ictl.nburst);
  uarg[10].fpv = NULL;
  strcpy( uarg[2].arg, "i.length" );
  uarg[2].ipv = NULL;
  uarg[2].fpv = & (pu->length_in_sec);
  strcpy( uarg[3].arg, "i.sr" );
  uarg[3].ipv = NULL;
  uarg[3].fpv = & (pu->input_nominal_sr);

  strcpy( uarg[11].arg, "o.nmode" );
  uarg[11].ipv = & (pb->ctl.octl.nmode);
  uarg[11].fpv = NULL;
  strcpy( uarg[12].arg, "o.vfs" );
  uarg[12].ipv = NULL;
  uarg[12].fpv = & (pb->ctl.octl.vfs);
  strcpy( uarg[13].arg, "o.nchans" );
  uarg[13].ipv = & (pb->ctl.octl.nchans);
  uarg[13].fpv = NULL;
  strcpy( uarg[14].arg, "o.nburst" );
  uarg[14].ipv = & (pb->ctl.octl.nburst);
  uarg[14].fpv = NULL;
  strcpy( uarg[15].arg, "o.length" );
  uarg[15].ipv = NULL;
  uarg[15].fpv = NULL;
  strcpy( uarg[16].arg, "o.sr" );
  uarg[16].ipv = NULL;
  uarg[16].fpv = & (pu->output_nominal_sr);
  strcpy( uarg[20].arg, "o.freq" );
  uarg[20].ipv = NULL;
  uarg[20].fpv = & (pu->out_freq);  
  strcpy( uarg[21].arg, "o.ampl" );
  uarg[21].ipv = NULL;
  uarg[21].fpv = & (pu->out_ampl);  

  strcpy( uarg[4].arg, "x.hd" );
  uarg[4].ipv = & (pu->flag_hd);
  uarg[4].fpv = NULL;
  strcpy( uarg[5].arg, "x.disp" );
  uarg[5].ipv = & (pu->flag_disp);
  uarg[5].fpv = NULL;
  strcpy( uarg[6].arg, "x.pd" );
  uarg[6].ipv = NULL;
  uarg[6].fpv = & (pu->xwin_pd);
  strcpy( uarg[7].arg, "x.vfs" );
  uarg[7].ipv = NULL;
  uarg[7].fpv = & (pu->xwin_fs);
  strcpy( uarg[8].arg, "x.fps" );
  uarg[8].ipv = NULL;
  uarg[8].fpv = & (pu->xwin_fps);
  strcpy( uarg[17].arg, "x.offset" );
  uarg[17].ipv = NULL;
  uarg[17].fpv = & (pu->xwin_offset);
  strcpy( uarg[18].arg, "x.ch" );
  uarg[18].ipv = NULL;
  uarg[18].fpv = & (pu->xwin_chan);
  strcpy( uarg[19].arg, "verb" );
  uarg[19].ipv = NULL;
  uarg[19].fpv = & (pu->flag_verbose);


  for ( n = 1; n < argc; n++)
  {
    if ( strstr( argv[n], ".daq" ) !=NULL )
    {
      strcpy(pu->daqfile, argv[n]);
      printf( "daqfile=%s\n", pu->daqfile );
      continue;
    }
    int ix2;
    for ( ix2 = 0; ix2<NARG; ix2++)
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
    //else printf("unknown arg: %s\n", argv[n]);
  }
  //printf("%d %f\n",board.ctl.ictl.nmode, nominal_sr);
  //exit(0);
  if ((pb->ctl.ictl.vfs != 10) && (pb->ctl.ictl.vfs != 5) && (pb->ctl.ictl.vfs !=2.5))
  {
    printf("input vfs should be one of { 2.5, 5, 10 }. Set to the default(10)\n");
    pb->ctl.ictl.vfs = 10;
  }

  if ((pb->ctl.octl.vfs != 10) && (pb->ctl.octl.vfs != 5) && (pb->ctl.octl.vfs !=2.5))
  {
    printf("output vfs should be one of { 2.5, 5, 10 }. Set to the default(10)\n");
    pb->ctl.octl.vfs = 10;
  }

  pb->ctl.ictl.ndiv	= gscClockDivider( pb->ctl.cfg.clock, pu->input_nominal_sr );
  pb->ctl.ictl.nrange	= gscRange( pb->ctl.ictl.vfs );	
  pb->ctl.octl.ndiv	= gscClockDivider( pb->ctl.cfg.clock, pu->output_nominal_sr );
  pb->ctl.octl.nrange	= gscRange( pb->ctl.octl.vfs );	

  pb->ctl.octl.ndata = pb->ctl.octl.nburst * pb->ctl.octl.nchans;
  pb->ctl.ictl.ndata = pb->ctl.ictl.nburst * pb->ctl.ictl.nchans;

  pb->ctl.ictl.clkmaster = 1;
  pb->ctl.octl.clkmaster = 1;
  pb->ctl.ictl.nmode	= 1;

  /* 0 = external software trigger (toggles the first dio line,
     the first dio line should be connected to the trigger input)
  */
  pb->ctl.trigmaster	= 1;


}


//===========================================================

void udelxwininit( UDELxwin * puxwin)
{
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
  XColor red, blue, green, white, black;
  Colormap screen_colormap;     

  /* open connection with the X server. */
  display = XOpenDisplay(display_name);
  if (display == NULL) {
    printf(" cannot connect to X server \n");
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
  XSetLineAttributes(display, gc, 1, LineSolid, CapRound, JoinRound); 
  XSelectInput(display, win, KeyPressMask);


  puxwin->red = red;
  puxwin->black = black;
  puxwin->white = white;
  puxwin->blue = blue;
  puxwin->green = green;
  puxwin->display = display;
  puxwin->win = win;
  puxwin->gc = gc;
  puxwin->win_width = win_width;
  puxwin->win_height = win_height;

}

//----------------------------------------------------

void udeldrawdata(UDELxwin * puxwin, UDELctl * puctl, DaqBoard * pboard, float *rdata, int NDATA)
{
  Display* display = puxwin->display;
  GC gc = puxwin->gc;
  Window win = puxwin->win;
  XColor red = puxwin->red;
  XColor black = puxwin->black;
  XColor white = puxwin->white;
//  XColor green = puxwin->green;
//  XColor blue = puxwin->blue;
  int win_width = puxwin->win_width;
  int win_height = puxwin->win_height;
  int nchans = pboard->ctl.ictl.nchans;
  int ix2,ix3,ix4;
  static int pixdata[1024], pixdata2[1024], first_run=1;
  for(ix2=0; ix2<win_width-1; ix2 ++ )
  { 
    if (first_run != 1)
   {
    XSetForeground(display, gc, white.pixel);
    XDrawLine(display, win, gc, ix2, pixdata2[ix2], ix2+1, pixdata2[ix2+1]);
   
   }
   
    ix3 = (int)(((float)ix2) / win_width * (NDATA- nchans + 1 ) / nchans) * nchans + puctl->xwin_chan;
    ix4 = (int)(((float)ix2+1.0) / win_width * (NDATA - nchans +1) / nchans) * nchans + puctl->xwin_chan;
    //XDrawLine(display, win, gc, ix2, (int)(-(rdata2[ix3] - puctl->xwin_offset) * win_height /puctl->xwin_fs  + win_height/2), ix2+1, (int)(-(rdata2[ix4] -puctl->xwin_offset) * win_height /puctl->xwin_fs  + win_height/2) );

    XSetForeground(display, gc, black.pixel);
    pixdata[ix2] = (int)(-(rdata[ix3] - puctl->xwin_offset) * win_height /puctl->xwin_fs  + win_height/2);
    pixdata[ix2+1] = (int)(-(rdata[ix4] - puctl->xwin_offset) * win_height /puctl->xwin_fs  + win_height/2);
    XDrawLine(display, win, gc, ix2, pixdata[ix2], ix2+1, pixdata[ix2+1] );
  }
 // printf("offset=%f\n",puctl->xwin_offset);
  memcpy(pixdata2, pixdata, 1024*sizeof(int)); 
  XSetForeground(display, gc, red.pixel);
  XDrawLine(display, win, gc, 0, win_height/2, win_width-1, win_height/2);
  XFlush(display);
  first_run = 0;
}  


