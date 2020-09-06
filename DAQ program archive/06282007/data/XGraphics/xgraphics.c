/*************************************************************

  xgraphics.c - x graphics library for realtime data monitoring
  and device control

  Author:  Mitchell C. Nelson
  Date:    August 12, 2002
  Project: x graphics library

  Copyright 2002, Mitchell C. Nelson, Project Technology, LLC

  Derived from:

    graphics.c from Das (Data Acquisition System for Vax,
    PDP-11)

    Copyright 1994, Mitchell C. Nelson 

**************************************************************/
#include <stdlib.h>
#include <stdio.h>

#include <semaphore.h>

#include <ctype.h>
#include <math.h>

#include "xgraphics.h"

int buttontest( GrxWindow *gw, XEvent *xevent );

/* ==============================================================
 */

unsigned long grxAllocColor( GrxDisplay *d,
			     unsigned short red,
			     unsigned short green,
			     unsigned short blue )
{
  XColor screen_in_out = { 0 };

  unsigned long pixel = 0;

  screen_in_out.red   = red;
  screen_in_out.green = green;
  screen_in_out.blue  = blue;

  if ( XAllocColor( d->display,
		    DefaultColormap(d->display,d->screennumber), 
		    &screen_in_out ) )
    {
      pixel = screen_in_out.pixel;
    }

  return( pixel );
}

unsigned long grxAllocNamedColor( GrxDisplay *d, char *colorname )
{
  unsigned long pixel = 0;
        
  XColor  exact_color;
  XColor  closest_color;
            
  if ( XAllocNamedColor( d->display,
			 DefaultColormap(d->display,d->screennumber),
			 colorname,
			 &closest_color, &exact_color) )
    {
      pixel = closest_color.pixel;
    }

  return (pixel);
}

unsigned long grxAllocPreselectedColor( GrxDisplay *d, int n)
{
  unsigned long pixel;

  char *colors[] = {
    "purple",     "blue",            "green",         "orange",
    "red",        "turquoise",       "gold",          "pink"
    "brown",      "GreenYellow",     "DarkSlateBlue", "DarkSlateGrey",   
    "black",      "grey",            "slate grey",    "DarkTurquoise"
  };

  n = n & (MAXCOLORS-1);

  pixel = grxAllocNamedColor( d, colors[n]);

  return( pixel);
}

/* ==============================================================
 */
GrxWindow *grxLastWindow( void *this )
{
  GrxWindow *tail = this;

  if ( tail )
    {
      while( tail->next ) tail = tail->next;
    }
  return( tail );
}

int grxLoadFont( GrxDisplay *d, int fontindex, char *name )
{
  int n = -1;

  if ( fontindex < MAXFONTS )
    {
      n = fontindex;
      if (d->font[n].xfs) XFreeFont( d->display, d->font[n].xfs );
      d->font[n].font = XLoadFont( d->display, name );
      d->font[n].xfs  = XQueryFont( d->display, d->font[n].font );
    }

  return( n );
}

GrxDisplay *grxOpenDisplay( char *name )
{
  GrxDisplay *d = NULL;

  if ( !(d = malloc( sizeof(GrxDisplay))) )
    {
      fprintf( stderr, "unable to malloc display struct %s\n", name );
    }

  else
    {
      memset( d, 0, sizeof(GrxDisplay) );

      if ( !(d->display = XOpenDisplay( name )) )
	{
	  fprintf( stderr, "unable to open display %s\n", name );
	  free( d );
	  d = NULL;
	}

      else
	{
	  d->next         = NULL;

	  d->screennumber = DefaultScreen( d->display );
	  d->screen       = DefaultScreenOfDisplay( d->display );
	  d->rootwindow   = RootWindow( d->display, d->screennumber );
	  d->width        = DisplayWidth( d->display, d->screennumber );
	  d->height       = DisplayHeight( d->display, d->screennumber );

	  grxLoadFont( d, DEFAULT,    DefaultFont );
	  grxLoadFont( d, ITALIC,     ItalicFont );
	  grxLoadFont( d, BOLD,       BoldFont );
	  grxLoadFont( d, BOLDITALIC, BoldFont );

	  d->doesbackingstore =
	    (DoesBackingStore(d->screen) == NotUseful) ? 0 : 1;

	  if ( !(d->doesbackingstore) )
	    {
	      fprintf( stderr, "screen does not support backing store\n" );
	    }

	  d->maxreq = XMaxRequestSize( d->display );

	}
    }

  return( d );
}

