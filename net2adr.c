/* проверено: компилируется и работает в win7-cygwin64 и astralinux open
 * правки от 18.09.2021
 * DONE: запустить в archlinux
 * DONE: вынести логику в функции
 * правки от 20.09.2021
 * DONE: добавить проверку на maskexception
 * правки от 21.09.2021
 * DONE: добавить функцию вывода всех адресов сети
 * DONE: объеденины функции проверки и разбора строки
 */

#include <stdio.h>
#include <stdlib.h>						
#include <math.h>
#include <string.h>
#include <stddef.h>
#include <regex.h>

#define TRUE  1
#define FALSE 0
#define MASK 5
#define NET_PATTERN "^([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})/([0-9]{1,2})$"
//////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
	unsigned char octet1;
	unsigned char octet2;
	unsigned char octet3;
	unsigned char octet4;
}fouroctets;
//////////////////////////////////////////////////////////////////////////////////////////
typedef struct{
	fouroctets address;
	fouroctets mask;
}ipv4net;
//////////////////////////////////////////////////////////////////////////////////////////
// prototypes										//
//////////////////////////////////////////////////////////////////////////////////////////
int	netstringcheckrefine	(char *string, unsigned char *net);
int	maskexceptioncheck	(unsigned char* net);
void	listaddresses		(ipv4net  net);
void	printnet		(unsigned char *net);		//debug purposes only
//////////////////////////////////////////////////////////////////////////////////////////
// main											//
//////////////////////////////////////////////////////////////////////////////////////////
int main (int argc, char* argv[]){
	ipv4net net;
	unsigned char *netptr = &net.address.octet1;
	if( netstringcheckrefine( argv[1], netptr ) ){
		if( maskexceptioncheck( netptr ) )
			listaddresses( net );
		else printf("Error! Illegal network address! Network and Hosts ranges intersects!\n");
	} else printf("Error! Check network address and try again!\n");
}
//////////////////////////////////////////////////////////////////////////////////////////
// functions										//
//////////////////////////////////////////////////////////////////////////////////////////
int netstringcheckrefine( char *string, unsigned char *net ){
	regex_t checkstringrx;
	regmatch_t matches[6];
	regcomp(&checkstringrx,NET_PATTERN,REG_EXTENDED);
	if(regexec(&checkstringrx,string,6,matches,0) == 0) {
		//once we've got regex check, lets refine network string into ipv4net structure
		for( int i = 1; i < 6; i++,net++ ){
			unsigned char intbuf = atoi(string+matches[i].rm_so);
			if( i == MASK ){
				for( int maskbitcount = intbuf; maskbitcount>0; maskbitcount-=8, net++ ){
					int currentoctet = pow ( 2, (maskbitcount < 8 ? maskbitcount : 8)) - 1;
					for ( int lshiftamount = 8 - maskbitcount; lshiftamount > 0; lshiftamount-- ) currentoctet *= 2;
					memmove( net, (char *)&currentoctet, 1);
				}					
			}
			else memmove( net, (char*) &intbuf, 1);
		}
		return TRUE;
	}
	else return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////
int maskexceptioncheck(unsigned char* net){
	for ( unsigned char *netcursor = net, *maskcursor = netcursor + 4, i = 0; i<4; netcursor++, maskcursor++, i++) {
		if( ( (*netcursor) & ( ~ ( *maskcursor ) ) ) != 0 ) return FALSE;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////////
void listaddresses( ipv4net net ){
	for( int octet1 = net.address.octet1, counter = (unsigned char)(~net.mask.octet1); counter >= 0; octet1++, counter--)
		for( int octet2 = net.address.octet2, counter = (unsigned char)(~net.mask.octet2); counter >= 0; octet2++, counter--)
			for( int octet3 = net.address.octet3, counter = (unsigned char)(~net.mask.octet3); counter >= 0; octet3++, counter--)
				for( int octet4 = net.address.octet4, counter = (unsigned char)(~net.mask.octet4); counter >= 0; octet4++, counter-- ){
					printf("%u.%u.%u.%u\n",octet1,octet2,octet3,octet4);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
void printnet(unsigned char *net){
	printf("%u.%u.%u.%u\n%u.%u.%u.%u\n", *net, *(net+1), *(net+2), *(net+3), *(net+4), *(net+5), *(net+6), *(net+7));
}
//////////////////////////////////////////////////////////////////////////////////////////
