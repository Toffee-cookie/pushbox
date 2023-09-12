#include<graphics.h>
#include<conio.h>
#include <iostream>
#include <stdlib.h>
#include <string>

#include"boxman.h"
#include"database.h"
using namespace std;

int map[LINE][COLUMN] = { 0 };
/*
int map[LINE][COLUMN] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
	{ 0, 1, 4, 1, 0, 2, 1, 0, 2, 1, 0, 0 },
	{ 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0 },
	{ 0, 1, 0, 2, 0, 1, 1, 4, 1, 1, 1, 0 },
	{ 0, 1, 1, 1, 0, 3, 1, 1, 1, 4, 1, 0 },
	{ 0, 1, 2, 1, 1, 4, 1, 1, 1, 1, 1, 0 },
	{ 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

*/
//0,0,0,0,0,0,0,0,0,0,0,0|0,1,0,1,1,1,1,1,1,1,0,0|0,1,4,1,0,2,1,0,2,1,0,0|0,1,0,1,0,1,0,0,1,1,1,0|0,1,0,2,0,1,1,4,1,1,1,0|0,1,1,1,0,3,1,1,1,4,1,0| 0,1,2,1,1,4,1,1,1,1,1,0|0,1,0,0,1,0,1,1,0,0,1,0|0,0,0,0,0,0,0,0,0,0,0,0
typedef enum _PROPS PROPS;
typedef enum _DIRECTION DIRECTION;
typedef struct _POS POS;

IMAGE images[ALL];
POS man;
levelinfo level;//�ṹ��
bool isGameOver() {
	for (int i = 0; i < level.map_row; i++) {
		for (int j = 0; j < level.map_column; j++) {
			if (map[i][j] == BOX_DES)
				return false;
		}
	}
	return true;
}

