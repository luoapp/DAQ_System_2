/* daqdrv

   daqdrv component for gsc daq card

   Dr. Mitchell C. Nelson

*/

#include <asm/uaccess.h>
#include <linux/sched.h>

#include "plx_regs.h"

#include "daqdrvmain.h"
#include "daqdrv.h"

#include "gsc16aiss16ao2regs.h"
#include "gsc16aiss8ao4.h"

#define LIMIT( a, max, min ) ( a > max ? max : ( a < min ? min : a) )

// ===================================================================

static
int countbits( unsigned int uval )
{
  int n = 0;

  while( uval ) {
    n += ( uval & 1 );
    uval = uval >> 1;
  }

  return( n );
}

/* Voltage Scale translations
*/

static
float gscVfs_16aiss16ao2( int range )
{
  float vfs[3] = { 2.5f, 5.f, 10.f };
  
  if ( range > 2 ) range = 2;
  else if ( range < 0 ) range = 0;
  
  return( vfs[range] );
}

static
void gscCheckCtl_16aiss16a02_( DaqAioCtl *ctl, int maxchannels )
{
  ctl->nchans = LIMIT( ctl->nchans, maxchannels, 0 );

  ctl->nburst = LIMIT( ctl->nburst, 0xFFFFFF, 0 );

  ctl->ndiv   = LIMIT( ctl->ndiv, 0xFFFFFF, 0x14 );

  ctl->nrange = LIMIT( ctl->nrange, 2, 0 );
}

static
void gscCheckCtls_16aiss16a02( DAQDRV_Ctl *b )
{
  DaqBoardConfig *cfg  = &(b->cfg);
  DaqAioCtl      *ictl = &(b->ictl);
  DaqAioCtl      *octl = &(b->octl);

  octl->nmode   = 0;
  octl->nburst  = 0;

  gscCheckCtl_16aiss16a02_( ictl, cfg->inchans );
  gscCheckCtl_16aiss16a02_( octl, cfg->outchans );
}

static
void daqReadBoardSpecs_16aiss16a02( void *iobase, DAQDRV_Ctl *b )
{
  unsigned int uval;

  uval = ioread32( (iobase+BRD_CONFIG) );
  
  b->cfg.version  = uval & 0xfff;
  b->cfg.inchans  = ((uval >> 16) & 1) ? 8 : 16;
  b->cfg.outchans = ((uval >> 17) & 1) ? 0 : 2;

  b->cfg.clock    = BOARDCLOCKDEFAULT_16AISS16AO2 ;
}

void daqReadCtls_16aiss16ao2( void *iobase, DAQDRV_Ctl *b )
{
  DaqBoardConfig *cfg  = &(b->cfg);
  DaqAioCtl      *ictl = &(b->ictl);
  DaqAioCtl      *octl = &(b->octl);
  
  unsigned int uval;

  daqReadBoardSpecs_16aiss16a02( iobase, b );

  uval = ioread32( (iobase+RATE_A) );

  ictl->ndiv   = uval & 0xFFFFFF;
  //  ictl->clockrate = gscClockRate( cfg->clock, ictl->ndiv );

  uval = ioread32( (iobase+AIN_ACTIVE_CHANNELS) );

  ictl->nchans = countbits( (uval&0xFFFF) );
  ictl->nchans = LIMIT( ictl->nchans, cfg->inchans, 0 );

  uval = ioread32( (iobase+AIN_BURST_SIZE) );

  ictl->nburst =  uval & 0xFFFFFF;
  ictl->ndata  = ictl->nchans * ictl->nburst;

  ictl->nthreshold = ioread32( (iobase+AIN_BUFF_THRSHLD) );

  uval = ioread32( (iobase+RATE_C) );

  octl->ndiv   = uval & 0xFFFFFF;
  //  octl->clockrate = gscClockRate( cfg->clock, octl->ndiv );

  uval = ioread32( (iobase+BUFF_OUT_OPS) );

  octl->nchans = countbits( (uval & 0x3) );
  octl->nchans = LIMIT( octl->nchans, cfg->outchans, 0 );

  octl->clkmaster = (uval >> 4) & 1;

  octl->nthreshold = ioread32( (iobase+AOUT_BUFF_THRSHLD) );

  uval = ioread32( (iobase+BCR) );

  ictl->nmode   = uval & 0x7;
  ictl->nrange  = (uval>>4) & 0x3;
  ictl->vfs     = gscVfs_16aiss16ao2( ictl->nrange );

  ictl->clkmaster = (uval >> 24) & 1;

  octl->nrange = (uval >> 16) & 3;
  octl->vfs    = gscVfs_16aiss16ao2( octl->nrange );

  octl->nburst  = 0;
  octl->nmode   = 0;

  b->trigmaster = (uval >> 21) & 1;
}

