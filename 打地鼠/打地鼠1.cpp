#include <graphics.h>//图形界面支持
#include <conio.h>
#include <mmsystem.h>
#include <Windows.h>
#include <stdio.h>
#pragma comment(lib,"Winmm.lib")
#include <time.h>
#define TEST 1
#define SPEED 20//地鼠移动间隔时间 ms
/*********************************************************/
#define WINDOW_WIDTH		640 //窗口宽
#define WINDOW_HEITH		478 //窗口高

#define RULE_WIDTH			800//规则窗口宽
#define RULE_HEITH			601//规则窗口高

#define TOP_HEITH			192 //背景图top高
#define MID_UP_HEITH		120 //背景图mid_up高
#define MID_DOWN_HEITH		167 //背景图mid_down高
#define BOTTOM_HEITH		116 //背景图bottom高

#define MID_UP_BEGIN_Y		164 //背景图mid_up起始y坐标
#define MID_DOWN_BEGIN_Y	247 //背景图mid_down起始y坐标
#define BOTTOM_BEGIN_Y		364 //背景图bottom起始y坐标

#define LITTLE_MOUSE_WIDTH	90 //小地鼠宽
#define LITTLE_MOUSE_HEITH  74 //小地鼠高
#define MID_MOUSE_WIDTH		102 //中地鼠宽
#define MID_MOUSE_HEITH		86 //中地鼠高
#define BIG_MOUSE_WIDTH		122 //大地鼠宽
#define BIG_MOUSE_HEITH		106 //大地鼠高

#define LITTLE_MOUSE_BEGIN_X	135 //小地鼠起始位置
#define LITTLE_MOUSE_BEGIN_Y	191
#define LITTLE_MOUSE_SPACE		137 //小地鼠间距

#define MID_MOUSE_BEGIN_X		106 //中地鼠起始位置
#define MID_MOUSE_BEGIN_Y		283
#define MID_MOUSE_SPACE			156 //中地鼠间距

#define BIG_MOUSE_BEGIN_X		65 //大地鼠起始位置
#define BIG_MOUSE_BEGIN_Y		410
#define BIG_MOUSE_SPACE			185 //大地鼠间距

#define UP                      1 //地鼠运动方向向上
#define DOWN					0 //地鼠运动方向向下

#define HAMMER_BEGIN_X	320 //锤子初始位置
#define HAMMER_BEGIN_Y	220

#define SCORE_WORDS_X	10 //"得分"文字描述与数字的位置
#define SCORE_NUMBER_X	90
#define SCORE_Y			30

#define TARGET_WORDS_X	5  //"目标"文字描述与数字的位置
#define TARGET_NUMBER_X	90
#define TARGET_Y		10

#define MISSED_WORDS_X	520 //"错过的"文字描述与数字的位置
#define MISSED_NUMBER_X 610
#define MISSED_Y		10

#define MAX_MISSED_WORDS_X	525//"生命值"文字描述与数字的位置
#define MAX_MISSED_NUMBER_X 610
#define MAX_MISSED_Y		30

#define EFFECTIVE_ZONE_X 36
#define EFFECTIVE_ZONE_Y 101
#define EFFECTIVE_ZONE_HEIGHT 33
#define EFFECTIVE_ZONE_WIDTH 64

#define FRAME 20	//锤子砸下状态转换持起状态的间隔帧

#define time_internal 100//从地鼠被打倒后,到再次刷新地鼠的时间间隔

#define SIMPLE 20//简单模式下需要锤的个数
#define SIMPLE_SPEED	3
#define MIDDLE_SPEED	5
#define HARD_SPEED		9
#define HAEEL_SPEED		9
/*********************************************************/
//图片数据

IMAGE top, mid_up, mid_down, bottom;//四张背景图原图
IMAGE top_y, mid_up_y, mid_down_y, bottom_y;//遮罩图

IMAGE little_mouse, mid_mouse, big_mouse;//老鼠原图
IMAGE little_mouse_y, mid_mouse_y, big_mouse_y;//遮罩图

IMAGE img_hammer, img_hammer_down;//锤子原图
IMAGE img_hammer_mask, img_hammer_down_mask;//锤子遮罩图

IMAGE img_hitted,img_hitted_mask;//击中图与遮罩图

HWND hWnd;//窗口句柄

//全局变量
bool isOver;/* = false*///判断是否结束
bool isWin = false;//是否获胜
bool isLose = false;//是否失败
int score;//初始分数
int target;//设置目标
int missed;//成功逃离的地鼠d
int max_miss;//最多可miss的地鼠数量
int life;//剩余生命
int m;//记录当前是哪个地鼠动
int speed;//地鼠的速度，不同难度下不同

