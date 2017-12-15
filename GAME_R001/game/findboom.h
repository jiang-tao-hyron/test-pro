#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void table_init(int table[][10],char find[][10],char show[][10]);
int cleck(char x,char y,int table[][10],char find[][10],char show[][10]);
char getkey_f(char* x,char* y);
void showboom(char show[][10]);

void table_init(int table[][10],char find[][10],char show[][10]){
	int i,j,count=20;
	srand(time(NULL));
	for(i=0;i<10;i++){
		for(j=0;j<10;j++){
			table[i][j]=0;
			find[i][j]=0;
			show[i][j]='.';
		}
	}
	
	while(count>0){
		i=rand()%9;
		j=rand()%9;
		if(table[i][j]==1) continue;
		table[i][j]=1;
		count--;
	}
	

	for(i=-1;i<10;i++){
		for(j=-1;j<10;j++){
			if(i==-1){printf("%d",j);continue;}
			if(j==-1){printf("%d ",i);continue;}
			printf("%d",table[i][j]);
		}
		printf("\n");
	}
}
char getkey_f(char* x,char* y){
	char a[3];
	scanf("%s",a);
	*x=a[0];
	*y=a[1];

	switch(*x){
		case 'r':return '2';
		case 'b':
		case 'q':return *x;
		default:break;
	}
	return 0;
}


int cleck(char x,char y,int table[][10],char find[][10],char show[][10]){
	int count=0;
	char i,j;

	
	if(table[x-'0'][y-'0'] ){
		for(i=0;i<10;i++){
			for(j=0;j<10;j++){
				if(table[i][j]==1) show[i][j]='0';
			}
		}
		
		show[x-'0'][y-'0']='*';
		return 1;
	}
	
	for(i=x-'0'-1;i<x-'0'+2;i++){
		if(i==-1)continue;
		if(i==10)continue;
		for(j=y-'0'-1;j<y-'0'+2;j++){
			if(j==-1)continue;
			if(j==10)continue;
			if(table[i][j]==1)count++;
		}
	}
	
	if(count==0){
		show[x-'0'][y-'0']=' ';
		find[x-'0'][y-'0']=1;
		for(i=x-'0'-1;i<x-'0'+2;i++){
			if(i==-1)continue;
			if(i==10)continue;
			for(j=y-'0'-1;j<y-'0'+2;j++){
				if(j==-1)continue;
				if(j==10)continue;
				if(find[i][j]==1)continue;
				
				cleck(i+'0',j+'0',table,find,show);
			}
		}
	}
	else show[x-'0'][y-'0']='0'+count;
	find[x-'0'][y-'0']=0;
	return 0;
}

void showboom(char show[][10]){
	int i,j;
	system("clear");
	printf("r:restart\tb:back\tq:quit\n");
	printf("  0123456789\n");
	for(i=0;i<10;i++){
		printf("%d|",i);
		for(j=0;j<10;j++){
			printf("%c",show[i][j]);
		}
		printf("|%d\n",i);
	}
	printf("  0123456789\n");
}


