/*
 * ConfFileChg.c for G2CAM
 * Copyright(C) 2002-2017 MegaChips Co.,Ltd All right reserved.
 * author: "Jiangtao"<jiangtao@hyron-js.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define D_CONFFILE_MAX		256
#define D_SCENENAME_LEN		32
#define D_CONFNAME_LEN		128
#define D_KEYWORD_MAX		128
#define D_KEYWORD_LEN		64
#define D_DATA_LEN			64
#define D_CONFILE_LINE		256
#define D_CONFFILE_LEN		512

static int read_csv(const char *filename);
static int read_conf(char *scene, char *filename);
static int changeLine(int line, char *key, char *data, char *filename);
static void change_ver(void);

/**  csvファイルから、情報を取得する変数  **/
char Scene[D_CONFFILE_MAX][D_SCENENAME_LEN];			// 監視シーン
char ConfName[D_CONFFILE_MAX][D_CONFNAME_LEN];			// confファイルの名前
char KeyWord[D_KEYWORD_MAX][D_KEYWORD_LEN];				// keywords
char data[D_CONFFILE_MAX][D_KEYWORD_MAX][D_DATA_LEN];	// data

/**  1つcsvファイルで修正した後、情報を保存する変数  **/
char VerChange[D_CONFFILE_MAX][D_CONFNAME_LEN];			// 修正したconfファイルの名前を保存する変数
char ChangeData[D_KEYWORD_MAX * 100];					// 全部修正した所の情報を保存する変数

char ConfFlie[D_CONFILE_LINE][D_CONFFILE_LEN];			// 1つconfファイルを保存する変数
char tmp[500];											// 文字列を組み合わせする変数

/**  変数制御構造体  **/
typedef struct ST_FILE_CONTRL_t
{
	int VerIndex;		// 修正したconfファイルの名前の総数
	int Conf_index;		// 1つcsvファイルの中に、処理中のconfファイルの位置
	int SceneIndex;		// version upの時、処理中監視シーンの位置
	int FirstOutput;	// 1つconfファイル処理する時、修正した回数
}ST_FILE_CONTRL;

ST_FILE_CONTRL st_file_ctrl;

//読み取るcsvファイル
const char *CsvFile[] = {	"MDTパラメータ一覧.csv",
							"デイナイト切替パラメータ一覧.csv",
							"マスクパラメータ一覧 (2210,2240).csv",
							"マスクパラメータ一覧 (2230).csv",
							"マスクパラメータ一覧(2220,2250).csv",
							"マスクパラメータ一覧(2260,2270).csv",
							"画像変化検索パラメータ一覧.csv",
							"近接異常検出ロジックパラメータ一覧.csv",
							"向き変更パラメータ一覧(2210,2240).csv",
							"向き変更パラメータ一覧(2220,2230,2250).csv",
							"向き変更パラメータ一覧(2260,2270) .csv",
							"照度不足パラメータ一覧 (2220,2250).csv",
							"照度不足パラメータ一覧 (2230).csv",
							"照度不足パラメータ一覧 (2260,2270).csv",
							"照度不足パラメータ一覧(2210,2240).csv"
							};

// VERSION同期化の監視シーン
const char *AllScene[] = {"scene_backlight", "scene_indoor", "scene_normal", "scene_outdoor", "scene_outdoor_asp", "scene_room"};

int main(int argc, char *argv[])
{
	int Line_Index, KeyWord_Index, rt;
	int CsvIndex = 0;
	FILE *fp = NULL;
	FILE *changefile = NULL;

	memset(&st_file_ctrl, 0, sizeof(st_file_ctrl));

	for( CsvIndex = 0; CsvIndex < sizeof(CsvFile)/sizeof(CsvFile[0]); CsvIndex++ )
	{
		memset(VerChange, 0, sizeof(VerChange) );
		st_file_ctrl.VerIndex = 0;

		printf("checking file: %s\n", CsvFile[CsvIndex]);
		rt = read_csv(CsvFile[CsvIndex]);
		if( rt == -1 )
		{
			printf("read csv faild! %s\n", CsvFile[CsvIndex]);
			continue;
		}

		st_file_ctrl.Conf_index = 0;
		while( ConfName[st_file_ctrl.Conf_index][0] )
		{
			memset(tmp, 0, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "config/%s", Scene[st_file_ctrl.Conf_index]);
			rt = read_conf(tmp, ConfName[st_file_ctrl.Conf_index]);
			if( rt == -1 )
			{
				printf("read conf faild!%s/%s\n", tmp, ConfName[st_file_ctrl.Conf_index]);
			}

			//data update
			Line_Index = 0;
			KeyWord_Index = 0;
			while( KeyWord[KeyWord_Index][0] )
			{
				for( Line_Index = 0; Line_Index < rt; Line_Index++ )
				{
					if( changeLine(Line_Index, KeyWord[KeyWord_Index], data[st_file_ctrl.Conf_index][KeyWord_Index], ConfName[st_file_ctrl.Conf_index]) == 0 )
					{
						break;
					}
				}
				if( rt <= Line_Index )
					printf("%20s   *************not find*************\n", KeyWord[KeyWord_Index]);
				KeyWord_Index++;
			}

			//build new file
			memset(tmp, 0, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "tmpfile/%s/%s", Scene[st_file_ctrl.Conf_index], ConfName[st_file_ctrl.Conf_index]);
			fp = fopen(tmp, "w+");
			if(!fp)
			{
				printf("open file faile!\n");
				return -1;
			}
			for( Line_Index = 0; Line_Index < rt; Line_Index++ )
			{
				fprintf(fp, "%s\n", ConfFlie[Line_Index]);
			}
			fclose(fp);
			fp = NULL;

			//write Change Place
			changefile = fopen("ConfFileChg_diff.csv", "a+");
			if(!changefile)
			{
				printf("open file faile!\n");
				return -1;
			}
			if(st_file_ctrl.FirstOutput)
			{
				fprintf(changefile, "%s", ChangeData);
			}
			fclose(changefile);
			changefile = NULL;

			st_file_ctrl.Conf_index++;
		}

		//VERSION update
		change_ver();
	}

	return 0;
}

