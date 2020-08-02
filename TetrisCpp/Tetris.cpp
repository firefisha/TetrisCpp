#include<windows.h>
#include<time.h>
#include<stdlib.h>

#define W 14
#define H 20
#define W1 6
#define BSIZE 25   // length of the game square length size
#define Y1 6
#define Y2 12
#define Y3 12
#define Y4 12  //coodination 
#define Cur_x W/2-1 //the initial place
#define Cur_y 1 //

#define BgColor RGB(0xF5,0xF5,0XDC)//color
#define FgColor RGB(255,153,204)//color
#define RED RGB(255,0,0)//color
#define ORANGE RGB(250,128,10)
#define YELLOW RGB(255,255,0)
#define GREEN RGB(0,255,0)
#define CYAN RGB(0,255,255)
#define LIGHT_BLUE RGB(0xA6,0xCA,0xF0) 
#define PURPLE RGB(225,0,255)

#define MS_NEWBLOCK WM_USER+1 //message id

int score = 0, level = 0, level_step = 100;
int top = H - 1;
int x, y;//current position

int cur_boxnum, next_boxnum; //current square number

struct BOARD {

	int var;//1 means taken over, 0 means not
	int color;

}board[H][W];//define the game board

struct BLOCK {

	int a[4][2];
	int color;
	int next;

};	

struct BLOCK block[19] = {
	//initial the beginning state
	{ 1,1,1,2,1,3,2,3,RED,1 },
	{ 0,2,1,2,2,2,0,3,RED,2 },
	{ 0,1,1,1,1,2,1,3,RED,3 },
	{ 2,1,0,2,1,2,2,2,RED,0 },
	{ 1,1,1,2,0,3,1,3,ORANGE,5 },
	{ 0,1,0,2,1,2,2,2,ORANGE,6 },
	{ 1,1,2,1,1,2,1,3,ORANGE,7 },
	{ 0,2,1,2,2,2,2,3,ORANGE,4 },
	{ 1,1,0,2,1,2,2,2,YELLOW,9 },
	{ 1,1,1,2,2,2,1,3,YELLOW,10 },
	{ 0,2,1,2,2,2,1,3,YELLOW,11 },
	{ 1,1,0,2,1,2,1,3,YELLOW,8 },
	{ 1,1,1,2,2,2,2,3,GREEN,13 },
	{ 1,2,2,2,0,3,1,3,GREEN,12 },
	{ 2,1,1,2,2,2,1,3,CYAN,15 },
	{ 0,2,1,2,1,3,2,3,CYAN,14 },
	{ 1,0,1,1,1,2,1,3,LIGHT_BLUE,17 },
	{ 0,2,1,2,2,2,3,2,LIGHT_BLUE,16 },
	{ 1,1,2,1,1,2,2,2,PURPLE,18 }

};

void Paint(HDC hdc, HPEN hpen)// this function is for initilization of interce
{
	int i, j;
	HPEN hpen1;//define the pen
	HBRUSH hbrush = CreateSolidBrush(BgColor);//brush

	hpen1 = CreatePen(PS_DASHDOTDOT, 3, FgColor);//brush pen initialization
	SelectObject(hdc, hpen1);
	MoveToEx(hdc, W * BSIZE, 0, NULL); //Move the cursor to (W*BSIZE,0)
	LineTo(hdc, W * BSIZE, H * BSIZE);
	DeleteObject(hpen1);

	SelectObject(hdc, hpen);
	SelectObject(hdc, hbrush);

	//Draw grid lines in the game area
	for (i = 0; i < H - 1; i++) {
		for (j = 0; j < W - 1; j++) {
			Rectangle(hdc, j * BSIZE, i * BSIZE, (j + 1) * BSIZE, (i + 1) * BSIZE);
		}
	}

	//Draw a grid line in the game preview area on the right status bar
	for (i = 0; i < 5; i++) {
		for (j = W + 1; j < W + W1 - 1; j++) {
			Rectangle(hdc, j * BSIZE, i * BSIZE, (j + 1) * BSIZE, (i + 1) * BSIZE);
		}
	}

	//draw the scores, level, help grid line 
	Rectangle(hdc, (W + 1) * BSIZE, Y2 * BSIZE, (W + W1 - 1) * BSIZE, (Y2 + 2) * BSIZE);
	Rectangle(hdc, (W + 1) * BSIZE, Y3 * BSIZE, (W + W1 - 1) * BSIZE, (Y3 + 2) * BSIZE); 
	Rectangle(hdc, (W + 1) * BSIZE, Y4 * BSIZE, (W + W1 - 1) * BSIZE, (Y4 + 4) * BSIZE); 

	TextOut(hdc, (W + 2) * BSIZE, (Y2 + 0.2) * BSIZE, " scores ", 8);
	TextOut(hdc, (W + 2) * BSIZE, (Y2 + 0.2) * BSIZE, " level ", 8);

	DeleteObject(hpen);
	DeleteObject(hbrush);

}

