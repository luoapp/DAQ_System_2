/*************************************************************

  xgraphics.h - x graphics library for realtime data monitoring
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
#ifndef _XGRAPHICS_H_
#define _XGRAPHICS_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#define MAINCOLOR "lightgrey"
#define DAUGHTERCOLOR "aliceblue"
#define MAXCOLORS 16

#define DefaultFont    "-*-*-Medium-R-Normal--*-120-*-*-P-*-ISO8859-1"
#define ItalicFont     "-*-*-Medium-I-Normal--*-120-*-*-P-*-ISO8859-1"
#define BoldFont       "-*-*-Bold-R-Normal--*-120-*-*-P-*-ISO8859-1"
#define BoldItalicFont "-*-*-Bold-I-Normal--*-120-*-*-P-*-ISO8859-1"

#define DEFAULT    0
#define ITALIC     1
#define BOLD       2
#define BOLDITALIC 3
#define MAXFONTS   8

#define LEFTTOP      0
#define LEFTCENTER   1
#define LEFTBOTTOM   2
#define CENTERTOP    3
#define CENTERCENTER 4
#define CENTERBOTTOM 5
#define RIGHTTOP     6
#define RIGHTCENTER  7
#define RIGHTBOTTOM  8

#define BUTTONHEIGHT 20
#define BUTTONWIDTH 80
#define SPACE 10
#define GRAPHTOP    SPACE+BUTTONHEIGHT
#define GRAPHBOTTOM (2*BUTTONHEIGHT)
#define GRAPHLEFT   BUTTONWIDTH
#define GRAPHRIGHT  SPACE

#define MAXPOINTS 8192

typedef struct grxflt {
  float x;
  float y;
} GrxFlt;

typedef struct grxuint {
  unsigned int x;
  unsigned int y;
} GrxUint;

typedef struct grxmap1 {
  unsigned int usize;
  float        xsize;
  unsigned int u0;
  unsigned int u1;
  float        x0;
  float        x1;
  float        dudx;
} GrxMap1;

typedef struct grxmap {
  GrxMap1 uxmap;
  GrxMap1 vymap;
  char    x_format[8];
  char    y_format[8];
} GrxMap;

typedef struct grxdialog {
  struct grxwindow *window;
  char      *prompt;
  char      *buf;
  int       nmax;
} GrxDialog;

typedef struct grxfont {
  Font        font;
  XFontStruct *xfs;
} GrxFont;

typedef struct grxdisplay {
  void         *next;
  unsigned int width;
  unsigned int height;
  Window       rootwindow;
  Display      *display;
  Screen       *screen;
  int          screennumber;
  int          doesbackingstore;
  GrxFont      font[MAXFONTS];
  unsigned long maxreq;
} GrxDisplay;

typedef struct grxwindow {
  void         *next;
  unsigned int width;
  unsigned int height;
  Window       window;
  unsigned long backgroundcolor;
  GrxDisplay   *display;
  GC           gc;
  GrxMap       map;
  XImage       *image;
  int          (*func)( struct grxwindow *gw, XEvent *report );
  void         *arg;
} GrxWindow;

typedef struct grxcolorkey {
  GrxWindow *parent;
  GrxWindow *colorbar;
  unsigned long *colorgradient;
  int ncolors;
  float zmin;
  float zmax;
  float dcdz;
  unsigned int x;
  unsigned int y;
  unsigned int w;
  unsigned int h;
} GrxColorKey;

void grxCalcMap1( GrxMap1 *m1 );

void grxSetMap1( GrxMap1 *m1,
		 unsigned int u0, unsigned int u1,
		 float x0, float x1 );

void grxReMap1( GrxMap1 *m1, float x0, float x1 );

unsigned int grxMap1( GrxMap1 *m1, float x );

float grxUnMap1( GrxMap1 *m1, unsigned int u );

void GrxCalcMap( GrxMap *m );

void grxSetMap( GrxMap *m,
		unsigned int u0, unsigned int u1,
		unsigned int v0, unsigned int v1,
		float x0, float x1, float y0, float y1 );

void grxRangeY( GrxMap *m, float *data, int ncolumns, int nrows );
     
GrxUint grxMapXY( GrxMap *g, GrxFlt *xy );

GrxFlt grxMapUV( GrxMap *g, GrxUint *uv );


int grxSetColorGradient( GrxDisplay *d,
			 unsigned long *gradient, int ncolors );

unsigned long grxAllocColor( GrxDisplay *d,
			     unsigned short red,
			     unsigned short green,
			     unsigned short blue );

unsigned long grxAllocNamedColor( GrxDisplay *d, char *colorname );

unsigned long grxAllocPreselectedColor( GrxDisplay *d, int n);

GrxDisplay *grxOpenDisplay( char *name );

void grxUnmapAll( GrxDisplay *d );

GrxWindow *grxCreateMainWindow( GrxDisplay *d,
				int x, int y, int w, int h,
				char *name,
				int (*func)(GrxWindow *, XEvent *),
				void *arg );

GrxWindow *grxCreateMainWindowByFraction( GrxDisplay *d,
					  float fraction,
					  char *title,
					  int (*func)(GrxWindow *, XEvent *),
					  void *arg );
GrxWindow *grxCreateDaughterWindow( GrxWindow *pw,
				    int x, int y,
				    unsigned int width,
				    unsigned int height,
				    unsigned int border,
				    int (*func)(GrxWindow *, XEvent *),
				    void *arg ); 

GrxWindow *grxCreateDialogWindow( GrxWindow *parent );

XImage *grxSaveWindow( GrxWindow *w );

XImage *grxRestoreWindow( GrxWindow *w,
			  unsigned int x, unsigned int y,
			  unsigned int width, unsigned int height );

void grxUnsaveWindow( GrxWindow *w );

void grxSaveAll( GrxWindow *window );

void grxDashedLines( GrxWindow *w );
void grxLineWidth( GrxWindow *w, int n );
void grxSolidLines( GrxWindow *w );

unsigned long grxGetBackgroundColor( GrxWindow *w );
void grxBackgroundColor( GrxWindow *w, char *colorname );

void grxForegroundColor( GrxWindow *w, char *colorname );
unsigned long grxGetForegroundColor( GrxWindow *w );
void grxSetForegroundColor( GrxWindow *w, unsigned long color );

void grxDrawRectangle( GrxWindow *w,
		       int x, int y,
		       unsigned int width, unsigned int height );
void grxClearRectangle( GrxWindow *w,
		       int x, int y,
		       unsigned int width, unsigned int height );
void grxDrawLine( GrxWindow *w,
		  int x1, int y1, int x2, int y2 );
void grxDrawLines( GrxWindow *w, XPoint *xp, int npoints );
int grxDrawText( GrxWindow *window, int x, int y, char *text,
		  int type,
		  int justification );

int grxDialog( GrxWindow *gw, char *prompt, char *buf, int nmax );

GrxWindow *grxButton( GrxWindow *parentwindow,
		      int x, int y,
		      unsigned int width,
		      unsigned int height,
		      char *text,
		      int (*func)(GrxWindow *, XEvent *),
		      void *arg );

void grxSetGraphMap( GrxWindow *w,
		      unsigned int u1, unsigned int u2,
		      unsigned int v1, unsigned int v2,
		      float xmin, float xmax, float ymin, float ymax );

void grxDrawGraphLabels( GrxWindow *grw, char *title );

void grxDrawGraphFrame( GrxWindow *grw );

GrxWindow *grxCreateGraph( GrxWindow *parent, 
			   int x, int y, unsigned int w, unsigned int h,
			   float x1, float y1, float x2, float y2,
			   char *title,
			   int (*func)(GrxWindow *, XEvent *), void *arg );

void grxClearGraph( GrxWindow *grw );

void grxResetGraph( GrxWindow *grw, char *title );

void grxPlotUintData( GrxWindow *grw,
		      unsigned int *data, float x0, float dx,
		      unsigned int nskip, unsigned int npoints );

void grxPlotYData( GrxWindow *grw,
		   float *data, float x0, float dx,
		   unsigned int nskip, unsigned int npoints );

void grxEventLoop( GrxDisplay *gd );

void grxLabelColorKey( GrxColorKey *key );

void grxSetColorKeyMap( GrxColorKey *key, float zmin, float zmax );

unsigned long grxGetColorKey( GrxColorKey *key, float z );

GrxWindow *grxCreateColorKey( GrxColorKey *key, 
			      GrxWindow *parent,
			      int x, int y, unsigned int w, unsigned int h,
			      float zmin, float zmax,
			      unsigned long *colorgradient, int ncolors );

void grxSetXYFillMap( GrxWindow *w,
		      unsigned int u1, unsigned int u2,
		      unsigned int v1, unsigned int v2,
		      unsigned int ncolumns, unsigned int nrows );

void grxXYFill( GrxWindow *grw, unsigned int ncolumn, unsigned int nrow,
		unsigned long color );

void grxXYPattern( GrxWindow *grw, int ncolumns, int nrows,
		   unsigned long *colorgradient, int ncolors );

GrxWindow *grxCreateXYMap( GrxWindow *parent,
			   int x, int y, unsigned int w, unsigned int h,
			   unsigned int ncolumns, unsigned int nrows, 
			   unsigned long *colorgradient, int ncolors,
			   int (*func)(GrxWindow *, XEvent *), void *arg );

void grxXYGraph( GrxWindow *grw, GrxColorKey *key,
		 float *data,
		 unsigned int ncolumns, unsigned int ndata );
#endif
