#include <stdio.h>
#include <stdlib.h>						
#include <math.h>
#include <string.h>
#include <regex.h>
#define TRUE	1
#define FALSE	0
#define NET_PATTERN "^([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})/([0-9]{1,2})$"
#define CODE1MESSAGE "Error! Check network address!\n"
#define CODE2MESSAGE "Error! Illegal network address!\n" 
//////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
	unsigned char octet[4];
	unsigned char maskoctet[4];
}ipv4net;
//////////////////////////////////////////////////////////////////////////////////////////
// prototypes										//
//////////////////////////////////////////////////////////////////////////////////////////
int	refinenetstring		( char *netstring, unsigned char *netpointer );
void	refinemaskoctets	( int *mask, unsigned char *maskpointer );
int	maskexceptioncheck	( unsigned char *netpointer );
void	listaddresses		( ipv4net net );
void	errexit			( const int exitcode, const char* message );
//////////////////////////////////////////////////////////////////////////////////////////
// main											//
//////////////////////////////////////////////////////////////////////////////////////////
int main ( int argc, char* argv[] ){
	ipv4net net;
	unsigned char *netptr = &net.octet[0];
	if( refinenetstring( argv[1], netptr ) ){
		if( maskexceptioncheck( netptr ) ){
			listaddresses( net );
			exit(0);
		} else errexit( 2, CODE2MESSAGE );
	} else errexit( 1, CODE1MESSAGE );
}
//////////////////////////////////////////////////////////////////////////////////////////
// functions										//
//////////////////////////////////////////////////////////////////////////////////////////
int refinenetstring( char *netstring, unsigned char *netpointer ){
	regex_t checkstringrx;
	regmatch_t matches[6];
	regcomp( &checkstringrx, NET_PATTERN, REG_EXTENDED );
	if( regexec( &checkstringrx, netstring, 6, matches, 0 ) == 0 ) {
		for( int i = 1; i < 6; i++, netpointer++ ){
			int intbuf = atoi( netstring + matches[i].rm_so );
			if(  i % 5  ) memmove( netpointer, (char *) &intbuf, 1 );
			else if( intbuf > 32 ) return FALSE; else refinemaskoctets( &intbuf, netpointer );
		}
		return TRUE;
	} else return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////
void refinemaskoctets( int *mask, unsigned char *maskpointer ){
	for( int maskbitcounter = *mask; maskbitcounter > 0; maskbitcounter -= 8, maskpointer++ ){
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
void errexit( const int exitcode, const char* message ){
	fprintf(stderr,"%s", message);
	exit(exitcode);
}
//////////////////////////////////////////////////////////////////////////////////////////
