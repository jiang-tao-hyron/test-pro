#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define bg_red		"\033[41m"
#define bg_write	"\033[47m"
#define bg_blue		"\033[44m"
#define bg_purple	"\033[45m"
#define word_black	"\033[30m"
#define word_yellow	"\033[33m"
#define man 		"卒"
#define car			"車"
#define horse 		"馬"
#define elephant 	"象"
#define king 		"王"
#define queen		"后"

void show1(char* key1);
void show2(char* key1,char* key2);
int getkey1(char* key1);
int getkey2(char* key2);
void walk(char* key1,char* key2);

void play_man_black(char* key);
void play_man_yellow(char* key);
void play_car(char* key);
void play_horse(char* key);
void play_elephant(char* key);
void play_king(char* key);
void play_queen(char* key);

extern char table[8][8];
extern unsigned char bg[8];
unsigned int play=0;

void play_man_black(char* key1) {
	int i,j;
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			if (key1[1] - (8-i+'0') == -1 && key1[0] == (j+'A') && table[i][j] == 0)
				bg[i] |= 1<<j;
			if (key1[1] == '2' && (8-i+'0') == '4' && key1[0] == (j+'A') && table[i][j] == 0)
				bg[i] |= 1<<j;
			if (key1[1] - (8-i+'0') == -1 && abs(key1[0] - (j+'A')) == 1 && table[i][j])
				bg[i] |= 1<<j;
		}
	}
}

void play_man_yellow(char* key1) {
	int i,j;
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			if (key1[1] - (8-i+'0') == 1 && key1[0] == (j+'A') && table[i][j] == 0)
				bg[i] |= 1<<j;
			if (key1[1] == '7' && (8-i+'0') == '5' && key1[0] == (j+'A') && table[i][j] == 0)
				bg[i] |= 1<<j;
			if (key1[1] - (8-i+'0') == 1 && abs(key1[0] - (j+'A')) == 1 && table[i][j])
				bg[i] |= 1<<j;
		}
	}
}

void play_car(char* key1){
	int i,j;
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			if(key1[0] == (j+'A') || key1[1] == (8-i+'0'))
				bg[i] |= 1<<j;
		}
	}
}

void play_horse(char* key1){
	int i, j;
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			if(abs(key1[0]-(j+'A')) == 1 && abs(key1[1]-(8-i+'0')) == 2)
				bg[i] |= 1 << j;
			if(abs(key1[0]-(j+'A')) == 2 && abs(key1[1]-(8-i+'0')) == 1)
				bg[i] |= 1 << j;
		}
	}
}

void play_elephant(char* key1){
	int i,j;
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			if(abs(key1[0]-(j+'A')) == abs(key1[1]-(8-i+'0')))
				bg[i] |= 1<<j;
		}
	}
}

void play_king(char* key1){
	int i,j;
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			if(abs(key1[0]-(j+'A')) <= 1 && abs(key1[1]-(8-i+'0')) <= 1)
				bg[i] |= 1<<j;
		}
	}
}

void play_queen(char* key1) {
	int i, j;
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			if (key1[0] == (j+'A') || key1[1] == (8-i+'0')
					|| abs(key1[0] - (j+'A')) == abs(key1[1]-(8-i+'0')))
				bg[i] |= 1 << j;
		}
	}
}

int getkey1(char* key1) {
	memset(bg, 0, 8);
	if (table[8 - key1[1] + '0'][key1[0] - 'A'] == 0)
		return 0;
	if (table[8 - key1[1] + '0'][key1[0] - 'A'] / 7) {
		if (play % 2 )
			return 0;
		switch (table[8 - key1[1] + '0'][key1[0] - 'A']) {
		case 8:
			play_car(key1);
			break;
		case 9:
			play_horse(key1);
			break;
		case 10:
			play_elephant(key1);
			break;
		case 11:
			play_king(key1);
			break;
		case 12:
			play_queen(key1);
			break;
		case 13:
			play_man_black(key1);
			break;
		}
		return 1;
	} 
	else {
		if (play % 2== 0)
			return 0;
		switch (table[8 - key1[1] + '0'][key1[0] - 'A']) {
		case 1:
			play_car(key1);
			break;
		case 2:
			play_horse(key1);
			break;
		case 3:
			play_elephant(key1);
			break;
		case 4:
			play_king(key1);
			break;
		case 5:
			play_queen(key1);
			break;
		case 6:
			play_man_yellow(key1);
			break;
		}
		return 1;
	}
}


