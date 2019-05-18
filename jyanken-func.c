/**
 * じゃんけん関連関数ソースファイル
 * ヘッダーファイル：jyanken-func.h
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "libcol-t.h"
#include "libchose-t.h"


#define VERSION "0.0.1\0"
#define ONESET 10	//１セットの勝負回数
#define MAX_CURRENT 3	//記録する手の数


#define error_naibuti do{fprintf(stderr, RED "エラー発生！内部値が異常です\n終了します。\n\n" RESET); }while(0)


/*
 * セーブデータ読み込み用変数関連
 */
char ver[10];	//バージョン
int buttles = 0;	//対戦回数
float wonper;	//勝率
int each[3];	//それぞれの手	ここからnexまでにおいて・・・　配列の要素に GU TYOKI PA or WON LOSE SAME でアクセスする
int same[3];	//あいこの次
int nex[3][3];	//出した手の次 
int last100[MAX_CURRENT];	//最近のMAX_CURRENT手 **出した順ではない
int nowseek = 0;	//last100への次の書き込み位置
int gus = 0, pas = 0, tyos = 0;	//それぞれの手の回数
int befhand = -1;	//直前に出した手
int aikf = 0;	//あいこフラグ

//プレイヤーの手
int humhand;


//必勝フラグ
int onlywon;


/*
 * 手の列挙体
 */
enum {
	GU = 0,
	TYOKI,
	PA,
	WON = 0,	//ここからあいこ判定用
	LOSE,
	SAME,
}noname;


//手の文字
char *hand[] = {"グー", "チョキ", "パー",};


//タイトルで選択するコマンド
enum{
	USSAVE,		//セーブデータ使用
	NOSAVE,		//セーブデータを使用しない
	END_COM,		//終了
	KONAMIKOMAND,	//隠し要素
};


/*
 * セーブデータ作成関数
 * 引数：	システムファイルのファイルポインタ
 */
void makesf(FILE *);


/*
 * 現在時間を求める関数
 * 戻り値：	現在時間（文字列）
 */
char *getntime(void);


/*
 * タイトル表示＆モード選択関数
 * 戻り値：	コマンド (USSAVE, NOSAVE, END)
 */
int title(void);


/*
 * セーブデータ選択関数
 * 戻り値：	選択したセーブデータのファイルナンバー（失敗なら−１）
 */
int selectsave(void);


/*
 * コマンド入力関数
 * 戻り値：	値
 * 空白文字を返さない
 */
int getcmd(void);


/*
 * じゃんけん実行関数
 * 引数：	セーブファイルナンバー
 */
void dojyan(int);


/*
 * データ書き込み,読み込み関数
 * 引数：	セーブデータのファイルポインタ,モード：ｗ　or　ｒ
 */
void save_load(FILE*, char);

/*
 * 内容解析関数（コンピューターの手を決定）
 */
int analyze();


///タイトル表示＆モード選択関数
int title(void)
{
	int command = -1;
	const char *ravel[] = {"コンピューターと対戦", "コンピューターと対戦（セーブデータなし）", "終了", "  "};
	
	printf(BOLD CYAN "\n--------------------じゃんけんゲーム--------------------\n" RESET);
	choices(ravel, 4, &command);		//選択肢
	
	return command;
	
}


///セーブデータ選択関数
int selectsave(void)
{
	int savno = -1, i, savsum;
	char savfname[45], current[45], **ssfc;
	FILE *detafp = NULL;
	
	memset(current, '\0', sizeof(current));
	memset(savfname, '\0', sizeof(savfname));	//配列の初期化
	
	if((detafp = fopen("system.jsyd","r")) == NULL){
		printf(BLUE "セーブモードを実行するのは今回が初めてですね。\n\n" RESET);	//システムファイルがない　＝　初回起動
		
		if((detafp = fopen("system.jsyd", "a")) == NULL){		//初回起動なら、システムファイルを作成
			fprintf(stderr,RED "エラー発生！\nシステムファイルを作成できませんでした。\n" RESET);
			exit(0);
			
		} else {
			printf(BLUE "---------------システムファイルを作成しました。\n\n" RESET);
			fprintf(detafp,"%s %s %d", VERSION, getntime(), 0);	//システムファイルに基本情報を書き込み
			makesf(detafp);
			
		}
	
	}
	
	//ここよりセーブデータ選択ループ
	do{
		freopen("system.jsyd", "r+", detafp);	//システムファイルを再度開く
		fscanf(detafp, "%*s %*s %*s %*d %*d:%*d:%*d %*d %d ", &savsum);
		
		ssfc = (char **)calloc(savsum, sizeof(char *));
		for(i = 0; i < savsum + 1; i++) ssfc[i] = (char *)calloc(45, sizeof(char));	//実体化
		for(i = 1; i < savsum + 1; i++) fscanf(detafp, "%s", ssfc[i]);	//代入
		ssfc[0] = "新規作成";
		
		choices((const char **)ssfc, savsum + 1, &savno);
		
		if(savno == 0)
			makesf(detafp);
		else
			break;
		
		free(ssfc);
	}while(1);
	
	fseek(detafp, 0, SEEK_SET);
	fscanf(detafp, "%*s %*s %*s %*d %*d:%*d:%*d %*d %*d ");
	for(i = 1; i <= savno; i++) fscanf(detafp, "%s ", savfname);
	printf("セーブデータ(" YELLOW "%d" RESET ")\t%s\nを選択しました。\n\n", savno, savfname);
	
	return savno;
}


