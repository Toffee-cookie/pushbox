#include"database.h"
#include<mysql.h>
#include<stdio.h>

#define DB_NAME "box_man"
#define DB_HOST "127.0.0.1"
#define DB_PORT 3306
#define DB_USER "root"
#define DB_USER_PASSWD "YJX2003.3.27."


static int debug = 1;
bool connect_db(MYSQL& mysql) {
	//��ʼ�����ݿ���
	mysql_init(&mysql);

	//�����ַ�����
	mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");

	//�������ݿ�
	if (mysql_real_connect(& mysql, DB_HOST,DB_USER,DB_USER_PASSWD, DB_NAME, DB_PORT, NULL, 0) == NULL){
		printf("���ݿ����ӳ�������ԭ��: %s\n",mysql_error(&mysql));
	    return false;
	}
	return true;
}
/******************************
*���ܣ�ͨ���û����������ȡ�û���Ϣ
*���룺
*    user-�û���Ϣ���ṹ�壩
*����ֵ��
*    true--�ɹ�  false--ʧ��
*******************************/
bool fetch_user_info(userinfo& user) {
	MYSQL mysql;
	MYSQL_RES* res;//��ѯ�����
	MYSQL_ROW row;//��¼�ṹ��
	char sql[256];
	bool ret = false;

	//�������ݿ�
	if (connect_db(mysql) == false) {
		return false;
	}
	//��ȡ�û���Ϣ
	snprintf(sql, 256, "select id,level_id from users where username ='%s'and password='%s';", user.username.c_str(), user.passwd.c_str());
	ret = mysql_query(&mysql, sql);//�ɹ�����0

	if (ret) {
		printf("���ݿ��ѯ����%s ����ԭ�� %s\n", sql, mysql_error(&mysql));
		mysql_close(&mysql);
		return false;
	}

	//��ȡ���
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);

	if (row == NULL) {
		mysql_free_result(res);//�ͷŽ����
		mysql_close(&mysql);
		return false;
	}

	user.id = atoi(row[0]);
	user.level_id = atoi(row[1]);

	if(debug) printf("userid: %d   level_id: %d\n", user.id, user.level_id);


	//���ؽ��
	mysql_free_result(res);//�ͷŽ����

	mysql_close(&mysql);
	return true;
}

/******************************
*���ܣ�ͨ���û�����ȡ��һ�ؿ���ͼ���磺��ͼ����һ�أ�
*����ֵ��
*    true--�ɹ�  false--ʧ��
*******************************/
bool update_user_level(userinfo& user, int next_level_id) {
	MYSQL mysql;
	MYSQL_RES* res;//��ѯ�����
	MYSQL_ROW row;//��¼�ṹ��
	char sql[256];
	bool ret = false;

	//1.�������ݿ�
	if (connect_db(mysql) == false) {
		return false;
	}
	//2.�����û�ID������һ��
	snprintf(sql, 256, "update users set level_id=%d where id=%d;", next_level_id, user.id);

	ret = mysql_query(&mysql, sql);
	if (ret) {
		printf("���ݸ��³���%s  ����ԭ��: %s  \n", sql, mysql_error(&mysql));
		mysql_close(&mysql);
		return false;
	}

	//3.�ر����ݿ�
	mysql_close(&mysql);
	return true;

}

/******************************
*���ܣ�ͨ���ؿ�ID��ȡ�����ؿ���Ϣ���磺��ͼ����һ�أ�
*���룺
*    level-����ҹؿ���Ϣ���ṹ�壩
*    level_id-Ҫ��ȡ��ϸ�ؿ���Ϣ�Ĺؿ�ID
*����ֵ��
*    true--�ɹ�  false--ʧ��
*******************************/

bool fecth_level_info(levelinfo& level, int level_id) {
	MYSQL mysql;
	MYSQL_RES* res;//��ѯ�����
	MYSQL_ROW row;//��¼�ṹ��
	char sql[256];
	bool ret = false;

	//1.�������ݿ�
	if (connect_db(mysql) == false) {
		return false;
	}

	//2.���ݹؿ�id��ѯ���ݿ��ùؿ���ͼ��Ϣ
	snprintf(sql,256,"select name,map_row,map_column,map_data,next_level_id from levels where id=%d",level_id);
	ret = mysql_query(&mysql, sql);//�ɹ�����0

	if (ret) {
		printf("���ݿ��ѯ����%s ����ԭ�� %s\n", sql, mysql_error(&mysql));
		mysql_close(&mysql);
		return false;
	} 

	//3.��ȡ�����
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);

	if (row == NULL) {
		mysql_free_result(res);//�ͷŽ����
		mysql_close(&mysql);
		return false;
	}

	level.id = level_id;
	level.name = row[0];
	level.map_row = atoi(row[1]);
	level.map_column = atoi(row[2]); 
	level.map_data = row[3];
	level.next_level = atoi(row[4]);

	if (debug) printf("level id: %d  name: %s map row: %d  map column: %d map data: %s next level: %d\n", level.id, level.name.c_str(), level.map_row, level.map_column, level.map_data.c_str(), level.next_level);

	mysql_free_result(res);//�ͷŽ����

	mysql_close(&mysql);
	return true;
}



//�����ݿ��е�ͼת����map��
bool transform_map_db2array(levelinfo& level, int map[LINE][COLUMN]) {
	if (level.map_row > LINE || level.map_column > COLUMN) {
		printf("��ͼ��������������!\n");
		return false;
	}

	if (level.map_data.length() < 1) {
		printf("��ͼ������������������!\n");
		return false;
	}

	int start = 0, end = 0;
	int row = 0, column = 0;

	do {
		end = level.map_data.find('|', start);

		//ȡ���һ��
		if (end < 0) {
			end = level.map_data.length();
		}

		if (start >= end) break;

		string line = level.map_data.substr(start, end - start);
		printf("get line: %s\n", line.c_str());

		//���е�ͼ���ݽ��н���
		char* next_token = NULL;
		char* item = strtok_s((char*)line.c_str(), ",", &next_token);

		column = 0;

		while (item && column < level.map_column) {
			printf("%s ", item);
			map[row][column] = atoi(item);
			column++;

			item = strtok_s(NULL, ",", &next_token);
		}
		//�Ϸ��Լ��
		if (column < level.map_column) {
			printf("��ͼ���ݽ�������,��ֹ!\n");
			return false;
		}

		printf("\n");
		row++;

		if (row >= level.map_row) {
			break;
		}

		start = end + 1;

	} while (1 == 1);

	if (row < level.map_row) {
		printf("��ͼ���������趨, %d(need: %d),��ֹ!\n", row, level.map_row);
		return false;
	}

	return true;
}