void ShowScore(HDC hdc)
{

	char score_str[4];//Define string to save the score value
	wsprintf(score_str, "%3d", score);//Convert the number score into a string and save it in score_str
	TextOut(hdc, (W + 2.5) * BSIZE, (Y2 + 1.2)* BSIZE, score_str, 3);//output them

}

void Showlevel(HDC hdc)
{

	char level_str[4];
	wsprintf(level_str, "%3d", level);
	TextOut(hdc, (W + 2.5) * BSIZE, (Y3 + 1.2) * BSIZE, level_str, 3);

}

void ShowHelp(HDC hdc)
{
	char help1[] = "↑ - rotate";
	char help2[] = "↓ - down";
	char help3[] = "← - left move";
	char help4[] = "→ - rigth move";
	
	TextOut(hdc, (W + 1.8) * BSIZE, (Y4 + 0.2) * BSIZE, help1, 9);
	TextOut(hdc, (W + 1.8) * BSIZE, (Y4 + 0.2) * BSIZE, help2, 9);
	TextOut(hdc, (W + 1.8) * BSIZE, (Y4 + 0.2) * BSIZE, help3, 9);
	TextOut(hdc, (W + 1.8) * BSIZE, (Y4 + 0.2) * BSIZE, help4, 9);

}

void EraseBox(HDC hdc, HPEN hpen, int x, int y, int num)//erase the block at position x y 
{
	
	int i;
	HBRUSH hbrush = CreateSolidBrush(BgColor);
	SelectObject(hdc, hpen);
	SelectObject(hdc, hbrush);

	for (i = 0; i < 4; i++) {

		Rectangle(hdc,
			(x + block[num].a[i][0]) * BSIZE,
			(y + block[num].a[i][1]) * BSIZE,
			(x + block[num].a[i][0] + 1) * BSIZE,
			(y + block[num].a[i][1] + 1) * BSIZE
		);
	}

	DeleteObject(hpen);
	DeleteObject(hbrush);

}

void ShowBox(HDC hdc, HPEN hpen, int x, int y, int num) 
{
	int i;
	HBRUSH hbrush;
	
	hbrush = CreateSolidBrush(block[num].color);
	SelectObject(hdc, hpen);
	SelectObject(hdc, hbrush);

	for (i = 0; i < 4; i++)
		Rectangle(hdc,
		(x + block[num].a[i][0]) * BSIZE,
			(y + block[num].a[i][1]) * BSIZE,
			(x + block[num].a[i][0] + 1) * BSIZE,
			(y + block[num].a[i][1] + 1) * BSIZE);

	DeleteObject(hpen);
	DeleteObject(hbrush);

}

// erase the full screen
void SetFullRow(HDC hdc, HPEN hpen) 
{

	int i, ii, j;
	int org_top = top;
	int flag = 0;
	HBRUSH hbrush;
	SelectObject(hdc, hpen);

	for (i = y; i < y + 4; i++)
	{
		if (i <= 0 || i >= H - 1) continue;//out of bound 

		for (j = 1; j < W - 1; j++)
			if (!board[i][j].var) break;

		if (j == W - 1)
		{
			for (ii = i; ii >= top; ii--)
				for (j = 1; j < W - 1; j++)
					board[ii][j] = board[ii - 1][j];
			top++;
			score = 10;
			flag = 1;

		}

	}

	//redraw the panel
	if (flag) 
	{
		for (i = org_top; i < y + 4; i++)
		{
			if (i <= 0 || i >= H - 1) continue;
			for (j = 1; j < W - 1; j++)
			{
				hbrush = CreateSolidBrush(board[i][j].color);
				SelectObject(hdc, hbrush);
				Rectangle(hdc, j * BSIZE, i * BSIZE, (j + 1) * BSIZE, (i + 1) * BSIZE);
				DeleteObject(hbrush);
			}
		}
		if (level != score / level_step)
			level = score / level_step;
		
		ShowScore(hdc);
		Showlevel(hdc);
	}

	DeleteObject(hpen);

}

