#include <stdio.h>
#include <stdlib.h>						
#include <math.h>
#include <string.h>
#include <regex.h>
#define TRUE	1
#define FALSE	0
#define NET_PATTERN "([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})/([1-9]{1}|([1-2][0-9])|([3][0-2]))"
//////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
	unsigned char octet[ 4 ];
	unsigned char maskoctet[ 4 ];
}IpV4Net;
const char *message[ 2 ] = { "Network does not fit IPv4 netowrk pattern!", "Network's IP address intersects with reverse mask!" };
//////////////////////////////////////////////////////////////////////////////////////////
// prototypes										//
//////////////////////////////////////////////////////////////////////////////////////////
int	ValidateAndExtract	( char *netstring, unsigned char *netpointer );
void	ExtractNetOctets	( char *netstring, regmatch_t *matches, unsigned char *netpointer );
void	ExtractMaskOctets	( char *netstring, regmatch_t *matches, unsigned char *maskpointer );
int	CheckMaskException	( unsigned char *netpointer );
void	ListAddresses		( unsigned char *netpointer );
void	QuitWithError		( const int exitcode, const char *string );
//////////////////////////////////////////////////////////////////////////////////////////
// main											//
//////////////////////////////////////////////////////////////////////////////////////////
int main ( int argc, char* argv[] ){
	unsigned char *netptr = malloc(sizeof(IpV4Net));
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
int ValidateAndExtract( char *netstring, unsigned char *netpointer ){
	regex_t netstringregex;
	regmatch_t matches[ 6 ];
	regcomp( &netstringregex, NET_PATTERN, REG_EXTENDED );
	if( regexec( &netstringregex, netstring, 6, matches, 0 ) == 0 ) {
		ExtractNetOctets  ( netstring, matches, netpointer   );
		ExtractMaskOctets ( netstring, matches, netpointer + 4 );
		return TRUE;
	} else return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////
void ExtractNetOctets( char *netstring, regmatch_t *matches, unsigned char *netpointer ){
	for( int i = 1; i < 5; i++, netpointer++ ){
		int buffer = atoi( netstring + matches[ i ].rm_so );
		memmove( netpointer, (char *) &buffer, 1 );
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
void ExtractMaskOctets( char *netstring, regmatch_t *matches, unsigned char *maskpointer ){
	for( int maskbitcounter = atoi( netstring + matches[ 5 ].rm_so ); maskbitcounter > 0; maskbitcounter -= 8, maskpointer++ ){
		int currentoctet = pow ( 2, (maskbitcounter < 8 ? maskbitcounter : 8 ) ) - 1;
		if( maskbitcounter < 8 ) currentoctet <<= ( 8 - maskbitcounter );
		memmove( maskpointer, (char *)&currentoctet, 1 );
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
int CheckMaskException( unsigned char* netpointer ){
	for ( unsigned char *maskpointer = netpointer + 4, i = 0; i < 4; netpointer++, maskpointer++, i++ )
		if( ( ( *netpointer ) & ( ~*maskpointer ) ) != 0 ) return FALSE;	
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////////
void ListAddresses( unsigned char *net ){
	for( int octet1 = *net, counter = (unsigned char) ~* ( net + 4 ); counter >= 0; octet1++, counter-- )
		for( int octet2 = *( net + 1), counter = (unsigned char) ~* ( net + 5 ); counter >= 0; octet2++, counter-- )
			for( int octet3 = *( net + 2 ), counter = (unsigned char) ~* ( net + 6 ); counter >= 0; octet3++, counter-- )
				for( int octet4 = *( net + 3 ), counter = (unsigned char) ~* ( net + 7 ); counter >= 0; octet4++, counter-- ){
					printf( "%u.%u.%u.%u\n", octet1, octet2, octet3, octet4 );
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
void QuitWithError( const int exitcode, const char *string ){
	fprintf( stderr, "\x1b[31;1m/!\\\x1b[0m\t%-50.50s  \x1b[31;1m%20.20s\x1b[0m\n", message[ exitcode - 1 ], string );
	exit( exitcode );
}
//////////////////////////////////////////////////////////////////////////////////////////
