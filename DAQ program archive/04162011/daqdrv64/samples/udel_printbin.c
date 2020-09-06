#define XK_MISCELLANY
#define XK_LATIN1

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
#include <X11/keysymdef.h>
#include <X11/X.h>
#include <X11/Xlib.h>
int main(int argc, char **argv)
{
  UDELdaqheader fileheader, filefin;
  DaqBoard board;
  UDELctl uctl;
  
  udelctlinit(&uctl);
  uctl.flag_disp=1;
  board.ctl.ictl.vfs=10;
  board.ctl.octl.vfs=10;
  udelargparser( argc, argv, &board, &uctl );
  
  
  
  
  FILE * pdaqfile;
  pdaqfile = fopen(uctl.daqfile,"rb+");

  printf("%s\n",uctl.daqfile);
  if (pdaqfile == NULL ) 
  {
    printf("daq file open error!\n");
    exit(1);
  }

  fseek( pdaqfile, 0, SEEK_END);
  int daqfilebytesize = ftell(pdaqfile);
  fseek( pdaqfile, 0, SEEK_SET);

  fread( &fileheader, sizeof( UDELdaqheader ), 1, pdaqfile );
  
  

  if (uctl.flag_verbose)
  {
    fseek( pdaqfile, -sizeof( UDELdaqheader ), SEEK_END );
    fread( &filefin, 1, sizeof( UDELdaqheader ), pdaqfile );
    //ix1 = fread( ix2, sizeof( char ), 2, pdaqfile);
    fseek( pdaqfile, sizeof( UDELdaqheader ), SEEK_SET );	
    printf( "recording start time(UTC) = %s \n", fileheader.UTCtime );
    printf( "recording stop  time(UTC) = %s \n", filefin.UTCtime );
    if ( uctl.flag_verbose == 2)
    {
      udelprintinfo( &fileheader.board.ctl );
      udelprintstatus( &fileheader.board.status );
      udelprintinfo( &filefin.board.ctl );
      udelprintstatus( &filefin.board.status );
      
    }
    else
    {
      daqCtlPrint(stdout, &fileheader.board.ctl );
      daqStatusPrint( stdout, &fileheader.board.status );
      daqCtlPrint(stdout, &filefin.board.ctl );
      daqStatusPrint( stdout, &filefin.board.status );
    }  
  }
  if (uctl.flag_disp == 0) exit(0);



//-----------------------------------------------------------------------
  UDELxwin uxwin;
  
  udelxwininit( &uxwin);

  int bytepersample = sizeof(unsigned short);
  int  ix,  NDATA;
  float *rdata;
  unsigned int * buff;
  XEvent uevent;
  KeySym key_symbol;


  NDATA=16*fileheader.board.ctl.ictl.nchans;
  printf("NDATA=%d %d\n ", NDATA, bytepersample);
  buff = malloc( NDATA * bytepersample );
  rdata = malloc( NDATA * sizeof(float));
  fread(buff, bytepersample, NDATA, pdaqfile);
  for (ix =0; ix< NDATA; ix++) rdata [ix] = udelraw2float( (unsigned int)buff[ix], fileheader.board.ctl.ictl.vfs );
  udeldrawdata(&uxwin, &uctl,  &(fileheader.board),rdata, NDATA);
  float offset_step = uctl.xwin_fs/10.0;
  UDELctl original_uctl = uctl;
  while(1)
  {
    XNextEvent(uxwin.display, &uevent);
    switch (uevent.type)
    {
      case KeyPress:
        key_symbol = XKeycodeToKeysym(uxwin.display, uevent.xkey.keycode, 0);
        switch (key_symbol) 
        {
          case XK_Home:
            uctl = original_uctl;
            NDATA=16*fileheader.board.ctl.ictl.nchans;
            fread(buff, bytepersample, NDATA, pdaqfile);
            for (ix =0; ix< NDATA; ix++) rdata [ix] = udelraw2float( buff[ix], fileheader.board.ctl.ictl.vfs );
            udeldrawdata(&uxwin, &uctl, &(fileheader.board), rdata, NDATA);
            break;
          case XK_w:
            uctl.xwin_offset -= offset_step;
            udeldrawdata(&uxwin, &uctl,  &(fileheader.board),rdata, NDATA);
            break;
          case XK_s:
            uctl.xwin_offset += offset_step;
            udeldrawdata(&uxwin, &uctl,  &(fileheader.board),rdata, NDATA);
            break;
          case XK_a:
            fseek(pdaqfile, -NDATA*bytepersample, SEEK_CUR);
            if (ftell(pdaqfile)- NDATA*bytepersample - sizeof(UDELdaqheader) > 0)
            {
              fseek(pdaqfile, -NDATA*bytepersample, SEEK_CUR);
            }
            else fseek(pdaqfile, sizeof(UDELdaqheader), SEEK_SET);
            fread(buff, bytepersample, NDATA, pdaqfile);
            for (ix =0; ix< NDATA; ix++) rdata [ix] = udelraw2float( buff[ix],fileheader.board.ctl.ictl.vfs );
            udeldrawdata(&uxwin, &uctl,  &(fileheader.board),rdata, NDATA);
            break;
          case XK_d:
            if (ftell(pdaqfile)+ NDATA*bytepersample + sizeof(UDELdaqheader) > daqfilebytesize)
            {
              fseek(pdaqfile, -NDATA*bytepersample-sizeof(UDELdaqheader), SEEK_END);
            }
            fread(buff, bytepersample, NDATA, pdaqfile);
            for (ix =0; ix< NDATA; ix++) rdata [ix] = udelraw2float( buff[ix],fileheader.board.ctl.ictl.vfs );
            udeldrawdata(&uxwin, &uctl,  &(fileheader.board),rdata, NDATA);
            break;
          case XK_Up:
            uctl.xwin_fs /= 2;
            offset_step = uctl.xwin_fs /10.;
            udeldrawdata(&uxwin, &uctl,  &(fileheader.board),rdata, NDATA);
            break;
          case XK_Down:
            uctl.xwin_fs *= 2;
            offset_step = uctl.xwin_fs /10.;
            udeldrawdata(&uxwin, &uctl,  &(fileheader.board),rdata, NDATA);
            break;
          case XK_Left:
            fseek(pdaqfile, -NDATA*bytepersample, SEEK_CUR);
            NDATA = NDATA/2;
            if (NDATA<16*fileheader.board.ctl.ictl.nchans) NDATA=16*fileheader.board.ctl.ictl.nchans;
            fread(buff, bytepersample, NDATA, pdaqfile);
            for (ix =0; ix< NDATA; ix++) rdata [ix] = udelraw2float( buff[ix],fileheader.board.ctl.ictl.vfs );
            udeldrawdata(&uxwin, &uctl,  &(fileheader.board),rdata, NDATA);
            break;
          case XK_Right:
            fseek(pdaqfile, -NDATA*bytepersample, SEEK_CUR);
            NDATA = NDATA*2;
            if (NDATA>UDELdaqblocksize) NDATA=UDELdaqblocksize;
            free( rdata );
            free( buff );
            buff = malloc( NDATA * bytepersample );
            rdata = malloc( NDATA * sizeof(float));
            fread(buff, bytepersample, NDATA, pdaqfile);
            for (ix =0; ix< NDATA; ix++) rdata [ix] = udelraw2float( buff[ix],fileheader.board.ctl.ictl.vfs );
            udeldrawdata(&uxwin, &uctl,  &(fileheader.board),rdata, NDATA);
            break;

          case XK_Escape:
            fclose(pdaqfile);
            XCloseDisplay(uxwin.display);
            free(rdata);
            free(buff);
            exit(0);
        }
    }
  }  
}
