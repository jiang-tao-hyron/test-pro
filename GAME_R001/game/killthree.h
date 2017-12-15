/*
 * killthree.h
 *
 *  Created on: 2017/01/25
 *      Author: mcc
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define write0 "\033[0m "
#define write1 "\033[41m1"
#define write2 "\033[42m2"
#define write3 "\033[43m3"
#define write4 "\033[44m4"
#define write5 "\033[45m5"


void kill_init(int table[][10]);
void show(int table[][10]);
char getkey_k(char* key);
void change_k(int table[][10],char* key);
int kill_check(int table[][10]);
void relive(int table[][10]);


void kill_init(int table[][10]){
	int i,j;
	for(i=0;i<10;i++){
		for(j=0;j<10;j++){
			table[i][j]=rand()%5+1;
		}		
	}
}

void show(int table[][10]){
	int i,j;
	char write[200];
	char I[3];
	I[1]='|';
	printf("r:restart\tb:back\tq:quit\n");
	printf("  0123456789\n");
	for(i=0;i<10;i++){
		I[0]=i+'0';
		strcat(write,"echo -e '\33[0m");
		strcat(write,I);
		for(j=0;j<10;j++){
			switch(table[i][j]){
			case 0:strcat(write,write0);break;
			case 1:strcat(write,write1);break;
			case 2:strcat(write,write2);break;
			case 3:strcat(write,write3);break;
			case 4:strcat(write,write4);break;
			case 5:strcat(write,write5);break;
			}
		}
		strcat(write,"\\033""[0m'");
		system(write);
		memset(write, 0, sizeof(write));
		
	}
	printf("  0123456789\n");
}

char getkey_k(char* key){
	while(1){
		scanf("%s",key);
		switch(key[0]){
		case 'r':return '4';break;
		case 'b':
		case 'q':return key[0];
		default:break;
		}
		if(key[0]==key[2])
			if(key[1]-key[3]==1 || key[1]-key[3]==-1)
				break;
		if(key[1]==key[3])
			if(key[0]-key[2]==1 || key[0]-key[2]==-1)
				break;
	}
	return 0;
}

void change_k(int table[][10],char* key){
	int p;
	p=table[key[0]-'0'][key[1]-'0'];
	table[key[0]-'0'][key[1]-'0']=table[key[2]-'0'][key[3]-'0'];
	table[key[2]-'0'][key[3]-'0']=p;
}

int kill_check(int table[][10]){
	int i,j,have=0;
	char mark[10][10]={0};
	for(i=0;i<10;i++){
		for(j=0;j<10;j++){
			if(table[i][j]==0)continue;
			if(table[i][j]==table[i][j+1] && table[i][j]==table[i][j+2] && j<8){
				mark[i][j]=1;
				mark[i][j+1]=1;
				mark[i][j+2]=1;
				have = 1;
			}
			if(table[i][j]==table[i+1][j] && table[i][j]==table[i+2][j] && i<8){
				mark[i][j]=1;
				mark[i+1][j]=1;
				mark[i+2][j]=1;
				have = 1;
			}
		}		
	}
	
	for(i=0;i<10;i++){
		for(j=0;j<10;j++){
			if(mark[i][j]){
				table[i][j]=0;
			}
		}
	}
	
	return have;
}

void relive(int table[][10]){
	int i,j,p;
	for(j=9;j>-1;j--){
		for(i=9;i>0;i--){
			if(table[i][j]==0){
				p=1;
				while(table[i-p][j]==0){
					if(i==p){
						table[0][j]=rand()%5+1;
						break;
					}
					p++;
				}
				table[i][j]=table[i-p][j];
				table[i-p][j]=0;
			}
		}
		if(table[0][j]==0)
			table[0][j]=rand()%5+1;
	}
}


