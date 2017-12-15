#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void init_p(char table_p[][3],int* center);
char getkey_p(char *key_p);
int change_p(char table_p[][3],int* center,char* key_p);
void show_p(char table_p[][3]);

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

char getkey_p(char *key_p){
	scanf("%s",key_p);
	switch(*key_p){
		case 'r':return '3';
		case 'b':
		case 'q':return *key_p;
		default:break;
		}
		return 0;
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

void show_p(char table_p[][3]){
	int i,j;
	printf("r:restart\tb:back\tq:quit\n");
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			printf("%c",table_p[i][j]);
		}
		printf("\n");
	}
}
