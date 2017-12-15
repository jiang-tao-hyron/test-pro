
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "chess.h"

char table[8][8]={
	{1,2,3,5,4,3,2,1},
	{6,6,6,6,6,6,6,6},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{13,13,13,13,13,13,13,13},
	{8,9,10,11,12,10,9,8},
};
unsigned char bg[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

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
