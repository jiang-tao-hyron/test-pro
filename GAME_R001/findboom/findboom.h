#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void table_init(void);
int cleck(int x,int y);

int table[10][10];
char show[10][10];
char find[10][10];


void table_init(void){
	int i,j,count=20;
	srand(time(NULL));
	while(count>0){
		i=rand()%9;
		j=rand()%9;
		if(table[i][j]==1) continue;
		table[i][j]=1;
		count--;
	}
	
	for(i=0;i<10;i++){
		for(j=0;j<10;j++){
			find[i][j]=0;
			show[i][j]='.';
		}
	}
}

int cleck(int x,int y){
	int count=0;
	int i,j;

	
	if(table[x][y] ){
		for(i=0;i<10;i++){
			for(j=0;j<10;j++){
				if(table[i][j]==1) show[i][j]='0';
			}
		}
		
		show[x][y]='*';
		return 1;
	}
	
	for(i=x-1;i<x+2;i++){
		if(i==-1)continue;
		if(i==10)continue;
		for(j=y-1;j<y+2;j++){
			if(j==-1)continue;
			if(j==10)continue;
			if(table[i][j]==1)count++;
		}
	}
	
	if(count==0){
		show[x][y]=' ';
		find[x][y]=1;
		for(i=x-1;i<x+2;i++){
			if(i==-1)continue;
			if(i==10)continue;
			for(j=y-1;j<y+2;j++){
				if(j==-1)continue;
				if(j==10)continue;
				if(find[i][j]==1)continue;
				
				cleck(i,j);
			}
		}
	}
	else show[x][y]='0'+count;
	find[x][y]=0;
	return 0;
}