GrxWindow *grxCreateMainWindow( GrxDisplay *d,
				int x, int y, int w, int h,
				char *name,
				int (*func)(GrxWindow *, XEvent *),
				void *arg )
{
  GrxWindow *gw = NULL;
  GrxWindow *tail = NULL;

  XSetWindowAttributes    xswa;
  int xswamask = 0;

  if ( !(gw = malloc( sizeof(GrxWindow))) )
    {
      fprintf( stderr, "unable to malloc window struct %s\n", name );
    }

  else
    {
      memset ( gw, 0, sizeof(GrxWindow) );

      tail = grxLastWindow( d );
      tail->next = gw;

      xswamask              = CWEventMask | CWBackPixel;
      xswa.event_mask       = ExposureMask | ButtonPressMask;
      xswa.background_pixel = grxAllocNamedColor( d, MAINCOLOR );
      // xswa.background_pixel = WhitePixel( d->display, d->screennumber);
      // xswa.background_pixel = grxAllocPreselectedColor( d, 0);

      if ( d->doesbackingstore )
	{
	  xswamask |= CWBackingStore;
	  xswa.backing_store = WhenMapped;  
	}

      gw->window =
	XCreateWindow( d->display, d->rootwindow, x, y, w, h, 2,
		       DefaultDepth( d->display, d->screennumber),
		       InputOutput,
		       DefaultVisual(d->display,d->screennumber),
		       xswamask, &xswa ); 

      XStoreName( d->display, gw->window, name) ;

      gw->gc = XCreateGC( d->display, gw->window, 0, NULL );

      XMapWindow( d->display, gw->window);

      //      XSynchronize( d->display, 1);
 
      gw->width  = w;
      gw->height = h;

      gw->display = d;
      gw->backgroundcolor = xswa.background_pixel;

      gw->func = func;
      gw->arg  = arg;

    }

  return( gw );
}

GrxWindow *grxCreateMainWindowByFraction( GrxDisplay *d,
					  float fraction,
					  char *title,
					  int (*func)(GrxWindow *, XEvent *),
					  void *arg )
{
  GrxWindow *gw = NULL;

  int x, y, width, height;

  if ( d )
    {
      if ( (fraction <= 0.) || (fraction > 1.0) ) fraction = 1.0;

      width  = d->width * fraction;
      height = d->height * fraction;

      x = (d->width - width) / 2;
      y = (d->height - height) / 2;

      gw = grxCreateMainWindow( d, x, y, width, height, title,
				func, arg);
    }

  return( gw );
}

GrxWindow *grxCreateDaughterWindow( GrxWindow *pw,
				    int x, int y,
				    unsigned int width,
				    unsigned int height,
				    unsigned int border,
				    int (*func)(GrxWindow *, XEvent *),
				    void *arg ) 
{
  GrxDisplay *pd = pw->display;

  GrxWindow  *gw = NULL;
  GrxWindow  *tail = NULL;

  XSetWindowAttributes    xswa;
  int xswamask = 0;

  if ( !(gw = malloc( sizeof(GrxWindow))) )
    {
      fprintf( stderr, "unable to malloc window struct\n" );
    }

  else
    {
      memset ( gw, 0, sizeof(GrxWindow) );

      tail = grxLastWindow( pw );
      tail->next = gw;

      gw->window =
	XCreateSimpleWindow( pd->display, pw->window,
			     x, y, width, height, border,
			     BlackPixel(pd->display,pd->screennumber),
			     WhitePixel(pd->display,pd->screennumber) );

      xswamask              = CWBackPixel;
      xswa.background_pixel = grxAllocNamedColor( pd, DAUGHTERCOLOR );

      if ( pd->doesbackingstore )
	{
	  xswamask |= CWBackingStore;
	  xswa.backing_store = WhenMapped; 
	}

      XChangeWindowAttributes(pd->display, gw->window, xswamask, &xswa );
  
      gw->gc = XCreateGC( pd->display, gw->window, 0, NULL );

      XMapWindow( pd->display, gw->window);

      gw->width  = width;
      gw->height = height;

      gw->display = pd;
      gw->backgroundcolor = xswa.background_pixel;

      gw->func = func;
      gw->arg  = arg;

      if (gw->func)
	XSelectInput( pd->display, gw->window,
		      ExposureMask | ButtonPressMask );

    }

  return( gw );
}


GrxWindow *grxCreateDialogWindow( GrxWindow *parent )
{
  GrxDisplay *pd = parent->display;
  GrxWindow  *gw;
  unsigned int w, h;

  int x, y;
  w      = 4*BUTTONWIDTH;
  h      = BUTTONHEIGHT;
  x      = parent->width/2 - w/2;
  y      = parent->height/2 + h/2;
  gw = grxCreateDaughterWindow( parent, x, y, w, h, 2, NULL, NULL );

  XSelectInput( pd->display, gw->window, ExposureMask | KeyPressMask );

  XUnmapWindow( pd->display, gw->window);

  return( gw );
}

/* ==============================================================
 */

XImage *grxSaveWindow( GrxWindow *w )
{
  GrxDisplay *d = w->display;

  if ( !(d->doesbackingstore) )
    {
      if ( w->image ) XDestroyImage( w->image );

      w->image =  XGetImage(d->display, w->window,
			 0, 0, w->width, w->height,
			 AllPlanes, XYPixmap );

      if ( !w->image )
	fprintf( stderr, "  failed to save image\n" );
    }

  return( w->image);
}
  
XImage *grxRestoreWindow( GrxWindow *w,
			  unsigned int x, unsigned int y,
			  unsigned int width, unsigned int height )
{
  GrxDisplay *d = w->display;

  if ( w->image )
    {
      if ( !x && !y && !width && !height )
	{
	  width  = w->width;
	  height = w->height;
	}

      XPutImage(d->display, w->window, w->gc,
		w->image, x, y, x, y, width, height);
    }

  return( w->image);
}

void grxUnsaveWindow( GrxWindow *w )
{
  if ( w->image )
    {
      XDestroyImage( w->image );
      w->image = NULL;
    }
}

void grxSaveAll( GrxWindow *window )
{
  GrxDisplay *d = window->display;

  if ( !(d->doesbackingstore) )
    {
      printf( "nobackingstore, saving windows\n" );
      while( window )
	{
	  grxSaveWindow( window );
	  window = window->next;
	}
    }
}