//change the panel state
void ChangeVar(void)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		board[y + block[cur_boxnum].a[i][1]][x + block[cur_boxnum].a[i][0]].var = 1;
		board[y + block[cur_boxnum].a[i][1]][x + block[cur_boxnum].a[i][0]].color = block[cur_boxnum].color;
	}

}

BOOL CanMove(void)
{
	int i;
	for (i = 0; i < 4; i++)
		if (board[y + block[cur_boxnum].a[i][1]][x + block[cur_boxnum].a[i][0]].var)
			return FALSE;

	return TRUE;

}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);//Declaration of window procedure function

//entrance function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprevInstance, PSTR szCmdLine, int iCmdShow)
{
	int screenwide, screenhight;
	char AppName[] = "Tetris";

	HWND hwnd;
	MSG msg;
	WNDCLASSEX wndclass;

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;

	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = CreateSolidBrush(BgColor);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = AppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wndclass))
	{
		MessageBeep(0);
		return FALSE;
	}

	screenwide = GetSystemMetrics(SM_CXFULLSCREEN);
	screenhight = GetSystemMetrics(SM_CYFULLSCREEN);

	hwnd = CreateWindow(
		AppName,
		"Tetris",
		WS_OVERLAPPEDWINDOW,
		(screenwide - (W + W1) * BSIZE) / 2,
		(screenhight - H * BSIZE) / 2,
		(W + W1) * BSIZE,
		(H + 1) * BSIZE,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hwnd) return FALSE;

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	MessageBox(hwnd, "tetrirs gogogo! ", "begin", MB_OK);
	SetTimer(hwnd, 1, 500, NULL);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{ //定义窗口过程函数
	int i, j;
	int old_boxnum; //用于保存之前的方块号
	HDC hdc; //定义DC句柄，DC里包含绘图默认的一些属性
	PAINTSTRUCT ps; //定义绘图结构，此结构包含的信息应用程序使用来绘制一个拥有该应用窗口客户区
	HPEN hpen = CreatePen(PS_SOLID, 1, FgColor); //选择画刷，用背景色作画刷
	switch (iMsg) //消息处理的过程
	{
	case WM_CREATE: ////当一个应用程序使用函数CreateWindow或CreateWindowEx来创建一个窗口时，
					//系统将发送该消息给此新建窗口过程。该消息将在创建窗口之后，显示窗口
					//之前发送该消息，该消息将在CreateWindow或CreateWindowEx函数返回之前发送。
		for (i = 1; i < H; i++) //初始化状态，讲两竖边状态初始化为1
			board[i][0].var = board[i][W - 1].var = 1;
		for (j = 1; j < W - 1; j++) //将最底部状态置1
			board[H - 1][j].var = 1;
		for (i = 1; i < H - 1; i++) //将游戏区域状态置0
			for (j = 1; j < W - 1; j++)
			{
				board[i][j].var = 0;
				board[i][j].color = BgColor;
			}
		srand((unsigned)time(NULL)); //初始化随机数发生器
		cur_boxnum = rand() % 19; //赋初值
		next_boxnum = rand() % 19; //赋初值
		x = Cur_x; //初始化方块横坐标
		y = Cur_y; //初始化方块纵坐标
		return 0; //直接退出窗口过程函数
	case WM_PAINT: //绘制界面，当应用程序适用UpdateWindow刷新窗口时，第一次发送该消息
		hdc = BeginPaint(hwnd, &ps); //给DC句柄赋初值
		Paint(hdc, hpen); //调用Paint函数绘制界面
		ShowScore(hdc); //显示分数
		Showlevel(hdc); //显示等级
		ShowHelp(hdc); //显示帮助栏
		ShowBox(hdc, hpen, x, y, cur_boxnum); //显示游戏区域中的游戏方块
		ShowBox(hdc, hpen, W + 1, 1, next_boxnum); //显示右边状态栏的游戏方块
		EndPaint(hwnd, &ps); //EndPaint函数标记指定窗口的绘画过程结束；这个函数在每次调用BeginPaint函数之后被请求，但仅仅在绘画完成以后。
		return 0;
	case WM_TIMER: //定时器消息，每0.5秒接受到一次
		hdc = GetDC(hwnd); //该函数检索一指定窗口的客户区域或整个屏幕的显示设备上下文环境的句柄，以后可以在GDI函数中使用该句柄来在设备上下文环境中绘图。
		y++; //y增1
		if (CanMove()) //如果能移动，则擦除原来位置方块，显示新位置方块，相当于是方块下落
		{
			EraseBox(hdc, hpen, x, y - 1, cur_boxnum);
			ShowBox(hdc, hpen, x, y, cur_boxnum);
		}
		else //如果不能移动，则到底了，y恢复之前的值，并发送MS_NEWBLOCK产生新的方块
		{
			y--;
			SendMessage(hwnd, MS_NEWBLOCK, 0, 0);
		}

		ReleaseDC(hwnd, hdc); //数释放设备上下文环境（DC）供其他应用程序使用
		return 0;
	case WM_KEYDOWN: //当按下键时，会发送该消息
		hdc = GetDC(hwnd);
		switch ((int)wParam) //判断具体按下的键
		{
		case VK_UP:
			old_boxnum = cur_boxnum; //保存当前方块号
			cur_boxnum = block[cur_boxnum].next; //方块号变为下一个方块号
			if (CanMove())
			{
				EraseBox(hdc, hpen, x, y, old_boxnum);
				ShowBox(hdc, hpen, x, y, cur_boxnum);
			}
			else
				cur_boxnum = old_boxnum; //恢复之前的值
			break;
		case VK_DOWN:
			y++;
			if (CanMove())
			{
				EraseBox(hdc, hpen, x, y - 1, cur_boxnum);
				ShowBox(hdc, hpen, x, y, cur_boxnum);
			}
			else
			{
				y--;
				SendMessage(hwnd, MS_NEWBLOCK, 0, 0); //不能下移，就到底了，产生新的方块
			}
			break;
		case VK_LEFT:
			x--; //横坐标减小1
			if (CanMove()) //如果能移动，则下移，不能移动则恢复之前坐标
			{
				EraseBox(hdc, hpen, x + 1, y, cur_boxnum);
				ShowBox(hdc, hpen, x, y, cur_boxnum);
			}
			else
				x++;
			break;
		case VK_RIGHT: //同上
			x++;
			if (CanMove())
			{
				EraseBox(hdc, hpen, x - 1, y, cur_boxnum);
				ShowBox(hdc, hpen, x, y, cur_boxnum);
			}
			else
				x--;
			break;
		}

		ReleaseDC(hwnd, hdc);
		return 0;
	case MS_NEWBLOCK:
		hdc = GetDC(hwnd);
		if (top > y + block[cur_boxnum].a[0][1])
			top = y + block[cur_boxnum].a[0][1]; //确定最高点
		ChangeVar(); //改变游戏主板状态
		SetFullRow(hdc, hpen); //满行处理
		cur_boxnum = next_boxnum;
		x = Cur_x; //重置方块坐标
		y = Cur_y;
		hpen = CreatePen(PS_SOLID, 1, FgColor);
		srand((unsigned)time(NULL)); //初始化随机数发生器
		next_boxnum = rand() % 19;
		EraseBox(hdc, hpen, W + 1, 1, cur_boxnum); //清除右边状态栏的方块
		ShowBox(hdc, hpen, W + 1, 1, next_boxnum); //显示右边状态栏的方块
		ShowBox(hdc, hpen, Cur_x, Cur_y, cur_boxnum); //显示游戏主板顶部方块

		if (!CanMove()) //刚移开始就不能移动，则结束程序退出
		{
			KillTimer(hwnd, 1);
			MessageBox(hwnd, "  退出游戏", "退出", MB_OK);
			PostQuitMessage(0);
		}

		ReleaseDC(hwnd, hdc);
		return 0;
	case WM_DESTROY: //退出游戏
		KillTimer(hwnd, 1);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam); //窗口默认处理，当消息处理函数未处理消息时就调用该函数进行处理
}