///新規セーブデータ作成関数
void makesf(FILE *sysfp)
{
	int i, sav;
	char savfname[45];
	FILE *savfp;
	char **sp;	//一時保存用(文字列の可変個配列)
	
	memset(savfname, '\0', sizeof(savfname));
	
	freopen("system.jsyd", "r", sysfp);	//モード統一のためにリオープン
	fscanf(sysfp, "%*s %*s %*s %*d %*d:%*d:%*d %*d %d ", &sav);
	
	sp = (char **)calloc(sav, sizeof(char *));
	for(i = 0; i < sav; i++) sp[i] = (char *)calloc(45, sizeof(char));	//実体化
	for(i = 0; i < sav; i++) fscanf(sysfp, "%s", sp[i]);	//代入
	
	printf("セーブデータを作成します。\n"
		"作成するセーブファイルの名前を入力してください。\n"
		BLUE "＊全角２０文字 or 半角４０文字以内(混合可)-----それ以上は切り捨てます\n" RESET);
	scanf("%40s",savfname);
	while(!isspace(getchar())) ;	//余分に入力された分、バッファから出しておく
	
	printf(BLUE "----------------セーブデータを作成しています。\n" RESET);
	sprintf(savfname, "%s.jsd", savfname);		//拡張子を付け足し
			
	if((savfp = fopen(savfname, "w")) == NULL){		//セーブファイルの作成
		fprintf(stderr,RED "エラー発生！\nセーブファイルを作成できませんでした。\n" RESET);
		exit(0);
		
	} else {
		freopen("system.jsyd", "w", sysfp);	//切り捨て
		
		fprintf(sysfp, "%s %s %d ", VERSION, getntime(), sav + 1);		//バーション　時間　セーブデータ合計数
		for(i = 0; i < sav; i++) fprintf(sysfp, "%s ", sp[i]);	//今までにあったぶんの書き込み直し
		fprintf(sysfp, "%s ", savfname);		//今回作成したデータ
		
		fprintf(savfp, "%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d ",
				VERSION, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);	//セーブファイル内容初期化
		
		printf(BLUE "---------------データNo.%d　%s\tを作成しました。\n\n" RESET,sav,savfname);
	}
	
	fflush(sysfp);	//内容保存(強制)
	fclose(savfp);
	freopen("system.jsyd", "r+", sysfp);	//最終的に"r+"で開き直す。
	
	for(i = 0; i < sav; i++) free(sp[i]);
	free(sp);	//ヒープの解放
	
	return;
}


///現在時間を求める関数
char *getntime(void)
{
	const char wday_name[7][3] = {
		"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
	};
	const char mon_name[12][3] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
	};
	
	time_t ty = time(NULL);
	struct tm *tm1 = localtime(&ty);
	static char ntime[25];
	
	sprintf(ntime, "%.3s %.3s %02d %02d:%02d:%02d %4d",
			wday_name[tm1->tm_wday], mon_name[tm1->tm_mon],
			tm1->tm_mday, tm1->tm_hour, tm1->tm_min,
			tm1->tm_sec, tm1->tm_year + 1900);
	ntime[25] = '\0';
	return ntime;
}



///コマンド入力関数
int getcmd(void)
{
	int cmd;
	while(isspace(cmd = getchar()) && cmd != EOF);
	return cmd;
}


