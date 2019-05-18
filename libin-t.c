//ORIGINAL-INPUT-LIBRARY
//Header File:	libin-t.h


#include <stdio.h>
#include <unistd.h>
#include <termios.h>


//端末設定保存用グローバル変数
struct termios CookedTermlos;	//cookedモード用
struct termios RawTermlos;		//rawモード用


//端末をRawモードにする
void startIN(void);


//Rawモードを終了する
void endOUT(void);


void startIN(void)
{
	//初期状態の端末設定を取得・保存
	tcgetattr(STDIN_FILENO, &CookedTermlos);
	
	//rawモードの端末設定を作成・保存
	RawTermlos = CookedTermlos;
	cfmakeraw(&RawTermlos);
	
	//端末をRawモードにする
	tcsetattr(STDIN_FILENO, 0, &RawTermlos);
	
	return;
}


void endOUT(void)
{
	//端末設定を元に戻す
	tcsetattr(STDIN_FILENO, 0, &CookedTermlos);
	putchar('\n');
	
	return;
}

