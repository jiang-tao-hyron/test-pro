#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void init_p(char table_p[][3],int* center);
void getkey_p(char *key_p);
int change_p(char table_p[][3],int* center,char* key_p);
int main(int argc, char *argv[]){
	char table_p[3][3];
	int center[2];
	int i,j,win;
	char key_p[2];
	
	center[0]=2;
	center[1]=2;
	
	init_p(table_p,center);
	system("clear");
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			printf("%c",table_p[i][j]);
		}
		printf("\n");
	}
	while(1){
		getkey_p(key_p);
		system("clear");
		win = change_p(table_p,center,key_p);
		
		for(i=0;i<3;i++){
			for(j=0;j<3;j++){
				printf("%c",table_p[i][j]);
			}
			printf("\n");
		}
		
		if(win){printf("win");break;}
	}
	
	
	return 0;
}

void init_p(char table_p[][3],int* center){
	int i,j;
	char k;
	srand(time(NULL));
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			table_p[i][j]='1'+i*3+j;
		}
	}
	table_p[2][2]=' ';
	
	for(i=0;i<100;i++){
		j=rand()%4;
		switch(j){
		case 0:k='w';break;
		case 1:k='a';break;
		case 2:k='s';break;
		case 3:k='d';break;
		}
		change_p(table_p,center,&k);
	}
}

void getkey_p(char *key_p){
	scanf("%s",key_p);
}

int change_p(char table_p[][3],int* center,char* key_p){
	int i;
	switch(key_p[0]){
	case 'w':	if(center[0]==2)break;
				table_p[ center[0]][center[1]]=table_p[center[0]+1][center[1]];
				center[0]++;
				table_p[center[0]][center[1]]=' ';
	break;
	
	case 's':	if(center[0]==0)break;	
				table_p[center[0]][center[1]]=table_p[center[0]-1][center[1]];
				center[0]--;
				table_p[center[0]][center[1]]=' ';
				break;
	
	case 'a':	if(center[1]==2)break;
				table_p[center[0]][center[1]]=table_p[center[0]][center[1]+1];
				center[1]++;
				table_p[center[0]][center[1]]=' ';
	break;
	
	case 'd':	if(center[1]==0)break;	
				table_p[center[0]][center[1]]=table_p[center[0]][center[1]-1];
				center[1]--;
				table_p[center[0]][center[1]]=' ';
				break;
	}
	if(center[0]==2 && center[1]==2){
		for(i=0;i<8;i++){
			if(table_p[i/3][i%3]!='1'+i)return 0;
		}
		return 1;
	}
	
	return 0;
}







