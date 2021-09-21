/*@author: Boris Ischenko
 * 23.09.2019
 */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

struct ip{
	unsigned char octet1;
	unsigned char octet2;
	unsigned char octet3;
	unsigned char octet4;
};

int main (int argc, char* argv[]){
	
	unsigned char *adr; 
	unsigned char *pref;
	unsigned char *segment;
	unsigned char *one = strdup(argv[1]);
	unsigned char count = 0;
	if (argc > 1) {
		while ((segment = strsep(&one,"/"))!=NULL)
		{
			count++;
			switch (count){
				case 1:
					adr = segment;
					break;
				case 2:
					pref = segment;
					break;
				default:
					break;
			}
		}
		count = 0;
		unsigned char* octet1;
		unsigned char* octet2;
		unsigned char* octet3;
		unsigned char* octet4;
		unsigned char* dup = strdup(adr);
		struct ip IP;
		while ((segment = strsep(&dup,"."))!=NULL)
		{
			count++;
			switch (count){
				case 1:
					IP.octet1 = (unsigned char)atoi(segment);
					break;
				case 2:
					IP.octet2 = (unsigned char)atoi(segment);
					break;
				case 3:
					IP.octet3 = (unsigned char)atoi(segment);
					break;
				case 4:
					IP.octet4 = (unsigned char)atoi(segment);
					break;
				default:
					break;
			}
		}
	//printf(">%d.%d.%d.%d\n",IP.octet1,IP.octet2,IP.octet3,IP.octet4);
	unsigned char arr[5] = {IP.octet1,IP.octet2,IP.octet3,IP.octet4,(unsigned char)atoi(pref)};
	unsigned char R_mask[4] = {0,0,0,0};		// обратная маска в виде кол-ва бит
	unsigned char R_mask_decima[4] = {0,0,0,0};	// обратная маска в десятичном виде
	unsigned char R_mask_probe[4]  = {0,0,0,0};	// обратная маска для разрешения списка ip адресов
	unsigned char Adr[4] = {0,0,0,0};			// условный адрес к опросу
	unsigned char RM_bitcounter = 32 - arr[4];	// счетчик битов к распределению по октетам обратной маски
	
	if (RM_bitcounter < 0) {
		return 2;
	} else {
		for (int k = 3;k>=0;k--){
			if(RM_bitcounter>=8) {
				R_mask[k] = 8;
				RM_bitcounter = RM_bitcounter - 8;
				R_mask_decima[k] = (int)pow(2,R_mask[k])-1;
				Adr[k] = arr[k]+R_mask_decima[k];
			} else {
				R_mask[k] = RM_bitcounter;
				RM_bitcounter = 0;
				R_mask_decima[k] = (int)pow(2,R_mask[k])-1;
			}			
		}		
		R_mask_probe[0] = R_mask_decima[0];
		R_mask_probe[1] = R_mask_decima[1];
		R_mask_probe[2] = R_mask_decima[2];
		R_mask_probe[3] = R_mask_decima[3];
		while (R_mask_probe[3] >= 0){
			while (R_mask_probe[2] >= 0){
				while (R_mask_probe[1] >= 0){
					while (R_mask_probe[0] >= 0){
						for (int k = 3;k>=0;k--){
							Adr[k] = arr[k]+R_mask_probe[k];
						}
						printf("%d.%d.%d.%d\n",Adr[0],Adr[1],Adr[2],Adr[3]);
						if(R_mask_probe[0] == 0) break;
						R_mask_probe[0]--;
					}
					R_mask_probe[0] = R_mask_decima[0];
					if(R_mask_probe[1] == 0) break;
					R_mask_probe[1]--;
				}
				R_mask_probe[1] = R_mask_decima[1];
				if(R_mask_probe[2] == 0) break;
				R_mask_probe[2]--;
			}
			R_mask_probe[2] = R_mask_decima[2];
			if(R_mask_probe[3] == 0) break;
			R_mask_probe[3]--;
		}
		return 0;		
	}
	} else {
		return 1;
	}
	
}

