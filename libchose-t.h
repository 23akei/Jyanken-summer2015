//ORIGINAL-CHOICES-LIBRARY
//Souce File:	libchose-t.c


#ifndef __INCLUDE_LIB_CHOICES__
#define __INCLUDE_LIB_CHOICES__


#include <stdio.h>
#include "libcol-t.h"
#include "libin-t.h"


//選択肢関数
//引数：		選択肢の文字列配列, 配列の要素数, 結果代入用変数
//戻り値：	正常なら0、異常なら−1
extern int choices(const char *cho[], int sizech, int *num);


#endif