void grxUnmapAll( GrxDisplay *d )
{
  GrxWindow *w;

  while( (w = d->next) )
  {
    d->next = w->next;

    XUnmapWindow( d->display, w->window );
  }      
}

/* ==============================================================
 */
void grxDashedLines( GrxWindow *w )
{
  XGCValues values;
  values.line_style = LineOnOffDash;
  XChangeGC( w->display->display, w->gc, GCLineStyle, &values );
}

void grxLineWidth( GrxWindow *w, int n )
{
  XGCValues values;
  values.line_width = n;
  XChangeGC( w->display->display, w->gc, GCLineWidth, &values );
}

void grxSolidLines( GrxWindow *w )
{
  XGCValues values;
  values.line_style = LineSolid;
  XChangeGC( w->display->display, w->gc, GCLineStyle, &values );
}

unsigned long grxGetBackgroundColor( GrxWindow *w )
{
  unsigned long retv = 0;

  if( w )
    {
      retv = w->backgroundcolor;
    }

  return( retv );
}

void grxBackgroundColor( GrxWindow *w, char *colorname )
{
  GrxDisplay           *d = w->display;  
  XSetWindowAttributes xswa;
  int                  xswamask;

  xswamask              = CWBackPixel;
  xswa.background_pixel = grxAllocNamedColor( d, colorname );

  XChangeWindowAttributes( d->display, w->window, xswamask, &xswa );
  w->backgroundcolor    = xswa.background_pixel;
}

void grxForegroundColor( GrxWindow *w, char *colorname )
{
  GrxDisplay *d = w->display;  
  XGCValues  values;

  values.foreground = grxAllocNamedColor( d, colorname );
  XChangeGC( w->display->display, w->gc, GCForeground, &values );
}

unsigned long grxGetForegroundColor( GrxWindow *w )
{
  XGCValues  values;

  unsigned long retv;

  XGetGCValues(w->display->display, w->gc, GCForeground, &values);
  retv = values.foreground;
  return( retv );
}

void grxSetForegroundColor( GrxWindow *w, unsigned long color )
{
  XGCValues  values;

  values.foreground = color;
  XChangeGC( w->display->display, w->gc, GCForeground, &values );
}

/* ==============================================================
 */

void grxDrawRectangle( GrxWindow *w,
		       int x, int y,
		       unsigned int width, unsigned int height )
{
  XDrawRectangle( w->display->display, w->window, w->gc,
		  x, y, width, height );

  //  grxSaveWindow( w );
}

void grxClearRectangle( GrxWindow *w,
		       int x, int y,
		       unsigned int width, unsigned int height )
{
  XClearArea( w->display->display, w->window, x, y, width, height, 0 );
}

void grxDrawLine( GrxWindow *w,
		  int x1, int y1, int x2, int y2 )
{
  XDrawLine( w->display->display, w->window, w->gc, x1, y1, x2, y2 );

  //  grxSaveWindow( w );
}

void grxDrawLines( GrxWindow *w, XPoint *xp, int npoints )
{
  GrxDisplay *gd = w->display;
  Display    *d  = gd->display;

  long nmax = 0;
  int  nreq = 0;
  int  n    = 0;

  nmax = gd->maxreq;

  while( n < npoints )
    {
      nreq = npoints - n;
      nreq = (nreq < nmax ) ? nreq : nmax;

      XDrawLines( d, w->window, w->gc, &xp[n], nreq, CoordModeOrigin );
      n += nreq;

      if ( n && (n < npoints) ) n--;
    } 
}

int grxDrawText( GrxWindow *window, int x, int y, char *text,
		  int type,
		  int justification )
{
  GrxDisplay *d = window->display;

  XTextItem ti;
  int h, w;

  type      = (type < MAXFONTS) ? type : DEFAULT;

  ti.chars  = text;
  ti.nchars = strlen(text);
  ti.delta  = 0;
  ti.font   = d->font[type].font;

  if ( justification )
    {
      w = XTextWidth( d->font[type].xfs, ti.chars, ti.nchars );
      h = d->font[type].xfs->ascent;

      switch( justification )
	{
	case RIGHTTOP:
	  x -= w;
	  y += h;
	  break;
	case RIGHTCENTER:
	  x -= w;
	  y += h/2;
	  break;
	case RIGHTBOTTOM:
	  x -= w;
	  break;

	case CENTERTOP:
	  x -= w/2;
	  y += h;
	  break;
	case CENTERCENTER:
	  x -= w/2;
	  y += h/2;
	  break;
	case CENTERBOTTOM:
	  x -= w/2;
	  break;

	case LEFTTOP:
	  y += h;
	  break;
	case LEFTCENTER:
	  y += h/2;
	  break;
	case LEFTBOTTOM:
	  break;
	}
    }

  XDrawText( d->display, window->window, window->gc, x, y, &ti, 1 );

  //  grxSaveWindow( window );

  return( w );
}

/* ==============================================================
 */

