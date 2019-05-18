//ORIGINAL-WAIT-LIBRARY
//Souce File:	libwait-t.c


#ifndef __INCLUDE_LIB_WAIT__
#define __INCLUDE_LIB_WAIT__


#include <stdio.h>


//ウェイト関数
//引数：		ウェイト時間（単位：ミリ秒）
extern void waitms(int milisec);


//ウェイト関数
//引数：		ウェイト時間（単位：マイクロ秒）
void waitus(int microsec);


#endif

