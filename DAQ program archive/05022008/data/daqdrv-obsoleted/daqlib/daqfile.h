// daqfile.h
// Copyright 2006 Mitchell C. Nelson, Ph.D.

#ifndef _DAQFILE_H_
#define _DAQFILE_H_

#include <stdio.h>

#include "daqdrv.h"
//#include "daqwaveforms.h"

#define DAQTAG "daq1.0:"
#define DAQCFG "daq.cfg"

#define DAQFLOATDATA 1
#define DAQRAWDATA 2
#define DAQUSHORTDATA 4

int daqCloseFile( FILE *fd );

FILE *daqOpenInputFile( char *filename );

FILE *daqOpenOutputFile( char *filename );

// ======================================================

int daqWriteHeader( FILE *fd, DAQDRV_Ctl *ctl, char *commments );

int daqWriteDataFloat( FILE *fd, void *p, DAQDRV_Ctl *ctl );

int daqWriteDataRaw( FILE *fd, void *p, DAQDRV_Ctl *ctl );

// ======================================================

int daqWriteDataBinary_( FILE *fd, unsigned int *udata, int ndata );

int daqWriteDataBinary( FILE *fd, void *p, DAQDRV_Ctl *ctl );

// ======================================================

int daqWriteFrame( FILE *fd, void *data, DAQDRV_Ctl *ctl, char *commments );

int daqWriteFrameRaw( FILE *fd, void *data, DAQDRV_Ctl *ctl, char *commments );

int daqWriteFrameBinary( FILE *fd, void *data, DAQDRV_Ctl *ctl, char *commments );

// ======================================================

int daqReadHeader( FILE *fd, DAQDRV_Ctl *ctl, char *commments, int maxc );

// ======================================================

int daqReadDataBinary_( FILE *fd, float *data, int ndata, float vfs );

int daqReadDataType( FILE *fd, int *ndata );

int daqReadData( FILE *fd, float *data, int maxdata, float vfs );

float *daqReadDataMalloc( FILE *fd, int *pndata, float vfs );

float *daqReadDataRealloc( FILE *fd, float *old, int *pndata, float vfs );

// ======================================================

int daqReadFrame( FILE *fd, float *data, int maxdata, DAQDRV_Ctl *ctl, char *comments, int maxc );

float *daqReadFrameMalloc( FILE *fd, DAQDRV_Ctl *ctl, char *comments, int maxc, int *pndata );

float *daqReadFrameRealloc( FILE *fd, DAQDRV_Ctl *ctl, char *comments, int maxc, void *old, int *pndata );

// ======================================================

int daqLoadCfg( DAQDRV_Ctl *ctl, char *comments, int maxc, char *filename );

int daqSaveCfg( char *filename, DAQDRV_Ctl *ctl, char *comments );

#endif
