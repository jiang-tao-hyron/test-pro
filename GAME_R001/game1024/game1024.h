#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void init_game1024(void);
char geton(void);
void getkey(void);
void change(void);

int blank[4][4];
char key[2];
int rannum[10]={1,2,4,8,8,16,16,32,32,32};
unsigned int score=0;

void init_game1024(void){
	int i,j;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			blank[i][j]=0;
		}
	}
}

char geton(){
	int max=0,i,j,count=0,ran=0;
	while( score/10>ran*ran*ran )ran++;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			if(max<blank[i][j])max=blank[i][j];
			if(blank[i][j]==0)count++;
		}
	}
	if(count==0)return 0;
	count=rand()%count;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			if(blank[i][j]==0){
				if(count==0){
					blank[i][j]=rannum[rand()%(ran+1)];
					return 1;
				}
				count--;			
			}
		}
	}
	return 0;
}

void getkey(void){
	scanf("%s",key);
}

void change(void){
	int i,j;
	
	switch(key[0]){
		case 'w':{
			for(j=0;j<4;j++){
				for(i=3;i>0;i--)
					if(blank[i-1][j]==0)
						{blank[i-1][j]=blank[i][j];blank[i][j]=0;}
				
				for(i=0;i<3;i++)
					if(blank[i+1][j]==blank[i][j])
						{blank[i][j]+=blank[i+1][j];score+=blank[i+1][j];blank[i+1][j]=0;}
					
				for(i=3;i>0;i--)
					if(blank[i-1][j]==0)
						{blank[i-1][j]=blank[i][j];blank[i][j]=0;}
			}
		}break;
	
		case 'a':{
			for(i=0;i<4;i++){
				for(j=3;j>0;j--)
					if(blank[i][j-1]==0)
						{blank[i][j-1]=blank[i][j];blank[i][j]=0;}
				
				for(j=0;j<3;j++)
					if(blank[i][j+1]==blank[i][j])
						{blank[i][j]+=blank[i][j+1];score+=blank[i][j+1];blank[i][j+1]=0;}
					
				for(j=3;j>0;j--)
					if(blank[i][j-1]==0)
						{blank[i][j-1]=blank[i][j];blank[i][j]=0;}
			}
		}break;
		
		case 's':{
			for(j=0;j<4;j++){
				for(i=0;i<3;i++)
					if(blank[i+1][j]==0)
						{blank[i+1][j]=blank[i][j];blank[i][j]=0;}
				
				for(i=3;i>0;i--)
					if(blank[i-1][j]==blank[i][j])
						{blank[i][j]+=blank[i-1][j];score+=blank[i-1][j];blank[i-1][j]=0;}
					
				for(i=0;i<3;i++)
					if(blank[i+1][j]==0)
						{blank[i+1][j]=blank[i][j];blank[i][j]=0;}
			}
		}break;

		case 'd':{
			for(i=0;i<4;i++){
				for(j=0;j<3;j++)
					if(blank[i][j+1]==0)
						{blank[i][j+1]=blank[i][j];blank[i][j]=0;}
				
				for(j=3;j>0;j--)
					if(blank[i][j-1]==blank[i][j])
						{blank[i][j]+=blank[i][j-1];score+=blank[i][j-1];blank[i][j-1]=0;}
					
				for(j=0;j<3;j++)
					if(blank[i][j+1]==0)
						{blank[i][j+1]=blank[i][j];blank[i][j]=0;}
			}
		}break;
	}
}
