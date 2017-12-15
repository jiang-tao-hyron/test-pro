/*
 * test.c
 *
 *  Created on: 2017/11/14
 *      Author: mcc
 */

#include <stdio.h>
#include <stdlib.h>

//#include "head.h"
#include "test2.c"

int main(int argc, char **argv)
{
	int bb;
	bb=func();
	bb+=func();
	bb+=func();
	bb+=func();
	bb+=func();
	bb+=func();
	bb+=func();
	bb+=func();
	bb+=func();
	bb+=func();
	bb+=func();
printf("bb=%d\n",bb);
printf("bb=%d\n",test2());
	return 0;
}