void set() {//全局变量初始化函数
	isOver = false;//是否结束赋值为false
	isWin = false;//是否获胜赋值为否
	isLose = false;//是否失败赋值为否
	score = 0;//初始分数设置为0
	target = SIMPLE;//设置目标为简单
	max_miss = SIMPLE;
	missed = 0;
	speed = HARD_SPEED;
	life = max_miss;
	srand(time(0));//播种
	m = rand() % 9;	//0-8
}

struct MyDiShu {
	int	x, y;//地鼠坐标
	IMAGE img, img_y;//地鼠图
	int dir;//记录地鼠运动方向 1-UP 0-DOWN
	bool hitted;//地鼠是否击中过

	void draw()//显示地鼠
	{
		putimage(x, y, &img_y, SRCAND);
		putimage(x, y, &img, SRCPAINT);
	}
	void moveUp()
	{
		y -= speed;//每次移动speed个像素
	}
	void moveDown()
	{
		y += speed;//每次移动speed个像素
	}
};//地鼠结构体

MyDiShu ds[9];//地鼠数组初始化

struct Hammer {
	int x, y;
	bool hit;
	IMAGE img, img_y;
	IMAGE img_hit, img_hit_y;

	void draw()//显示锤子
	{
		bool flag = false;
		bool musicPlay;
		if (hit == false) {
			putimage(x, y, &img_y, SRCAND);
			putimage(x, y, &img, SRCPAINT);
		}
		else /*(hit == true)*/ {//锤子向下打的状态
			//static bool flag;//标志是否打击中了，实则用持久化ds[m].hitted（该值会因为地鼠的改变而无法查询，所以用flag来记录）
			static int hitx, hity;//用于击中效果坐标的持久化，位置等于锤子锤下去时一瞬间的位置，为了击中效果图不随着锤子位置变化而变化，故加入此两个变量用于持久化坐标。
			if (ds[m].hitted == true) {
				flag = true;
				hitx = x - 5;//5为横坐标偏移量;
				hity = y + 40;//40为纵坐标坐标偏移量;
			}
			if (flag == true) {//如果打中了，锤子是向下状态时，那么播放击中图片
				
				//if(){
					mciSendString(_T("close hitmusic "), NULL, 0, NULL);//先关闭之前播放的本音乐
					mciSendString(_T("play hitmusic"), NULL, 0, NULL);//播放音乐文件
				//}
				
				putimage(hitx,hity, &img_hitted_mask, NOTSRCERASE);
				putimage(hitx,hity, &img_hitted, SRCINVERT);
			}
			
			putimage(x, y, &img_hit_y, SRCAND);
			putimage(x, y, &img_hit, SRCPAINT);
			static int count = 0;//count技术，每
			count++;
			if (count == FRAME) {
				count = 0;
				hit = false;//hit,flag同步改变（播放相同数量的帧数）
				flag = false;

			}
		}
	}
};//锤子结构体

Hammer hammer;//锤子结构体初始化

