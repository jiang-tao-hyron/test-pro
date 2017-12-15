/*
 * test.c
 *
 *  Created on: 2017/01/22
 *      Author: mcc
 */

#include <stdio.h>
#include <string.h>

#define filename "data"

int read(char chess[][5]);
void show(char table[][9][13],char air[][41],char chess[][5]);
void showchess(char chess);
int main(int argc, char *argv[]) {
	char table[12][9][13]={
			{" ┌──","─","本営","─","─┬──","─","本営","─","─┐"},
			{" ┣━━","━","━┿━━","━","━┿━━","━","━┿━━","━","━┫"},
			{" ┠──","─","行営","─","─┼──","─","行営","─","─┨"},
			{" ┠──","─","─┼──","─","行営","─","─┼──","─","─┨"},
			{" ┠──","─","行営","─","─┼──","─","行営","─","─┨"},
			{" ┣━━","━","━┻━━","━","━╋━━","━","━┻━━","━","━┫"},
			{" ┣━━","━","━┳━━","━","━╋━━","━","━┳━━","━","━┫"},
			{" ┠──","─","行営","─","─┼──","─","行営","─","─┨"},
			{" ┠──","─","─┼──","─","行営","─","─┼──","─","─┨"},
			{" ┠──","─","行営","─","─┼──","─","行営","─","─┨"},
			{" ┣━━","━","━┿━━","━","━┿━━","━","━┿━━","━","━┫"},
			{" └──","─","本営","─","─┴──","─","本営","─","─┘"},
	};
	char air[12][41]={
			{" │    │    │    │    │"},
			{" ┃ ↘  │ ↙  │ ↘  │ ↙  ┃"},
			{" ┃ ↗  │ ↖  │ ↗  │ ↖  ┃"},
			{" ┃ ↘  │ ↙  │ ↘  │ ↙  ┃"},
			{" ┃ ↗  │ ↖  │ ↗  │ ↖  ┃"},
			{" ┃         ┃         ┃"},
			{" ┃         ┃         ┃"},
			{" ┃ ↘  │ ↙  │ ↘  │ ↙  ┃"},
			{" ┃ ↗  │ ↖  │ ↗  │ ↖  ┃"},
			{" ┃ ↘  │ ↙  │ ↘  │ ↙  ┃"},
			{" ┃ ↗  │ ↖  │ ↗  │ ↖  ┃"},
			{" │    │    │    │    │"}
	};
	char chess[12][5]={0};
	
	
	
	read(chess);
	show(table,air,chess);
	
	return 0;
}


int read(char chess[][5]) {
	FILE *fp = NULL;
	int i = 6,j=0;
	char p;
	fp = fopen(filename, "r");
	if (!fp) {
		printf("open file error\n");
		return -1;
	}
	while (!feof(fp)) {
		fscanf(fp, "%c", &p);
		if (p == '\n') {
			i++;
			j = 0;
			continue;
		}
		chess[i][j] = p;
		j++;
	}
	printf("\n");

	fclose(fp);
	return 1;
}

void show(char table[][9][13],char air[][41],char chess[][5]){
	int i,j;
	
	for(i=0;i<6;i++){
		for (j = 0; j < 9; j++) {
			if (j % 2 == 0 && chess[i][j / 2]!='0')
				printf("■■■ ");
			else
				printf("%s", table[i][j]);
			}
		printf("\n");
		printf("%s", air[i]);
		printf("\n");
	}
	
	for(i=6;i<12;i++){
		printf("%s", air[i]);
		printf("\n");
		for (j = 0; j < 9; j++) {
			if (j % 2 == 0 && chess[i][j / 2]!='0')
				showchess(chess[i][j / 2]);
			else
				printf("%s", table[i][j]);
		}
		printf("\n");

	}
}

void showchess(char chess){
	switch(chess){
	case '1':printf("工兵");break;
	case '2':printf("排長");break;
	case '3':printf("連長");break;
	case '4':printf("営長");break;
	case '5':printf("団長");break;
	case '6':printf("旅長");break;
	case '7':printf("師長");break;
	case '8':printf("軍長");break;
	case '9':printf("司令");break;
	case 'a':printf("軍旗");break;
	case 'b':printf("地雷");break;
	case 'c':printf("炸弾");break;
		
	}
}





