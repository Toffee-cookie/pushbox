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
levelinfo level;//结构体
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
	settextstyle(20, 0, _T("宋体"));
	drawtext(_T("恭喜您~ \n 通关啦！"),&rec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void gameNextScene(IMAGE* bg) {
	putimage(0, 0, bg);
	settextcolor(WHITE);
	RECT rec = { 0, 0, SCREEN_WIDETH, SCREEN_HEIGHT };
	settextstyle(20, 0, _T("宋体"));
	drawtext(_T("恭喜您~ \n 此关挑战成功，任意键跳转到下一关！"), &rec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	cleardevice();
	::system("pause");
}

/***************************************
*改变游戏地图视图中一格对应道具并重新显示
*输入：
	  pos-道具的位置
	  prop-道具的类型
****************************************/
void changeMap(POS* pos, PROPS prop) {
	map[pos->x][pos->y] = prop;
	putimage(START_X + pos->y * RATIO, START_Y + pos->x * RATIO, &images[prop]);
}


/***************************************  
*实现游戏四个方向的控制
*direct - 人前进的方向
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
		//箱子前面是地板
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
		cout << "请输入用户名: ";
		cin >> user.username;

		cout << "请输入密码：";
		cin >> user.passwd;


		ret = fetch_user_info(user);
		times++;

		if (times >= MAX_RETRY_TIMES) {
			break;
		}
		if (ret == false) {
			cout << "登录失败，请重新输入！" << endl;
			cout << "您还有" << MAX_RETRY_TIMES - times <<"次机会" << endl;
		}
	} while (!ret);

	return ret;
}


void init_graphic(IMAGE &bg_img) {
	//戏台
	initgraph(SCREEN_WIDETH, SCREEN_HEIGHT);

	//设置背景
	loadimage(&bg_img, "blackground.bmp", SCREEN_WIDETH, SCREEN_HEIGHT, true);
	putimage(0, 0, &bg_img);

	//墙: 0，地板: 1，箱子目的地: 2, 小人: 3, 箱子: 4, 箱子命中目标: 5
	loadimage(&images[WALL], "wall_right.bmp", RATIO, RATIO, true);
	loadimage(&images[FLOOR], "floor.bmp", RATIO, RATIO, true);
	loadimage(&images[BOX_DES], "des.bmp", RATIO, RATIO, true);
	loadimage(&images[MAN], "man.bmp", RATIO, RATIO, true);
	loadimage(&images[BOX], "box.bmp", RATIO, RATIO, true);
	loadimage(&images[HIT], "box.bmp", RATIO, RATIO, true);

}

int main(void) {
	//用户身份验证
	userinfo user;
	levelinfo level;
	bool ret = false;
	IMAGE bg_img;

	if (!login(user)) {
		cout << "登录失败，请重新登录！" << endl;
		::system("pause");
		exit(-1);
	}
	else {
		cout << "登录成功,用户ID："<<user.id<<",您所在的关卡是 ："<<user.level_id<<",请开始您的表演！" << endl;
		::system("pause");
	}
	//初始化游戏舞台
	init_graphic(bg_img);
	bool quit = false;

	do {
		//根据用户的关卡ID获取关卡数据
		ret = fecth_level_info(level, user.level_id);

		if (!ret) {
			cout << "获取关卡数据失败，请重试！" << endl;
			::system("pause");
			exit(-1);
		}
		//把数据库中地图转换到map中
		ret = transform_map_db2array(level, map);
		if (!ret) {
			cout << "转化地图失败，请重试！" << endl;
			::system("pause");
			exit(-1);
		}

		for (int i = 0; i < level.map_row; i++) {
			for (int j = 0; j < level.map_column; j++) {
				if (map[i][j] == MAN) {    //小人的位置
					man.x = i;
					man.y = j;
				}
				putimage(START_X + j * RATIO, START_Y + i * RATIO, &images[map[i][j]]);
			}
		}

		//热键控制

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