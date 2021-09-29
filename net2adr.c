#include <stdio.h>	// printf(), fprintf(), stderr
#include <stdlib.h>	// atoi()
#include <math.h>	// pow()
#include <string.h>	// memmove()
#include <regex.h>	// regexec(), regcomp(), regex_t, regmatch_t
#define TRUE	1
#define FALSE	0
#define NET_PATTERN "^([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})/([1-9]{1}|([1-2][0-9])|([3][0-2]))$"
//////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
	unsigned int address;
	unsigned int revmask;
}IpV4Net;
const char *message[ 2 ] = { "Network does not fit IPv4 netowrk pattern!", "Network's IP address intersects with reverse mask!" };
//////////////////////////////////////////////////////////////////////////////////////////
// prototypes										//
//////////////////////////////////////////////////////////////////////////////////////////
int	ValidateAndExtract	( char *, unsigned int * );
void	ExtractNet		( char *, regmatch_t *, unsigned int * );
void	ExtractMask		( char *, regmatch_t *, unsigned int * );
int	CheckMaskException	( unsigned int * );
void	ListAddresses		( unsigned int * );
void	QuitWithError		( const int, const char * );
//////////////////////////////////////////////////////////////////////////////////////////
// main											//
//////////////////////////////////////////////////////////////////////////////////////////
int main ( int argc, char* argv[] ){
	unsigned int *netptr = malloc(sizeof(IpV4Net));
	if( ValidateAndExtract( argv[ 1 ], netptr ) ){
		if( CheckMaskException( netptr ) ){
			ListAddresses( netptr );
			exit( 0 );
		} else QuitWithError( 2, argv[ 1 ] );
	} else QuitWithError( 1, argv[ 1 ] );
}
//////////////////////////////////////////////////////////////////////////////////////////
// functions										//
//////////////////////////////////////////////////////////////////////////////////////////
int ValidateAndExtract( char *netstring, unsigned int *netpointer ){
	regex_t netstringregex;
	regmatch_t matches[ 6 ];
	regcomp( &netstringregex, NET_PATTERN, REG_EXTENDED );
	if( regexec( &netstringregex, netstring, 6, matches, 0 ) == 0 ) {
		ExtractNet  ( netstring, matches, netpointer   );
		ExtractMask ( netstring, matches, netpointer + 1 );
		return TRUE;
	} else return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////
void ExtractNet( char *netstring, regmatch_t *matches, unsigned int *net ){
	unsigned int address = 0;
	for( unsigned int i = 1, octet = atoi( netstring + matches[ i ].rm_so ); i < 5; address += ( octet * pow ( 256, 4 - i ) ), octet = atoi (netstring + matches[ ++i ].rm_so ) ){ }
	memmove( net, (unsigned int *)&address, sizeof(unsigned int));
}
//////////////////////////////////////////////////////////////////////////////////////////
void ExtractMask( char *netstring, regmatch_t *matches, unsigned int *mask ){
	unsigned int revmask = (unsigned int) pow( 2, 32 - atoi( netstring + matches[5].rm_so ) ) - 1;
	memmove( mask, (unsigned int*)&revmask, sizeof(unsigned int) );
}
//////////////////////////////////////////////////////////////////////////////////////////
int CheckMaskException( unsigned int *net ){
	if( ( *net & *(net + 1) ) != 0 ) return FALSE;
	else return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////////
void ListAddresses( unsigned int *net ){
	for(unsigned int finalip = *net | *( net + 1), ip = *net; ip <= finalip; ip++)
		printf( "%u.%u.%u.%u\n", (ip&4278190080)>>24, (ip&16711680)>>16, (ip&65280)>>8 , ip&255 );
}
//////////////////////////////////////////////////////////////////////////////////////////
void QuitWithError( const int exitcode, const char *netstring ){
	fprintf( stderr, "\x1b[31;1m/\x1b[33;1m!\x1b[31;1m\\\x1b[0m\t%-50.50s \x1b[31;1m%20.20s\x1b[0m\n", message[ exitcode - 1 ], netstring );
	exit( exitcode );
}
//////////////////////////////////////////////////////////////////////////////////////////
