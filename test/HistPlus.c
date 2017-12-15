/*
 * HistPlus.c
 *
 *  Created on: 2017/12/14
 *      Author: mcc
 */
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#define NELEMENT(a)		(sizeof((a))/sizeof((a)[0]))

int const ab_1[]={
		4097,4096,4099,4098,4101,4100,4103,4102,4105,4104,4107,4106,4109,4108,
		4111,4110,4113,4112,4115,4114,4117,4116,4353,4352,4355,4354,4357,4356,
		4359,4358,4361,4360,4363,4362,4365,4364,4609,4608,4611,4610,4865,4864,
		4867,4866,4869,4868,4871,4870,4873,4872,4877,4876,4879,4878,4881,4880,
		4883,4882,4885,4884,4887,4886,4889,4888,4891,4890,4893,4892,4895,4894,
		4897,4896,4899,4898,4901,4900,4903,4902,4905,4904,4907,4906,4909,4908,
		4911,4910,4913,4912,4915,4914,4917,4916,4919,4918,4921,4920,4923,4922,
		5121,5120,5123,5122,5125,5124,5127,5126
};
int const ab_2[]={0,1};

int const inout_1[]={8193,8195,8449,8451};
int const inout_2[]={1,2};
int const inout_3[]={0,1};

int const op[][2]={
		{16389,1},{16389,2},{16389,3},{16389,4},{16389,5},{16389,6},{16391,1},
		{16391,2},{16391,3},{16391,4},{16391,5},{16391,6},{16385,0},{16387,0},
		{16395,1},{16395,2},{16397,1},{16397,2},{16649,0},{16393,0}
};

int const menu_1[]={
		20481,20483,20485,20487,20737,20739,20741,20743,20745,20747,20749,
		20751,20753,20755,20757,20759,20761,20763,20993,20995,20997,21249,
		21251,21253,21255,21257,21259,21261,21263,21275,21277,21507,// type a 0-31
		21265,21267,21269,21271,// type b 32-35
		21273,// type c 36
		21505,21283// type d 37-38
};
int const menu_2[][2]={
		{20481,1},{20481,2},{20481,3},{20481,4},{20481,5},{20483,1},{20483,2},{20483,3},{20485,1},
		{20485,2},{20485,3},{20485,4},{20485,5},{20487,1},{20487,2},{20487,3},{20487,4},{20487,5},
		{20737,1},{20737,2},{20737,3},{20739,1},{20739,2},{20739,3},{20741,1},{20741,2},{20741,3},
		{20743,1},{20743,2},{20743,3},{20743,4},{20745,1},{20745,2},{20745,3},{20745,4},{20747,1},
		{20747,2},{20747,3},{20747,4},{20749,1},{20749,2},{20749,3},{20751,1},{20751,2},{20751,3},
		{20753,1},{20753,2},{20753,3},{20755,1},{20755,2},{20757,1},{20757,2},{20759,1},{20759,2},
		{20761,1},{20761,2},{20763,1},{20763,2},{20763,3},{20763,4},{20763,5},{20763,6},{20763,7},
		{20763,8},{21263,1},{21263,2},{21263,3},{21249,1},{21249,2},{21249,3},{21257,1},{21257,2},
		{21259,1},{21259,2},{21261,1},{21261,2},{21265,1},{21265,2},{21265,3},{21265,4},{21267,1},
		{21267,2},{21267,3},{21267,4},{21269,1},{21269,2},{21269,3},{21269,4},{21271,1},{21271,2},
		{21271,3},{21271,4},{21505,1},{21505,2},{21505,3},{21505,4},{21505,5},{21505,6},{21505,7},
		{21505,8},{21505,9},{21505,10},{21505,11},{21505,12},{21505,13},{21505,14},{21505,15},{21505,16},
		{21505,17},{21505,18},{21505,19},{21505,20},{21505,21},{21505,23},{21505,24},{21505,24},{21283,1},
		{21283,2},{21283,3},{21283,4}
};
int const menu_3[][2]={
		{20481,1},{20481,2},{20481,3},{20481,4},{20481,5},{20481,6},{20483,1},
		{20483,2},{20485,1},{20485,2},{21249,1},{21249,2},{21249,3},{21259,1},
		{21259,2},{21261,1},{21261,2}
};

int insert_ab(int num_ab)
{
	int ret;
	char			sql[256];					// sqlite操作文
	sqlite3		*db;
	unsigned long time = 1293807600;

	if(!num_ab) return 0;

	ret = sqlite3_open("/var/log/hist_ab.bin", &db);
	if( ret )
	{
		printf("Can't open database /var/log/hist_ab.bin\n");
		return 1;
	}
	while(num_ab){
		snprintf(sql, 256, "INSERT INTO HIST_Error_Tbl_v1 VALUES(null,%lu,%d,'%.7s','%.7s','%.16s',%d);",
				time, ab_1[random()%NELEMENT(ab_1)], "m_code", "MfgNo", "name", ab_2[random()%NELEMENT(ab_2)]);
		ret = sqlite3_exec(db, sql, 0, 0, NULL);
		if( ret )
		{
			printf("INSERT failed /var/log/hist_ab.bin\n");
			return 1;
		}

		time += random()%10;
		num_ab--;
	}
	sqlite3_close(db);
	return 0;
}

