#include <stdio.h>
#include <string.h>
#define filename "data.txt"
							//"/home/mcc/workspace/find/src/data.txt"

int read(void);
int write(int x);
void check(void);
int update(int* x,int* y);
int a[9][9][10];
int main(int argc, char *argv[])
{
	
	int i=0,j=0;
	if( read()==-1)return 0;
	write(1);

	
	
	
	for(i=0;i<9;i++){
		for(j=0;j<9;j++){
			printf("%d",a[i][j][0]);
		}
		printf("\n");
	}

	 return 0;
}
int read(void){
	FILE *fp=NULL;
	int i=0,j=0,h=0;
	char p;
	fp=fopen(filename,"r");
	if ( !fp ){
		printf("open file error\n");
		return -1;
	 }
	 while( !feof(fp) ){
		 for(i=0;i<9;i++){
			 for(j=0;j<10;j++){
				fscanf( fp,"%c",&p);
				if(p=='\n')
					break;
				
				if(p=='0')
					for(h=0;h<10;h++)a[i][j][h]=h;
				
				a[i][j][0]=p-48;
				printf("%c",a[i][j][0]); 
			 }
			 printf("\n"); 
		 }
		 break;
	 }
	printf("\n");
	
	fclose(fp);
	return 1;
}

int write(int x){
	int i,j,c,p=-1,finsh=0;
	int xp,yp;
	int X[10];
	int Y[10];
	
	for(i=0;i<9;i++){
		X[i]=9;
		Y[i]=9;
	}
	
	while(1){
		finsh =0;
		for(i=0;i<9;i++){
			for(j=0;j<9;j++){
				if(a[i][j][0]==0)finsh=1;
			}
		}
		if(finsh){
			check();
			c=update(&xp,&yp);
			switch(c){
				case 0:  break;
				case 1:  break;
				default:{
						p++;
						X[p]=xp;
						Y[p]=yp;
				}
			}
		}
		else return 0;
		
	}
	return 0;
}

void check()
{
	int i=0,j=0,x=0;
	int p;
	
	for(i=0;i<9;i++){
		for(j=0;j<9;j++){	
			if(a[i][j][0]!=0){
				p=a[i][j][0];
				for(x=0;x<9;x++){
					if(a[i][x][0]==0)a[i][x][p]=0;
					if(a[x][j][0]==0)a[x][j][p]=0;	
				 }
				for(x=0;x<9;x++){
					if(a[i/3*3][j/3*3][0]==0)a[i/3*3][j/3*3][p]=0;
					if(a[i/3*3][j/3*3+1][0]==0)a[i/3*3][j/3*3+1][p]=0;
					if(a[i/3*3][j/3*3+2][0]==0)a[i/3*3][j/3*3+2][p]=0;
				}
			 }		
		 }
	 }
 }

int update(int* x,int* y){
	int i=0,j=0,h=0,p=0;
	int count=0,count_min=9;
	for(i=0;i<9;i++){
		for(j=0;j<10;j++){
			if(a[i][j][0]==0){
				count = 0;
				for(h=1;h<10;h++){
					if(a[i][j][h]!=0){
						p=a[i][j][h];
						count++;
					}
				}
				if(count<count_min)count_min = count;
				if(count==1)a[i][j][0]=p;
				if(count==0)return 0;
			}
		}
	}
	return count_min;
}







