#pragma once

#define SCREEN_WIDETH 800
#define SCREEN_HEIGHT 650
//#define LINE 48
//#define COLUMN 48
#define RATIO 50
#define START_X 100
#define START_Y 100 

#define KEY_UP      'w'
#define KEY_LEFT    'a'
#define KEY_RIGHT   'd'
#define KEY_DOWN    's'
#define KEY_QUIT    'q'

#define MAX_RETRY_TIMES 5
#define isValid(pos)(pos.x>=0&&pos.x<LINE && pos.y >= 0 && pos.y < COLUMN)

enum _PROPS {
	WALL,
	FLOOR,
	BOX_DES,
	MAN,
	BOX,
	HIT,
	ALL
};

//游戏控制方向
enum _DIRECTION {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct _POS {
	int x;
	int y;
};

