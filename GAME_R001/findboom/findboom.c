#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "findboom.h"

int main(int argc, char *argv[]){
	int i,j,x,y;
	
	table_init();
	
	for(i=-1;i<10;i++){
		for(j=-1;j<10;j++){
			if(i==-1){printf("%d",j);continue;}
			if(j==-1){printf("%d ",i);continue;}
			printf("%d",table[i][j]);
		}
		printf("\n");
	}
	
	while(1){
		system("clear");
		for(i=-1;i<10;i++){
			for(j=-1;j<10;j++){
				if(i==-1){printf("%d",j);continue;}
				if(j==-1){printf("%d|",i);continue;}
				printf("%c",show[i][j]);
			}
			printf("\n");
		}
		scanf("%d.%d",&x,&y);
		
		if(cleck(x,y)){
			system("clear");
			for(i=-1;i<10;i++){
				for(j=-1;j<10;j++){
					if(i==-1){printf("%d",j);continue;}
					if(j==-1){printf("%d|",i);continue;}
					printf("%c",show[i][j]);
				}
				printf("\n");
			}
			printf("gameover\n");
			return 0;
		}
	}
	return 0;
}