void daqWriteCtls_16aiss16ao2( void *iobase, DAQDRV_Ctl *b )
{
  DaqAioCtl      *ictl = &(b->ictl);
  DaqAioCtl      *octl = &(b->octl);

  unsigned int uval;

  daqReadBoardSpecs_16aiss16a02( iobase, b );

  gscCheckCtls_16aiss16a02( b );

  iowrite32( ictl->ndiv, (iobase+RATE_A) ); 

  uval = (1<<(ictl->nchans)) - 1;
  iowrite32( uval, (iobase+AIN_ACTIVE_CHANNELS) ); 

  uval = ictl->nburst;
  iowrite32( uval, (iobase+AIN_BURST_SIZE) ); 

  if( (ictl->nthreshold >= 0) && (ictl->nthreshold <= 0x40000) )
    {
      iowrite32( ictl->nthreshold , (iobase+AIN_BUFF_THRSHLD) );
    }

  iowrite32( octl->ndiv, (iobase+RATE_C) ); 

  uval = ((1<<(octl->nchans))-1);          // turn on everything
  uval |= octl->clkmaster ? 1<<4 : 0;      // configure clock initiator/target
  uval |= 1<<5;                            // enable output clocking
  uval |= 1<<11;                           // open and clear the output buffer, 
  uval |= 1<<19;                           // enable output burst

  iowrite32( uval, (iobase+BUFF_OUT_OPS) ); 

  if( (octl->nthreshold >= 0) && (octl->nthreshold <= 0x40000) )
    {
      iowrite32( octl->nthreshold , (iobase+AOUT_BUFF_THRSHLD) );
    }
      
  uval = ictl->nmode;                           // input mode select
  uval |= (ictl->nrange<<4);                    // input range A select
  uval |= (ictl->nrange<<6);                    // input range B select
  //  uval |= (ictl->nburst)? (1<<9) : 0;           // input burst enable, if burst
  uval |= 1<<9;                                 // input burst enable, always
  uval |= 3<<12;                                // input buffer enable and clear
  uval |= (octl->nrange<<16);                   // output range
  uval |= 1<<18;                                // simultaneous outputs
  uval |= 1<<19;                                // output fifo buffer enable
  uval |= b->trigmaster ? (1<<21) : 0;          // trigger master
  uval |= (1<<22);                              // Rate-C clock enable
  uval |= ictl->clkmaster ? (1<<24) : 0;        // input clock master
  uval |= (1<<25);                              // offset binary
  uval |= (1<<26);                              // Rate-A clock enable

  // special accomodation for clock rates over 800 KSPS
  if ( ictl->ndiv < WARPLIMIT_16AISS16AO2 ) uval |= (1<<30);

  iowrite32( uval, (iobase+BCR) ); 

  daqReadCtls_16aiss16ao2( iobase, b );
}

void daqInputTrigger_16aiss16ao2( void *iobase )
{
  unsigned int uval;

  uval  = ioread32( (iobase+BCR) );
  uval |= (1<<11);

  iowrite32( uval, (iobase+BCR) ); 
}

void daqOutputTrigger_16aiss16ao2( void *iobase )
{
  unsigned int uval;

  uval  = ioread32( (iobase+BCR) );
  uval |= (1<<23);

  iowrite32( uval, (iobase+BCR) ); 
}

void daqInputOutputTrigger_16aiss16ao2( void *iobase )
{
  unsigned int uval;

  uval  = ioread32( (iobase+BCR) );
  uval |= (1<<11) | (1<<23);

  iowrite32( uval, (iobase+BCR) ); 
}
