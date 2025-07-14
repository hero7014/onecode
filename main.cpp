#include<stdio.h>
#include<graphics.h>
#include"tools.h"
#include<time.h>
#include<mmsystem.h>
#pragma comment (lib,"winmm.lib")
#define WIN_WIDTH 900
#define WIN_HEIGHT 600

enum
{WAN_DOU, XIANG_RI_KUI, ZHI_WU_COUNT};

IMAGE imgBg;
IMAGE imgBar;
IMAGE imgCards[ZHI_WU_COUNT];
IMAGE* imgZhiWu[ZHI_WU_COUNT][20];
int curX, curY;
int curZhiWu;
int sunshine;
struct zhiwu {
	int type;
	int frameIndex;
};
struct sunshineBall
{
	int x, y;
	int frameIndex;
	int destY;
	bool used;
	int timer;
};
struct sunshineBall balls[10];
IMAGE imgsunshineBall[29];





struct zhiwu map[3][9];

bool fileExist(const char* name) {
	FILE* fp = fopen(name, "r");
	if (fp == NULL) {
		return false;
	}
	else {
		fclose(fp);
		return true;
	}
}


void gameInit() {
	initgraph(WIN_WIDTH, WIN_HEIGHT,1);
	loadimage(&imgBg, "res/bg.jpg");
	loadimage(&imgBar, "res/bar5.png");
	memset(imgZhiWu, 0, sizeof(imgZhiWu));
	memset(map, 0, sizeof(map));
	memset(balls, 0, sizeof(balls));
	char name[64];
	for (int i = 0;i < ZHI_WU_COUNT;i++) {
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png",i+1);
		loadimage(&imgCards[i], name);
		
		for (int j = 0; j < 20; j++) {
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
			if (fileExist(name)) {
				imgZhiWu[i][j] = new IMAGE;
				loadimage(imgZhiWu[i][j],name);
			}
			else {
				break;
			}
		}
	}
	for (int i = 0; i < 29;i++) {
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png",i + 1);
		loadimage(&imgsunshineBall[i], name);

	}
	sunshine = 50;
	srand(time(NULL));
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWeight = 15;
	strcpy(f.lfFaceName, "Segoe UI Black");
	f.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&f);
	setbkmode(TRANSPARENT);
	setcolor(BLACK);


}
void UpdateWindow() {
	BeginBatchDraw();
	putimage(0,0,&imgBg);
	putimagePNG(250, 0, &imgBar);
	for (int i = 0;i < ZHI_WU_COUNT;i++) {
		int x = 338 + i * 65;
		int y = 6;
		putimage(x, y, &imgCards[i]);
	}
	
	for (int i = 0;i < 3;i++) {
		for (int j = 0;j < 9;j++) {
			if (map[i][j].type > 0) {
				int x = 256 + j * 81;
				int y = 179 + i * 102+14;
				int zhiWuType = map[i][j].type-1;
				int index = map[i][j].frameIndex;
				putimagePNG(x, y, imgZhiWu[zhiWuType][index]);
			}
		}
	}
	if (curZhiWu > 0) {
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
	}
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0;i < ballMax; i++) {
		if (balls[i].used) {
			IMAGE * img = &imgsunshineBall[balls[i].frameIndex];
			putimagePNG(balls[i].x, balls[i].y, img);
		}
		
	}
	char scoreText[8];
	sprintf_s(scoreText, sizeof(scoreText), "%d", sunshine);
	outtextxy(276, 67, scoreText);

	EndBatchDraw();
}
void collectSunshine(ExMessage* msg) {
	int count = sizeof(balls) / sizeof(balls[0]);
	int w = imgsunshineBall[0].getwidth();
	int h = imgsunshineBall[0].getheight();
	for (int i = 0; i < count; i++) {
		if (balls[i].used) {
			int x = balls[i].x;
			int y = balls[i].y;
			if (msg->x > x && msg->x <x + w
				&& msg->y > y && msg->y < y + h) {
				balls[i].used = false;
				sunshine += 25;
				mciSendString("play res/sunshine.mp3", 0, 0, 0);
			}
			
		}
	}
	
}
void userClick() {
	ExMessage msg;
	static int status = 0;
	if (peekmessage(&msg)) {
		if (msg.message == WM_LBUTTONDOWN) {
			if (msg.x > 338 && msg.x < 338 + 65 * ZHI_WU_COUNT && msg.y < 96) {
				int index = (msg.x - 338) / 65;
				status = 1;
				curZhiWu = index + 1;
				
			}
			else {
				collectSunshine(&msg);
			}
		}
		else if (msg.message == WM_MOUSEMOVE&& status == 1) {
			curX = msg.x;
			curY = msg.y;
			
		}
		else if (msg.message == WM_LBUTTONUP) {
			if (msg.x > 256 && msg.y > 179 && msg.y < 489) {
				int row = (msg.y - 179) / 102;
				int col = (msg.x - 256) / 81;
				if (map[row][col].type == 0) {
					map[row][col].type = curZhiWu;
					map[row][col].frameIndex = 0;
				}
				

			}
			
			curZhiWu = 0;
			status = 0;
		}

	}
}
void creatSunshine() {
	static int count = 0;
	static int fre = 400;
		count++;
	if (count >= fre) {
		fre = 200 + rand() % 200;
		count = 0;
		int ballMax = sizeof(balls) / sizeof(balls[0]);
		int i;
		for (i = 0; i < ballMax && balls[i].used;i++);
		if (i >= ballMax)return;
		balls[i].used = true;
		balls[i].frameIndex = 0;
		balls[i].x = 260 + rand() % (900 - 260);
		balls[i].y = 60;
		balls[i].destY = 200 + (rand() % 4) * 90;
		balls[i].timer = 0;

	}
	
}
void updatesunshine() {
	int ballMax = sizeof(balls) / sizeof (balls[0]);
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {
			balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;
			if (balls[i].y < balls[i].destY) {
				balls[i].y += 2;
			}
			else{
			
				balls[i].timer++;
				
				if (balls[i].timer > 100) {
					balls[i].used = false;
				}
				

			}

		}
	}


}
void updateGame() {
	for (int i = 0;i < 3;i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				map[i][j].frameIndex++;
				int zhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;
				if (imgZhiWu[zhiWuType][index] == NULL) {
					map[i][j].frameIndex = 0;
				}

			
			}
		}
	}
	creatSunshine();
	updatesunshine();
}