/******************************************************************************/
/*! @brief		read csv file
 * @param[in]	filename	csvファイルの名前
 * @return		lines		csvファイルの総行数(lines = -1 fail to read)
 * @details
 * csvファイルのフォマット:
 * scene,paramfile,key1,key2,key3,......,keyn
 * scene1,paramfile1,data11,data12,......,data1n
 * scene2,paramfile2,data21,data22,......,data2n
 * ......
 * scenen,paramfilen,datan1,datan2,......,datann
 *
 * １つcellの中に『,』があれば、『""』で表示する
 * 例：data1,"data2,data2",data3,...
******************************************************************************/
static int read_csv(const char *filename)
{
	memset(Scene, 0, sizeof(Scene));
	memset(ConfName, 0, sizeof(ConfName));
	memset(KeyWord, 0, sizeof(KeyWord));
	memset(data, 0, sizeof(data));

	FILE *fp = NULL;
	char ch;
	int coln = 0;								// csvファイルにcellの列数
	int line = 0;								// csvファイルにcellの行数
	int Index = 0;								// cellの中にもじの位置
	int quotes = 0;								// 『"』引用符モード;0:close, 1:open
	if ((fp = fopen(filename, "r")) == NULL )
	{
		printf("fail to read!\n");
		return (-1);
	}
	while( !feof(fp) )
	{
		if(!fscanf( fp, "%c", &ch))
		{
			printf("fscanf faild!\n");
			fclose(fp);
			fp = NULL;
			return (-1);
		}

		if( ch == '\n' || ch == '\r' )
		{
			line++;
			coln = 0;
			Index = 0;
			continue;
		}

		//line:0 is keyword
		if( line == 0 )
		{
			if( ch == ',' )
			{
				coln++;
				Index = 0;
				continue;
			}
			//give up the first word and the second word
			if( coln < 2 ) continue;
			KeyWord[coln-2][Index] = ch;
			Index++;
		}
		else
		{
			//csvの中引用符を判断する
			if( quotes == 0 )
			{
				if( ch == ',' )
				{
					coln++;
					Index = 0;
					continue;
				}
				if( ch == '"' )
				{
					quotes = !quotes;
					continue;
				}
			}
			else
			{
				if( ch == '"' )
				{
					quotes = !quotes;
					continue;
				}
			}

			//the first coln is Scene
			if( coln == 0 )
			{
				Scene[line-1][Index] = ch;
				Index++;
			}
			//the second coln is ConfName
			else if( coln == 1 )
			{
				ConfName[line-1][Index] = ch;
				Index++;
			}
			//the others are data
			else
			{
				data[line-1][coln-2][Index] = ch;
				Index++;
			}
		}
	}
	fclose(fp);
	fp = NULL;
	return line;
}

/******************************************************************************/
/*! @brief		read conf file
 * @param[in]	scene		監視シーン
 * @param[in]	filename	confファイルの名前
 * @return		lines		csvファイルの総行数(lines = -1 fail to read)
 * @details
******************************************************************************/
static int read_conf(char *scene, char *filename)
{
	FILE *fp = NULL;
	char buf[256];
	int Index = 0;

	char Path[D_CONFNAME_LEN];
	memset(ChangeData, 0, sizeof(ChangeData));
	memset(Path, 0, sizeof(Path));
	snprintf(Path, D_CONFNAME_LEN, "%s/%s", scene, filename);
	
	fp = fopen(Path, "r");
	if (!fp)
	{
		printf("fail to read!!!\n");
		return (-1);
	}

	//new conf file start
	st_file_ctrl.FirstOutput=0;

	memset(ConfFlie, 0, sizeof(ConfFlie));
	while( fgets(buf, 256, fp) != NULL )
	{
		strcpy(ConfFlie[Index], buf);
		ConfFlie[Index][strlen(ConfFlie[Index])-1]=0;
		Index++;
	}
	fclose(fp);
	return Index;
}

