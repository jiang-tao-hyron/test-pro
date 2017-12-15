#include <stdio.h>
#include <string.h>
#define filename "data.txt"
							//"/home/mcc/workspace/find/src/data.txt"

int read(void);
void find(char* word);
char a[10][10];
int main(int argc, char *argv[])
{
	if( read()==-1)return 0;
	char word[10];
	do{
		scanf("%s",word);
		if(word[0]=='0')break;
		//printf("%s\n",word);
		find(word);
	}
	while(1);

	 return 0;
}
int read(void){
	FILE *fp=NULL;
	int i=0,j=0;
	char p;
	fp=fopen(filename,"r");
	if ( !fp ){
		printf("open file error\n");
		return -1;
	 }
	 while( !feof(fp) ){
		 for(i=0;i<10;i++){
			 for(j=0;j<11;j++){
				fscanf( fp,"%c",&p);
				if(p=='\n') break;
				a[i][j]=p;
				printf("%c",a[i][j]); 
			 }
			 printf("\n"); 
		 }
		 break;
	 }
	printf("\n");
	
	fclose(fp);
	return 1;
}

void find(char* word){
	int i=0,j=0,p=0;
	int len =strlen(word);
	
	for(i=0;i<10;i++){
		for(j=0;j<10;j++){
			while(p<len){//right
				if(a[i][j+p] != word[p])break;
				p++;
			}	
			if(p==len)printf("(%d,%d)right\t",i,j);
			p=0;
			
			while(p<len){//left
				if(a[i][j-p] != word[p])break;
				p++;
			}	
			if(p==len)printf("(%d,%d)left\t",i,j);
			p=0;
			
			while(p<len){//down
				if(a[i+p][j] != word[p])break;
				p++;
			}	
			if(p==len)printf("(%d,%d)down\t",i,j);
			p=0;
						
			while(p<len){//up
				if(a[i-p][j] != word[p])break;
				p++;
			}	
			if(p==len)printf("(%d,%d)up\t",i,j);
			p=0;
		}
	}
	printf("\n");
}

