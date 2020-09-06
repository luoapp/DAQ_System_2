#include "daqdrv.h"

#define UDELdaqblocksize 262144
#define UDELdaqfilesize  (2147483648/2)  //2147483648
#define xwin_bufferlen 1024

#define UDELsignature "UDELdaq"

typedef struct udeldaqheader {
   char				udel_sig[8];		//UDELdaq
	char				UTCtime[32];
	DaqBoard		   board;
   unsigned int   sample_per_block;   
   unsigned int   block_requested;
} UDELdaqheader;

typedef struct udelarg {
   char           arg[32];
   int *          ipv;
   float *        fpv;
} UDELarg;

FILE * udeldaqfileinit( DaqBoard *, unsigned int);
int udeldaqfileclose( FILE * , DaqBoard *);
float udelraw2float ( unsigned int , float );
void udelprintinfo ( DAQDRV_Ctl * );
void udelprintstatus ( DAQDRV_Status * );

void udeltime( char * );
void * xwin_drawthread ( void );

	