#ifndef _GSC16AISS8AO4REGS_H_
#define _GSC16AISS8AO4REGS_H_

#define DAQDRV_SUBSYSTEM_ID_16AISS8AO4 0x3172

#define BOARDCLOCKDEFAULT_16AISS8AO4 (40.320E6)

#define PLXSIZE 0x108
#define CARDSIZE 0x80

#define FIFOSIZE 262144
#define FIFOMASK 0x7FFFF
#define FIFOOVFL 0x80000

#define MAXCLKDIV 0xFFFFFF
#define MINCLKDIV 0x14

//#define MAXINPUTCHANNELSPERCARD 8
//#define MAXOUTPUTCHANNELSPERCARD 4

//==============================================================================
// Board Register Addresses
//==============================================================================
// Define Board Register Addresses
#define       BCR                 0x00
#define       DIO_IO              0x04
#define       AOUT_00             0x08
#define       AOUT_01             0x0C
#define       AOUT_02             0x10
#define       AOUT_03             0x14
#define       AIN_DATA            0x18
#define       RATE_A              0x1C
#define       RATE_B              0x20
#define       AIN_CONFIG          0x24
#define       AIN_BUFF_SIZE       0x28
#define       AIN_BUFF_THRSHLD    0x2C
#define       ICR                 0x30
#define       BRD_CONFIG          0x34
#define       AUTOCAL             0x38
#define       BUFF_OUT_OPS        0x3C
#define       AOUT_BUFF_THRSHLD   0x40
#define       AOUT_BUFF_SIZE      0x44
#define       AOUT_DATA           0x48
#define       RATE_C	          0x4C

#define       ICR_AUTOCAL                   (1<<16)
#define       ICR_INPUT_HIGH_TO_LOW         (1<<17)
#define       ICR_INPUT_LOW_TO_HIGH         (1<<18)
#define       ICR_INPUT_OVERFLOW            (1<<19)
#define       ICR_INPUT_BURST_BUSY          (1<<20)
#define       ICR_INPUT_BURST_DONE          (1<<21)
#define       ICR_INPUT_CLOCK               (1<<22)
#define       ICR_OUTPUT_CLOCK              (1<<23)
#define       ICR_DIO_00                    (1<<24)
#define       ICR_OUTPUT_HIGH_TO_LOW        (1<<25)
#define       ICR_OUTPUT_LOW_TO_HIGH        (1<<26)
#define       ICR_OUTPUT_LOAD_DONE          (1<<27)
#define       ICR_OUTPUT_LOAD_READY         (1<<28)
#define       ICR_OUTPUT_BURST_READY        (1<<29)
#define       ICR_OUTPUT_OVERFLOW           (1<<30)

#define       ICR_AUTOCAL_SELECT                   (1<<0)
#define       ICR_INPUT_HIGH_TO_LOW_SELECT         (1<<1)
#define       ICR_INPUT_LOW_TO_HIGH_SELECT         (1<<2)
#define       ICR_INPUT_OVERFLOW_SELECT            (1<<3)
#define       ICR_INPUT_BURST_BUSY_SELECT          (1<<4)
#define       ICR_INPUT_BURST_DONE_SELECT          (1<<5)
#define       ICR_INPUT_CLOCK_SELECT               (1<<6)
#define       ICR_OUTPUT_CLOCK_SELECT              (1<<7)
#define       ICR_DIO_00_SELECT                    (1<<8)
#define       ICR_OUTPUT_HIGH_TO_LOW_SELECT        (1<<9)
#define       ICR_OUTPUT_LOW_TO_HIGH_SELECT        (1<<10)
#define       ICR_OUTPUT_LOAD_DONE_SELECT          (1<<11)
#define       ICR_OUTPUT_LOAD_READY_SELECT         (1<<12)
#define       ICR_OUTPUT_BURST_READY_SELECT        (1<<13)
#define       ICR_OUTPUT_OVERFLOW_SELECT           (1<<14)

#endif