/******************************************************************************/
/*! @brief ファイルにて該当するパラメータの値を変更
 * @param[in]	line		confファイルの行数
 * @param[in]	key			keyword
 * @param[in]	data		keywordの値
 * @param[in]	filename	confファイルの名前
 * @return		-1			not find keyword
 * 				0			succes to change
 * @details
******************************************************************************/
static int changeLine(int line, char *key, char *data, char *filename)
{
	int len = strlen(key);
	int loop, Index;
	char olddata[D_DATA_LEN];
	char newconf_line[D_CONFFILE_LEN];
	memset(olddata, 0, sizeof(olddata));
	memset(newconf_line, 0, sizeof(newconf_line));
	memset(tmp, 0, sizeof(tmp));


	//find keyword
	if( ConfFlie[line][len] != '=' )
	{
		return -1;
	}

	for( loop = 0; loop < len; loop++ )
	{
		tmp[loop] = ConfFlie[line][loop];
	}
	if( strcmp(tmp, key) )
	{
		return -1;
	}

	//get old data
	loop = 0;
	while( ConfFlie[line][len + 1 + loop] != ';' && ConfFlie[line][len + 1 + loop] != ' ' && ConfFlie[line][len + 1 + loop] != '\0' )
	{
		olddata[loop] = ConfFlie[line][len + 1 + loop];
		loop++;
	}

	//version update
	if( !strcmp(data, "ver++") )
	{
		printf("################################################################################\n");
		printf("config/%s/%s\n", AllScene[st_file_ctrl.SceneIndex], filename);
		printf("                                 keyword             OldData             NewData\n");

		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%04d", atoi(olddata) + 1);
		printf("%40s%20s%20s\n", key, olddata, tmp);
		snprintf(newconf_line, D_CONFFILE_LEN, "%s=%s%s", key, tmp, &ConfFlie[line][len + 1 + loop]);
		memset(ConfFlie[line], 0, sizeof(ConfFlie[line]));
		strcpy(ConfFlie[line], newconf_line);
		return 0;
	}

	//data update
	if( strcmp(olddata, data) )
	{
		//変更したファイルの名前を取る
		for( Index=0; Index < st_file_ctrl.VerIndex; Index++ )
		{
			if( !strcmp(VerChange[Index], filename) )
				break;
		}
		if( st_file_ctrl.VerIndex <= Index )
		{
			strcpy(VerChange[st_file_ctrl.VerIndex], filename);
			st_file_ctrl.VerIndex++;
		}

		//output table title
		if(st_file_ctrl.FirstOutput == 0)
		{
			printf("################################################################################\n");
			printf("config/%s/%s\n", Scene[st_file_ctrl.Conf_index], ConfName[st_file_ctrl.Conf_index]);
			printf("                                 keyword             OldData             NewData\n");
		}

		//write change place
		snprintf(tmp, sizeof(tmp), "%s%s%s,%s,%s\n", Scene[st_file_ctrl.Conf_index], filename, key, olddata, data);
		strcat(ChangeData, tmp);

		//show data
		printf("%40s%20s%20s\n", key, olddata, data);
		st_file_ctrl.FirstOutput++;

		//write new data
		snprintf(newconf_line, D_CONFFILE_LEN, "%s=%s%s", key, data, &ConfFlie[line][len + 1 + loop]);
		memset(ConfFlie[line], 0, sizeof(ConfFlie[line]));
		strcpy(ConfFlie[line], newconf_line);
	}

	return 0;
}

/******************************************************************************/
/*! @brief		全監視シーンの中で同じ名前のファイルのVERSIONを変更する
 * @details
******************************************************************************/
static void change_ver(void)
{
	int loop, Line_Index, rt;
	FILE *fp = NULL;
	//no change の場合
	if( !VerChange[0][0] )
	{
		return;
	}

	printf("------------------change version!------------------\n");
	for( loop = 0; loop < st_file_ctrl.VerIndex; loop++ )
	{
		if( VerChange[loop][0] )
		{
			for( st_file_ctrl.SceneIndex = 0; st_file_ctrl.SceneIndex < sizeof(AllScene)/sizeof(AllScene[0]); st_file_ctrl.SceneIndex++ )
			{
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "tmpfile/%s", AllScene[st_file_ctrl.SceneIndex]);
				rt = read_conf(tmp, VerChange[loop]);
				if( rt == -1 )
				{
					printf("read conf faild!%s/%s\n", AllScene[st_file_ctrl.SceneIndex], VerChange[loop]);
					printf("read conf faild!\n");
				}

				for( Line_Index = 0; Line_Index < rt; Line_Index++ )
				{
					if( changeLine(Line_Index, "VERSION", "ver++", VerChange[loop]) == 0 )
					{
						break;
					}
				}
				if( rt <= Line_Index ) printf("%40s   ############not find#############\n", "VERSION");

				/**write new data**/
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "%s/%s", AllScene[st_file_ctrl.SceneIndex], VerChange[loop]);
				fp = fopen(tmp, "w+");
				if(!fp)
				{
					printf("open file faile!\n");
					return ;
				}

				for( Line_Index = 0; Line_Index < rt; Line_Index++ )
				{
					fprintf(fp, "%s\n", ConfFlie[Line_Index]);
				}
				fclose(fp);
				fp = NULL;
			}
		}
	}

	return;
}
