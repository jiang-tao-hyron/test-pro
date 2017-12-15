#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "game1024.h"


int main(int argc, char *argv[]){

	int i,j;
	
	init_game1024();
	
	while(1){
		for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				if(blank[i][j]==0)printf(".\t");
				else printf("%d\t",blank[i][j]);
			}
			printf("\n");
		}
		printf("score:%d\n",score);
		system("sleep 1");
		system("clear");
		
		if(geton()==0){
			printf("gameover!score=%d\n",score);
			return 0;
		}
		for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				if(blank[i][j]==0)printf(".\t");
				else printf("%d\t",blank[i][j]);
			}
			printf("\n");
		}
		printf("score:%d\t\t",score);
		getkey();
		system("clear");
		change();
	}
	return 0;
}