int grxDialogPaint( GrxWindow *gw, char *prompt, char *buf )
{
  GrxDisplay   *gd   = gw->display;

  int x, y;
  unsigned int w, h, n;

  h = gd->font[BOLD].xfs->ascent;

  x = SPACE;
  y = gw->height/2 + h/2;

  if ( prompt && prompt[0] )
    {
      XClearWindow( gd->display, gw->window );

      n = strlen( prompt );
      w = XTextWidth( gd->font[BOLD].xfs, prompt, n );

      XDrawString( gd->display, gw->window, gw->gc, x, y, prompt, n );
      x += (w + SPACE);
    }

  if ( buf && buf[0] )
    {
      n = strlen( buf );
      w = XTextWidth( gd->font[DEFAULT].xfs, buf, n );

      XDrawString( gd->display, gw->window, gw->gc, x, y, buf, n );
      x += (w + SPACE);
    }

  return( x );
}

int grxDialogUpdate( GrxWindow *gw, int x, char buf )
{
  GrxDisplay   *gd   = gw->display;

  unsigned int h, w;

  int y;

  if ( buf )
    {
      h = gd->font[BOLD].xfs->ascent;
      y = gw->height/2 + h/2;

      w = XTextWidth( gd->font[DEFAULT].xfs, &buf, 1 );

      XDrawString( gd->display, gw->window, gw->gc, x, y, &buf, 1 );
      x += w;
    }

  return( x );
}

int grxDialog( GrxWindow *gw, char *prompt, char *buf, int nmax )
{
  GrxDisplay     *gd  = gw->display;

  XEvent         xreport;
  XKeyEvent      *xkey = &xreport.xkey;

  int            n = 0;

  KeySym         keysym;
  XComposeStatus compose;

  int x;

  int retv = -1;


  XMapRaised( gd->display, gw->window );

  memset( buf, 0, nmax );

  x = grxDialogPaint( gw, prompt, buf );
  XSync( gd->display, 0 );

  while( retv < 0 )
    {
      XWindowEvent(gd->display,gw->window,(KeyPressMask|ExposureMask),&xreport);

      switch( xreport.type )
	{
	case Expose:
	  x = grxDialogPaint( gw, prompt, buf );
	  break;

	case KeyPress:

	  n += XLookupString( xkey, &buf[n], (nmax-n), &keysym, &compose);

	  switch( keysym )
	    {
	    case XK_Return:
	    case XK_KP_Enter:
	    case XK_Linefeed:
	      XUnmapWindow( gd->display, gw->window );
	      if ( n )
		{
		  n--;
		  buf[n] = 0;
		}
	      retv = n;
	      break;

	    case XK_BackSpace:
	    case XK_Delete:
	      if ( n > 0 )
		{
		  n--;
		  buf[n] = 0;
		}
	      x = grxDialogPaint( gw, prompt, buf );
	      break;

	    default:
	      if ( n && isprint( buf[n-1] ) )
		{
		  x = grxDialogUpdate( gw, x, buf[n-1] );
		}
	    }
	}
    }

  return( n );
}

/* ==============================================================
 */

GrxWindow *grxButton( GrxWindow *parentwindow,
		      int x, int y,
		      unsigned int width,
		      unsigned int height,
		      char *text,
		      int (*func)(GrxWindow *, XEvent *),
		      void *arg )
{
  GrxWindow *button;

  button = grxCreateDaughterWindow( parentwindow,
				    x, y,
				    width, height, 1,
				    func, arg );

  grxDrawText( button, (width/2), (height/2), text,
	       BOLD, CENTERCENTER );

  return( button );
}

/* ==============================================================
 */

void grxCalcMap1( GrxMap1 *m1 )
{
  m1->usize = m1->u1 - m1->u0;
  if (!m1->usize) m1->usize = 1;

  m1->xsize = m1->x1 - m1->x0;
  if ( (m1->xsize == 0.0) ) m1->xsize = 1.;

  m1->dudx = m1->usize/m1->xsize;
}

void grxSetMap1( GrxMap1 *m1,
		 unsigned int u0, unsigned int u1,
		 float x0, float x1 )
{
  m1->u0 = u0;
  m1->u1 = u1;

  m1->x0 = x0;
  m1->x1 = x1;

  grxCalcMap1( m1 );
}

void grxReMap1( GrxMap1 *m1, float x0, float x1 )
{
  m1->x0 = x0;
  m1->x1 = x1;

  grxCalcMap1( m1 );
}

unsigned int grxMap1( GrxMap1 *m1, float x )
{
  float u;

  x = (x - m1->x0) * m1->dudx;
  x = (x < 0) ? 0 : ( (x > m1->usize) ? m1->usize : x );
  u = x + m1->u0;

  return( u );
}

float grxUnMap1( GrxMap1 *m1, unsigned int u )
{
  float x;

  u = (u < m1->u0) ? m1->u0 : ( (u > m1->u1) ? m1->u1 : u );
  x = m1->x0 + (u - m1->u0) / m1->dudx;

  return( x );
}

void grxCalcMap( GrxMap *m )
{
  grxCalcMap1( &m->uxmap );
  grxCalcMap1( &m->vymap );
}


void grxSetMap( GrxMap *m,
		unsigned int u0, unsigned int u1,
		unsigned int v0, unsigned int v1,
		float x0, float x1, float y0, float y1 )
{
  GrxMap1   *ux = &m->uxmap;
  GrxMap1   *vy = &m->vymap;

  grxSetMap1( ux, u0, u1, x0, x1 );
  grxSetMap1( vy, v0, v1, y1, y0 );
}

