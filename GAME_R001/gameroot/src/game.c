
#include <stdio.h>
#include <stdlib.h>

void mycellup(int fn);
void mycelldown(int fn);
void mycell0(void);
void mycell1(void);
void mycell2(void);
void mycell3(void);
void mycell4(void);
void mycell5(void);
void mycell6(void);
void mycell7(void);
void mycell8(void);
void mycell9(void);

char n[10];//読み込み表示
int s[11];//順番表示
int ps=-1;//順番表示
int nn[10]={20,15,-13,14,-15,25,-12,-10,30,-20};//cell値
int x=0;//合計

int main(int argc, char *argv[]){
	int i,mod;
	for(i=0;i<11;i++){
		n[i]=1;
		s[i]=-1;
	}
	printf("入力MOD:");
	scanf("%d",&mod);
	if(mod)
		for(i=0;i<10;i++){
				printf("入力数%d:",i+1);
				scanf("%d",&nn[i]);
			}
	mycell0();
	
    return 0;
}

void mycellup(int fn){
	ps++;
	s[ps]=fn;
	x+=nn[fn];
	n[fn]=0;
}
void mycelldown(int fn){
	n[fn]=1;
	x-=nn[fn];
	s[ps]=-1;
	ps--;
}

void mycell0(){
	mycellup(0);
	if(n[1]) mycell1();
	if(n[2]) mycell2();
	mycelldown(0);
}
void mycell1(){
	mycellup(1);
	if(n[0]) mycell0();
	if(n[3]) mycell3();
	if(n[4]) mycell4();
	mycelldown(1);
}
void mycell2(){
	mycellup(2);
	if(n[0]) mycell0();
	if(n[4]) mycell4();
	if(n[5]) mycell5();
	if(n[6]) mycell6();
	mycelldown(2);
}
void mycell3(){
	mycellup(3);
	if(n[1]) mycell1();
	if(n[7]) mycell7();
	mycelldown(3);
}
void mycell4(){
	mycellup(4);
	if(n[1]) mycell1();
	if(n[2]) mycell2();
	if(n[8]) mycell8();
	mycelldown(4);
}
void mycell5(){
	mycellup(5);
	if(n[2]) mycell2();
	if(n[6]) mycell6();
	if(n[8]) mycell8();
	if(n[9]) mycell9();
	mycelldown(5);
}
void mycell6(){
	mycellup(6);
	if(n[2]) mycell2();
	if(n[5]) mycell5();
	if(n[9]) mycell9();
	mycelldown(6);
}
void mycell7(){
	mycellup(7);
	if(n[3]) mycell3();
	if(n[8]) mycell8();
	if(n[9]) mycell9();
	mycelldown(7);
}
void mycell8(){
	mycellup(8);
	if(n[4]) mycell4();
	if(n[5]) mycell5();
	if(n[7]) mycell7();
	mycelldown(8);
}
void mycell9(){
	mycellup(9);
	printf("%d\t",x);
	
	int i;
	for(i=0;i<11;i++){
		if(s[i]==-1){printf("\n");break;}
		printf("%d ",s[i]);
	}
	mycelldown(9);
}