int insert_inout(int num_inout)
{
	int ret;
	char			sql[256];					// sqlite操作文
	sqlite3		*db;
	unsigned long time = 1293807600;

	if(!num_inout) return 0;

	ret = sqlite3_open("/var/log/hist_inout.bin", &db);
	if( ret )
	{
		printf("Can't open database /var/log/hist_inout.bin\n");
		return 1;
	}
	while(num_inout){
		snprintf(sql, 256, "INSERT INTO HIST_InOut_Tbl_v1 VALUES(null,%lu,%d,%d,%d,%d,'%.7s','%.7s','%.16s',%d);",
						time, inout_1[random()%NELEMENT(inout_1)], 1, inout_2[random()%NELEMENT(inout_2)],
						inout_3[random()%NELEMENT(inout_3)], "m_code", "MfgNo", "name", 2);
		ret = sqlite3_exec(db, sql, 0, 0, NULL);
		if( ret )
		{
			printf("INSERT failed /var/log/hist_inout.bin\n");
			return 1;
		}

		time += random()%10;
		num_inout--;
	}
	return 0;
}

int insert_op(int num_op)
{
	int ret;
	char			sql[256];					// sqlite操作文
	sqlite3		*db;
	unsigned long time = 1293807600;
	int rand;

	if(!num_op) return 0;

	ret = sqlite3_open("/var/log/hist_op.bin", &db);
	if( ret )
	{
		printf("Can't open database /var/log/hist_op.bin\n");
		return 1;
	}
	while(num_op){
		rand = random()%NELEMENT(op);
		snprintf(sql, 256, "INSERT INTO HIST_Ope_Tbl_v1 VALUES(null,%lu,%hu,'%.7s',%lu,'%.16s',%d);",
						time, op[rand][0], "m_code", (long unsigned)123456, "name", op[rand][1]);
		ret = sqlite3_exec(db, sql, 0, 0, NULL);
		if( ret )
		{
			printf("INSERT failed /var/log/hist_inout.bin\n");
			return 1;
		}

		time += random()%10;
		num_op--;
	}
	return 0;
}
/*
int insert_menu(int num_menu)
{
	int ret;
	char			sql[256];					// sqlite操作文
	sqlite3		*db;
	unsigned long time = 1234567890;
	int rand;

	if(!num_menu) return 0;

	ret = sqlite3_open("/var/log/hist_op.bin", &db);
	if( ret )
	{
		printf("Can't open database /var/log/hist_op.bin\n");
		return 1;
	}
	while(num_menu){
		rand = random()%NELEMENT(menu_1);

		if(rand<=31)
			snprintf(sql, 256, "INSERT INTO HIST_Menu_ctrl_v1 VALUES(null);INSERT INTO HIST_Menu_TypeA_v1 VALUES((SELECT max(id) from HIST_Menu_ctrl_v1),%lu,%hu,'%.7s',%lu,'%.16s',%d,%d,%d,%hu,%hu);",
					time, menu_1[rand],
					"m_code", "MfgNo", "name",
					p_menu->u.type_a.anyset1, p_menu->u.type_a.anyset2, p_menu->u.type_a.anyset3,
					123, 456);
		else if(rand<=35)
			snprintf(sql, D_HIST_SQL_SIZE, "INSERT INTO HIST_Menu_ctrl_v1 VALUES(null);" D_HIST_MENU_B_WRITE,
					p_menu->time, p_menu->type,
					p_menu->u.type_b.m_code, p_menu->u.type_b.init_kind, p_menu->u.type_b.MfgNo,
					p_menu->u.type_b.name, p_menu->u.type_b.obj_machine,
					p_menu->u.type_b.format_ver, p_menu->u.type_b.connect_id);
		else if(rand=36)
			snprintf(sql, D_HIST_SQL_SIZE, "INSERT INTO HIST_Menu_ctrl_v1 VALUES(null);" D_HIST_MENU_C_WRITE,
					p_menu->time, p_menu->type,
					p_menu->u.type_c.MfgNo, p_menu->u.type_c.opr_code, p_menu->u.type_c.m_code,
					p_menu->u.type_c.name, p_menu->u.type_c.preVersion, p_menu->u.type_c.currVersion,
					p_menu->u.type_c.format_ver, p_menu->u.type_c.connect_id);
		else
			snprintf(sql, D_HIST_SQL_SIZE, "INSERT INTO HIST_Menu_ctrl_v1 VALUES(null);" D_HIST_MENU_D_WRITE,
					p_menu->time, p_menu->type,
					p_menu->u.type_d.opr_code, p_menu->u.type_d.MfgNo, p_menu->u.type_d.anyset1,
					p_menu->u.type_d.name, p_menu->u.type_d.m_code,
					p_menu->u.type_d.format_ver, p_menu->u.type_d.connect_id);


		ret = sqlite3_exec(db, sql, 0, 0, NULL);
		if( ret )
		{
			printf("INSERT failed /var/log/hist_inout.bin\n");
			return 1;
		}

		time += random()%10;
		num_menu--;
	}
	return 0;
}
*/
int main(	int argc,char *argv[])
{
	int num_ab,num_inout,num_op,num_menu;

	num_ab = atoi(argv[1]);
	num_inout = atoi(argv[2]);
	num_op = atoi(argv[3]);
	//num_menu = atoi(argv[4]);

	insert_ab(num_ab);
	insert_inout(num_inout);
	insert_op(num_op);
	//int insert_menu(int num_menu);
	return 0;
}




