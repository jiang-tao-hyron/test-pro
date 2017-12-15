#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]){
	char word_r[20];
	char word_w[21];
	char word_c[21];
	word_c[30]=0;
	int i,count;
	while(1){
		for(i=0;i<20;i++){
			word_c[i]=0;
			word_r[i]=rand()%26+97;
			printf("%c",word_r[i]);
		}
		printf("\n");
		
		scanf("%s",word_w);
		
		count=20;
		for(i=0;word_w[i]!=0;i++){
			if(word_w[i]!=word_r[i])
				word_c[i]=word_r[i];
			else{
				count--;
				word_c[i]='_';
			}
		}
		for(i=0;word_w[i]!=0;i++){
			printf("%c",word_c[i]);
		}
		printf("\nエラー数：%d\t正確率：%d%c\n",count,(20-count)*100/20,'%');
	}
	
	return 0;
}