void initGame() //游戏框架初始化
{
	//1 设置窗口
	
	hWnd = initgraph(WINDOW_WIDTH, WINDOW_HEITH);
	//2 图片资源引入,背景音乐资源引入
	loadimage(&top, _T("picture\\top.bmp"));
	loadimage(&mid_up, _T("picture\\mid_up.bmp"));
	loadimage(&mid_down, _T("picture\\mid_down.bmp"));
	loadimage(&bottom, _T("picture\\bottom.bmp"));

	loadimage(&top_y, _T("picture\\top_y.bmp"));
	loadimage(&mid_up_y, _T("picture\\mid_up_y.bmp"));
	loadimage(&mid_down_y, _T("picture\\mid_down_y.bmp"));
	loadimage(&bottom_y, _T("picture\\bottom_y.bmp"));

	loadimage(&little_mouse, _T("picture\\little_mouse.bmp"));
	loadimage(&mid_mouse, _T("picture\\mid_mouse.bmp"));
	loadimage(&big_mouse, _T("picture\\big_mouse.bmp"));

	loadimage(&little_mouse_y, _T("picture\\little_mouse_y.bmp"));
	loadimage(&mid_mouse_y, _T("picture\\mid_mouse_y.bmp"));
	loadimage(&big_mouse_y, _T("picture\\big_mouse_y.bmp"));

	loadimage(&little_mouse_y, _T("picture\\little_mouse_y.bmp"));
	loadimage(&mid_mouse_y, _T("picture\\mid_mouse_y.bmp"));
	loadimage(&big_mouse_y, _T("picture\\big_mouse_y.bmp"));

	loadimage(&img_hammer, _T("picture\\hammer.bmp"));
	loadimage(&img_hammer_mask, _T("picture\\hammer_y.bmp"));

	loadimage(&img_hammer_down, _T("picture\\hammer_down.bmp"));
	loadimage(&img_hammer_down_mask, _T("picture\\hammer_down_y.bmp"));

	loadimage(&img_hitted, _T("picture\\died.png"));
	loadimage(&img_hitted_mask, _T("picture\\died_mask.png"));

	//mciSendString(_T("open music\\bkmusic.mp3 alias bkmusic"), NULL, 0, NULL);
	//mciSendString(_T("play bkmusic repeat"), NULL, 0, NULL);

	mciSendString(_T("open music\\hammerDown.wav alias hitmusic"), NULL, 0, NULL);//重新打开读取音乐文件

	//3 结构体数据初始化
	for (int i = 0; i < 9; i++)
	{
		if (i >= 0 && i < 3)
		{
			ds[i].img = little_mouse;
			ds[i].img_y = little_mouse_y;

			ds[i].x = LITTLE_MOUSE_BEGIN_X + i * LITTLE_MOUSE_SPACE;
			ds[i].y = LITTLE_MOUSE_BEGIN_Y;
		}
		if (i >= 3 && i < 6)
		{
			ds[i].img = mid_mouse;
			ds[i].img_y = mid_mouse_y;

			ds[i].x = MID_MOUSE_BEGIN_X + (i - 3) * MID_MOUSE_SPACE;
			ds[i].y = MID_MOUSE_BEGIN_Y;
		}
		if (i >= 6 && i < 9)
		{
			ds[i].img = big_mouse;
			ds[i].img_y = big_mouse_y;

			ds[i].x = BIG_MOUSE_BEGIN_X + (i - 6) * BIG_MOUSE_SPACE;
			ds[i].y = BIG_MOUSE_BEGIN_Y;
		}

		ds[i].dir = 1;//1-up   0down
		ds[i].hitted = false;//地鼠是否被击过中赋值为否
	}

	hammer.img = img_hammer;
	hammer.img_y = img_hammer_mask;
	hammer.img_hit = img_hammer_down;
	hammer.img_hit_y = img_hammer_down_mask;
	hammer.x = HAMMER_BEGIN_X;
	hammer.y = HAMMER_BEGIN_Y;
	hammer.hit = false;

}


//画面显示
void drawGame() {
	BeginBatchDraw();
	putimage(0, 0, &top_y, SRCAND);//显示上部分背景图
	putimage(0, 0, &top, SRCPAINT);

	for (int i = 0; i < 3; i++)//显示小地鼠
	{
		ds[i].draw();
	}

#if TEST
	putimage(0, MID_UP_BEGIN_Y, &mid_up_y, SRCAND);//显示上半部分背景
	putimage(0, MID_UP_BEGIN_Y, &mid_up, SRCPAINT);
#endif	
	for (int i = 3; i < 6; i++)//显示中地鼠
	{
		ds[i].draw();
	}
#if TEST
	putimage(0, MID_DOWN_BEGIN_Y, &mid_down_y, SRCAND);//显示下半部分背景
	putimage(0, MID_DOWN_BEGIN_Y, &mid_down, SRCPAINT);
#endif
	for (int i = 6; i < 9; i++)//显示大地鼠
	{
		ds[i].draw();
	}
#if TEST
	putimage(0, BOTTOM_BEGIN_Y, &bottom_y, SRCAND);//显示下部分背景
	putimage(0, BOTTOM_BEGIN_Y, &bottom, SRCPAINT);


	hammer.draw();//显示锤子

	//绘制当前得分与目标
	settextcolor(GREEN);
	TCHAR s1[] = _T("当前得分:");
	outtextxy(SCORE_WORDS_X, SCORE_Y, s1);
	TCHAR s_12[5];
	_stprintf_s(s_12, _T("%d"), score);
	outtextxy(SCORE_NUMBER_X, SCORE_Y, s_12);

	settextcolor(YELLOW);
	TCHAR s2[] = _T(" 目标得分:");
	outtextxy(TARGET_WORDS_X, TARGET_Y, s2);
	_stprintf_s(s_12, _T("%d"), target);
	outtextxy(TARGET_NUMBER_X, TARGET_Y, s_12);

	settextcolor(RED);
	TCHAR s3[] = _T("剩余生命:");
	outtextxy(MAX_MISSED_WORDS_X, MAX_MISSED_Y, s3);
	_stprintf_s(s_12, _T("%d"), life);
	outtextxy(MAX_MISSED_NUMBER_X, MAX_MISSED_Y, s_12);

	settextcolor(YELLOW);
	TCHAR s4[] = _T(" 初始血量:");
	outtextxy(MISSED_WORDS_X, MISSED_Y, s4);
	_stprintf_s(s_12, _T("%d"), max_miss);
	outtextxy(MAX_MISSED_NUMBER_X, MISSED_Y, s_12);


#endif
	EndBatchDraw();
}

