// daqparse.h
// Copyright 2006 Mitchell C. Nelson, Ph.D.

#ifndef _DAQPARSE_H_
#define _DAQPARSE_H_

#include <stdio.h>

#include "daqdrv.h"
#include "daqwaveforms.h"

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

int daqUnparse( char *s, DAQDRV_Ctl *ctl, WAVESPEC *wspec, int maxwspec );

int daqParse( char *s, DAQDRV_Ctl *ctl, WAVESPEC *wspec, int maxwspec );


#endif
