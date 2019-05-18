//ORIGINAL-CHOICES-LIBRARY
//Header File:	libchose-t.h


#include <stdio.h>
#include "libcol-t.h"
#include "libin-t.h"
#include "libwait-t.h"


//選択肢関数
//引数：		選択肢の文字列配列, 配列の要素数, 結果代入用変数
//戻り値：	正常なら0、異常なら−1
int choices(const char *cho[], int sizech, int *num);


//選択肢関数
//引数：		選択肢の文字列配列, 配列の要素数, 結果代入用変数
//戻り値：	正常なら0、異常なら−1
int choices(const char *cho[], int sizech, int *num)
{
	int i;		//ループ用変数
	int seek = 0;		//現在選択している選択肢の位置（デフォルトでは一番上を選択）
	
	printf(GREEN "\t\t\t\t\t十字キーの上下で選択、Enterキーで決定\n" RESET);
	
	//メインループ
	do{
		
		printf(BLUE "No:%d\n" RESET, seek);
		
		//表示
		for(i = 0; i < sizech; i++){
			
			if(i == seek){		//現在選択しているところを反転
				printf(REV_C "%s\n" RESET, cho[i]);
			}else{
				printf("%s\n", cho[i]);
			}
		}
		
		startIN();		///Rawモード開始
		fflush(stdin);
		//入力受付
		switch(getchar()){
			
			case 65:		//上
				if(!(seek <= 0)){		//seekが0以下なら変えない
					seek--;
				}
			break;
			
			case 66:		//下
				if(!(seek + 1 >= sizech)){		//seekがsizech - 1以上なら変えない
					seek++;
				}
			break;
			
			case 13:		//Enter
				//現在位置（＝選択肢番号）を返す
				*num = seek;
				endOUT();
				return 0;		//<<------------------正規終了位置
			break;
			
			default:
			break;
		}
		endOUT();		///Rawモード終了
		
		//表示した文字の消去
		for(i = 0; i < sizech + 2; i++) printf("\x1b[1A");
		
	}while(1);
	
	return -1;
}

