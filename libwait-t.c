//ORIGINAL-WAIT-LIBRARY
//Header File:	libwait-t.h


#include <stdio.h>
#include <time.h>

#define MILI_SEC 1000000	//nanosleep関数の待機する時間の基準は小さすぎるので、この数をかけることでミリ秒単位で指定できる


//ウェイト関数
//引数：		ウェイト時間（単位：ミリ秒）
void waitms(int milisec);


//ウェイト関数
//引数：		ウェイト時間（単位：マイクロ秒）
void waitus(int microsec);


//ウェイト関数
//引数：		ウェイト時間（単位：ミリ秒）
void waitms(int milisec)
{
	struct timespec req = {0, milisec * MILI_SEC};	//nanosleep関数に渡す構造体
	nanosleep(&req, NULL);
	
	return;
}

//ウェイト関数
//引数：		ウェイト時間（単位：マイクロ秒）
void waitus(int microsec)
{
	struct timespec req = {0, microsec * MILI_SEC / 1000};	//nanosleep関数に渡す構造体
	nanosleep(&req, NULL);
	
	return;
}

