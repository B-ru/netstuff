#include <stdio.h>
#include <stdlib.h>						
#include <math.h>
#include <string.h>
#include <regex.h>
#define TRUE	1
#define FALSE	0
#define MSG_CAP	2
#define NET_PATTERN "^([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})/([1-9]{1}|([1-2][0-9])|([3][0-2]))$"
//////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
	unsigned char octet[4];
	unsigned char maskoctet[4];
}ipv4net;
const char *message[2] = {"Network does not fit IPv4 netowrk pattern!","Network's IP address intersects with reverse mask!"};
//////////////////////////////////////////////////////////////////////////////////////////
// prototypes										//
//////////////////////////////////////////////////////////////////////////////////////////
int	validatenetstring	( char *netstring, unsigned char *netpointer );
void	extractnetoctets	( char *netstring, regmatch_t *matches, unsigned char *netpointer );
void	extractmaskoctets	( char *netstring, regmatch_t *matches, unsigned char *maskpointer );
int	maskexceptioncheck	( unsigned char *netpointer );
void	listaddresses		( ipv4net net );
void	errexit			( const int exitcode, const char *string );
//////////////////////////////////////////////////////////////////////////////////////////
// main											//
//////////////////////////////////////////////////////////////////////////////////////////
int main ( int argc, char* argv[] ){
	ipv4net net;
	unsigned char *netptr = &net.octet[0];
	if( validatenetstring( argv[1], netptr ) ){
		if( maskexceptioncheck( netptr ) ){
			listaddresses( net );
			exit(0);
		} else errexit( 2, argv[1] );
	} else errexit( 1, argv[1] );
}
//////////////////////////////////////////////////////////////////////////////////////////
// functions										//
//////////////////////////////////////////////////////////////////////////////////////////
int validatenetstring( char *netstring, unsigned char *netpointer ){
	regex_t checkstringrx;
	regmatch_t matches[6];
	regcomp( &checkstringrx, NET_PATTERN, REG_EXTENDED );
	if( regexec( &checkstringrx, netstring, 6, matches, 0 ) == 0 ) {
		extractnetoctets  ( netstring, matches, netpointer   );
		extractmaskoctets ( netstring, matches, netpointer+4 );
		return TRUE;
	} else return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////
void extractnetoctets( char *netstring, regmatch_t *matches, unsigned char *netpointer ){
	for( int i = 1; i < 5; i++, netpointer++ ){
		int buffer = atoi( netstring + matches[i].rm_so );
		memmove( netpointer, (char *) &buffer, 1 );
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
void extractmaskoctets( char *netstring, regmatch_t *matches, unsigned char *maskpointer ){
	for( int maskbitcounter = atoi( netstring + matches[5].rm_so ); maskbitcounter > 0; maskbitcounter -= 8, maskpointer++ ){
		int currentoctet = pow ( 2, (maskbitcounter < 8 ? maskbitcounter : 8) ) - 1;
		if( maskbitcounter < 8 ) currentoctet <<= ( 8 - maskbitcounter );
		memmove( maskpointer, (char *)&currentoctet, 1 );
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
int maskexceptioncheck( unsigned char* netpointer ){
	for ( unsigned char *maskpointer = netpointer + 4, i = 0; i < 4; netpointer++, maskpointer++, i++ ) {
		if( ( (*netpointer) & ( ~ ( *maskpointer ) ) ) != 0 ) return FALSE;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////////
void listaddresses( ipv4net net ){
	for( int octet1 = net.octet[0], counter = (unsigned char)(~net.maskoctet[0]); counter >= 0; octet1++, counter-- )
		for( int octet2 = net.octet[1], counter = (unsigned char)(~net.maskoctet[1]); counter >= 0; octet2++, counter-- )
			for( int octet3 = net.octet[2], counter = (unsigned char)(~net.maskoctet[2]); counter >= 0; octet3++, counter-- )
				for( int octet4 = net.octet[3], counter = (unsigned char)(~net.maskoctet[3]); counter >= 0; octet4++, counter-- ){
					printf( "%u.%u.%u.%u\n", octet1, octet2, octet3, octet4 );
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
void errexit( const int exitcode, const char *string ){
	fprintf( stderr,"/!\\\t%-50.50s\t%16.16s\n", message[exitcode-1], string );
	exit( exitcode );
}
//////////////////////////////////////////////////////////////////////////////////////////
