#include <stdio.h>
#include <stdlib.h>						
#include <math.h>
#include <string.h>
#include <regex.h>
#define TRUE	1
#define FALSE	0
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
int	netstringcheckrefine	( char *string, unsigned char *net );
int	maskexceptioncheck	( unsigned char* net );
void	listaddresses		( ipv4net  net );
void	printnet		( unsigned char *net );		//debug purposes only
//////////////////////////////////////////////////////////////////////////////////////////
// main											//
//////////////////////////////////////////////////////////////////////////////////////////
int main ( int argc, char* argv[] ){
	ipv4net net;
	unsigned char *netptr = &net.octet1;
	if( netstringcheckrefine( argv[1], netptr ) ){
		if( maskexceptioncheck( netptr ) ){
			listaddresses( net );
			return 0;
		} else {
			fprintf(stderr,"Error! Illegal network address! Network and Hosts ranges intersects!\n");
			return 2;
		}
	} else {
		fprintf(stderr,"Error! Check network address and try again!\n");
		return 1;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
// functions										//
//////////////////////////////////////////////////////////////////////////////////////////
int netstringcheckrefine( char *string, unsigned char *net ){
	regex_t checkstringrx;
	regmatch_t matches[6];
	regcomp( &checkstringrx, NET_PATTERN, REG_EXTENDED );
	if( regexec( &checkstringrx, string, 6, matches, 0 ) == 0 ) {
		for( int i = 1; i < 6; i++, net++ ){
			unsigned char intbuf = atoi( string + matches[i].rm_so );
			if( i == MASK ){
				for( int maskbitcount = intbuf; maskbitcount > 0; maskbitcount -= 8, net++ ){
					int currentoctet = pow ( 2, (maskbitcount < 8 ? maskbitcount : 8) ) - 1;
					if( maskbitcount < 8 ) currentoctet <<= ( 8 - maskbitcount );
					memmove( net, (char *)&currentoctet, 1 );
				}
			}
			else memmove( net, (char*) &intbuf, 1 );
		}
		return TRUE;
	}
	else return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////
int maskexceptioncheck( unsigned char* net ){
	for ( unsigned char *netcursor = net, *maskcursor = netcursor + 4, i = 0; i < 4; netcursor++, maskcursor++, i++ ) {
		if( ( (*netcursor) & ( ~ ( *maskcursor ) ) ) != 0 ) return FALSE;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////////
void listaddresses( ipv4net net ){
	for( int octet1 = net.octet1, counter = (unsigned char)(~net.maskoctet1); counter >= 0; octet1++, counter-- )
		for( int octet2 = net.octet2, counter = (unsigned char)(~net.maskoctet2); counter >= 0; octet2++, counter-- )
			for( int octet3 = net.octet3, counter = (unsigned char)(~net.maskoctet3); counter >= 0; octet3++, counter-- )
				for( int octet4 = net.octet4, counter = (unsigned char)(~net.maskoctet4); counter >= 0; octet4++, counter-- ){
					printf("%u.%u.%u.%u\n",octet1,octet2,octet3,octet4);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
void printnet( unsigned char *net ){
	printf("%u.%u.%u.%u\n%u.%u.%u.%u\n", *net, *(net+1), *(net+2), *(net+3), *(net+4), *(net+5), *(net+6), *(net+7));
}
//////////////////////////////////////////////////////////////////////////////////////////
