/*
 * test.c
 *
 *  Created on: 2017/11/14
 *      Author: mcc
 */

#include <stdio.h>
#include <stdlib.h>

#include "head.h"

int test2()
{
	int a;
	a=func();
	a+=func();
	return a;
}
