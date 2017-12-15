/*
 * head.h
 *
 *  Created on: 2017/11/14
 *      Author: mcc
 */


#include <stdio.h>
#include <stdlib.h>

int func()
{
int i=0;
int b=0;
int c =0;
for (i=0;i<10;i++)
{
	b=b+i;
	c=c+i;
}
for (i=0;i<10;i++)
{
	b=b+i;
	c=c+i;
}
for (i=10;i<20;i++)
{
	b=b+i;
	c=c+i;
}
for (i=20;i<30;i++)
{
	b=b+i;
	c=c+i;
}
	return c;
}

