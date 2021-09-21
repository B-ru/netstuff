/* проверено: компилируется и работает в win7-cygwin64 и astralinux open
 * правки от 18.09.2021
 * DONE: запустить в archlinux
 * DONE: вынести логику в функции
 * правки от 20.09.2021
 * DONE: добавить проверку на maskexception
 * правки от 21.09.2021
 * DONE: добавить функцию вывода всех адресов сети
 * TODO: refactor parsenetstring() function
 */

#include <stdio.h>
#include <stdlib.h>						
#include <math.h>
#include <string.h>
#include <stddef.h>
#include <regex.h>

#define TRUE  1
#define FALSE 0
#define NET_PATTERN "^[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}/[0-9]{1,2}$"
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
//prototypes										//
//////////////////////////////////////////////////////////////////////////////////////////
ipv4net	parsenetstring		(char *string);
int	netstringcheck 		(char *string);
int	maskexceptioncheck	(ipv4net  net);
void	listaddresses		(ipv4net  net);
//////////////////////////////////////////////////////////////////////////////////////////
// main											//
//////////////////////////////////////////////////////////////////////////////////////////
int main (int argc, char* argv[]){
	if( netstringcheck( argv[1] ) ){
		ipv4net net = parsenetstring( argv[1] );
		if( maskexceptioncheck( net ) ) {
			listaddresses( net );
		}
		else printf("Error! Illegal network address! Network and Hosts ranges intersects!\n");
		
	} else {
		printf("Error! Check network address and try again!\n");
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
//functions										//
//////////////////////////////////////////////////////////////////////////////////////////
ipv4net parsenetstring(char *string){
	ipv4net parsingnet;
	char *cursor = &parsingnet.address.octet1;
	unsigned char count = 0;
	char *netpart;
	int currentoctet;
	while ( (netpart = strsep(&string,"/")) != NULL )	
	{
		switch (++count){
			case 1:							// address part
				for(	char* addrpart;
					(addrpart = strsep(&netpart,".")) != NULL;
					cursor++ ) 
				{
					currentoctet = atoi(addrpart);
					memcpy(cursor,(char*)&currentoctet,1);	// int -> char
				}
				break;
			case 2:							// mask part
				for(int maskbitcount = atoi(netpart);maskbitcount>0;maskbitcount-=8,cursor++){
					currentoctet = pow ( 2, (maskbitcount < 8 ? maskbitcount : 8)) - 1;
					for (int lshiftamount = 8 - maskbitcount;lshiftamount > 0; lshiftamount--) currentoctet *= 2;
					memcpy( cursor, (char *)&currentoctet, 1);
				}					
				break;
			default:
				break;
		}
	}
	return parsingnet;
}
//////////////////////////////////////////////////////////////////////////////////////////
int netstringcheck(char *string){
	regex_t checkstringrx;
	regmatch_t matches[1];
	regcomp(&checkstringrx,NET_PATTERN,REG_EXTENDED);
	if(regexec(&checkstringrx,string,1,matches,0) == 0) return TRUE;
	else return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////
int maskexceptioncheck(ipv4net net){
	for ( unsigned char *netcursor = &net.address.octet1, *maskcursor = netcursor + 4, i = 0; i<4; netcursor++, maskcursor++, i++) {
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
