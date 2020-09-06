#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
  int ix;
  unsigned int *outbuffer = NULL;
  for( ix = 1024; ix < pow(2,31); ix=ix*2 )
    {
      outbuffer = malloc(ix*sizeof(float)) ;
      if ( !outbuffer ) { printf( "malloc outbuffer failed (%dB), %f\n",ix, ix/100e3/8/60); exit(1); }
      free(outbuffer);
    }
}
	