///じゃんけん実行関数
void dojyan(int fnum)
{
	char **hands;
	FILE *savfp, *system = fopen("system.jsyd", "r+");
	int comhand, i, j;		//コンピューターの手、　ループ用変数
	int wons = 0, aik = 0;	//人間の勝った回数、　あいこの回数
	char savf[45];	//セーブファイル名
	memset(savf, '\0', sizeof(savf));
	
	if(fnum != -1){	//セーブデータの情報を表示
		fscanf(system, "%*s %*s %*s %*d %*d:%*d:%*d %*d %*d ");
		for(i = 0; i < fnum; i++){
			fscanf(system, "%s", savf);
		}
		savfp = fopen(savf, "r");
		save_load(savfp, 'r');	//値のロード
		printf(CYAN "\n前回の勝率：" YELLOW "%5.2f\n" CYAN "対戦回数：" YELLOW"%d\n" RESET, wonper, buttles);
	}
	
	//ONESETの回数戦う
	printf(GREEN "\n~~~~~コンピュータとの" YELLOW "%d" GREEN "回勝負~~~~~\n" RESET, ONESET);
	for(j = 1; j <= ONESET; j++){
		
		printf("\n\t\t\t-----" YELLOW "%d" WHITE " / " YELLOW "%d" WHITE
				"-----\n\n" YELLOW "%d勝\t\t" BLUE "%d敗\n\n" WHITE,
				j, ONESET, wons, j - 1 -wons - aik);
		
		//コンピューターの手を決定
		if(fnum == -1){		//fnum == -1なら、NOSAVEモード（コンピューターの手は乱数で決定）
			comhand = rand() % 3;
			
		} else {
			
			savfp = fopen(savf, "r");
			save_load(savfp, 'r');	//値のロード
			comhand = analyze();	//内容解析関数呼び出し
			
			
		}
		
		if(onlywon == 1) printf(MAGENTA "\t\t\tComputer's hand is ... " CYAN "%s\n" WHITE,hand[comhand]);
		
		//プレイヤーの手を入力
		hands = (char **)calloc(3, sizeof(char **));
		for(i = 0; i < PA + 1; i++) hands[i] = (char *)calloc(15, sizeof(char *));
		for(i = 0; i < PA + 1; i++) strcpy(hands[i], hand[i]);
		printf("あなたの手を入力してください\n\n");
		choices((const char **)hands, 3, &humhand);
		
		//手の記録
		if(fnum != -1){	//NOSAVEモードではやらない
			buttles++;	//対戦回数を増加
			
				if(humhand == GU) ++gus, each[GU]++, befhand = GU;
			else if(humhand == TYOKI) ++tyos, each[TYOKI]++, befhand = TYOKI;
			else if(humhand == PA) ++pas, each[PA]++, befhand = PA;
			else{
				fprintf(stderr, RED "エラー：内部データが改ざんされました。\n終了します\n\n" RESET);
				exit(0);
			}
			//データ編集
			if(aikf == 1){	//sameへの書き込み
				switch((befhand - humhand + 3) % 3){
					case 0:
						same[SAME]++;
					break;
					
					case 1:
						same[LOSE]++;
					break;
					
					case 2:
						same[WON]++;
					break;
				}
			}
			
			if(befhand != -1){	//nexへの書き込み。最初の１回はやらない
				switch(befhand){
					case GU:
						if(humhand == GU) nex[GU][GU]++;
						else if(humhand == TYOKI) nex[GU][TYOKI]++;
						else if(humhand == PA) nex[GU][PA]++;
						else{
							error_naibuti;
							exit(0);
						}
					break;
					
					case TYOKI:
						if(humhand == GU) nex[TYOKI][GU]++;
						else if(humhand == TYOKI) nex[TYOKI][TYOKI]++;
						else if(humhand == PA) nex[TYOKI][PA]++;
						else{
							error_naibuti;
							exit(0);
						}
					break;
					
					case PA:
						if(humhand == GU) nex[PA][GU]++;
						else if(humhand == TYOKI) nex[PA][TYOKI]++;
						else if(humhand == PA) nex[PA][PA]++;
						else{
							error_naibuti;
							exit(0);
						}
					break;
					
					default:
						fprintf(stderr, RED "エラー発生！内部値が異常です\n終了します。\n\n" RESET);
						exit(0);
					break;
				}
			}
		}
		
		//手の表示
		printf(GREEN "じゃんけんぽん！\n" WHITE "プレイヤーの手\t\t\t\tコンピューターの手\n" CYAN "\t%s\t\t\t\t\t" RED "%s\n\n\n" WHITE,
				hand[humhand], hand[comhand]);
		
		//判定
		switch((humhand - comhand + 3) % 3){
			case 0:	//引き分け
				printf(BOLD MAGENTA "あいこです。\n" WHITE);
				aikf = 1;
				++aik;
				break;
				
			case 1:	//人間の負け
				printf(BLUE "あなたの負けです。\n" WHITE);
				aikf = 0;
				break;
			
			case 2:	//人間の勝ち
				printf(YELLOW "あなたの勝ちです。\n" WHITE);
				aikf = 0;
				++wons;
				break;
		}

		if(fnum != -1){
			//データ書き込み
			freopen(savf, "w", savfp);
			save_load(savfp, 'w');
			fclose(savfp);
		}
		
		printf(GREEN "\n\nPlease Press Enter Key......\n" WHITE);
		getchar();
		printf("\x1b[2J" RESET);		//画面消去
		
	}
	
	//勝率表示＆記録
	printf(CYAN "\n-----あなたの勝率：" YELLOW "%5.2f%%" CYAN "-----\n**あいこ：" YELLOW "%d" CYAN"回\n\n" WHITE,
			(wonper = (float)wons / ONESET * 100), aik);
	if(fnum != -1){
		
		savfp = fopen(savf, "w");
		save_load(savfp, 'w');
		fclose(savfp);
	}
	
	return;
}


