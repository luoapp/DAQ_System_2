/* daqdrv.h

   Copyright 2006, Mitchell C. Nelson, Ph.D.

*/

#ifndef _DAQDRV_H_
#define _DAQDRV_H_

#define DAQRTSIG 5

#define MAXDAQDEV 4

//#define MAXDAQINCHANS (MAXDAQDEV*MAXINPUTCHANNELSPERCARD)
//#define MAXDAQOUTCHANS (MAXDAQDEV*MAXOUTPUTCHANNELSPERCARD)
#define MAXDAQOUTCHANS 64

// status bits
#define SEMINIT 1
#define IRQCNCT 2
#define DEVOPEN 4
#define DMAMEM  8

#define INPUTOVERFLOW  0x10
#define OUTPUTOVERFLOW 0x20
#define IOERROR        0xF0

typedef struct daqboardconfig {
  // board assembly configuration
  int   version;
  int   inchans;
  int   outchans;
  float clock;
} DaqBoardConfig;

typedef struct daqaioctl {
  float vfs;
  float clockrate;
  int   ndiv;
  int   clkmaster;
  int   nmode;
  int   nrange;
  int   nchans;
  int   nburst;
  int   nthreshold;
  int   ndata;
} DaqAioCtl;

typedef struct daq_ctl {

  DaqBoardConfig cfg;
  DaqAioCtl      ictl;
  DaqAioCtl      octl;
  int            trigmaster;
} DAQDRV_Ctl;

typedef struct daqdrv_regreq {
  unsigned int offset;
  unsigned int value;
} DAQDRV_RegReq;

typedef struct daqdrv_fiforeq {
  void *data;
  int ndata;
} DAQDRV_Fifo;

typedef struct daqdrv_status {
  unsigned int status;
  unsigned int intstatus;
  unsigned int dma0counter;
  unsigned int dma1counter;
  unsigned int dma0overflow;
  unsigned int dma1overflow;
  unsigned int userdata;
  unsigned int useraddr0;
  unsigned int useraddr1;
  unsigned int usersize0;
  unsigned int usersize1;
} DAQDRV_Status;


#ifndef DAQOFFLINE

#include <asm/ioctl.h>

#define DAQDRV_IOC_NUMBER 0XAA

#define DAQDRV_IOC_REGISTER_READ     _IOR( DAQDRV_IOC_NUMBER,  1, long )
#define DAQDRV_IOC_REGISTER_WRITE    _IOW( DAQDRV_IOC_NUMBER,  2, long )
#define DAQDRV_IOC_REGISTER_OR       _IOWR( DAQDRV_IOC_NUMBER, 3, long )
#define DAQDRV_IOC_REGISTER_AND      _IOWR( DAQDRV_IOC_NUMBER, 4, long )
#define DAQDRV_IOC_REGISTER_TOGGLE   _IOWR( DAQDRV_IOC_NUMBER, 5, long )

#define DAQDRV_IOC_PLX_READ          _IOR( DAQDRV_IOC_NUMBER,  6, long )
#define DAQDRV_IOC_PLX_WRITE         _IOW( DAQDRV_IOC_NUMBER,  7, long )
#define DAQDRV_IOC_PLX_OR            _IOWR( DAQDRV_IOC_NUMBER, 8, long )
#define DAQDRV_IOC_PLX_AND           _IOWR( DAQDRV_IOC_NUMBER, 9, long )
#define DAQDRV_IOC_PLX_TOGGLE        _IOWR( DAQDRV_IOC_NUMBER, 10, long )

#define DAQDRV_IOC_STATUS_READ       _IOWR( DAQDRV_IOC_NUMBER, 11, long )
#define DAQDRV_IOC_CONTROLS_READ     _IOWR( DAQDRV_IOC_NUMBER, 12, long )
#define DAQDRV_IOC_CONTROLS_WRITE    _IOWR( DAQDRV_IOC_NUMBER, 13, long )

#define DAQDRV_IOC_SET_USERDATA      _IOWR( DAQDRV_IOC_NUMBER, 14, long )

#define DAQDRV_IOC_SIGINTEST         _IOWR( DAQDRV_IOC_NUMBER, 15, long )
#define DAQDRV_IOC_SIGOUTTEST        _IOWR( DAQDRV_IOC_NUMBER, 16, long )

#define DAQDRV_IOC_FIFO_READPIO      _IOWR( DAQDRV_IOC_NUMBER, 17, long )
#define DAQDRV_IOC_FIFO_WRITEPIO     _IOWR( DAQDRV_IOC_NUMBER, 18, long )

#define DAQDRV_IOC_ADD_INPUT_BUFFER        _IOWR( DAQDRV_IOC_NUMBER, 21, long )
#define DAQDRV_IOC_RELEASE_INPUT_BUFFERS   _IOWR( DAQDRV_IOC_NUMBER, 22, long )

#define DAQDRV_IOC_START_INPUT_DMA         _IOWR( DAQDRV_IOC_NUMBER, 23, long )
#define DAQDRV_IOC_CANCEL_INPUT_DMA        _IOWR( DAQDRV_IOC_NUMBER, 24, long )

#define DAQDRV_IOC_ADD_OUTPUT_BUFFER       _IOWR( DAQDRV_IOC_NUMBER, 25, long )
#define DAQDRV_IOC_RELEASE_OUTPUT_BUFFERS  _IOWR( DAQDRV_IOC_NUMBER, 26, long )

#define DAQDRV_IOC_START_OUTPUT_DMA        _IOWR( DAQDRV_IOC_NUMBER, 27, long )
#define DAQDRV_IOC_CANCEL_OUTPUT_DMA       _IOWR( DAQDRV_IOC_NUMBER, 28, long )

#define DAQDRV_IOC_SIGMSGTEST              _IOWR( DAQDRV_IOC_NUMBER, 30, long )
#define DAQDRV_IOC_SIGERRTEST              _IOWR( DAQDRV_IOC_NUMBER, 31, long )
#define DAQDRV_IOC_SIGPRITEST              _IOWR( DAQDRV_IOC_NUMBER, 32, long )

#define DAQDRV_IOC_TEST_INPUT_BUFFER       _IOWR( DAQDRV_IOC_NUMBER, 33, long )
#define DAQDRV_IOC_TEST_OUTPUT_BUFFER      _IOWR( DAQDRV_IOC_NUMBER, 34, long )
#define DAQDRV_IOC_TEST_DMA                _IOWR( DAQDRV_IOC_NUMBER, 35, long )

#define DAQDRV_IOC_TRIGGER_INPUT           _IOWR( DAQDRV_IOC_NUMBER, 41, long )
#define DAQDRV_IOC_TRIGGER_OUTPUT          _IOWR( DAQDRV_IOC_NUMBER, 42, long )
#define DAQDRV_IOC_TRIGGER_INPUTOUTPUT     _IOWR( DAQDRV_IOC_NUMBER, 43, long )

#define DAQDRV_IOC_SUBSYSTEMID             _IOWR( DAQDRV_IOC_NUMBER, 50, long )

#endif // DAOFFLINE

#endif

