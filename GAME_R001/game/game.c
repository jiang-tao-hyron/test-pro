#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "game1024.h"
#include "findboom.h"
#include "picture.h"
#include "killthree.h"

int game1024(void);
char findboom(void);
int picture(void);
int killthree(void);
char mod;
unsigned int score=0;

int main(int argc, char *argv[]){
	system("clear");

	printf("1:game1024\t2:findboom\t3:picture\t4:killthree\tq:quit\n");
	scanf("%c",&mod);

	while(1){
		system("sleep 0.5");
		switch(mod){
			case '1':mod=game1024();break;
			case '2':mod=findboom();break;
			case '3':mod=picture();break;
			case '4':mod=killthree();break;
			case 'b':scanf("%c",&mod);break;
			case 'q':return 0;
			default:printf("1:game1024\t2:findboom\t3:picture\t4:killthree\tq:quit\n");
					scanf("%c",&mod);
		}
	}

	return 0;
}

int game1024(void){
	int i,j;
	char key,k;
	int blank[4][4];
	score=0;
	init_game1024(blank);
	system("clear");
	while(1){
		show1024(blank,&score);
		
		system("sleep 1");
		system("clear");
		if(geton(blank,&score)==0){
			printf("gameover!score=%d\n",score);
			return 0;
		}
		
		show1024(blank,&score);

		k=getkey(&key);
		if(k)return k;
		
		system("clear");
		change(&key,blank,&score);

	}
	return 0;
}

char findboom(void){
	char x,y;
	char k;
	int table[10][10];
	char find[10][10];
	char show[10][10];
	
	table_init(table,find,show);

	while(1){
		showboom(show);
		
		k=getkey_f(&x,&y);
		if(k)return k;
		
		if(cleck(x,y,table,find,show)){
			showboom(show);
			printf("gameover\n");
			
			return 'b';
		}
	}
	return 0;
}

int picture(void){
	char table_p[3][3];
	int center[2];
	int i,j,win;
	char key_p[2];
	char k;
	center[0]=2;
	center[1]=2;
	
	init_p(table_p,center);
	system("clear");
	
	show_p(table_p);
	
	while(1){

		k=getkey_p(key_p);
		if(k)return k;
		
		system("clear");
		win = change_p(table_p,center,key_p);
		
		show_p(table_p);
		
		if(win){printf("win\n");break;}
	}

	return 'b';
}


int killthree(){
	int table[10][10];
	char key[5],k;

	srand(time(NULL));
	
	kill_init(table);
	system("sleep 1");
	system("clear");
	
	while(1){
		show(table);
		system("sleep 1");
		system("clear");
		
		while( kill_check(table) ){
			show(table);
			system("sleep 1");
			system("clear");
			relive(table);
			show(table);
			system("sleep 1");
			system("clear");
		}
		
		show(table);
		
		k=getkey_k(key);
		if(k)return k;
		
		system("sleep 1");
		system("clear");
		
		change_k(table,key);
		show(table);
		system("sleep 1");
		system("clear");
		
		if( kill_check(table) == 0){
			change_k(table,key);
		}
		else {
			show(table);
			system("sleep 1");
			system("clear");
			relive(table);
		}

	}
	
	return 0;
}