void grxRangeY( GrxMap *m, float *data, int ncolumns, int nrows )
{
  GrxMap1   *vy = &m->vymap;
  float     ymax, ymin;

  ymax = ymin = (*data);
        
  while( nrows-- )
    {
      data     += ncolumns;
      ymax = (*data) > ymax ? (*data) : ymax;
      ymin = (*data) < ymin ? (*data) : ymin;
    }

  grxReMap1( vy, ymin, ymax );
}
     
GrxUint grxMapXY( GrxMap *m, GrxFlt *xy )
{
  GrxMap1   *ux = &m->uxmap;
  GrxMap1   *vy = &m->vymap;

  GrxUint uv;

  uv.x = grxMap1( ux, xy->x );
  uv.y = grxMap1( vy, xy->y );

  return ( uv );
}

GrxFlt grxMapUV( GrxMap *m, GrxUint *uv )
{
  GrxMap1   *ux = &m->uxmap;
  GrxMap1   *vy = &m->vymap;

  GrxFlt xy;

  xy.x = grxUnMap1( ux, uv->x );
  xy.y = grxUnMap1( vy, uv->y );

  return( xy );
}

void grxSetGraphMap( GrxWindow *w,
		      unsigned int u1, unsigned int u2,
		      unsigned int v1, unsigned int v2,
		      float xmin, float xmax, float ymin, float ymax )
{
  GrxMap     *m  = &w->map;
  GrxMap1    *ux = &(m->uxmap);
  GrxMap1    *vy = &(m->vymap);

  grxSetMap1( ux, u1, u2, xmin, xmax );
  grxSetMap1( vy, v1, v2, ymax, ymin );
}

void grxDrawGraphLabels( GrxWindow *grw, char *title )
{
  GrxMap     *m  = &grw->map;
  GrxMap1    *ux = &(m->uxmap);
  GrxMap1    *vy = &(m->vymap);

  int          x, y;
  unsigned int w, h;
  float        r, dr;
  int          n = 0;
  char         label[32];

  w  = ux->usize;
  h  = vy->usize;

  /* Draw the title
   */
  if ( title && title[0] )
    {
      x  = ux->u0 + w/2;
      y  = vy->u0 - SPACE;
      grxDrawText( grw, x, y, title, DEFAULT, CENTERBOTTOM );
    }

  /* Draw the Y coordinate labels
   */
  x  = ux->u0;

  y  = vy->u0;
  r  = vy->x0;
  dr = vy->xsize/8;
  sprintf( label, "%.5g", r );
  grxDrawText( grw, (x-SPACE), y, label, DEFAULT, RIGHTTOP );

  for ( n = 0; n < 7; n++ )
    {
      y += h/8;
      r += dr;
      sprintf( label, "%.5g", r );
      grxDrawText( grw, (x-SPACE), y, label, DEFAULT, RIGHTCENTER );
    }

  y  = vy->u1;
  r  = vy->x1;
  sprintf( label, "%.5g", r );
  grxDrawText( grw, (x-SPACE), y, label, DEFAULT, RIGHTBOTTOM );

  /* Draw the X coordinate labels
   */
  y = vy->u1;

  x = ux->u0;
  r  = ux->x0;
  dr = ux->xsize/8;
  sprintf( label, "%.6g", r );
  grxDrawText( grw, x, y+SPACE, label, DEFAULT, CENTERTOP );

  for ( n = 0; n < 7; n++ )
    {
      x += w/8;
      r += dr;
      sprintf( label, "%.6g", r );
      grxDrawText( grw, x, y+SPACE, label, DEFAULT, CENTERTOP );
    }

  x = ux->u1;
  r = ux->x1;
  sprintf( label, "%.6g", r );
  grxDrawText( grw, x+SPACE, y+SPACE, label, DEFAULT, RIGHTTOP );
}

void grxDrawGraphFrame( GrxWindow *grw )
{
  GrxMap     *m  = &grw->map;
  GrxMap1    *ux = &(m->uxmap);
  GrxMap1    *vy = &(m->vymap);

  int          x, y;
  unsigned int w, h;
  int          n = 0;

  /* Draw the box
   */
  x = ux->u0;
  y = vy->u0;
  w = ux->usize;
  h = vy->usize;
  grxDrawRectangle( grw, x, y, w, h );

  /* Draw the Y coordinate hash lines
   */
  grxDashedLines( grw);

  x = ux->u0;
  y = vy->u0;

  for ( n = 0; n < 7; n++ )
    {
      y += h/8;
      grxDrawLine( grw, x, y, x+w, y );
    }

  /* Draw the X coordinate hash lines
   */
  x = ux->u0;
  y = vy->u1;

  for ( n = 0; n < 7; n++ )
    {
      x += w/8;
      grxDrawLine( grw, x, y, x, y-h );
    }

  grxSolidLines( grw);
}

GrxWindow *grxCreateGraph( GrxWindow *parent, 
			   int x, int y, unsigned int w, unsigned int h,
			   float x1, float y1, float x2, float y2,
			   char *title,
			   int (*func)(GrxWindow *, XEvent *), void *arg )
{
  GrxWindow *grw = NULL;

  grw = grxCreateDaughterWindow( parent, x, y, w, h, 1, func, arg );

  if ( grw )
    {
      grxSetGraphMap( grw,
		      GRAPHLEFT, (w-GRAPHRIGHT), GRAPHTOP, (h-GRAPHBOTTOM),
		      x1, x2, y1, y2 );

      grxDrawGraphFrame( grw );
      grxDrawGraphLabels( grw, title );
    }

  return( grw );
}

