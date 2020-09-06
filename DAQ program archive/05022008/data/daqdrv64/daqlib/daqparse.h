/* daqparse.h


   Copyright 2006, Mitchell C. Nelson, Ph.D.

   This software is confidential and proprietary. This software is provided
   only subject to specific written agreements. No rights whatsoever are
   hereby provided.

   No part of this software may be copied, stored, reproduced or transmitted in
   any form or by any means without the written permission of the author.

   No warranty or representation is provided of suitability for any purpose
   whatsoever.
*/

#ifndef _DAQPARSE_H_
#define _DAQPARSE_H_

#include <stdio.h>

#include "daqdrv.h"
//#include "daqwaveforms.h"

int daqCtlParse_( DaqBoardConfig *cfg, DaqAioCtl *ctl, char *s );

int daqCtlParse( DAQDRV_Ctl *ctl, char *s );

// ============================================================================

int daqCtlHelpSprint( char *s );

int daqCtlHelpPrint( );

// ============================================================================

int daqAioCtlSprint( char *s, DaqAioCtl *a );

int daqCtlSprint( char *s, DAQDRV_Ctl *ctl );

int daqCtlPrint( FILE *fd, DAQDRV_Ctl *ctl );

// ============================================================================

int daqStatusPrint( FILE *fout, DAQDRV_Status *p );

// ============================================================================

//int daqUnparse( char *s, DAQDRV_Ctl *ctl, WAVESPEC *wspec, int maxwspec );

//int daqParse( char *s, DAQDRV_Ctl *ctl, WAVESPEC *wspec, int maxwspec );


#endif
