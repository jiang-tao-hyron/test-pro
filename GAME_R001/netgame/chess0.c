
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "chess.h"

extern char table[8][8];
extern unsigned char bg[8];

int main(int argc, char *argv[]){
	char key1[3];
	char key2[3];
	system("clear");
	show2(key1,key2);
	while(1){
		getkey1(key1);
		system("clear");
		show1(key1);
		
		getkey2(key2);
		walk(key1,key2);
		system("clear");
		show2(key1,key2);
	}
	
	return 0;
}