void grxResetGraph( GrxWindow *grw, char *title )
{
  grxClearRectangle( grw, 0, 0, grw->width, grw->height );
  grxDrawGraphFrame( grw );
  grxDrawGraphLabels( grw, title );
}

void grxClearGraph( GrxWindow *grw )
{
  GrxMap     *m  = &grw->map;
  GrxMap1    *ux = &(m->uxmap);
  GrxMap1    *vy = &(m->vymap);

  int          x, y;
  unsigned int w, h;

  x = ux->u0;
  y = vy->u0;
  w = ux->usize;
  h = vy->usize;
  grxClearRectangle( grw, x, y, w, h );

  grxDrawGraphFrame( grw );
}

static inline int sameXpoint( XPoint *a, XPoint *b )
{
  int retv = 0;
  retv = ( (a->x == b->x) && (a->y == b->y) ) ? 1 : 0;
  return( retv );
}
  
void grxPlotUintData( GrxWindow *grw,
		      unsigned int *data, float x0, float dx,
		      unsigned int nskip, unsigned int npoints )
{
  GrxMap     *m  = &grw->map;
  GrxMap1    *ux = &(m->uxmap);
  GrxMap1    *vy = &(m->vymap);

  XPoint xp[MAXPOINTS];
  int    n    = 0;
  int    np   = 0;

  float  r;

  nskip = (nskip) ? nskip : 1;

  r = data[n];
  xp[np].x = grxMap1( ux, x0 );
  xp[np].y = grxMap1( vy, data[n] );

  while( n < npoints )
    {
      while( (np < MAXPOINTS) && (n < npoints) )
	{
	  x0 += dx;
	  n  += nskip;
	  np++;

	  r = data[n];
	  xp[np].x = grxMap1( ux, x0 );
	  xp[np].y = grxMap1( vy, r );

	  if( np && sameXpoint(&xp[np],&xp[np-1]) ) np--;
	}

      grxDrawLines( grw, xp, np );
      np = 0;
      xp[0].x = xp[np-1].x;
      xp[0].y = xp[np-1].y;
    }

}

void grxPlotYData( GrxWindow *grw,
		   float *data, float x0, float dx,
		   unsigned int nskip, unsigned int npoints )
{
  GrxMap     *m  = &grw->map;
  GrxMap1    *ux = &(m->uxmap);
  GrxMap1    *vy = &(m->vymap);

  XPoint xp[MAXPOINTS];
  int    n    = 0;
  int    np   = 0;

  nskip = (nskip) ? nskip : 1;

  xp[np].x = grxMap1( ux, x0 );
  xp[np].y = grxMap1( vy, data[n] );

  while( n < npoints )
    {
      while( (np < MAXPOINTS) && (n < npoints) )
	{
	  x0 += dx;
	  n  += nskip;
	  np++;
	  xp[np].x = grxMap1( ux, x0 );
	  xp[np].y = grxMap1( vy, data[n] );
	  if( np && sameXpoint(&xp[np],&xp[np-1]) ) np--;
	}

      grxDrawLines( grw, xp, np );
      np = 0;
      xp[0].x = xp[np-1].x;
      xp[0].y = xp[np-1].y;
    }

}


/* ==============================================================
 */

GrxWindow *grxFindWindow( GrxWindow *pw, Window w )
{
  while( pw && (pw->window != w ) )
    pw = pw->next;

  return ( pw );
}

void grxEventLoop( GrxDisplay *gd )
{
  XEvent       report;
  XButtonEvent *xbutton = &report.xbutton;
  XExposeEvent *xexpose = &report.xexpose;
  //  XKeyEvent    *xkey    = &report.xkey;

  GrxWindow *gw;

  int buttonretv = 0;

  while( 1)
    {
      XNextEvent( gd->display, &report );

      switch( report.type )
	{
	case Expose:
	  gw = grxFindWindow( gd->next, xexpose->window );
	  if ( gw && gw->image )
	    grxRestoreWindow( gw, xexpose->x, xexpose->y,
			      xexpose->width, xexpose->height);
	  break;

	case ButtonPress:
	  gw = grxFindWindow( gd->next, xbutton->window );
	  if ( gw && gw->func )
	    {
	      buttonretv   = (*gw->func)(gw, &report );
	    }
	  break;
	  /*
	case KeyPress:
	  gw = grxFindWindow( gd->next, xkey->window );
	  if ( gw && gw->func )
	    {
	      buttonretv   = (*gw->func)(gw, &report );
	    }
	  break;
	  */
	}

      if ( buttonretv < 0 )
	break;
    }
}
/* ======================================================================== */

static void makecolors( unsigned short *red, 
			unsigned short *green,
			unsigned short *blue,
			unsigned short intensity,
			float x0  )
{
  float r;

  r = 1. - ((1. - x0) * (1. - x0))/4;
  r = intensity * r;
  *red = r;

  r = 1. - (x0 * x0)/1;
  r = intensity * r;
  *green = r;

  r = 1. - ((1. + x0) * (1. + x0))/4;
  r = intensity * r;
  *blue = r;

}

static int formatlabel( char *label, float value )
{
  int retv = 0;

  if ( (value > 1000.) || (value < -1000.) )
    {
      retv = sprintf( label, "%.3g", value );
    }

  else
    {
      retv = sprintf( label, "%.3f", value );
    }

  return( retv );
}