void startUI() {
	IMAGE  imgBg, imgMenu1, imgMenu2;
	loadimage(&imgBg, "res/menu.png");
	loadimage(&imgMenu1, "res/menu1.png");
	loadimage(&imgMenu2, "res/menu2.png");
	int flag = 0;
	while (1) {
		BeginBatchDraw();
		putimage(0, 0, &imgBg);
		putimagePNG(474, 75, flag ? &imgMenu2: &imgMenu1);
		
		ExMessage msg;
		if (peekmessage(&msg)) {
			if (msg.message == WM_LBUTTONDOWN &&
				msg.x > 474 && msg.x < 474 + 300 &&
				msg.y > 75 && msg.y < 75 + 140) {
				flag = 1;
			}
		}
		else if (msg.message == WM_LBUTTONUP && flag == 1) {
			return;
			}
		EndBatchDraw();
				
	}


}

void releaseResources() {
	for (int i = 0; i < ZHI_WU_COUNT; i++) {
		for (int j = 0; j < 20; j++) {
			if (imgZhiWu[i][j] != NULL) {
				delete imgZhiWu[i][j];  
				imgZhiWu[i][j] = NULL;
			}
		}
	}
}




int main(void) {
	gameInit();
	startUI();
	bool flag = true;
	int timer = 0;


	while (1) {
		userClick();
		timer += getDelay();
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
			break;  
		}
		if (timer > 20) {
			flag = true;
			timer = 0;
		}
		if (flag) {
			flag = false;
			UpdateWindow();
			updateGame();

		}
	    
	}
	

	releaseResources();
	system("pause");
	return 0;
}