void gameOverScene(IMAGE* bg) {
	putimage(0, 0, bg);
	settextcolor(WHITE);
	RECT rec = { 0, 0, SCREEN_WIDETH, SCREEN_HEIGHT };
	settextstyle(20, 0, _T("����"));
	drawtext(_T("��ϲ��~ \n ͨ������"),&rec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void gameNextScene(IMAGE* bg) {
	putimage(0, 0, bg);
	settextcolor(WHITE);
	RECT rec = { 0, 0, SCREEN_WIDETH, SCREEN_HEIGHT };
	settextstyle(20, 0, _T("����"));
	drawtext(_T("��ϲ��~ \n �˹���ս�ɹ����������ת����һ�أ�"), &rec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	cleardevice();
	::system("pause");
}

/***************************************
*�ı���Ϸ��ͼ��ͼ��һ���Ӧ���߲�������ʾ
*���룺
	  pos-���ߵ�λ��
	  prop-���ߵ�����
****************************************/
void changeMap(POS* pos, PROPS prop) {
	map[pos->x][pos->y] = prop;
	putimage(START_X + pos->y * RATIO, START_Y + pos->x * RATIO, &images[prop]);
}


/***************************************  
*ʵ����Ϸ�ĸ�����Ŀ���
*direct - ��ǰ���ķ���
****************************************/
void gameControl(DIRECTION direct) {
	POS next_pos = man;
	POS next_next_pos = man;
	switch (direct) {
	case UP: 
		next_pos.x = man.x - 1;
		next_next_pos.x = man.x - 2;
		break;
	case DOWN:
		next_pos.x = man.x + 1;
		next_next_pos.x = man.x + 2;
		break;
	case LEFT:
		next_pos.y = man.y - 1;
		next_next_pos.y = man.y - 2;
		break;
	case RIGHT:
		next_pos.y = man.y + 1;
		next_next_pos.y = man.y + 2;
		break;
	}

	if (isValid(next_pos) && map[next_pos.x][next_pos.y] == FLOOR) {
		changeMap(&next_pos, MAN);
		changeMap(&man, FLOOR);
		man = next_pos;
	}

	if (isValid(next_pos) && map[next_pos.x][next_pos.y] == BOX) {
		//����ǰ���ǵذ�
		if (isValid(next_pos) && map[next_next_pos.x][next_next_pos.y] == FLOOR) {
			changeMap(&next_next_pos, BOX);
			changeMap(&next_pos, MAN);
			changeMap(&man, FLOOR);
			man = next_pos;
		}
		else if (isValid(next_pos) && map[next_next_pos.x][next_next_pos.y] == BOX_DES) {
			changeMap(&next_next_pos, HIT);
			changeMap(&next_pos, MAN);
			changeMap(&man, FLOOR);
			man = next_pos;
		}
	}
}

bool login(userinfo& user) {
	int times = 0;
	bool ret = false;

	do {
		cout << "�������û���: ";
		cin >> user.username;

		cout << "���������룺";
		cin >> user.passwd;


		ret = fetch_user_info(user);
		times++;

		if (times >= MAX_RETRY_TIMES) {
			break;
		}
		if (ret == false) {
			cout << "��¼ʧ�ܣ����������룡" << endl;
			cout << "������" << MAX_RETRY_TIMES - times <<"�λ���" << endl;
		}
	} while (!ret);

	return ret;
}


void init_graphic(IMAGE &bg_img) {
	//Ϸ̨
	initgraph(SCREEN_WIDETH, SCREEN_HEIGHT);

	//���ñ���
	loadimage(&bg_img, "blackground.bmp", SCREEN_WIDETH, SCREEN_HEIGHT, true);
	putimage(0, 0, &bg_img);

	//ǽ: 0���ذ�: 1������Ŀ�ĵ�: 2, С��: 3, ����: 4, ��������Ŀ��: 5
	loadimage(&images[WALL], "wall_right.bmp", RATIO, RATIO, true);
	loadimage(&images[FLOOR], "floor.bmp", RATIO, RATIO, true);
	loadimage(&images[BOX_DES], "des.bmp", RATIO, RATIO, true);
	loadimage(&images[MAN], "man.bmp", RATIO, RATIO, true);
	loadimage(&images[BOX], "box.bmp", RATIO, RATIO, true);
	loadimage(&images[HIT], "box.bmp", RATIO, RATIO, true);

}

int main(void) {
	//�û������֤
	userinfo user;
	levelinfo level;
	bool ret = false;
	IMAGE bg_img;

	if (!login(user)) {
		cout << "��¼ʧ�ܣ������µ�¼��" << endl;
		::system("pause");
		exit(-1);
	}
	else {
		cout << "��¼�ɹ�,�û�ID��"<<user.id<<",�����ڵĹؿ��� ��"<<user.level_id<<",�뿪ʼ���ı��ݣ�" << endl;
		::system("pause");
	}
	//��ʼ����Ϸ��̨
	init_graphic(bg_img);
	bool quit = false;

	do {
		//�����û��Ĺؿ�ID��ȡ�ؿ�����
		ret = fecth_level_info(level, user.level_id);

		if (!ret) {
			cout << "��ȡ�ؿ�����ʧ�ܣ������ԣ�" << endl;
			::system("pause");
			exit(-1);
		}
		//�����ݿ��е�ͼת����map��
		ret = transform_map_db2array(level, map);
		if (!ret) {
			cout << "ת����ͼʧ�ܣ������ԣ�" << endl;
			::system("pause");
			exit(-1);
		}

		for (int i = 0; i < level.map_row; i++) {
			for (int j = 0; j < level.map_column; j++) {
				if (map[i][j] == MAN) {    //С�˵�λ��
					man.x = i;
					man.y = j;
				}
				putimage(START_X + j * RATIO, START_Y + i * RATIO, &images[map[i][j]]);
			}
		}

		//�ȼ�����

		do {
			if (_kbhit()) {
				char ch = _getch();
				if (ch == KEY_UP) {
					gameControl(UP);
				}
				else if (ch == KEY_DOWN) {
					gameControl(DOWN);
				}
				else if (ch == KEY_LEFT) {
					gameControl(LEFT);
				}
				else if (ch == KEY_RIGHT) {
					gameControl(RIGHT);
				}
				else if (ch == KEY_QUIT) {
					quit = false;
				}
				if (isGameOver()) {
					
					//quit = true;
					if (level.next_level < 1) {
						gameOverScene(&bg_img);
						quit = true;
						break;
					}
					gameNextScene(&bg_img);
					if (update_user_level(user, level.next_level)) {
						user.level_id = level.next_level;
						putimage(0, 0, &bg_img);
					}
					break;
				}
			}
			Sleep(100);
		} while (quit == false);


	} while (quit == false);


	::system("pause");
	closegraph();
	return 0;
}