/*
 * hist_db.c
 *
 *  Created on: 2017/10/30
 *      Author: mcc
 */
#include <stdio.h>
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

static CHAR *_filename = __FILE__;
//******************************************************************************
//		マクロ
//******************************************************************************
#define Detail(arg...)						DetailPrint( D_COM_LOGID_IO, ##arg )
#define Trace(arg...)						TracePrint( D_COM_LOGID_IO, ##arg )
#define Warn(arg...)						WarnPrint( D_COM_LOGID_IO, ##arg )
#define Error(arg...)						ErrorPrint( D_COM_LOGID_IO, ##arg )
#define Log(arg...)							ComPrint( D_COM_LOGID_IO, ##arg )					// for SIGUSR2
#define Maker(arg...)						MarkerPrint( D_COM_LOGID_IO, ##arg )

#define	D_HIST_TBL_END					(0xFFFF)	// テーブル終了マック
#define D_FILE_DB_PATH				"/mnt/hist.db"
#define D_FILE_DB_BUSY_TIMEOUT		(1000)
#define D_CREATE_CODE2NAME_TABLE	"CREATE TABLE CODE2NAME_TBL ("\
										"idx INTEGER PRIMARY KEY,"\
										"hist_name VARCHAR(32),"\
										"details1 VARCHAR(16),"\
										"details2 VARCHAR(16),"\
										"hist_type INTEGER,"\
										"anyset1 SMALLINT,"\
										"anyset2 SMALLINT);"
#define D_CODE2NAME_TBL_INSERT		"INSERT INTO CODE2NAME_TBL VALUES(%d,'%s','%s','%s',%d,%d,%d);"

// 履歴情報構成テーブル
typedef struct HIST_TYPE_INFO_t {
	CHAR		KindNm[40];						// 履歴種別名称
	CHAR		Detail1[40];					// 詳細名称1
	CHAR		Detail2[40];					// 詳細名称2
	USHORT		KindNo;							// 履歴種別
	CHAR		anyset1;						// 任意設定1(履歴種別により、セットするパラメータは異なる)
	CHAR		anyset2;						// 任意設定2(履歴種別により、セットするパラメータは異なる)
}HIST_TYPE_INFO;

