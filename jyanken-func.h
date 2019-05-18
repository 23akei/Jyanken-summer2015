/**
 * じゃんけん関連ヘッダーファイル
 * ソースファイル：jyanken.c
 */


#ifndef __DEFINE__JYANKEN__
#define __DEFINE__JYANKEN__


#include <stdio.h>



#define VERSION "0.0.2\0"
#define ONESET 10	//１セットの勝負回数
#define MAX_CURRENT 5	//記録する手の数


/**
 * システム情報の書かれたファイル
 * ＊半角スペースで区切る
 * ファイル名：	system.jsysd
 * 内容：	バージョン 前回実行時間 セーブデータ数 ファイル名,・・・
 * 	
 * セーブファイル
 * 半角スペースで区切る
 * 
 * 内容：	バージョン 出した手の数（勝負した回数） 勝率 それぞれの手の回数 あいこの次に出す手（前のコンピューターに勝つ、負ける、同じ）
 * 			手のパターン（グーの次にグー・・・） 最近のMAX_CURRENT手
 */


/*
 * セーブデータ読み込み用変数関連
 */
extern char ver[10];	//バージョン
extern int buttles;	//対戦回数
extern float wonper;	//勝率
extern int each[3];	//それぞれの手	ここからnexまでにおいて・・・　配列の要素に GU TYOKI PA WON LOSE SAME でアクセスする
extern int same[3];	//あいこの次
extern int nex[3][3];	//出した手の次 
extern int last100[MAX_CURRENT];	//最近のMAX_CURRENT手 **出した順ではない
extern int nowseek;	//last100への次の書き込み位置
extern int gus, pas, tyos;	//それぞれの手の回数
extern int befhand;	//直前に出した手


//プレイヤーの手
extern int humhand;


//必勝フラグ1
int onlywon;


//手の文字
extern char *hand[];


//タイトルで選択するコマンド
extern enum {
	USSAVE,		//セーブデータ使用
	NOSAVE,		//セーブデータを使用しない
	END_COM,		//終了
	KONAMIKOMAND,	//隠し要素
}noname;


/*
 * セーブデータ作成関数
 * 引数：	システムファイルのファイルポインタ
 */
extern void makesf(FILE *);


/*
 * 現在時間を求める関数
 * 戻り値：	現在時間（文字列）
 */
extern char *getntime(void);


/*
 * タイトル表示＆モード選択関数
 * 戻り値：	コマンド (USSAVE, NOSAVE, END)
 */
extern int title(void);


/*
 * セーブデータ選択関数
 * 戻り値：	選択したセーブデータのファイルナンバー（失敗なら−１）
 */
extern int selectsave(void);


/*
 * コマンド入力関数
 * 戻り値：	値
 * 空白文字を返さない
 */
extern int getcmd(void);


/*
 * じゃんけん実行関数
 * 引数：	セーブファイルナンバー
 */
extern void dojyan(int);


/*
 * データ書き込み,読み込み関数
 * 引数：	セーブデータのファイルポインタ,モード：ｗ　or　ｒ
 */
extern void save_load(FILE*, char);

/*
 * 内容解析関数（コンピューターの手を決定）
 */
extern int analyze();


#endif
