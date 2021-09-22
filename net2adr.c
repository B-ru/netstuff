#include <stdio.h>
#include <stdlib.h>						
#include <math.h>
#include <string.h>
#include <regex.h>
#define TRUE	1
#define FALSE	0
#define OCT1	1
#define OCT2	2
#define OCT3	3
#define OCT4	4
#define MASK	5
#define NET_PATTERN "^([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})/([0-9]{1,2})$"
//////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
	unsigned char octet1;
	unsigned char octet2;
	unsigned char octet3;
	unsigned char octet4;
	unsigned char maskoctet1;
	unsigned char maskoctet2;
	unsigned char maskoctet3;
	unsigned char maskoctet4;
}ipv4net;
//////////////////////////////////////////////////////////////////////////////////////////
// prototypes										//
//////////////////////////////////////////////////////////////////////////////////////////
int	refinenetstring		( char *netstring, unsigned char *netpointer );
int	maskexceptioncheck	( unsigned char *netpointer );
void	listaddresses		( ipv4net  net );
void	resolvemaskoctets	( int *mask, unsigned char *maskpointer );
void	printnet		( unsigned char *netpointer );		//debug purposes only
//////////////////////////////////////////////////////////////////////////////////////////
// main											//
//////////////////////////////////////////////////////////////////////////////////////////
int main ( int argc, char* argv[] ){
	ipv4net net;
	unsigned char *netptr = &net.octet1;
	if( refinenetstring( argv[1], netptr ) ){
		if( maskexceptioncheck( netptr ) ){
			listaddresses( net );
			return 0;
		} else {
			fprintf( stderr,"Error! Illegal network address! Network and Hosts ranges intersects!\n" );
			return 2;
		}
	} else {
		fprintf( stderr,"Error! Check network address and try again!\n" );
		return 1;
	}
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
			switch (i){
				case OCT1: case OCT2: case OCT3: case OCT4: memmove( netpointer, (char *) &intbuf, 1 ); break;
				case MASK: resolvemaskoctets( &intbuf, netpointer ); break;
			}
		}
		return TRUE;
	} else return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////
void resolvemaskoctets( int *mask, unsigned char *maskpointer ){
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
	for( int octet1 = net.octet1, counter = (unsigned char)(~net.maskoctet1); counter >= 0; octet1++, counter-- )
		for( int octet2 = net.octet2, counter = (unsigned char)(~net.maskoctet2); counter >= 0; octet2++, counter-- )
			for( int octet3 = net.octet3, counter = (unsigned char)(~net.maskoctet3); counter >= 0; octet3++, counter-- )
				for( int octet4 = net.octet4, counter = (unsigned char)(~net.maskoctet4); counter >= 0; octet4++, counter-- ){
					printf( "%u.%u.%u.%u\n", octet1, octet2, octet3, octet4 );
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
void printnet( unsigned char *netpointer ){
	for( int i = 0; i < 8; i++, netpointer++ ) printf( "%u%s", *netpointer, ( i % 4 == 3 ? "\n" : "." ) );
}
//////////////////////////////////////////////////////////////////////////////////////////
