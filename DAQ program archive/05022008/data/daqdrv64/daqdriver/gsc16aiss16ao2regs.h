#ifndef _GSC16AISS16AO2REGS_H_
#define _GSC16AISS16AO2REGS_H_

#include "gsc16aiss8ao4regs.h"

#define DAQDRV_SUBSYSTEM_ID_16AISS16AO2 0x3243
#define BOARDCLOCKDEFAULT_16AISS16AO2 (45.0E6)
#define WARPLIMIT_16AISS16AO2 (57)

// Registers that are different from the aiss8o4

#define       AIN_BURST_SIZE      0x24
#define       AIN_ACTIVE_CHANNELS 0x14

#endif
