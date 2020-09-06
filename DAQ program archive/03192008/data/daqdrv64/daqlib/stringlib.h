/* stringlib.h

   Copyright 2006, Mitchell C. Nelson, Ph.D.

   This software is confidential and proprietary. This software is provided
   only subject to specific written agreements. No rights whatsoever are
   hereby provided.

   No part of this software may be copied, stored, reproduced or transmitted in
   any form or by any means without the written permission of the author.

   No warranty or representation is provided of suitability for any purpose
   whatsoever.

*/

#ifndef _STRINGLIB_H_
#define _STRINGLIB_H_

#include <stdio.h>

char *strNext( char *s );

// copy string until newline or null

int strCopy( char *out, char *in );

int strTokenCopy( char *out, char *s );

// match string to key, no leading spaces

int strMatch( char *s, char *key );

int strMatchPrefix( char *s, char *key );

int strMatchList( char *s, char *list[] );

int strTrim( char *s );


// return string after first occurence of key

char *strAfter( char *s, char *key );

// match next token to key, return token if matched or return current string

char *strArg( char *s, char *key );

// numerical value parsing, requires space or eol termination.

int strDbl( char *s, double *d );

int strFlt( char *s, float *f );

int strInt( char *s, int *i );

int strUInt( char *s, unsigned int *u );

int strOUInt( char *s, unsigned int *u );

// numerical values with range checking

int strFltRange( char *s, float *f, float fmax, float fmin );

int strIntRange( char *s, int *i, int imax, int imin );

int strUIntRange( char *s, unsigned int *u, unsigned int umax, unsigned int umin );

// these accept numerical values followed by non-space characters

int strFltPrefix( char *s, float *f );

int strIntPrefix( char *s, int *i );

int strUIntPrefix( char *s, unsigned int *i );

// numerical value after first occurence of key

int strFltAfter( char *s, char *key, float *val );

int strIntAfter( char *s, char *key, int *val );

// numerical value after matching token

int strFltArg( char *s, char *key, float *val );

int strIntArg( char *s, char *key, int *val );

int strFltVector( char *s, float *data, int ndata );

int strIntVector( char *s, int *data, int ndata );

int strUIntVector( char *s, unsigned int *data, int ndata );

int readFltCols( FILE *fin, float *fltdata, int maxdata, int *ncols );

int readFlt( FILE *fin, float *data, int ndata );

int readUInt( FILE *fin, unsigned int *data, int ndata );

int readStr( FILE *fin, char *s, int nmax );

#endif
