/* daqdrv 

  daqregs.c

   Copyright 2006, Mitchell C. Nelson, Ph.D.

   This software is confidential and proprietary. This software is provided
   only subject to specific written agreements. No rights whatsoever are
   hereby provided.

   No part of this software may be copied, stored, reproduced or transmitted in
   any form or by any means without the written permission of the author.

   No warranty or representation is provided of suitability for any purpose
   whatsoever.
      NOTE:
        
*/

#ifndef _DAQREGS_H_
#define _DAQREGS_H_

int daqSubsystemid( int fd, unsigned int *value );

// ====================================================================

int daqRegisterRead( int fd, unsigned int offset, unsigned int *value );

int daqRegisterWrite( int fd, unsigned int offset, unsigned int value );

int daqRegisterAnd( int fd, unsigned int offset, unsigned int value );

int daqRegisterOr( int fd, unsigned int offset, unsigned int value );

int daqRegisterToggle( int fd, unsigned int offset, unsigned int value );

// ====================================================================

int daqFifoReadPio( int fd, unsigned int *data, int ndata );

int daqFifoWritePio( int fd, unsigned int *data, int ndata );

// ====================================================================

int daqPlxRead( int fd, unsigned int offset, unsigned int *value );

int daqPlxWrite( int fd, unsigned int offset, unsigned int value );

int daqPlxAnd( int fd, unsigned int offset, unsigned int value );

int daqPlxOr( int fd, unsigned int offset, unsigned int value );

int daqPlxToggle( int fd, unsigned int offset, unsigned int value );

// ====================================================================

int daqRegisterPrint( int fd, unsigned int offset );

int daqPlxPrint( int fd, unsigned int offset );

int daqDump( int fd );

int daqDumpFormatted( int fd );


#endif
