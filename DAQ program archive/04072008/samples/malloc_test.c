#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
  int ix;
  unsigned int *outbuffer = NULL;
  for( ix = 10; ix < 2^10; ix++ )
    {
      daqMallocAligned( outbuffer, 2^20*ix*sizeof(unsigned int) );
      if ( !outbuffer ) { printf( "malloc outbuffer failed (%dMB)",ix); exit(1); }
    }
}
	
