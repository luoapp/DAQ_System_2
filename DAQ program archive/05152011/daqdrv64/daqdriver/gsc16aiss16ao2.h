/* daqdrv

   gsc daq card driver

   Copyright 2007, Mitchell C. Nelson, Ph.D.

   No representation is provided of suitability for any purpose whatsoever.

*/

#ifndef _GSC16AISS16AO2_H_
#define _GSC16AISS16AO2_H_

#include <asm/io.h>
#include <linux/sched.h>

#include "gsc16aiss16ao2regs.h"

void daqReadCtls_16aiss16ao2( void *iobase, DAQDRV_Ctl *b );
void daqWriteCtls_16aiss16ao2( void *iobase, DAQDRV_Ctl *b );
void daqInputTrigger_16aiss16ao2( void *iobase );
void daqOutputTrigger_16aiss16ao2( void *iobase );
void daqInputOutputTrigger_16aiss16ao2( void *iobase );

#endif
