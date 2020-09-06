#include "daqdrv.h"
#include <X11/Xlib.h>

#define UDELdaqblocksize 262144
#define UDELdaqfilesize  (2147483648/2)  //2147483648
#define xwin_bufferlen 1024

#define UDELsignature "UDELdaq"

typedef struct udeldaqheader {
   char				udel_sig[8];		//UDELdaq
	char				UTCtime[32];
	DaqBoard		   board;
   unsigned int   sample_per_block;   
   unsigned int   block_requested;
} UDELdaqheader;

typedef struct udelarg {
   char           arg[32];
   int *          ipv;
   float *        fpv;
} UDELarg;

typedef struct udelctl{
  float length_in_sec;
  float input_nominal_sr;
  float output_nominal_sr;
  int flag_hd;
  int flag_disp;
  float xwin_pd;
  float xwin_fs;
  float xwin_fps;
  float xwin_offset;
  float xwin_chan;
  char daqfile[64];
  float flag_verbose;
  float out_freq;
  float out_ampl;
} UDELctl;

typedef struct udelxwin{
  Display* display;	
  Window win;			
  GC gc;
  XColor red;
  XColor blue;
  XColor green;
  XColor white;
  XColor black;
  unsigned int win_width;
  unsigned int win_height;
} UDELxwin;

FILE * udeldaqfileinit( DaqBoard *, unsigned int);
int udeldaqfileclose( FILE * , DaqBoard *);
float udelraw2float ( unsigned int , float );
void udelprintinfo ( DAQDRV_Ctl * );
void udelprintstatus ( DAQDRV_Status * );

Window create_simple_window(Display* display, int width, int height, int x, int y);
GC create_gc(Display* display, Window win, int reverse_video);

void udeltime( char * );
int genwaveforms( unsigned int *raw, float f, float a, float vfs, int nsamples, float outrate, int nchans );
void udelargparser(int argc, char **argv, DaqBoard * pb, UDELctl * pu );	
void udelctlinit(UDELctl * puctl);
void udelxwininit( UDELxwin * puxwin);
void udeldrawdata(UDELxwin * puxwin, UDELctl * puctl, DaqBoard * pboard, float *rdata, int ndata);