int getkey2(char* key2){
	if(bg[8- (key2[1]-'0') ] & 1 << (key2[0]-'A') )return 1;
	return 0;
}

void walk(char* key1,char* key2){
	table[8-key2[1]+'0'][key2[0]-'A'] = table[8-key1[1]+'0'][key1[0]-'A'];
	table[8-key1[1]+'0'][key1[0]-'A'] = 0;
}

void show1(char* key1){
	int i,j;
	char write[256]={0};
	char other[2]={0,0};
	
	printf("第%d歩",play);
	if(play%2)printf(" 黄走\n");
	else printf(" 黒走\n");
	system("echo -e ' ＡＢＣＤＥＦＧＨ'");
	for(i=0;i<8;i++){
		strcat(write,"echo -e '\33[0m");
		other[0]=8-i+'0';
		strcat(write,other);
		for(j=0;j<8;j++){
			if (bg[i] & 1 << j)
				strcat(write, bg_blue);
			else if ((i + j) % 2)
				strcat(write, bg_write);
			else
				strcat(write, bg_red);
			if (key1[0]==(j+'A') && key1[1]==(8-i+'0'))
				strcat(write, bg_purple);
			
			if(table[i][j]/7)strcat(write,word_black);
			else strcat(write,word_yellow);
			
			switch (table[i][j]%7){
			case 0:strcat(write,"  ");break;
			case 1:strcat(write,car);break;
			case 2:strcat(write,horse);break;
			case 3:strcat(write,elephant);break;
			case 4:strcat(write,king);break;
			case 5:strcat(write,queen);break;
			case 6:strcat(write,man);break;
			}
		}
		strcat(write,"\033[0m");
		strcat(write,other);
		strcat(write,"'");
		system(write);
		memset(write, 0, sizeof(write));
	}
	system("echo -e ' ＡＢＣＤＥＦＧＨ'");
}

void show2(char* key1,char* key2){
	int i,j;
	char write[256]={0};
	char other[2]={0,0};
	play++;
	printf("第%d歩",play);
	if(play%2)printf(" 黄走\n");
	else printf(" 黒走\n");
	system("echo -e ' ＡＢＣＤＥＦＧＨ'");
	for(i=0;i<8;i++){
		strcat(write,"echo -e '\33[0m");
		other[0]=8-i+'0';
		strcat(write,other);
		for(j=0;j<8;j++){
			if (key1[0]==(j+'A') && key1[1]==(8-i+'0') || key2[0]==(j+'A') && key2[1] == (8-i+'0'))
				strcat(write, bg_purple);
			else if ((i + j) % 2)
				strcat(write, bg_write);
			else
				strcat(write, bg_red);
			
			if(table[i][j]/7)strcat(write,word_black);
			else strcat(write,word_yellow);
			
			switch (table[i][j]%7){
			case 0:strcat(write,"  ");break;
			case 1:strcat(write,car);break;
			case 2:strcat(write,horse);break;
			case 3:strcat(write,elephant);break;
			case 4:strcat(write,king);break;
			case 5:strcat(write,queen);break;
			case 6:strcat(write,man);break;
			}
		}
		strcat(write,"\033[0m");
		strcat(write,other);
		strcat(write,"'");
		system(write);
		memset(write, 0, sizeof(write));
	}
	system("echo -e ' ＡＢＣＤＥＦＧＨ'");
}