///データ書き込み,読み込み関数
void save_load(FILE *savefp, char mode)
{
	int i, j;	//ループ用変数
	
	if(mode == 'w'){	//書き込みモード
		
		fprintf(savefp, "%s %d %f %d %d %d %d %d %d ",
				VERSION, buttles, wonper, each[GU], each[TYOKI],
				each[PA], same[WON], same[LOSE], same[SAME]);
		for(i = 0; i <= PA; i++) for(j = 0; j <= PA; j++) fprintf(savefp, "%d ", nex[i][j]);
		last100[nowseek] = humhand;	//last100の値の更新
		if(nowseek < MAX_CURRENT) nowseek++;
		else if(nowseek >= MAX_CURRENT) nowseek = 0;		//nowseekの値の更新
		for(i = 0; i < MAX_CURRENT; i++) fprintf(savefp, "%d ", last100[i]);
		fflush(savefp);
		
	}else if(mode == 'r'){	//読み込みモード
		
		fscanf(savefp, "%s %d %f %d %d %d %d %d %d ",
			ver, &buttles, &wonper, &each[GU], &each[TYOKI], &each[PA], &same[WON], &same[LOSE], &same[SAME]);
		for(i = 0; i <= PA; i++) for(j = 0; j <= PA; j++) fscanf(savefp, "%d ", &nex[i][j]);
		for(i = 0; i < MAX_CURRENT; i++) fscanf(savefp, "%d ", &last100[i]);
		
	}else{	//エラー：コマンドが不正
		fprintf(stderr, RED "内部エラー発生、終了します\n" RESET);
		exit(0);
	}
	
	return;
}


///内容解析関数（コンピューターの手を決定）
int analyze()
{

	int comh = 0, i;	//コンピューターの手, ループ用変数
	int cper[3] = {};	//それぞれの手を出す確率
	srand(time(NULL));
	
	//解析
	
	//最近のMAX_CURRENT手
	for(i = 0; i < MAX_CURRENT; i++){
		if(last100[i] == GU) cper[GU]++;
		else if(last100[i] == TYOKI) cper[TYOKI]++;
		else if(last100[i] == PA) cper[PA]++;
		else{
			fprintf(stderr, RED "エラー：\tセーブデータが破損しています。last100[%d] = " YELLOW "%d" RED "\n終了します\n\n" RESET, i, last100[i]);
			exit(0);
		}
	}
	
	//今回のプレイでの手
	if(gus >= pas && gus >= tyos) cper[PA]++;
	else if(tyos >= gus && tyos >= pas) cper[GU]++;
	else if(pas >= gus && pas >= tyos) cper[TYOKI]++;
	
	//前までに出した手
	if(each[GU] > each[TYOKI] && each[GU] > each[PA]) cper[PA]++;
	else if(each[TYOKI] > each[GU] && each[TYOKI] > each[PA]) cper[GU]++;
	else if(each[PA] > each[GU] && each[PA] > each[TYOKI]) cper[TYOKI]++;
	
	//あいこのつぎなら・・・
	if(aikf == 1){
		if(same[SAME] > same[LOSE] && same[SAME] > same[WON]) cper[(befhand % 3 - 3) + 2]++;
		else if(same[LOSE] > same[SAME] && same[LOSE] > same[WON]) cper[befhand]++;
		else if(same[WON] > same[SAME] && same[WON] > same[LOSE]) cper[(befhand % 3 - 3) + 1]++;
	}
	
	//前の手を参考にして・・・
	if(nex[befhand][GU] > nex[befhand][TYOKI] && nex[befhand][GU] > nex[befhand][PA]) cper[PA]++;
	else if(nex[befhand][TYOKI] > nex[befhand][GU] && nex[befhand][TYOKI] > nex[befhand][PA]) cper[GU]++;
	else if(nex[befhand][PA] > nex[befhand][GU] && nex[befhand][PA] > nex[befhand][TYOKI]) cper[TYOKI]++;
	
	//最終的な手の決定
	if(cper[GU] > cper[TYOKI] && cper[GU] > cper[PA]) comh = GU;
	else if(cper[TYOKI] > cper[GU] && cper[TYOKI] > cper[PA]) comh = TYOKI;
	else if(cper[PA] > cper[GU] && cper[PA] > cper[TYOKI]) comh = PA;
	else comh = (rand() % 3);	//どのても同じくらいの割合で出せばいい時はランダムな値
	
	return comh;
}

