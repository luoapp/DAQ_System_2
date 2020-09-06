#include "udel_daq.h"
#include "udel_xwin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <X11/Xlib.h>
#include <unistd.h>	

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

//==================================================================

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
/*
extern pthread_mutex_t mymutex;
extern unsigned int win_width, win_height;
extern int pt_stop;
void * xwin_drawthread ( void )
{
  float xwin_buffer_fg[UDELdaqblocksize],xwin_buffer_fg1[UDELdaqblocksize];
  int ix,ix2=0;
  while(pt_stop == 0)
  {
  sleep(1./xwin_fps);
  ix2++;
  
  pthread_mutex_lock(&mymutex);
  memcpy(xwin_buffer_fg, xwin_buffer2, UDELdaqblocksize * sizeof (float));
  pthread_mutex_unlock(&mymutex);
  if (ix2==1) memcpy(xwin_buffer_fg1, xwin_buffer_fg, UDELdaqblocksize * sizeof (float));
  XSetForeground(display, gc, white.pixel);
  for ( ix=0; ix<win_width; ix++)
  XDrawPoint(display, win, gc, ix, xwin_buffer_fg1[ix*8]/10.*win_height+win_height/2);
  XSetForeground(display, gc, black.pixel);
  for ( ix=0; ix<win_width; ix++)
  XDrawPoint(display, win, gc, ix, xwin_buffer_fg[ix*8]/10.*win_height+win_height/2);
  //printf("%f",xwin_buffer_fg[ix]);
  XFlush(display);
  memcpy(xwin_buffer_fg1, xwin_buffer_fg, UDELdaqblocksize * sizeof (float));
  }return NULL;
}*/
