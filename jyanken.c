/**
 * じゃんけんゲーム
 * 何回でもプレイ可能
 * セーブデータ作成　←　コンピューターが出す手の元になる
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "jyanken-func.h"
#include "libcol-t.h"
#include "libchose-t.h"


enum{
	YES,
	NO,
};


///メイン関数
int main(void)
{
	int flag001 = 0;
	char conami[10];
	const char *end[] = {"YES", "NO"};
	int endcmd;	//終了確認コマンド用変数
	
	srand(time(NULL));	//乱数にシード値を与える
	
	//メインループ
	do{
		switch(title()){
			
			case USSAVE:
				dojyan(selectsave());
			break;
			
			case NOSAVE:
				dojyan(-1);
			break;
			
			case KONAMIKOMAND:
				printf("COMMAND:");
				scanf("%10s", conami);
				if(strcmp(conami, "wwssadadxz") == 0){
					if(onlywon == 0){
						onlywon = 1;
						printf("CheatCommand001 : ON\n");
					}else{
						onlywon = 0;
						printf("CheatCommand001 : OFF\n");
					}
				}else {
					printf("There is no command : %s\n", conami);
					flag001++;
					if(flag001 == 30){
						printf("Hint :  Applied conami command\n");
					}
				}
			break;
			
			case END_COM:	//終了コマンド
				printf("終了しますか？\n");
				choices(end, 2, &endcmd);
				if(endcmd == YES){
					printf("終了します\n");
					return 0;		//<<--------------------正規終了位置
				}
			break;	//タイトルに戻る
			}
		}while(1);
}