int grxSetColorGradient( GrxDisplay *d,
			 unsigned long *gradient, int ncolors )
{
  unsigned short red;
  unsigned short green;
  unsigned short blue;

  unsigned int n = 0;
  float x0 = -1.;
  float dx = 2./ncolors;

  while( n < ncolors )
    {
      makecolors( &red, &green, &blue, 65535, x0 );
      gradient[n] = grxAllocColor( d, red, green, blue );
      if( !gradient[n] ) break;
      x0 += dx;
      n++;
    }

  return( n );
}

void grxFillRectangle( GrxWindow *gw, int x, int y, unsigned int width,
		      unsigned int height, unsigned long lcolor )
{
  GrxDisplay *gd  = gw->display;

  grxSetForegroundColor( gw, lcolor );

  XFillRectangle( gd->display, gw->window, gw->gc, x, y, width, height);
}

void grxLabelColorKey( GrxColorKey *key )
{
  char label[64];

  formatlabel( label, key->zmax );

  grxClearRectangle( key->parent,
		     (key->x-SPACE-BUTTONWIDTH), key->y, 
		     BUTTONWIDTH, BUTTONHEIGHT );

  grxDrawText( key->parent, 
	       (key->x-SPACE), key->y, label, 
	       DEFAULT, RIGHTTOP );

  formatlabel( label, key->zmin );

  grxClearRectangle( key->parent,
		     (key->x-SPACE-BUTTONWIDTH), (key->y+key->h-BUTTONHEIGHT), 
		     BUTTONWIDTH, BUTTONHEIGHT );

  grxDrawText( key->parent, 
	       (key->x-SPACE), (key->y+key->h), 
	       label, DEFAULT, RIGHTBOTTOM );
}

void grxSetColorKeyMap( GrxColorKey *key,
			float zmin, float zmax )
{
  float r = 1.;

  if ( zmax == zmin ) zmax = zmin + 1;
  if ( key->ncolors > 1 ) r = key->ncolors - 1;

  key->zmin = zmin;
  key->zmax = zmax;
  key->dcdz = r/(zmax - zmin);

  grxLabelColorKey( key );
}

unsigned long grxGetColorKey( GrxColorKey *key, float z )
{
  int n = 0;

  z -= key->zmin;
  z *= key->dcdz;

  if ( z >= key->ncolors )
    n = key->ncolors - 1;

  else if ( z < 0 )
    n = 0;

  else
    n = z;

  return( n );
}

GrxWindow *grxCreateColorKey( GrxColorKey *key, GrxWindow *parent,
			      int x, int y, unsigned int w, unsigned int h,
			      float zmin, float zmax,
			      unsigned long *colorgradient, int ncolors )
{
  GrxWindow  *grw = NULL;

  unsigned int dh = 0;


  if ( ncolors )
    {
      dh = h/ncolors;
      h  = dh * ncolors;

      grw = grxCreateDaughterWindow( parent, x, y, w, h, 1, NULL, 0 );

      if ( grw )
	{
	  key->parent        = parent;
	  key->colorbar      = grw;
	  key->colorgradient = colorgradient;
	  key->ncolors       = ncolors;
	  key->x             = x;
	  key->y             = y;
	  key->w             = w;
	  key->h             = h;

	  grxSetColorKeyMap( key, zmin, zmax );

	  grxLabelColorKey( key );

	  x = 0;
	  y = 0;

	  while( ncolors )
	    {
	      ncolors--;
	      grxFillRectangle( grw, x, y, w, dh, colorgradient[ncolors] );
	      y += dh;
	    }
	}
    }

  return( grw );
}

void grxSetXYFillMap( GrxWindow *w,
		      unsigned int u1, unsigned int u2,
		      unsigned int v1, unsigned int v2,
		      unsigned int ncolumns, unsigned int nrows )
{
  GrxMap     *m  = &w->map;
  GrxMap1    *ux = &(m->uxmap);
  GrxMap1    *vy = &(m->vymap);

  float rx;
  float ry;

  rx = ncolumns;
  ry = nrows;

  grxSetMap1( ux, u1, u2, 0., rx );
  grxSetMap1( vy, v1, v2, 0., ry );
}

void grxXYFill( GrxWindow *grw, unsigned int ncolumn, unsigned int nrow,
		unsigned long color )
{

  GrxMap     *m  = &grw->map;
  GrxMap1    *ux = &(m->uxmap);
  GrxMap1    *vy = &(m->vymap);

  float r;

  unsigned int x, y, w, h;

  r = ncolumn;
  x = grxMap1( ux, r );

  r += 1.;
  w = grxMap1( ux, r ) - x;

  r = nrow;
  y = grxMap1( vy, r );

  r += 1.;
  h = grxMap1( vy, r ) - y;

  grxFillRectangle( grw, x, y, w, h, color );
}

void grxXYGraph( GrxWindow *grw, GrxColorKey *key,
		 float *data,
		 unsigned int ncolumns, unsigned int ndata )
{
  int nrows = 0;
  int nc = 0;
  int nr = 0;
  int n = 0;

  unsigned long color;

  if ( ndata && ncolumns )
    {
      nrows = ndata/ncolumns;

      grxSetXYFillMap( grw, 0, grw->width, 0, grw->height, ncolumns, nrows );

      for( nr = 0; nr < nrows; nr++ )
	{
	  for( nc = 0; nc < ncolumns; nc++ )
	    {	      
	      color = grxGetColorKey( key, data[n] );

	      color = key->colorgradient[color];

	      grxXYFill( grw, nc, nr, color );
	      n++;
	    }
	}
    }
}