// 履歴種別
static	HIST_TYPE_INFO type_info_tbl[] =
{
		//履歴種別名称					詳細名称1			詳細名称2			履歴種別									書込みテスト用1				書込みテスト用2

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------//
		// 	 異常履歴 																											                                     //
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------//
		//履歴種別名称					詳細名称1			詳細名称2			履歴種別									任意1							未使用

		{"機器異常(発生)",				"発生",					"",				D_COM_HIST_MACHINE_ALM_MACHINE_ON,			D_COM_HIST_ERR_ON,				0	},
		{"機器異常(復旧)",				"復旧",					"",				D_COM_HIST_MACHINE_ALM_MACHINE_OFF,			D_COM_HIST_ERR_OFF,				0	},
		{"通信異常(発生)",				"発生",					"",				D_COM_HIST_MACHINE_ALM_COMMU_ON,			D_COM_HIST_ERR_ON,				0	},
		{"通信異常(復旧)",				"復旧",					"",				D_COM_HIST_MACHINE_ALM_COMMU_OFF,			D_COM_HIST_ERR_OFF,				0	},
		{"外部機器異常(発生)",			"発生",					"",				D_COM_HIST_MACHINE_ALM_EXT_ON,				D_COM_HIST_ERR_ON,				0	},
		{"外部機器異常(復旧)",			"復旧",					"",				D_COM_HIST_MACHINE_ALM_EXT_OFF,				D_COM_HIST_ERR_OFF,				0	},
		{"接点入力１断線(発生)",		"発生",					"",				D_COM_HIST_MACHINE_ALM_EQUIPINPUT1_ON,		D_COM_HIST_ERR_ON,				0	},
		{"接点入力１断線(復旧)",		"復旧",					"",				D_COM_HIST_MACHINE_ALM_EQUIPINPUT1_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"接点入力２断線(発生)",		"発生",					"",				D_COM_HIST_MACHINE_ALM_EQUIPINPUT2_ON,		D_COM_HIST_ERR_ON,				0	},
		{"接点入力２断線(復旧)",		"復旧",					"",				D_COM_HIST_MACHINE_ALM_EQUIPINPUT2_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"停電(発生)",					"発生",					"",				D_COM_HIST_MACHINE_ALM_BLACKOUT_ON,			D_COM_HIST_ERR_ON,				0	},
		{"停電(復旧)",					"復旧",					"",				D_COM_HIST_MACHINE_ALM_BLACKOUT_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"温度超過(発生)",				"発生",					"",				D_COM_HIST_MACHINE_ALM_TEMPOVER_ON,			D_COM_HIST_ERR_ON,				0	},
		{"温度超過(復旧)",				"復旧",					"",				D_COM_HIST_MACHINE_ALM_TEMPOVER_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"FAN故障(発生)",				"発生",					"",				D_COM_HIST_MACHINE_ALM_FANFAIL_ON,			D_COM_HIST_ERR_ON,				0	},
		{"FAN故障(復旧)",				"復旧",					"",				D_COM_HIST_MACHINE_ALM_FANFAIL_OFF,			D_COM_HIST_ERR_OFF,				0	},
		{"蓋開異常(発生)",				"発生",					"",				D_COM_HIST_MACHINE_ALM_COVEROPEN_ON,		D_COM_HIST_ERR_ON,				0	},
		{"蓋開異常(復旧)",				"復旧",					"",				D_COM_HIST_MACHINE_ALM_COVEROPEN_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"デイナイト異常(発生)",		"発生",					"",				D_COM_HIST_MACHINE_ALM_DAYNIGHT_ON,			D_COM_HIST_ERR_ON,				0	},
		{"デイナイト異常(復旧)",		"復旧",					"",				D_COM_HIST_MACHINE_ALM_DAYNIGHT_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"NVR代表異常(発生)",			"発生",					"",				D_COM_HIST_MACHINE_ALM_DVRFAIL_ON,			D_COM_HIST_ERR_ON,				0	},
		{"NVR代表異常(復旧)",			"復旧",					"",				D_COM_HIST_MACHINE_ALM_DVRFAIL_OFF,			D_COM_HIST_ERR_OFF,				0	},

		{"カメラマスク(発生)",			"発生",					"",				D_COM_HIST_CAMERA_ALM_CAMERA_MASK_ON,		D_COM_HIST_ERR_ON,				0	},
		{"カメラマスク(復旧)",			"復旧",					"",				D_COM_HIST_CAMERA_ALM_CAMERA_MASK_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"照度不足(発生)",				"発生",					"",				D_COM_HIST_CAMERA_ALM_LACK_ILLUM_ON,		D_COM_HIST_ERR_ON,				0	},
		{"照度不足(復旧)",				"復旧",					"",				D_COM_HIST_CAMERA_ALM_LACK_ILLUM_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"向き変更・DOME開(発生)",		"発生",					"",				D_COM_HIST_CAMERA_ALM_CHG_DIRECT_ON,		D_COM_HIST_ERR_ON,				0	},
		{"向き変更・DOME開(復旧)",		"復旧",					"",				D_COM_HIST_CAMERA_ALM_CHG_DIRECT_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"ビデオロスト(発生)",			"発生",					"",				D_COM_HIST_CAMERA_ALM_VIDEO_LOST_ON,		D_COM_HIST_ERR_ON,				0	},
		{"ビデオロスト(復旧)",			"復旧",					"",				D_COM_HIST_CAMERA_ALM_VIDEO_LOST_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"パイロットロスト(発生)",		"発生",					"",				D_COM_HIST_CAMERA_ALM_PILOT_LOST_ON,		D_COM_HIST_ERR_ON,				0	},
		{"パイロットロスト(復旧)",		"復旧",					"",				D_COM_HIST_CAMERA_ALM_PILOT_LOST_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"カメラロスト(発生)",			"発生",					"",				D_COM_HIST_CAMERA_ALM_CAMERA_LOST_ON,		D_COM_HIST_ERR_ON,				0	},
		{"カメラロスト(復旧)",			"復旧",					"",				D_COM_HIST_CAMERA_ALM_CAMERA_LOST_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"CMC・CMS異常(発生)",			"発生",					"",				D_COM_HIST_CAMERA_ALM_CMCCMS_FAIL_ON,		D_COM_HIST_ERR_ON,				0	},
		{"CMC・CMS異常(復旧)",			"復旧",					"",				D_COM_HIST_CAMERA_ALM_CMCCMS_FAIL_OFF,		D_COM_HIST_ERR_OFF,				0	},

		{"未記録時間超過(発生)",		"発生",					"",				D_COM_HIST_REC_ALM_SET_TIMEOVER_ON,			D_COM_HIST_ERR_ON,				0	},
		{"未記録時間超過(復旧)",		"復旧",					"",				D_COM_HIST_REC_ALM_SET_TIMEOVER_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"MDT未設定(発生)",				"発生",					"",				D_COM_HIST_REC_ALM_SET_MDT_NO_ON,			D_COM_HIST_ERR_ON,				0	},
		{"MDT未設定(復旧)",				"復旧",					"",				D_COM_HIST_REC_ALM_SET_MDT_NO_OFF,			D_COM_HIST_ERR_OFF,				0	},
		{"HDD異常(発生)",				"発生",					"",				D_COM_HIST_REC_ALM_HDD_FAIL_ON,				D_COM_HIST_ERR_ON,				0	},
		{"HDD異常(復旧)",				"復旧",					"",				D_COM_HIST_REC_ALM_HDD_FAIL_OFF,			D_COM_HIST_ERR_OFF,				0	},
		{"HDD未実装(発生)",				"発生",					"",				D_COM_HIST_REC_ALM_HDD_NON_ON,				D_COM_HIST_ERR_ON,				0	},
		{"HDD未実装(復旧)",				"復旧",					"",				D_COM_HIST_REC_ALM_HDD_NON_OFF,				D_COM_HIST_ERR_OFF,				0	},
		{"HDD稼動時間超過(発生)",		"発生",					"",				D_COM_HIST_REC_ALM_HDD_RUNOVER_ON,			D_COM_HIST_ERR_ON,				0	},
		{"HDD稼動時間超過(復旧)",		"復旧",					"",				D_COM_HIST_REC_ALM_HDD_RUNOVER_OFF,			D_COM_HIST_ERR_OFF,				0	},
		{"HDD読込不良(発生)",			"発生",					"",				D_COM_HIST_REC_ALM_HDD_READFAIL_ON,			D_COM_HIST_ERR_ON,				0	},
		{"HDD読込不良(復旧)",			"復旧",					"",				D_COM_HIST_REC_ALM_HDD_READFAIL_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"HDD交換時間超過(発生)",		"発生",					"",				D_COM_HIST_REC_ALM_HDD_CHGOVER_ON,			D_COM_HIST_ERR_ON,				0	},
		{"HDD交換時間超過(復旧)",		"復旧",					"",				D_COM_HIST_REC_ALM_HDD_CHGOVER_OFF,			D_COM_HIST_ERR_OFF,				0	},
		{"冗長化構成異常(発生)",		"発生",					"",				D_COM_HIST_REC_ALM_HDD_RAIDFAIL_ON,			D_COM_HIST_ERR_ON,				0	},
		{"冗長化構成異常(復旧)",		"復旧",					"",				D_COM_HIST_REC_ALM_HDD_RAIDFAIL_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"ミラー不一致(発生)",			"発生",					"",				D_COM_HIST_REC_ALM_HDD_MIRRORFAIL_ON,		D_COM_HIST_ERR_ON,				0	},
		{"ミラー不一致(復旧)",			"復旧",					"",				D_COM_HIST_REC_ALM_HDD_MIRRORFAIL_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"HDDフル(発生)",				"発生",					"",				D_COM_HIST_REC_ALM_HDD_FULL_ON,				D_COM_HIST_ERR_ON,				0	},
		{"HDDフル(復旧)",				"復旧",					"",				D_COM_HIST_REC_ALM_HDD_FULL_OFF,			D_COM_HIST_ERR_OFF,				0	},
		{"HDDニアフル(発生)",			"発生",					"",				D_COM_HIST_REC_ALM_HDD_NEARFULL_ON,			D_COM_HIST_ERR_ON,				0	},
		{"HDDニアフル(復旧)",			"復旧",					"",				D_COM_HIST_REC_ALM_HDD_NEARFULL_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"HDD異常1(発生)",				"発生",					"",				D_COM_HIST_REC_ALM_HDD1_FAIL_ON,			D_COM_HIST_ERR_ON,				0	},
		{"HDD異常1(復旧)",				"復旧",					"",				D_COM_HIST_REC_ALM_HDD1_FAIL_OFF,			D_COM_HIST_ERR_OFF,				0	},
		{"HDD異常2(発生)",				"発生",					"",				D_COM_HIST_REC_ALM_HDD2_FAIL_ON,			D_COM_HIST_ERR_ON,				0	},
		{"HDD異常2(復旧)",				"復旧",					"",				D_COM_HIST_REC_ALM_HDD2_FAIL_OFF,			D_COM_HIST_ERR_OFF,				0	},
		{"HDD異常3(発生)",				"発生",					"",				D_COM_HIST_REC_ALM_HDD3_FAIL_ON,			D_COM_HIST_ERR_ON,				0	},
		{"HDD異常3(復旧)",				"復旧",					"",				D_COM_HIST_REC_ALM_HDD3_FAIL_OFF,			D_COM_HIST_ERR_OFF,				0	},
		{"HDD異常4(発生)",				"発生",					"",				D_COM_HIST_REC_ALM_HDD4_FAIL_ON,			D_COM_HIST_ERR_ON,				0	},
		{"HDD異常4(復旧)",				"復旧",					"",				D_COM_HIST_REC_ALM_HDD4_FAIL_OFF,			D_COM_HIST_ERR_OFF,				0	},
		{"冗長化構成1異常(発生)",		"発生",					"",				D_COM_HIST_REC_ALM_HDD1_RAIDFAIL_ON,		D_COM_HIST_ERR_ON,				0	},
		{"冗長化構成1異常(復旧)",		"復旧",					"",				D_COM_HIST_REC_ALM_HDD1_RAIDFAIL_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"冗長化構成2異常(発生)",		"発生",					"",				D_COM_HIST_REC_ALM_HDD2_RAIDFAIL_ON,		D_COM_HIST_ERR_ON,				0	},
		{"冗長化構成2異常(復旧)",		"復旧",					"",				D_COM_HIST_REC_ALM_HDD2_RAIDFAIL_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"冗長化構成3異常(発生)",		"発生",					"",				D_COM_HIST_REC_ALM_HDD3_RAIDFAIL_ON,		D_COM_HIST_ERR_ON,				0	},
		{"冗長化構成3異常(復旧)",		"復旧",					"",				D_COM_HIST_REC_ALM_HDD3_RAIDFAIL_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"冗長化構成4異常(発生)",		"発生",					"",				D_COM_HIST_REC_ALM_HDD4_RAIDFAIL_ON,		D_COM_HIST_ERR_ON,				0	},
		{"冗長化構成4異常(復旧)",		"復旧",					"",				D_COM_HIST_REC_ALM_HDD4_RAIDFAIL_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"HDD1運用不一致(発生)",		"発生",					"",				D_COM_HIST_REC_ALM_HDD1_MIRRORFAIL_ON,		D_COM_HIST_ERR_ON,				0	},
		{"HDD1運用不一致(復旧)",		"復旧",					"",				D_COM_HIST_REC_ALM_HDD1_MIRRORFAIL_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"HDD2運用不一致(発生)",		"発生",					"",				D_COM_HIST_REC_ALM_HDD2_MIRRORFAIL_ON,		D_COM_HIST_ERR_ON,				0	},
		{"HDD2運用不一致(復旧)",		"復旧",					"",				D_COM_HIST_REC_ALM_HDD2_MIRRORFAIL_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"HDD3運用不一致(発生)",		"発生",					"",				D_COM_HIST_REC_ALM_HDD3_MIRRORFAIL_ON,		D_COM_HIST_ERR_ON,				0	},
		{"HDD3運用不一致(復旧)",		"復旧",					"",				D_COM_HIST_REC_ALM_HDD3_MIRRORFAIL_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"HDD4運用不一致(発生)",		"発生",					"",				D_COM_HIST_REC_ALM_HDD4_MIRRORFAIL_ON,		D_COM_HIST_ERR_ON,				0	},
		{"HDD4運用不一致(復旧)",		"復旧",					"",				D_COM_HIST_REC_ALM_HDD4_MIRRORFAIL_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"HDD1読込不良(発生)",			"発生",					"",				D_COM_HIST_REC_ALM_HDD1_READFAIL_ON,		D_COM_HIST_ERR_ON,				0	},
		{"HDD1読込不良(復旧)",			"復旧",					"",				D_COM_HIST_REC_ALM_HDD1_READFAIL_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"HDD2読込不良(発生)",			"発生",					"",				D_COM_HIST_REC_ALM_HDD2_READFAIL_ON,		D_COM_HIST_ERR_ON,				0	},
		{"HDD2読込不良(復旧)",			"復旧",					"",				D_COM_HIST_REC_ALM_HDD2_READFAIL_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"HDD3読込不良(発生)",			"発生",					"",				D_COM_HIST_REC_ALM_HDD3_READFAIL_ON,		D_COM_HIST_ERR_ON,				0	},
		{"HDD3読込不良(復旧)",			"復旧",					"",				D_COM_HIST_REC_ALM_HDD3_READFAIL_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"HDD4読込不良(発生)",			"発生",					"",				D_COM_HIST_REC_ALM_HDD4_READFAIL_ON,		D_COM_HIST_ERR_ON,				0	},
		{"HDD4読込不良(復旧)",			"復旧",					"",				D_COM_HIST_REC_ALM_HDD4_READFAIL_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"HDD1稼動時間超過(発生)",		"発生",					"",				D_COM_HIST_REC_ALM_HDD1_RUNOVER_ON,			D_COM_HIST_ERR_ON,				0	},
		{"HDD1稼動時間超過(復旧)",		"復旧",					"",				D_COM_HIST_REC_ALM_HDD1_RUNOVER_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"HDD2稼動時間超過(発生)",		"発生",					"",				D_COM_HIST_REC_ALM_HDD2_RUNOVER_ON,			D_COM_HIST_ERR_ON,				0	},
		{"HDD2稼動時間超過(復旧)",		"復旧",					"",				D_COM_HIST_REC_ALM_HDD2_RUNOVER_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"HDD3稼動時間超過(発生)",		"発生",					"",				D_COM_HIST_REC_ALM_HDD3_RUNOVER_ON,			D_COM_HIST_ERR_ON,				0	},
		{"HDD3稼動時間超過(復旧)",		"復旧",					"",				D_COM_HIST_REC_ALM_HDD3_RUNOVER_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"HDD4稼動時間超過(発生)",		"発生",					"",				D_COM_HIST_REC_ALM_HDD4_RUNOVER_ON,			D_COM_HIST_ERR_ON,				0	},
		{"HDD4稼動時間超過(復旧)",		"復旧",					"",				D_COM_HIST_REC_ALM_HDD4_RUNOVER_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"SD認識異常(発生)",			"発生",					"",				D_COM_HIST_REC_ALM_SD_RECOG_FAIL_ON,		D_COM_HIST_ERR_ON,				0	},
		{"SD認識異常(復旧)",			"復旧",					"",				D_COM_HIST_REC_ALM_SD_RECOG_FAIL_OFF,		D_COM_HIST_ERR_OFF,				0	},
		{"SD未実装(発生)",				"発生",					"",				D_COM_HIST_REC_ALM_SD_NON_ON,				D_COM_HIST_ERR_ON,				0	},
		{"SD未実装(復旧)",				"復旧",					"",				D_COM_HIST_REC_ALM_SD_NON_OFF,				D_COM_HIST_ERR_OFF,				0	},
		{"SD未転送(発生)",				"発生",					"",				D_COM_HIST_REC_ALM_SD_NOTTRANS_ON,			D_COM_HIST_ERR_ON,				0	},
		{"SD未転送(復旧)",				"復旧",					"",				D_COM_HIST_REC_ALM_SD_NOTTRANS_OFF,			D_COM_HIST_ERR_OFF,				0	},
		{"SD書込異常(発生)",			"発生",					"",				D_COM_HIST_REC_ALM_SD_WRITEFAIL_ON,			D_COM_HIST_ERR_ON,				0	},
		{"SD書込異常(復旧)",			"復旧",					"",				D_COM_HIST_REC_ALM_SD_WRITEFAIL_OFF,		D_COM_HIST_ERR_OFF,				0	},

		// ---------------------------------------------------------------------------------------------------------------------//
		// 	 入出力履歴 																										//
		// ---------------------------------------------------------------------------------------------------------------------//
		//履歴種別名称				詳細名称1				詳細名称2			履歴種別									端子							状態

		{"接点入力1",				"端子1",				"非検知",			D_COM_HIST_INPUT_SYS_TERM_IN1,				D_COM_HIST_TERMINAL_COM1,		D_COM_HIST_CONTACT_STATE_BREAK	},	// 端子1,非検知
		{"接点入力1",				"端子1",				"検知",				D_COM_HIST_INPUT_SYS_TERM_IN1,				D_COM_HIST_TERMINAL_COM1,		D_COM_HIST_CONTACT_STATE_MAKE	},	// 端子1,検知
		{"接点入力2",				"端子2",				"非検知",			D_COM_HIST_INPUT_SYS_TERM_IN2,				D_COM_HIST_TERMINAL_COM2,		D_COM_HIST_CONTACT_STATE_BREAK	},	// 端子2,非検知
		{"接点入力2",				"端子2",				"検知",				D_COM_HIST_INPUT_SYS_TERM_IN2,				D_COM_HIST_TERMINAL_COM2,		D_COM_HIST_CONTACT_STATE_MAKE	},	// 端子2,検知
		{"接点出力1",				"端子1",				"OFF",				D_COM_HIST_OUTPUT_SYS_TERM_OUT1,			D_COM_HIST_TERMINAL_COM1,		D_COM_HIST_CONTACT_STATE_OFF	},	// 端子1,OFF
		{"接点出力1",				"端子1",				"ON",				D_COM_HIST_OUTPUT_SYS_TERM_OUT1,			D_COM_HIST_TERMINAL_COM1,		D_COM_HIST_CONTACT_STATE_ON		},	// 端子1,ON
		{"接点出力2",				"端子2",				"OFF",				D_COM_HIST_OUTPUT_SYS_TERM_OUT2,			D_COM_HIST_TERMINAL_COM2,		D_COM_HIST_CONTACT_STATE_OFF	},	// 端子2,OFF
		{"接点出力2",				"端子2",				"ON",				D_COM_HIST_OUTPUT_SYS_TERM_OUT2,			D_COM_HIST_TERMINAL_COM2,		D_COM_HIST_CONTACT_STATE_ON		},	// 端子2,ON

		// ---------------------------------------------------------------------------------------------------------------------//
		// 	 操作履歴 																											//
		// ---------------------------------------------------------------------------------------------------------------------//
		//履歴種別名称						詳細名称1			詳細名称2		履歴種別								任意1							未使用

		{"ロック",							"本体ボタン",		"",				D_COM_HIST_OWN_SYS_LOCK,				D_COM_HIST_LOCK_OWN,			0	},
		{"ロック",							"リモコン",			"",				D_COM_HIST_OWN_SYS_LOCK,				D_COM_HIST_LOCK_RMTCTRL,		0	},
		{"ロック",							"SET",				"",				D_COM_HIST_OWN_SYS_LOCK,				D_COM_HIST_LOCK_SET,			0	},
		{"ロック",							"IXログイン",		"",				D_COM_HIST_OWN_SYS_LOCK,				D_COM_HIST_LOCK_IXLOGIN,		0	},
		{"ロック",							"IXログアウト",		"",				D_COM_HIST_OWN_SYS_LOCK,				D_COM_HIST_LOCK_IXLOGOUT,		0	},
		{"ロック",							"リモート",			"",				D_COM_HIST_OWN_SYS_LOCK,				D_COM_HIST_LOCK_RMT_OP,			0	},
		{"ロック解除",						"本体ボタン",		"",				D_COM_HIST_OWN_SYS_UNLOCK,				D_COM_HIST_LOCK_OWN,			0	},
		{"ロック解除",						"リモコン",			"",				D_COM_HIST_OWN_SYS_UNLOCK,				D_COM_HIST_LOCK_RMTCTRL,		0	},
		{"ロック解除",						"SET",				"",				D_COM_HIST_OWN_SYS_UNLOCK,				D_COM_HIST_LOCK_SET,			0	},
		{"ロック解除",						"IXログイン",		"",				D_COM_HIST_OWN_SYS_UNLOCK,				D_COM_HIST_LOCK_IXLOGIN,		0	},
		{"ロック解除",						"IXログアウト",		"",				D_COM_HIST_OWN_SYS_UNLOCK,				D_COM_HIST_LOCK_IXLOGOUT,		0	},
		{"ロック解除",						"リモート",			"",				D_COM_HIST_OWN_SYS_UNLOCK,				D_COM_HIST_LOCK_RMT_OP,			0	},
		{"ログイン",						"",					"",				D_COM_HIST_OWN_SYS_LOGIN,				0,								0	},
		{"ログアウト",						"",					"",				D_COM_HIST_OWN_SYS_LOGOUT,				0,								0	},
		{"記録切替",						"有効",				"",				D_COM_HIST_OWN_SYS_RECCHG,				D_COM_HIST_OPDTAIL_RECVALID,	0	},
		{"記録切替",						"無効",				"",				D_COM_HIST_OWN_SYS_RECCHG,				D_COM_HIST_OPDTAIL_RECINVALID,	0	},
		{"ブックマーク",					"ライブ時",			"",				D_COM_HIST_OWN_SYS_BOOKMARK,			D_COM_HIST_BOOKMARK_LIVE,		0	},
		{"ブックマーク",					"再生時",			"",				D_COM_HIST_OWN_SYS_BOOKMARK,			D_COM_HIST_BOOKMARK_PLAY,		0	},
		{"リモートイベント",				"",					"",				D_COM_HIST_EXTERNAL_SYS_RMTEVT1,		0,								0	},
		{"アクセス",						"",					"",				D_COM_HIST_OWN_SYS_ACCESS,				0,								0	},

		// ---------------------------------------------------------------------------------------------------------------------//
		// 	 メニュー操作履歴																									//
		// ---------------------------------------------------------------------------------------------------------------------//
		//  TypeA
		//履歴種別名称						詳細名称1		詳細名称2			履歴種別							任意1							任意2

		{"画像コピー",						"開始",			"DVD-R(Video)",		D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_START,	D_COM_HIST_IMGCPY_MEDIA_DVD_R_V	},
		{"画像コピー",						"開始",			"DVD-R",			D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_START,	D_COM_HIST_IMGCPY_MEDIA_DVD_R	},
		{"画像コピー",						"開始",			"DVR-RAM",			D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_START,	D_COM_HIST_IMGCPY_MEDIA_DVD_RAM	},
		{"画像コピー",						"開始",			"BD-R",				D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_START,	D_COM_HIST_IMGCPY_MEDIA_BD_R	},
		{"画像コピー",						"開始",			"SD",				D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_START,	D_COM_HIST_IMGCPY_MEDIA_SD		},
		{"画像コピー",						"開始",			"USB",				D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_START,	D_COM_HIST_IMGCPY_MEDIA_USB		},
		{"画像コピー",						"継続",			"DVD-R(Video)",		D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_CONTINUE,	D_COM_HIST_IMGCPY_MEDIA_DVD_R_V	},
		{"画像コピー",						"継続",			"DVD-R",			D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_CONTINUE,	D_COM_HIST_IMGCPY_MEDIA_DVD_R	},
		{"画像コピー",						"継続",			"DVR-RAM",			D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_CONTINUE,	D_COM_HIST_IMGCPY_MEDIA_DVD_RAM	},
		{"画像コピー",						"継続",			"BD-R",				D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_CONTINUE,	D_COM_HIST_IMGCPY_MEDIA_BD_R	},
		{"画像コピー",						"継続",			"SD",				D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_CONTINUE,	D_COM_HIST_IMGCPY_MEDIA_SD		},
		{"画像コピー",						"継続",			"USB",				D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_CONTINUE,	D_COM_HIST_IMGCPY_MEDIA_USB		},
		{"画像コピー",						"終了",			"DVD-R(Video)",		D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_END,		D_COM_HIST_IMGCPY_MEDIA_DVD_R_V	},
		{"画像コピー",						"終了",			"DVD-R",			D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_END,		D_COM_HIST_IMGCPY_MEDIA_DVD_R	},
		{"画像コピー",						"終了",			"DVR-RAM",			D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_END,		D_COM_HIST_IMGCPY_MEDIA_DVD_RAM	},
		{"画像コピー",						"終了",			"BD-R",				D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_END,		D_COM_HIST_IMGCPY_MEDIA_BD_R	},
		{"画像コピー",						"終了",			"SD",				D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_END,		D_COM_HIST_IMGCPY_MEDIA_SD		},
		{"画像コピー",						"終了",			"USB",				D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_END,		D_COM_HIST_IMGCPY_MEDIA_USB		},
		{"画像コピー",						"中断",			"DVD-R(Video)",		D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_PAUSE,	D_COM_HIST_IMGCPY_MEDIA_DVD_R_V	},
		{"画像コピー",						"中断",			"DVD-R",			D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_PAUSE,	D_COM_HIST_IMGCPY_MEDIA_DVD_R	},
		{"画像コピー",						"中断",			"DVR-RAM",			D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_PAUSE,	D_COM_HIST_IMGCPY_MEDIA_DVD_RAM	},
		{"画像コピー",						"中断",			"BD-R",				D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_PAUSE,	D_COM_HIST_IMGCPY_MEDIA_BD_R	},
		{"画像コピー",						"中断",			"SD",				D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_PAUSE,	D_COM_HIST_IMGCPY_MEDIA_SD		},
		{"画像コピー",						"中断",			"USB",				D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_PAUSE,	D_COM_HIST_IMGCPY_MEDIA_USB		},
		{"画像コピー",						"失敗",			"DVD-R(Video)",		D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_ERR,		D_COM_HIST_IMGCPY_MEDIA_DVD_R_V	},
		{"画像コピー",						"失敗",			"DVD-R",			D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_ERR,		D_COM_HIST_IMGCPY_MEDIA_DVD_R	},
		{"画像コピー",						"失敗",			"DVR-RAM",			D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_ERR,		D_COM_HIST_IMGCPY_MEDIA_DVD_RAM	},
		{"画像コピー",						"失敗",			"BD-R",				D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_ERR,		D_COM_HIST_IMGCPY_MEDIA_BD_R	},
		{"画像コピー",						"失敗",			"SD",				D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_ERR,		D_COM_HIST_IMGCPY_MEDIA_SD		},
		{"画像コピー",						"失敗",			"USB",				D_COM_HIST_MEDIACOPY_IMGCPY,		D_COM_HIST_IMGCPY_OPE_ERR,		D_COM_HIST_IMGCPY_MEDIA_USB		},
		{"ダイレクトコピー",				"開始",			"SD",				D_COM_HIST_MEDIACOPY_RTCPY,			D_COM_HIST_RTCPY_OPE_START,		D_COM_HIST_RTCPY_MEDIA_SD		},
		{"ダイレクトコピー",				"開始",			"USB",				D_COM_HIST_MEDIACOPY_RTCPY,			D_COM_HIST_RTCPY_OPE_START,     D_COM_HIST_RTCPY_MEDIA_USB		},
		{"ダイレクトコピー",				"終了",			"SD",				D_COM_HIST_MEDIACOPY_RTCPY,			D_COM_HIST_RTCPY_OPE_END,		D_COM_HIST_RTCPY_MEDIA_SD		},
		{"ダイレクトコピー",				"終了",			"USB",				D_COM_HIST_MEDIACOPY_RTCPY,			D_COM_HIST_RTCPY_OPE_END,       D_COM_HIST_RTCPY_MEDIA_USB		},
		{"ダイレクトコピー",				"失敗",			"SD",				D_COM_HIST_MEDIACOPY_RTCPY,			D_COM_HIST_RTCPY_OPE_ERR,		D_COM_HIST_RTCPY_MEDIA_SD		},
		{"ダイレクトコピー",				"失敗",			"USB",				D_COM_HIST_MEDIACOPY_RTCPY,			D_COM_HIST_RTCPY_OPE_ERR,       D_COM_HIST_RTCPY_MEDIA_USB		},
		{"画像ダウンロード",				"開始",			"",					D_COM_HIST_MEDIACOPY_IMGDOWNLOAD,	D_COM_HIST_IMGDL_OPE_START,		0								},
		{"画像ダウンロード",				"継続",			"",					D_COM_HIST_MEDIACOPY_IMGDOWNLOAD,	D_COM_HIST_IMGDL_OPE_CONTINUE,	0								},
		{"画像ダウンロード",				"終了",			"",					D_COM_HIST_MEDIACOPY_IMGDOWNLOAD,	D_COM_HIST_IMGDL_OPE_END,		0								},
		{"画像ダウンロード",				"中断",			"",					D_COM_HIST_MEDIACOPY_IMGDOWNLOAD,	D_COM_HIST_IMGDL_OPE_PAUSE,		0								},
		{"画像ダウンロード",				"失敗",			"",					D_COM_HIST_MEDIACOPY_IMGDOWNLOAD,	D_COM_HIST_IMGDL_OPE_ERR,		0								},
		{"予約コピー",						"開始",			"SD",				D_COM_HIST_MEDIACOPY_SCHEDULECPY,	D_COM_HIST_SCHECPY_OPE_START,	D_COM_HIST_SCHECPY_MEDIA_SD		},
		{"予約コピー",						"開始",			"USB",				D_COM_HIST_MEDIACOPY_SCHEDULECPY,	D_COM_HIST_SCHECPY_OPE_START,	D_COM_HIST_SCHECPY_MEDIA_USB	},
		{"予約コピー",						"継続",			"SD",				D_COM_HIST_MEDIACOPY_SCHEDULECPY,	D_COM_HIST_SCHECPY_OPE_CONTINUE,D_COM_HIST_SCHECPY_MEDIA_SD		},
		{"予約コピー",						"継続",			"USB",				D_COM_HIST_MEDIACOPY_SCHEDULECPY,	D_COM_HIST_SCHECPY_OPE_CONTINUE,D_COM_HIST_SCHECPY_MEDIA_USB	},
		{"予約コピー",						"終了",			"SD",				D_COM_HIST_MEDIACOPY_SCHEDULECPY,	D_COM_HIST_SCHECPY_OPE_END,		D_COM_HIST_SCHECPY_MEDIA_SD		},
		{"予約コピー",						"終了",			"USB",				D_COM_HIST_MEDIACOPY_SCHEDULECPY,	D_COM_HIST_SCHECPY_OPE_END,		D_COM_HIST_SCHECPY_MEDIA_USB	},
		{"予約コピー",						"中断",			"SD",				D_COM_HIST_MEDIACOPY_SCHEDULECPY,	D_COM_HIST_SCHECPY_OPE_PAUSE,	D_COM_HIST_SCHECPY_MEDIA_SD		},
		{"予約コピー",						"中断",			"USB",				D_COM_HIST_MEDIACOPY_SCHEDULECPY,	D_COM_HIST_SCHECPY_OPE_PAUSE,	D_COM_HIST_SCHECPY_MEDIA_USB	},
		{"予約コピー",						"失敗",			"SD",				D_COM_HIST_MEDIACOPY_SCHEDULECPY,	D_COM_HIST_SCHECPY_OPE_ERR,		D_COM_HIST_SCHECPY_MEDIA_SD		},
		{"予約コピー",						"失敗",			"USB",				D_COM_HIST_MEDIACOPY_SCHEDULECPY,	D_COM_HIST_SCHECPY_OPE_ERR,		D_COM_HIST_SCHECPY_MEDIA_USB	},
		{"機器構成設定",					"登録",			"",					D_COM_HIST_MACHINE_SET_RGSTCODE1,	D_COM_HIST_MACSET_OPE_REG,		0								},
		{"機器構成設定",					"削除",			"",					D_COM_HIST_MACHINE_SET_RGSTCODE1,	D_COM_HIST_MACSET_OPE_DEL,		0								},
		{"機器構成設定",					"交換",			"",					D_COM_HIST_MACHINE_SET_RGSTCODE1,	D_COM_HIST_MACSET_OPE_CHG,		0								},
		{"ネットワーク設定",				"ネットワーク設定",		"",			D_COM_HIST_MACHINE_SET_NETWORK,		D_COM_HIST_NETSET_OPE_NET,		0								},
		{"ネットワーク設定",				"メール設定",			"",			D_COM_HIST_MACHINE_SET_NETWORK,		D_COM_HIST_NETSET_OPE_MAIL,		0								},
		{"時刻設定(変更前)",				"設定",			"",					D_COM_HIST_MACHINE_SET_TIMESET_BEF,	D_COM_HIST_TIMESET_OPE_SET,		0								},
		{"時刻設定(変更前)",				"登録",			"",					D_COM_HIST_MACHINE_SET_TIMESET_BEF,	D_COM_HIST_TIMESET_OPE_REG,		0								},
		{"時刻設定(変更後)",				"設定",			"",					D_COM_HIST_MACHINE_SET_TIMESET_AFT,	D_COM_HIST_TIMESET_OPE_SET,		0								},
		{"時刻設定(変更後)",				"登録",			"",					D_COM_HIST_MACHINE_SET_TIMESET_AFT,	D_COM_HIST_TIMESET_OPE_REG,		0								},
		{"認証設定",						"登録",			"ユーザーNo.2",		D_COM_HIST_MACHINE_SET_AUTH1,		D_COM_HIST_AUTHSET_OPE_REG,		0x02							},
		{"認証設定",						"削除",			"ユーザーNo.2",		D_COM_HIST_MACHINE_SET_AUTH1,		D_COM_HIST_AUTHSET_OPE_DEL,		0x02							},
		{"認証設定",						"変更",			"ユーザーNo.2",		D_COM_HIST_MACHINE_SET_AUTH1,		D_COM_HIST_AUTHSET_OPE_CHG,		0x02							},
		{"認証設定",						"認証設定",		"ユーザーNo.2",		D_COM_HIST_MACHINE_SET_AUTH1,		D_COM_HIST_AUTHSET_OPE_SET,		0x02							},
		{"HDD運用設定",						"運用変更",		"",					D_COM_HIST_MACHINE_SET_HDDUSE1,		D_COM_HIST_HDDUSE_OPE_OPECHG,	0								},
		{"HDD運用設定",						"書込変更",		"",					D_COM_HIST_MACHINE_SET_HDDUSE1,		D_COM_HIST_HDDUSE_OPE_WRITECHG,	0								},
		{"HDD運用設定",						"SD転送設定",	"",					D_COM_HIST_MACHINE_SET_HDDUSE1,		D_COM_HIST_HDDUSE_OPE_SDTRANS,	0								},
		{"名称設定",						"",				"",					D_COM_HIST_CAMERA_SET_GROUPNAME,	0						,		0								},
		{"レイアウト設定",					"レイアウト",	"",					D_COM_HIST_MACHINE_SET_LAYOUT,		D_COM_HIST_LAYOUT_OPE_LAYOUT,	0								},
		{"レイアウト設定",					"ログアウト",	"",					D_COM_HIST_MACHINE_SET_LAYOUT,		D_COM_HIST_LAYOUT_OPE_LOGOUT,	0								},
		{"モニタ設定",						"モニター",		"",					D_COM_HIST_MACHINE_SET_MONITOR,		D_COM_HIST_MONITOR_OPE_MONITOR,	0								},
		{"モニタ設定",						"表示",			"",					D_COM_HIST_MACHINE_SET_MONITOR,		D_COM_HIST_MONITOR_OPE_CHGDISP,	0								},
		{"モニタ設定",						"エコ",			"",					D_COM_HIST_MACHINE_SET_MONITOR,		D_COM_HIST_MONITOR_OPE_ECO,		0								},
		{"モニタ設定",						"電子ズーム",	"",					D_COM_HIST_MACHINE_SET_MONITOR,		D_COM_HIST_MONITOR_OPE_ZOOM,	0								},
		{"モニタ設定",						"PTZ",			"",					D_COM_HIST_MACHINE_SET_MONITOR,		D_COM_HIST_MONITOR_OPE_PTZ,		0								},
		{"モニタ設定",						"ライブ音声",	"",					D_COM_HIST_MACHINE_SET_MONITOR,		D_COM_HIST_MONITOR_OPE_SEQ,		0								},
		{"モニタ設定",						"ブザー",		"",					D_COM_HIST_MACHINE_SET_MONITOR,		D_COM_HIST_MONITOR_OPE_BUZZ,	0								},
		{"モニタ設定",						"リモコン",		"",					D_COM_HIST_MACHINE_SET_MONITOR,		D_COM_HIST_MONITOR_OPE_REMOCON,	0								},
		{"表示設定",						"",				"",					D_COM_HIST_MACHINE_SET_DISP,		0,								0								},
		{"エコモード設定",					"",				"",					D_COM_HIST_MACHINE_SET_ECOMODE,		0,								0								},
		{"入出力設定",						"接点入力",		"",					D_COM_HIST_MAINT_SET_INOUT,			D_COM_HIST_INOUTKIND_IN,		0								},
		{"入出力設定",						"接点出力",		"",					D_COM_HIST_MAINT_SET_INOUT,			D_COM_HIST_INOUTKIND_OUT,		0								},
		{"簡単記録設定",					"",				"",					D_COM_HIST_REC_SET_SIMPLE,			0,								0								},
		{"詳細記録設定",					"",				"",					D_COM_HIST_REC_SET_DETAIL,			0,								0								},
		{"特定日記録設定",					"",				"",					D_COM_HIST_REC_SET_INDFDAY,			0,								0								},
		{"履歴保存",						"SD",			"",					D_COM_HIST_MAINT_SET_HISTSAVE,		D_COM_HIST_HISTSAVE_MEDIA_SD,	0								},
		{"履歴保存",						"USB",			"",					D_COM_HIST_MAINT_SET_HISTSAVE,		D_COM_HIST_HISTSAVE_MEDIA_USB,	0								},
		{"履歴保存",						"WEB",			"",					D_COM_HIST_MAINT_SET_HISTSAVE,		D_COM_HIST_HISTSAVE_MEDIA_WEB,	0								},
		{"HDD保守",							"HDD",			"追加",				D_COM_HIST_MAINT_SET_HDD,			D_COM_HIST_HDD_OPE_HDD,			D_COM_HIST_HDD_STAT_ADD			},
		{"HDD保守",							"HDD",			"交換",				D_COM_HIST_MAINT_SET_HDD,			D_COM_HIST_HDD_OPE_HDD,			D_COM_HIST_HDD_STAT_EXCH		},
		{"HDD保守",							"書込順・位置変",		"",			D_COM_HIST_MAINT_SET_HDD,			D_COM_HIST_HDD_OPE_WRITECHG,	0								},
		{"HDD保守",							"拡張記録装置削除",		"",			D_COM_HIST_MAINT_SET_HDD,			D_COM_HIST_HDD_OPE_PLAY,		0								},
		{"記録・再生管理",					"",				"",					D_COM_HIST_MAINT_SET_REC_MNG,		0,								0								},
		{"帯域制限",						"",				"",					D_COM_HIST_MAINT_SET_BAND,			0,								0								},
		{"システム停止",					"",				"",					D_COM_HIST_MAINT_SET_SYSTEMSTOP,	0,								0								},
		{"リモート接続",					"",				"",					D_COM_HIST_MAINT_SET_SERIAL_NW,		0,								0								},
		{"NTSC出力",						"",				"",					D_COM_HIST_MAINT_SET_NTSCOUT,		0,								0								},
		{"設定保存",						"保存",			"SD",				D_COM_HIST_MAINT_SET_SETSAVE1,		D_COM_HIST_SETSAVE_OPE_SAVE,	D_COM_HIST_SETSAVE_MEDIA_SD		},
		{"設定保存",						"保存",			"USB",				D_COM_HIST_MAINT_SET_SETSAVE1,		D_COM_HIST_SETSAVE_OPE_SAVE,	D_COM_HIST_SETSAVE_MEDIA_USB	},
		{"設定保存",						"保存",			"WEB",				D_COM_HIST_MAINT_SET_SETSAVE1,		D_COM_HIST_SETSAVE_OPE_SAVE,	D_COM_HIST_SETSAVE_MEDIA_WEB	},
		{"設定保存",						"読込",			"SD",				D_COM_HIST_MAINT_SET_SETSAVE1,		D_COM_HIST_SETSAVE_OPE_LOAD,	D_COM_HIST_SETSAVE_MEDIA_SD		},
		{"設定保存",						"読込",			"USB",				D_COM_HIST_MAINT_SET_SETSAVE1,		D_COM_HIST_SETSAVE_OPE_LOAD,	D_COM_HIST_SETSAVE_MEDIA_USB	},
		{"設定保存",						"読込",			"WEB",				D_COM_HIST_MAINT_SET_SETSAVE1,		D_COM_HIST_SETSAVE_OPE_LOAD,	D_COM_HIST_SETSAVE_MEDIA_WEB	},
		{"履歴表示・保存",					"",				"",					D_COM_HIST_MAINT_SET_HISTORY,		0,								0								},
		{"出力設定",						"",				"",					D_COM_HIST_MAINT_SET_OUTPUT,		0,								0								},

		//  TypeB
		//履歴種別名称						詳細名称1		詳細名称2			履歴種別								初期化種別						未使用
		{"初期化",							"メディア",		"",					D_COM_HIST_MAINT_SET_INITIALIZE1,		D_COM_HIST_INITKIND_MEDIA,		0							},
		{"初期化",							"設定",			"",					D_COM_HIST_MAINT_SET_INITIALIZE1,		D_COM_HIST_INITKIND_SET,		0							},
		{"初期化",							"データ",		"",					D_COM_HIST_MAINT_SET_INITIALIZE1,		D_COM_HIST_INITKIND_REC,		0							},
		{"初期化",							"全て",			"",					D_COM_HIST_MAINT_SET_INITIALIZE1,		D_COM_HIST_INITKIND_ALL,		0							},

		//  TypeC
		//履歴種別名称						詳細名称1		詳細名称2			履歴種別								未使用							未使用
		{"バージョンアップ",				"",				"",					D_COM_HIST_MAINT_SET_FWUPDATE,			0,								0							},

		//  TypeD
		//履歴種別名称						詳細名称1			詳細名称2		履歴種別								任意1								未使用
		{"カメラ設定",						"カメラ名称設定",	"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_NAME,		0						},
		{"カメラ設定",						"ネットワーク設定",	"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_NETWORK,		0						},
		{"カメラ設定",						"記録設定",			"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_REC,			0						},
		{"カメラ設定",						"異常監視設定",		"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_ERRWATCH,	0						},
		{"カメラ設定",						"監視シーン設定",	"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_WATCHSENCE,	0						},
		{"カメラ設定",						"機能設定",			"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_FUNC,		0						},
		{"カメラ設定",						"WDR設定",			"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_WDR,			0						},
		{"カメラ設定",						"NR設定",			"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_NR,			0						},
		{"カメラ設定",						"フリッカー設定",	"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_FLICKER,		0						},
		{"カメラ設定",						"MDT設定",			"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_MDT,			0						},
		{"カメラ設定",						"プライバシー設定",	"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_PMASK,		0						},
		{"カメラ設定",						"デイナイト設定",	"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_DAYNIGHT,	0						},
		{"カメラ設定",						"接点入力設定",		"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_INPUT,		0						},
		{"カメラ設定",						"接点出力設定",		"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_OUTPUT,		0						},
		{"カメラ設定",						"カメラ映像設定",	"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_VEDIO,		0						},
		{"カメラ設定",						"帯域制御設定",		"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_BAND,		0						},
		{"カメラ設定",						"メール設定",		"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_MAIL,		0						},
		{"カメラ設定",						"時刻設定",			"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_TIME,		0						},
		{"カメラ設定",						"CMC・CMS接続設定",	"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_CMCCMS,		0						},
		{"カメラ設定",						"解像度設定",		"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_HIGHRESOL,	0						},
		{"カメラ設定",						"PTZカメラ設定",	"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_PTZ,			0						},
		{"カメラ設定",						"ピント調整モード",	"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_PINT,		0						},
		{"カメラ設定",						"SD転送設定",		"",				D_COM_HIST_CAMERA_SET_CAMSET,			D_COM_MENU_HIST_CAMSET_SDTRANS,		0						},

		{"SD転送",							"スケジュール開始",	"",				D_COM_HIST_MAINT_SET_SDTRANS,			D_COM_MENU_HIST_SDTRANS_SCHESTART,	0						},
		{"SD転送",							"スケジュール停止",	"",				D_COM_HIST_MAINT_SET_SDTRANS,			D_COM_MENU_HIST_SDTRANS_SCHESTOP,	0						},
		{"SD転送",							"手動開始",			"",				D_COM_HIST_MAINT_SET_SDTRANS,			D_COM_MENU_HIST_SDTRANS_MANSTART,	0						},
		{"SD転送",							"手動停止",			"",				D_COM_HIST_MAINT_SET_SDTRANS,			D_COM_MENU_HIST_SDTRANS_MANSTOP,	0						},

		// テーブル終了
		{ "",								"",				"",					D_HIST_TBL_END, 						0,								0							}
};

int insert_data(sqlite3 *db, HIST_TYPE_INFO *tbl, INT size)
{
	CHAR sql[400];
	INT ret;
	INT index;
	// transaction
	sqlite3_exec(db, "BEGIN", 0, 0, NULL);
	for(index = 0; index < size; index++)
	{
		snprintf(sql, 400, D_CODE2NAME_TBL_INSERT,
				index,tbl[index].KindNm,tbl[index].Detail1,tbl[index].Detail2,
				tbl[index].KindNo,tbl[index].anyset1,tbl[index].anyset2);
		ret = sqlite3_exec(db, sql, 0, 0, NULL);
		if( ret != SQLITE_OK )
		{
			Error("Insert CODE2NAME_TBL failed:\n");
			sqlite3_exec(db, "ROLLBACK", 0, 0, NULL);
			return 0;
		}
	}
	sqlite3_exec(db, "COMMIT", 0, 0, NULL);
	return 0;
}

int main( int argc, char **argv )
{
//	struct stat	st;
//	INT ret;
//	sqlite3		*db;
//	char			*sql;
//	// SCT.dbの存在をチェエクする
//	ret = stat(D_FILE_DB_PATH, &st);
//	if(ret == 0)
//	{
//		// すでにあれば、HDD初期化より、該当DBを削除する
//		return 0;
//	}
//
//	ret = sqlite3_open(D_FILE_DB_PATH, &db);
//	if( ret )
//	{
//		//Error("Can't open database: %s\n", sqlite3_errmsg(db));
//		sqlite3_close(db);
//		return 0;
//	}
//
//	sqlite3_busy_timeout(db,D_FILE_DB_BUSY_TIMEOUT);//1ms
//
//	sql = D_CREATE_CODE2NAME_TABLE;
//	ret = sqlite3_exec(db, sql, 0, 0, NULL);
//	if( ret != SQLITE_OK )
//	{
//		//Error("Create CODE2NAME failed: %s\n", sqlite3_errmsg(db));
//		sqlite3_close(db);
//		return 0;
//	}
//
//	insert_data(db, type_info_tbl, NELEMENT(type_info_tbl));

	printf("\t%d\n",D_COM_HIST_CAMERA_SET_CAMSET);
	printf("\t%d\n",D_COM_HIST_MAINT_SET_SDTRANS);
	//printf("\t%d\n",D_COM_HIST_MAINT_SET_INITIALIZE3);
	//printf("\t%d\n",D_COM_HIST_MAINT_SET_INITIALIZE4);

	return 0;
}