bool canNotMoveUp()//检查是否往上走到顶了
{
	if (m >= 0 && m < 3) {
		if (ds[m].y <= LITTLE_MOUSE_BEGIN_Y - LITTLE_MOUSE_HEITH)
			return true;
	}
	if (m >= 3 && m < 6) {
		if (ds[m].y <= MID_MOUSE_BEGIN_Y - MID_MOUSE_HEITH)
			return true;
	}
	if (m >= 6 && m < 9) {
		if (ds[m].y <= BIG_MOUSE_BEGIN_Y - BIG_MOUSE_HEITH)
			return true;
	}
	return false;
}

bool canNotMoveDown()//检查是否往下走到底了
{
	if (m >= 0 && m < 3) {
		if (ds[m].y >= LITTLE_MOUSE_BEGIN_Y)
			return true;
	}
	if (m >= 3 && m < 6) {
		if (ds[m].y >= MID_MOUSE_BEGIN_Y)
			return true;
	}
	if (m >= 6 && m < 9) {
		if (ds[m].y >= BIG_MOUSE_BEGIN_Y)
			return true;
	}
	return false;
}

void ctolGame()//控制 
{
	if (ds[m].dir == UP) {
		ds[m].moveUp();
		if (canNotMoveUp())
			ds[m].dir = DOWN;
	}
	else {
		ds[m].moveDown();
		if (canNotMoveDown()) {
			ds[m].dir = UP;
			if (ds[m].hitted == false) {
				missed++;
				life--;
			}
			else {
				ds[m].hitted = false;
			}
			int tmp = m;
			while (tmp == m)
				m = rand() % 9;//刷新地鼠
		}
	}
}

void refreshDiShu() {
	if (m >= 0 && m < 3)
		ds[m].y = LITTLE_MOUSE_BEGIN_Y + 1;
	else if (m >= 3 && m < 6)
		ds[m].y = MID_MOUSE_BEGIN_Y + 1;
	else if (m >= 6 && m < 9)
		ds[m].y = BIG_MOUSE_BEGIN_Y + 1;
}

void control() {
	ExMessage m;//新版写法
	while (peekmessage(&m, EM_MOUSE, PM_NOREMOVE))
	{
		m = getmessage();
		if (m.message == WM_MOUSEMOVE)//更新锤子坐标
		{
			hammer.x = m.x - (img_hammer.getwidth() / 2);
			hammer.y = m.y - (img_hammer.getheight() / 2);
		}
		if (m.message == WM_LBUTTONDOWN && hammer.hit == false)//左键垂下去
		{
			hammer.hit = true;
		}
	}
}

void logic() {
	int effectiveZoneX, effectiveZoneY, effectiveZoneHeight, effectiveZoneWidth;//锤子的有效击中区域
	effectiveZoneX = hammer.x + EFFECTIVE_ZONE_X;
	effectiveZoneY = hammer.y + EFFECTIVE_ZONE_Y;
	effectiveZoneHeight = EFFECTIVE_ZONE_HEIGHT;
	effectiveZoneWidth = EFFECTIVE_ZONE_WIDTH;

	if (hammer.hit == true && ds[m].hitted == false) {
		if (abs(effectiveZoneX - ds[m].x) < effectiveZoneWidth / 2 + ds[m].img.getwidth() / 2 &&
			abs(effectiveZoneY - ds[m].y) < effectiveZoneHeight / 2 + ds[m].img.getheight() / 2) {
			score++;
			ds[m].hitted = true;
			ds[m].dir = DOWN;
			refreshDiShu();
		}
	}

	if (score == target) {
		isWin = true;
	}
	if (life == 0) {
		isLose = true;
	}
	if (isWin == true || isLose == true) {
	//	isOver = true;
	}

}

void drawRules() {
	ExMessage em;
	IMAGE img_rules;
	hWnd = initgraph(RULE_WIDTH,RULE_HEITH);
	loadimage(&img_rules, _T("picture\\rules.jpg"));
	putimage(0,0,&img_rules);

	bool exit = false;
	while (true) {
		while (peekmessage(&em, EM_MOUSE, PM_NOREMOVE)) {
			em = getmessage();
			if (em.message == WM_KEYDOWN || em.message == WM_LBUTTONDOWN || em.message == WM_RBUTTONDOWN) { //按下键盘任意键，鼠标左右键时，退出规则，开始游戏。
				exit = true;
				closegraph();
				break;
			}
		}
		if (exit == true) {
			break;
		}
	}
	
}

int main()
{

	drawRules();
	set();//全局变量初始化
	initGame();//游戏框架初始化
	SetTimer(hWnd,2333, SPEED, (TIMERPROC)ctolGame);//生成计时器
	while (!isOver)//未结束则循环
	{
		logic();//逻辑部分
		drawGame();//画面显示
		control();//控制部分
	}
	while (1);
	return 0;
}