void grxXYPattern( GrxWindow *grw, int ncolumns, int nrows,
		   unsigned long *colorgradient, int ncolors )
{
  int nc, nr;
  int n = 0;
  int incr = 0;

  if ( ncolumns && nrows )
    {
      grxSetXYFillMap( grw, 0, grw->width, 0, grw->height, ncolumns, nrows );

      if ( ncolors )
	{
	  incr = ncolors / (ncolumns * nrows);

	  for ( nc = 0; nc < ncolumns; nc++ )
	    {
	      for ( nr = 0; nr < nrows; nr++ )
		{
		  grxXYFill( grw, nc, nr, colorgradient[n] );
		  n += incr;
		  if ( n >= ncolors ) n = 0;
		}
	    }
	}
    }
}

GrxWindow *grxCreateXYMap( GrxWindow *parent,
			   int x, int y, unsigned int w, unsigned int h,
			   unsigned int ncolumns, unsigned int nrows, 
			   unsigned long *colorgradient, int ncolors,
			   int (*func)(GrxWindow *, XEvent *), void *arg )
{
  GrxWindow  *grw = NULL;

  if ( ncolors )
    {
      grw = grxCreateDaughterWindow( parent, x, y, w, h, 1, func, arg );

      if ( grw )
	{
	  grxXYPattern( grw, ncolumns, nrows, colorgradient, ncolors );
	}
    }

  return( grw );
}
/* ======================================================================== */
#ifdef XGRAPHICSTEST

int buttonexit( GrxWindow *gw, XEvent *xreport )
{
  printf( "%s: exit button\n", (char *)gw->arg);

  return( -1 );
}

int buttontest( GrxWindow *gw, XEvent *xreport )
{
  XButtonEvent *b    = &xreport->xbutton;

  switch( xreport->type )
    {
    case Expose:
      printf( "%s: expose\n", (char *) gw->arg );
      break;

    case ButtonPress:
      printf( "%s: button push at %d, %d\n", (char *)gw->arg, b->x, b->y );
      break;
    }

  return( 0 );
}

int dialogtest( GrxWindow *gw, XEvent *xreport )
{
  GrxDialog    *d = gw->arg;

  int  n;

  switch( xreport->type )
    {
    case Expose:
      printf( "%s: expose\n", (char *) d->prompt );
      break;

    case ButtonPress:
      printf( "%s: button\n", (char *) d->prompt );
      n = grxDialog( d->window, d->prompt, d->buf, d->nmax );
      printf( "  returns %s\n", d->buf );
      break;
    }

  return( 0 );
}  


#define NPOINTS 2048

int main( int argc, char **argv )
{
  GrxDisplay *gd;
  GrxWindow  *main_window;
  GrxWindow  *exitbutton;
  GrxWindow  *button;
  GrxWindow  *graph;

  GrxDialog  grxdialog;
  GrxDialog  *dialog = &grxdialog;

  int x, y, w, h;

  float data[NPOINTS];
  float theta = -3.14;
  float dtheta = 6.28/NPOINTS;
  int   n;

  char buf[256];

  /* Generate some data
   */
  for( n = 0, theta = -3.14; n < NPOINTS; n++, theta += dtheta )
    data[n] = sin( theta);


  /* Test the graphics
   */

  gd = grxOpenDisplay( NULL );

  main_window = grxCreateMainWindowByFraction( gd, 0.8, "test",
					       buttontest,
					       "main window");

  x = SPACE;
  y = BUTTONHEIGHT;

  grxDrawText( main_window, x, y, "Main Window", BOLD, LEFTTOP );

  x = SPACE;
  y = 2 * BUTTONHEIGHT;
  w = BUTTONWIDTH;
  h = BUTTONHEIGHT;

  exitbutton = grxButton( main_window, x, y, w, h,
			  "exit", buttonexit, "exit button" );

  y += SPACE + BUTTONHEIGHT;

  button = grxButton( main_window, x, y, w, h,
		      "button", buttontest, "test button" );

  y += SPACE + BUTTONHEIGHT;
  button = grxButton( main_window, x, y, w, h,
		      "enter text", dialogtest, dialog );

  x = 2 * SPACE + BUTTONWIDTH;
  y = 2 * BUTTONHEIGHT;

  w = main_window->width - (x + 2*SPACE);
  h = main_window->height - (y + 2*SPACE);

  graph  = grxCreateGraph( main_window, x, y, w, h,
			   -3.14, -1.0, 3.14, 1.0,
			   "Amplitude( angle )",
			   buttontest, "graphics window" );

  dialog->window = grxCreateDialogWindow( main_window );
  dialog->prompt = "enter a string";
  dialog->buf    = buf;
  dialog->nmax   = sizeof(buf);

  grxSaveAll( main_window );

  XSync( gd->display, 0 );

  grxPlotYData( graph, data, -3.14, dtheta, 1, NPOINTS );

  XSync( gd->display, 0 );

  sleep( 1 );

  grxClearGraph( graph );
  grxPlotYData( graph, data, -3.14, dtheta, 1, NPOINTS );

  grxEventLoop( gd );

  return( 1 );
}
#endif
