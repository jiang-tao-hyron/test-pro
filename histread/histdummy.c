////////////////////////////////////////////////////////////////////////////////
// FILE
// プロジェクト : 次世代DVRシステム
// 概要         : 履歴情報の入力を模擬する
//------------------------------------------------------------------------------
// Copyright(C) 2010 MegaChips Co.,Ltd. All right reserved.
// ファイル名   : histdummy.c
// 作成者       :
// CPU,OS       : ARM Linux
// 言語         : C
//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/mman.h>
#include <sqlite3.h>

#include "com_type.h"
#include "com_hist_def.h"
#include "com_debug.h"
#include "com_log.h"
#include "sys_com_def.h"

static INT headflg;

void usage(void)
{
	printf(	" Usage: histread : <KIND> \n" );
	printf(	"    <KIND>(HEX/DEC)  : 履歴種別 \n" );
	printf(	"                 : %d 異常履歴\n", D_SYS_LOG_KIND_ERR );
	printf(	"                 : %d 入出力履歴\n", D_SYS_LOG_KIND_INOUT );
	printf(	"                 : %d 操作履歴\n", D_SYS_LOG_KIND_OPE );
	printf(	"                 : %d メニュー操作履歴\n", D_SYS_LOG_KIND_MENU );
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
	if(headflg == 0)
	{
		for(i=0; i<argc; i++)
		{
			printf("%20s", azColName[i]);
		}
		printf("\n");
		headflg = 1;
	}

	for(i=0; i<argc; i++)
	{
		printf("%20s", argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// 関数名   : main
// 概要     : 履歴を扱うツールのメイン関数
//------------------------------------------------------------------------------
// 入力     : int	 argc	: 引数の数
//			  char	**argv	: 引数文字列へのポインタ
// 戻り値   :
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	CHAR *sql = NULL;
	INT loop;

	if(argc < 2)
	{
		usage();
		return 0;
	}

	for(loop=1; loop<argc; loop++)
	{
		switch( strtol(argv[loop], NULL, 0) )
		{
		case D_SYS_LOG_KIND_ERR:
			sqlite3_open(D_HIST_ERR_DB_PATH, &db);
			sql = "SELECT * FROM HIST_Error_show_View_v1;";
			printf("######################################## 異常履歴 ########################################\n");
			break;
		case D_SYS_LOG_KIND_INOUT:
			sqlite3_open(D_HIST_INOUT_DB_PATH, &db);
			sql = "SELECT * FROM HIST_InOut_show_View_v1;";
			printf("####################################### 入出力履歴 #######################################\n");
			break;
		case D_SYS_LOG_KIND_OPE:
			sqlite3_open(D_HIST_OPE_DB_PATH, &db);
			sql = "SELECT * FROM HIST_Ope_show_View_v1;";
			printf("######################################## 操作履歴 ########################################\n");
			break;
		case D_SYS_LOG_KIND_MENU:
			sqlite3_open(D_HIST_MENU_DB_PATH, &db);
			sql = "SELECT * FROM HIST_Menu_show_View_v1;";
			printf("#################################### メニュー操作履歴 ####################################\n");
			break;
		default:
			usage();
			return 0;
		}
		headflg = 0;
		sqlite3_exec(db, sql, callback, 0, &zErrMsg);
		sqlite3_close(db);
	}

	return 0;
}


////////////////////////////////////////////////////////////////////////////////
// end of file
////////////////////////////////////////////////////////////////////////////////
