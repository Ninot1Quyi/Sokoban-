#include<stdio.h>
#include<conio.h>
#include<graphics.h>
#include<stdlib.h>

#define RATIO 56			 //数组对应坐标系的单位距离—“一个格子的大小”
#define WIDTH  840			 //窗口宽度
#define HEIGHT 640			 //窗口高度
#define START_X 90			 //初始位置X
#define START_Y 67			 //初始位置Y
#define LINE 9				 //地图行数
#define COLUMN 12			 //地图列数
#define MAN_V RATIO			 //玩家移动速度“一次一格”
#define START_EDIT_X 250	 //编辑器的初始位置X
#define START_EDIT_Y 0		 //编辑器的初始位置Y
#define CLOSE_RATIO 30		 //"close"按钮的大小
#define URDEFINE_X 620		 //“自定义关卡”初始位置X
#define URDEFINE_Y	HEIGHT - (S_hight+ S_hight/2)+5 //“自定义关卡”初始位置Y
#define URDEFINE_W	200		 //“自定义关卡”的宽
#define URDEFINE_H	55		 //“自定义关卡”的高
#define MOVE_TIME 2			 //人物移动一个的时间
#define MOVE_I 3			 //人物一次运动几个像素点
#define MAXBACK 10			 //最多悔步次数
#define PAGE 4				 //地图的页数——总关卡数
#define RBACKMAX 10		     //最多悔步次数
#define BACKMAX (RBACKMAX+1) //修正悔步，以保证首尾不丢失
#pragma warning(disable:6385)



int START(void);	       //生成开始界面
void G_a_map(void);        //生成地图
int man_Pos_num(void);	   //“得到人物的位置对应的数字代码”函数说明
int move(int state);       //人物移动
IMAGE MAN_IMAGE(int, char);//判断应该输出的人物状态
void change(int H, int L); //人物移动后交换数组中的数字
void judge_crate(int H, int L, char dir, IMAGE*);//实现推箱子
int iScore(int state);     //计分
void Read_map(FILE* fp);   //读取地图
int map_addit(void);       //地图编辑器
void G_a_map_addit(void);  //生成编辑地图
int BLOCK_Pos_H(int Sy);   //计算方块处于第几行
int BLOCK_Pos_L(int Sx);   //计算方块处于第几列
void map_addit_file(int);  //储存地图编辑文件
void OutputMove(int);      //显示玩家步数
void RecordMove(int iMoveNum, int map[BACKMAX][LINE][COLUMN]);    //记录玩家移动
void BACKMOVE(int iMoveNum, int mapback[BACKMAX][LINE][COLUMN]);//“悔步”操作
void OutputBack(int iBack);//显示可悔步次数
int JudgeEnd(void);		   //判断箱子是否推到死角


IMAGE image[6], man_image[8];//全局变量，用于储存素材
int map[PAGE][LINE][COLUMN] = { 0 }, Crate_N = 0;//地图数据

int H = 99, L = 99;			//初始化 H，L 使其不在编辑区域之内
int page = 0;				//初始化游戏关卡序号——从第一关开始

struct A//玩家、箱子目的地
{
	int x;
	int y;
	int H;
	int L;
}man, crate_point[100];


//游戏主体
int main()
{
	
	IMAGE bk_img;//游戏背景

	int state = 0;//state = 1 ————“开始游戏”  
				  //state = 2 ————“自定义关卡”
				 

	initgraph(WIDTH, HEIGHT);							//创建窗口
	loadimage(&bk_img, L"bk.jfif", WIDTH, HEIGHT, true);//加载背景
	
	//加载素材资源
	loadimage(&image[0], L"Wall_Beige.png",   RATIO, RATIO, true);//墙_0
	loadimage(&image[1], L"floor.png",        RATIO, RATIO, true);//地板_1
	loadimage(&image[2], L"EndPoint_Red.png", RATIO, RATIO, true);//目的地_2
	loadimage(&image[3], L"start.png",        RATIO, RATIO, true);//人物初始状态_3
	loadimage(&image[4], L"Crate_Red.png",    RATIO, RATIO, true);//红色箱子_4
	loadimage(&image[5], L"bk1.png",          RATIO, RATIO, true);//编辑器背景_5

	//加载人物状态
	loadimage(&man_image[0], L"left1.png",  RATIO, RATIO, true);//左1
	loadimage(&man_image[1], L"left2.png",  RATIO, RATIO, true);//左2
	loadimage(&man_image[2], L"douwn1.png", RATIO, RATIO, true);//下1
	loadimage(&man_image[3], L"douwn2.png", RATIO, RATIO, true);//下2
	loadimage(&man_image[4], L"right1.png", RATIO, RATIO, true);//右1
	loadimage(&man_image[5], L"right2.png", RATIO, RATIO, true);//右2
	loadimage(&man_image[6], L"up1.png",    RATIO, RATIO, true);//上1
	loadimage(&man_image[7], L"up2.png",    RATIO, RATIO, true);//上2

	//游戏主体框架
	while(true)
	{ 
		state = START();		//开始界面
		putimage(0, 0, &bk_img);//游戏背景
		G_a_map();				//生成地图
		move(state);			//人物移动
	}
	return 0;
}




//******************************************
//
//生成开始界面
//
//******************************************
int START(void)
{
	MOUSEMSG m;			   //获取鼠标信息
	IMAGE Start0, Bk_start;//定义“开始游戏”按钮 和 开始背景
	IMAGE Start1;		   //定义当鼠标置于“开始游戏”上方时的按钮图形
	IMAGE ADDIT0, ADDIT1;  //定义“自定义地图”的按钮图形
	IMAGE bk_img;		   //游戏背景
	IMAGE User_define0, User_define1;//“自定义关卡”按钮

	const int  S_wigth = 200, S_hight = 55;									     //“开始游戏”按钮的“长”、“宽”
	const int Sp_x = WIDTH / 4 - 30, Sp_y = HEIGHT - (S_hight + S_hight / 2) + 5;//“开始游戏”的位置（x,y）


	loadimage(&Bk_start, L"bk0.png", WIDTH, HEIGHT, true);    //加载开始背景
	loadimage(&Start0, L"start0.png", S_wigth, S_hight, true);//加载“开始游戏”按钮0
	loadimage(&Start1, L"start1.png", S_wigth, S_hight, true);//加载“开始游戏”按钮1
	loadimage(&ADDIT0, L"addit0.png", S_wigth, S_hight, true);//加载“自定义地图”按钮0
	loadimage(&ADDIT1, L"addit1.png", S_wigth, S_hight, true);//加载“自定义地图”按钮1
	loadimage(&bk_img, L"bk.jfif", WIDTH, HEIGHT, true);//加载背景
	loadimage(&User_define0, L"user_define0.png", URDEFINE_W, URDEFINE_H, true);//加载“自定义关卡”按钮0
	loadimage(&User_define1, L"user_define1.png", URDEFINE_W, URDEFINE_H, true);//加载“自定义关卡”按钮1

	putimage(0, 0, &Bk_start);								//生成开始背景
	putimage(Sp_x, Sp_y, &Start0);						    //生成“开始游戏”按钮
	putimage(Sp_x + S_wigth + 20, Sp_y, &ADDIT0);			//生成“自定义地图”按钮
	putimage(URDEFINE_X, URDEFINE_Y, &User_define0);		//生成“自定义关卡”按钮


	while (true)										    //等待鼠标消息
	{

		m = GetMouseMsg();
		switch (m.uMsg)
		{
		case WM_MOUSEMOVE:  //检测鼠标是否置于“按钮”上
			if (m.x >= Sp_x && m.x <= Sp_x + S_wigth && m.y >= Sp_y && m.y <= Sp_y + S_hight)
			{
				putimage(Sp_x, Sp_y, &Start1);
			}
			else
			{
				putimage(Sp_x, Sp_y, &Start0);
			}
			if (m.x >= Sp_x + S_wigth + 10 && m.x <= Sp_x + S_wigth + 10 + S_wigth && m.y >= Sp_y && m.y <= Sp_y + S_hight)
			{
				putimage(Sp_x + S_wigth + 20, Sp_y, &ADDIT1);
			}
			else
			{
				putimage(Sp_x + S_wigth + 20, Sp_y, &ADDIT0);
			}
			if (m.x >= URDEFINE_X && m.x <= URDEFINE_X + URDEFINE_W && m.y >= URDEFINE_Y && m.y <= URDEFINE_Y + URDEFINE_H)
			{
				putimage(URDEFINE_X, URDEFINE_Y, &User_define1);			//生成“自定义关卡”按钮1
			}
			else
			{
				putimage(URDEFINE_X, URDEFINE_Y, &User_define0);			//生成“自定义关卡”按钮0
			}
			break;
		case WM_LBUTTONUP:	//鼠标左键选中按钮，“左键抬起”后进入
			if (m.x >= Sp_x && m.x <= Sp_x + S_wigth && m.y >= Sp_y && m.y <= Sp_y + S_hight)
			{	//进入“开始游戏”
				Crate_N = 0;
				page = 0;
				FILE* fp = NULL;
				errno_t err;
				err = fopen_s(&fp, "map.txt", "r");
				Read_map(fp);
				return 1;
			}
			if (m.x >= Sp_x + S_wigth + 10 && m.x <= Sp_x + S_wigth + 10 + S_wigth && m.y >= Sp_y && m.y <= Sp_y + S_hight)
			{
				//进入地图编辑模式
				Crate_N = 0;
				map_addit();
				putimage(0, 0, &Bk_start);							//生成开始背景
				putimage(Sp_x, Sp_y, &Start0);						//生成“开始游戏”按钮
				putimage(Sp_x + S_wigth + 20, Sp_y, &ADDIT0);		//生成“自定义地图”按钮
				putimage(URDEFINE_X, URDEFINE_Y, &User_define0);	//生成“自定义关卡”按钮
				break;
			}
			if (m.x >= URDEFINE_X && m.x <= URDEFINE_X + URDEFINE_W && m.y >= URDEFINE_Y && m.y <= URDEFINE_Y + URDEFINE_H)
			{	//进入“自定义关卡”
				Crate_N = 0;
				page = 0;
				FILE* fp = NULL;
				errno_t err;
				err = fopen_s(&fp, "map_addit.txt", "r");
				Read_map(fp);
				return 2;
			}
		}
	}
}






//******************************************
//
//读取地图
//
//******************************************
void Read_map(FILE* fp)
{
	rewind(fp);
	fseek(fp, 234L * page, SEEK_SET);
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 12; j++)
		{
			fscanf_s(fp, "%d,", &map[page][i][j]);
		}
	}
	fclose(fp);
}






//******************************************
//
//生成地图
//
//******************************************
void G_a_map(void)
{

	int iC = 0;
	for (int i = 0; i < LINE; i++)
	{

		for (int j = 0; j < COLUMN; j++)
		{
			putimage(START_X + j * RATIO, START_Y + i * RATIO, &image[map[page][i][j]]);
			//找到玩家的坐标
			if (map[page][i][j] == 3)
			{
				man.x = START_X + j * RATIO;
				man.y = START_Y + i * RATIO;
			}
			//找到“箱子目的地”的坐标？？？
			if (map[page][i][j] == 2)
			{
				crate_point[iC].H = i;
				crate_point[iC].L = j;
				map[page][i][j] = 1;
				Crate_N++;
				iC++;
			}
			if (map[page][i][j] == 5)
			{
				putimage(START_X + j * RATIO, START_Y + i * RATIO, &image[1]);
			}

		}
	}
	while (iC < 100 - 1)
	{
		crate_point[iC].H = 0;
		crate_point[iC].L = 0;
		iC++;
	}
}






//******************************************
//
//人物移动
//
//******************************************
int move(int state)
{
	int iMoveNum = 0, ipage = 0, iBack = BACKMAX;   //iMvoeNum 玩家移动步数
													//ipage    当前关卡数
													//iBack    可悔步次数
	int mapback[BACKMAX][LINE][COLUMN] = { 0 };		//游戏时储存地图数据
	int j = 0;								        //用以判断人物移动时的状态的数据
	IMAGE Man, END;
	while (1)
	{
		char dir = '\0';

		if (iScore(state) == 1)//玩家得分
		{


			loadimage(&END, L"victory.jpg", WIDTH, HEIGHT, true);
			putimage(0, 0, &END);
			Sleep(5000);
			return 0;
		}
		if (ipage != page)
		{
			iMoveNum = 0;
			ipage = page;
		}
		int H, L;
		H = BLOCK_Pos_H(man.y);		  //当前位置对应的数组位置“行”
		L = BLOCK_Pos_L(man.x);		  //当前位置对应的数组位置“列”
		OutputMove(iMoveNum);		  //显示玩家步数
		OutputBack(iBack);			  //显示玩家可悔步次数
		RecordMove(iMoveNum, mapback);//记录玩家移动
		dir = _getch();				  //获取键盘按键值

		Man = MAN_IMAGE(j, dir);	  //判断应该输出的人物状态
		switch (dir)
		{
		case 72://向上↑
		case 'w':
		case 'W':
			j++;//改变人物运动状态
			man.y -= MAN_V;
			if (man_Pos_num() == 0)
			{
				man.y += MAN_V;
				putimage(man.x, man.y, &Man);
			}
			else if (man_Pos_num() == 4 && (map[page][(man.y - RATIO - START_Y) / RATIO][(man.x - START_X) / RATIO] == 0
				|| map[page][(man.y - RATIO - START_Y) / RATIO][(man.x - START_X) / RATIO] == 4))
			{

				man.y += MAN_V;
				putimage(man.x, man.y, &Man);
			}
			else
			{


				for (int i = 0; i <= MAN_V; i += MOVE_I)
				{
					j++;
					Man = MAN_IMAGE(j, dir);//判断应该输出的人物状态
					if (man_Pos_num() == 1 || man_Pos_num() == 5)
					{
						putimage(START_X + RATIO * L, START_Y + RATIO * (H), &image[1]);
						putimage(man.x, man.y + RATIO - i, &Man);
					}
					if (man_Pos_num() == 4)
					{
						putimage(START_X + RATIO * L, START_Y + RATIO * (H), &image[1]);
						putimage(man.x, man.y - i, &image[4]);
						putimage(man.x, man.y + RATIO - i, &Man);
					}

					Sleep(MOVE_TIME);
				}
				judge_crate(H, L, dir, &image[4]);
				change(H, L);
				iMoveNum++;//记录玩家步数
			}
			break;
		case 80://向下↓
		case 's':
		case 'S':
			j++;//改变人物运动状态
			man.y += MAN_V;
			if (man_Pos_num() == 0)
			{
				man.y -= MAN_V;
				putimage(man.x, man.y, &Man);
			}
			else if (man_Pos_num() == 4 && (map[page][(man.y + RATIO - START_Y) / RATIO][(man.x - START_X) / RATIO] == 0
				|| map[page][(man.y + RATIO - START_Y) / RATIO][(man.x - START_X) / RATIO] == 4))
			{

				man.y -= MAN_V;
				putimage(man.x, man.y, &Man);
			}
			else
			{
				for (int i = 0; i <= MAN_V; i += MOVE_I)
				{
					j++;
					Man = MAN_IMAGE(j, dir);//判断应该输出的人物状态


					if (man_Pos_num() == 1 || man_Pos_num() == 5)
					{
						putimage(START_X + RATIO * L, START_Y + RATIO * (H), &image[1]);
						putimage(man.x, man.y - RATIO + i, &Man);
					}
					if (man_Pos_num() == 4)
					{
						putimage(START_X + RATIO * L, START_Y + RATIO * (H), &image[1]);
						putimage(man.x, man.y + i, &image[4]);
						putimage(man.x, man.y - RATIO + i, &Man);
					}


					Sleep(MOVE_TIME);
				}
				putimage(man.x, man.y - MAN_V, &image[1]);
				judge_crate(H, L, dir, &image[4]);
				change(H, L);
				iMoveNum++;//记录玩家步数
			}

			break;
		case 75://向左←
		case 'a':
		case 'A':
			j++;//改变人物运动状态
			man.x -= MAN_V;
			if (man_Pos_num() == 0)
			{
				man.x += MAN_V;
				putimage(man.x, man.y, &Man);
			}
			else if (man_Pos_num() == 4 && (map[page][(man.y - START_Y) / RATIO][(man.x - RATIO - START_X) / RATIO] == 0
				|| map[page][(man.y - START_Y) / RATIO][(man.x - RATIO - START_X) / RATIO] == 4))
			{

				man.x += MAN_V;
				putimage(man.x, man.y, &Man);
			}
			else
			{
				for (int i = 0; i <= MAN_V; i += MOVE_I)
				{
					j++;
					Man = MAN_IMAGE(j, dir);//判断应该输出的人物状态


					if (man_Pos_num() == 1 || man_Pos_num() == 5)
					{
						putimage(START_X + RATIO * L, START_Y + RATIO * (H), &image[1]);
						putimage(man.x + RATIO - i, man.y, &Man);
					}
					if (man_Pos_num() == 4)
					{
						putimage(START_X + RATIO * L, START_Y + RATIO * (H), &image[1]);
						putimage(man.x - i, man.y, &image[4]);
						putimage(man.x + RATIO - i, man.y, &Man);
					}
					Sleep(MOVE_TIME);
				}
				putimage(man.x + MAN_V, man.y, &image[1]);
				judge_crate(H, L, dir, &image[4]);
				change(H, L);
				iMoveNum++;//记录玩家步数
			}
			break;
		case 77://向右→
		case 'd':
		case 'D':
			j++;//改变人物运动状态
			man.x += MAN_V;
			if (man_Pos_num() == 0)
			{
				man.x -= MAN_V;
				putimage(man.x, man.y, &Man);
			}
			else if (man_Pos_num() == 4 && (map[page][(man.y - START_Y) / RATIO][(man.x + RATIO - START_X) / RATIO] == 0
				|| map[page][(man.y - START_Y) / RATIO][(man.x + RATIO - START_X) / RATIO] == 4))
			{

				man.x -= MAN_V;
				putimage(man.x, man.y, &Man);
			}
			else
			{
				for (int i = 0; i <= MAN_V; i += MOVE_I)
				{
					j++;
					Man = MAN_IMAGE(j, dir);//判断应该输出的人物状态
					if (man_Pos_num() == 1 || man_Pos_num() == 5)
					{
						putimage(START_X + RATIO * L, START_Y + RATIO * (H), &image[1]);
						putimage(man.x - RATIO + i, man.y, &Man);
					}

					if (man_Pos_num() == 4)
					{
						putimage(START_X + RATIO * L, START_Y + RATIO * (H), &image[1]);
						putimage(man.x + i, man.y, &image[4]);
						putimage(man.x - RATIO + i, man.y, &Man);
					}

					Sleep(MOVE_TIME);
				}
				putimage(man.x - MAN_V, man.y, &image[1]);
				judge_crate(H, L, dir, &image[4]);
				change(H, L);
				iMoveNum++; //记录玩家步数
			}
			break;
		case 'q':
		case 'Q':
			if (page > 0)
			{
				iMoveNum = 0;//“玩家步数”清零
				Crate_N = 0;
				page--;
				FILE* fp = NULL;
				errno_t err;
				if (state == 1)
				{
					err = fopen_s(&fp, "map.txt", "r");
				}
				else if (state == 2)
				{
					err = fopen_s(&fp, "map_addit.txt", "r");
				}
				Read_map(fp);
				G_a_map();   //生成地图
			}
			break;
		case 'e':
		case 'E':
			if (page < PAGE - 1)
			{
				iMoveNum = 0;//“玩家步数”清零
				Crate_N = 0;
				page++;
				FILE* fp = NULL;
				errno_t err;
				if (state == 1)
				{
					err = fopen_s(&fp, "map.txt", "r");
				}
				else if (state == 2)
				{
					err = fopen_s(&fp, "map_addit.txt", "r");
				}
				Read_map(fp);
				G_a_map();   //生成地图
			}
			break;
		case 'b':
		case 'B':
			if (iMoveNum > 0)
			{
				if (iBack == 1)
				{
					break;
				}
				iBack--;

				iMoveNum--;
				BACKMOVE(iMoveNum, mapback);//“悔步”操作

				for (int i = 0; i < LINE; i++)
				{

					for (int j = 0; j < COLUMN; j++)
					{

						putimage(START_X + j * RATIO, START_Y + i * RATIO, &image[map[page][i][j]]);
						//找到玩家的坐标
						if (map[page][i][j] == 3)
						{
							man.x = START_X + j * RATIO;
							man.y = START_Y + i * RATIO;
						}
						if (map[page][i][j] == 5)
						{
							putimage(START_X + j * RATIO, START_Y + i * RATIO, &image[1]);
						}

					}
				}
			}

			break;
		case 27:	 //退出游戏ESC
			return 0;//返回开始界面
			break;
		}
		if ((JudgeEnd() == 1)&& (iBack == 1))//箱子推到死角游戏结束
		{
			Sleep(2000);
			loadimage(&END, L"defeat.jpg", WIDTH, HEIGHT, true);
			putimage(0, 0, &END);
			Sleep(5000);
			return 0;
		}

	}
}





//******************************************
//
//记录玩家移动
//
//******************************************
void RecordMove(int iMoveNum, int mapback[BACKMAX][LINE][COLUMN])
{
	int i = 0, j = 0;
	for (i = 0; i < LINE; i++)
	{
		for (j = 0; j < COLUMN; j++)
		{
			mapback[iMoveNum % BACKMAX][i][j] = map[page][i][j];
		}
	}
}






//******************************************
//
//“悔步”操作
//
//******************************************
void BACKMOVE(int iMoveNum, int mapback[BACKMAX][LINE][COLUMN])
{
	int i = 0, j = 0;
	for (i = 0; i < LINE; i++)
	{
		for (j = 0; j < COLUMN; j++)
		{
			map[page][i][j] = mapback[iMoveNum % BACKMAX][i][j];
		}
	}
}


//******************************************
//
//显示可悔步次数
//
//******************************************
void OutputBack(int iBack)
{
	outtextxy(0, 0, _T("可悔步"));
	TCHAR c[10] = { _T('0'), _T('1'),_T('2'),_T('3'),_T('4'),_T('5'),_T('6'),_T('7'),_T('8'),_T('9') };
	if (iBack == BACKMAX)
	{
		outtextxy(70, 0, c[1]);
		outtextxy(80, 0, c[0]);
	}
	else
	{
		outtextxy(70, 0, c[0]);
		outtextxy(80, 0, c[iBack - 1]);
	}

}



//******************************************
//
//判断箱子是否推到了死角
//
//******************************************
int JudgeEnd(void)
{
	int i = 0, j = 0, ic = 0, jc = 0;
	int iflag = 0;//箱子无法移动为“1”，箱子可以移动为“0”
	for (i = 0; i < LINE; i++)
	{
		for (j = 0; j < COLUMN; j++)
		{
			if (map[page][i][j] == 4)
			{
				int end = 1;  //用以判断是否结束的标志
				for (int iC = 0; iC < Crate_N; iC++)
				{
					if (crate_point[iC].H == i && crate_point[iC].L == j)
					{
						end = 0;
					}
				}
				if (end == 1)
				{
					//****************推到墙角**********************
					if (map[page][i - 1][j] == 0 && map[page][i][j - 1] == 0)//上0 + 左0
					{
						iflag = 1;
					}
					if (map[page][i - 1][j] == 0 && map[page][i][j + 1] == 0)//上0 + 右0
					{
						iflag = 1;
					}
					if (map[page][i + 1][j] == 0 && map[page][i][j - 1] == 0)//下0 + 左0
					{
						iflag = 1;
					}
					if (map[page][i + 1][j] == 0 && map[page][i][j + 1] == 0)//下0 + 右0
					{
						iflag = 1;
					}
					//****************箱子并排**********************
					if (map[page][i - 1][j] == 4 && map[page][i][j - 1] == 0 && map[page][i - 1][j - 1] == 0)//上4 + 左0 + 左上0 
					{
						iflag = 1;
					}
					if (map[page][i - 1][j] == 4 && map[page][i][j + 1] == 0 && map[page][i + 1][j + 1] == 0)//上4 + 右0 + 右上0
					{
						iflag = 1;
					}
					if (map[page][i + 1][j] == 4 && map[page][i][j - 1] == 0 && map[page][i + 1][j - 1] == 0)//下4 + 左0 + 左下0
					{
						iflag = 1;
					}
					if (map[page][i + 1][j] == 4 && map[page][i][j + 1] == 0 && map[page][i + 1][j + 1] == 0)//下4 + 右0 + 右下0
					{
						iflag = 1;
					}
				}
			}
		}
	}
	return iflag;
}






//******************************************
//
//显示玩家步数
//
//******************************************
void OutputMove(int iMoveNum)
{
	outtextxy(WIDTH - 80, 0, _T("步数"));
	TCHAR c[10] = { _T('0'), _T('1'),_T('2'),_T('3'),_T('4'),_T('5'),_T('6'),_T('7'),_T('8'),_T('9') };
	if (iMoveNum / 10 == 0)
	{

		outtextxy(WIDTH - 20, 0, c[iMoveNum]);
		outtextxy(WIDTH - 30, 0, c[0]);
		outtextxy(WIDTH - 40, 0, c[0]);
	}
	else if (iMoveNum / 100 == 0 && iMoveNum / 10 != 0)
	{
		outtextxy(WIDTH - 20, 0, c[iMoveNum % 10]);
		outtextxy(WIDTH - 30, 0, c[iMoveNum / 10]);
		outtextxy(WIDTH - 40, 0, c[0]);
	}
	else if (iMoveNum / 1000 == 0 && iMoveNum / 100 != 0)
	{
		outtextxy(WIDTH - 20, 0, c[iMoveNum % 10]);
		outtextxy(WIDTH - 30, 0, c[iMoveNum / 10 % 10]);
		outtextxy(WIDTH - 40, 0, c[iMoveNum / 100]);
	}
}






//******************************************
//
//实现推箱子
//
//******************************************
void judge_crate(int H, int L, char dir, IMAGE* image_crate)
{
	if (map[page][BLOCK_Pos_H(man.y)][BLOCK_Pos_L(man.x)] == 4)//玩家遇到了箱子
	{
		int ch2;
		struct	  //箱子的“数组坐标”&“直接坐标”
		{
			int H;//箱子的“行”
			int L;//箱子的“列”
			int x;//箱子的“x”
			int y;//箱子的“y”
		}crate;
		crate.H = BLOCK_Pos_H(man.y);
		crate.L = BLOCK_Pos_L(man.x);
		crate.x = man.x;
		crate.y = man.y;
		if (dir == 72 || dir == 'w' || dir == 'W')//向上↑
		{
			if (map[page][crate.H - 1][crate.L] == 0 || map[page][crate.H - 1][crate.L] == 4)
			{
				man.y += MAN_V;
			}
			else
			{
				ch2 = map[page][crate.H - 1][crate.L];
				map[page][crate.H - 1][crate.L] = 4;
				map[page][crate.H][crate.L] = ch2;
			}
		}
		if (dir == 80 || dir == 's' || dir == 'S')//向下↓
		{
			if (map[page][crate.H + 1][crate.L] == 0 || map[page][crate.H + 1][crate.L] == 4)
			{
				man.y -= MAN_V;
			}
			else
			{

				ch2 = map[page][crate.H + 1][crate.L];
				map[page][crate.H + 1][crate.L] = 4;
				map[page][crate.H][crate.L] = ch2;

			}
		}
		if (dir == 75 || dir == 'a' || dir == 'A')//向左←
		{
			if (map[page][crate.H][crate.L - 1] == 0 || map[page][crate.H][crate.L - 1] == 4)
			{
				man.x += MAN_V;
			}
			else
			{

				ch2 = map[page][crate.H][crate.L - 1];
				map[page][crate.H][crate.L - 1] = 4;
				map[page][crate.H][crate.L] = ch2;

			}
		}
		if (dir == 77 || dir == 'd' || dir == 'D')//向右→
		{
			if (map[page][crate.H][crate.L + 1] == 0 || map[page][crate.H][crate.L + 1] == 4)
			{
				man.x -= MAN_V;
			}
			else
			{
				ch2 = map[page][crate.H][crate.L + 1];
				map[page][crate.H][crate.L + 1] = 4;
				map[page][crate.H][crate.L] = ch2;
			}
		}
	}
}






//******************************************
//
//人物分数
//
//******************************************
int iScore(int state)
{
	int i = 0, j = 0;
	int score = 0;//玩家分数
	score = 0;
	for (i = 0; i < Crate_N; i++)
	{
		if (map[page][crate_point[i].H][crate_point[i].L] == 4)
		{

			score++;
		}
		if (map[page][crate_point[i].H][crate_point[i].L] != 4 && map[page][crate_point[i].H][crate_point[i].L] != 3)
		{
			putimage(START_X + crate_point[i].L * RATIO, START_Y + crate_point[i].H * RATIO, &image[2]);
		}

	}
	if (score == Crate_N)
	{

		Sleep(1000);
		Crate_N = 0;

		page++;
		if (page == PAGE)
		{


			return 1;
		}
		FILE* fp = NULL;
		errno_t err;
		if (state == 1)
		{
			err = fopen_s(&fp, "map.txt", "r");
		}
		else if (state == 2)
		{
			err = fopen_s(&fp, "map_addit.txt", "r");
		}
		Read_map(fp);
		G_a_map();//生成地图
		return 0;
	}
	return 0;
}






//******************************************
//
//判断应该输出的人物状态
//
//******************************************
IMAGE MAN_IMAGE(int j, char dir)
{
	IMAGE	Man;
	j %= 10;
	if (dir == 72 || dir == 'w' || dir == 'W')//向上移动
	{
		if (j % 2 == 1)
		{
			Man = man_image[6];//上1
		}
		else
		{
			Man = man_image[7];//上2
		}
	}
	if (dir == 80 || dir == 's' || dir == 'S')//向下移动
	{
		if (j % 2 == 1)
		{
			Man = man_image[2];//下1
		}
		else
		{
			Man = man_image[3];//下2
		}
	}
	if (dir == 75 || dir == 'a' || dir == 'A')//向左移动
	{
		if (j % 2 == 1)
		{
			Man = man_image[0];//左1
		}
		else
		{
			Man = man_image[1];//左2
		}
	}
	if (dir == 77 || dir == 'd' || dir == 'D')//向右移动
	{
		if (j % 2 == 1)
		{
			Man = man_image[4];//右1
		}
		else
		{
			Man = man_image[5];//右2
		}
	}
	return Man;
}





//******************************************
//
//地图编辑器
//
//******************************************
int map_addit(void)
{

	MOUSEMSG m, n;		  //鼠标消息
	IMAGE save0, save1, BK, BK1;
	IMAGE close_image;	  //返回按钮“←”
	IMAGE image_2[6];     //鼠标选中时的方块状态
	int Sx = START_EDIT_X;//初始化编辑器的开始位置X
	int Sy = START_EDIT_Y;//初始化编辑器的开始位置Y
	wchar_t s[10];		  //从对话框中接收用户输入
	int iAdditPage = 0;   //默认编辑关卡为“第一关”
	struct {
		int x;
		int y;
		int num;
	}S_block[6];		  //编辑器中6个方块的坐标

	loadimage(&BK1, L"bk1.png", RATIO, RATIO);//地图的底色
	loadimage(&BK, L"bk.jfif", WIDTH, HEIGHT);//编辑器背景
	loadimage(&save0, L"save0.jfif", 155, 57);//保存按钮save0
	loadimage(&save1, L"save1.jfif", 155, 57);//保存按钮save1
	loadimage(&close_image, L"close.png", CLOSE_RATIO, CLOSE_RATIO);//加载返回按钮“←”
	loadimage(&image_2[0], L"Wall_Beige_2.png", RATIO, RATIO, true);//墙_0选中状态
	loadimage(&image_2[1], L"floor_2.png", RATIO, RATIO, true);//地板_1选中状态
	loadimage(&image_2[2], L"EndPoint_Red_2.png", RATIO, RATIO, true);//目的地_2选中状态
	loadimage(&image_2[3], L"start_2.png", RATIO, RATIO, true);//人物初始状态_3选中状态
	loadimage(&image_2[4], L"Crate_Red_2.png", RATIO, RATIO, true);//红色箱子_4选中状态
	loadimage(&image_2[5], L"bk1_2.png", RATIO, RATIO, true);//编辑器背景_5选中状态

	putimage(0, 0, &BK);		 //输出背景
	putimage(0, 0, &close_image);//输出关闭按钮
	putimage(WIDTH - 155, 2, &save0);

	for (int i = 0; i < LINE; i++)
	{
		for (int j = 0; j < COLUMN; j++)
		{
			map[page][i][j] = 5;
		}
	}
	for (int i = 0; i < 6; i++)//初始化编辑器中各个方块的“坐标”和“数字”
	{
		S_block[i].x = START_EDIT_X + i * RATIO;
		S_block[i].y = START_EDIT_Y;
		S_block[i].num = i;
	}
	for (int i = 0; i < 6; i++)//输出编辑栏
	{
		putimage(S_block[i].x, S_block[i].y, &image[i]);
	}

	//***************************对话框************************
	InputBox(s, 10, L"请输入需要编辑的“自定义游戏”关卡（1-4）");//
	iAdditPage = _wtoi(s) - 1;							     //
	//***************************对话框************************

	G_a_map_addit();//生成地图
	while (true)
	{
		IMAGE ch_image;				//储存需要编辑的对象
		int sx = 0, sy = 0, num = 0;//初始化编辑对象的 位置 和 数值
		m = GetMouseMsg();		    //获取鼠标信息

		switch (m.uMsg)
		{
		case WM_LBUTTONDOWN://左键单击“关闭”||“保存”按钮
			if (m.x >= 0 && m.x <= CLOSE_RATIO && m.y >= 0 && m.y <= CLOSE_RATIO)//“关闭”按钮
			{
				return 0;
			}
			if (m.x >= WIDTH - 155 && m.x <= WIDTH && m.y >= 0 && m.y <= 57)	 //“保存”按钮
			{

				map_addit_file(iAdditPage);
				Sleep(100);
				return 0;
			}
			break;
		case WM_MOUSEMOVE://鼠标移动信息
			if (m.x >= WIDTH - 155 && m.x <= WIDTH && m.y >= 0 && m.y <= 57)	 //保存按钮
			{
				putimage(WIDTH - 155, 2, &save1);
			}
			else
			{
				putimage(WIDTH - 155, 2, &save0);
			}
			for (int i = 0; i < 6; i++)//输出鼠标选中的方块状态
			{
				if (m.x > S_block[i].x && m.x < S_block[i].x + RATIO && m.y > S_block[i].y && m.y < S_block[i].y + RATIO)
				{
					putimage(S_block[i].x, S_block[i].y, &image_2[i]);
					break;
				}
				else
				{
					putimage(S_block[i].x, S_block[i].y, &image[i]);
				}
			}
			for (int i = 6; i >= 0; i--)//输出鼠标选中的方块状态
			{
				if (m.x > S_block[i].x && m.x < S_block[i].x + RATIO && m.y > S_block[i].y && m.y < S_block[i].y + RATIO)
				{
					putimage(S_block[i].x, S_block[i].y, &image_2[i]);
					break;
				}
				else
				{
					putimage(S_block[i].x, S_block[i].y, &image[i]);
				}
			}
			break;
		case WM_RBUTTONDOWN://右键下落
		case WM_RBUTTONUP:

			for (int i = 0; i < 6; i++)//识别鼠标点击的对象
			{
				if (m.x > S_block[i].x && m.x < S_block[i].x + RATIO && m.y > S_block[i].y && m.y < S_block[i].y + RATIO)
				{

					ch_image = image[i];
					num = S_block[i].num;
					Sx = S_block[i].x;
					Sy = S_block[i].y;
					break;
				}
			}
			if (m.x > Sx && m.x < Sx + RATIO && m.y > Sy && m.y < Sy + RATIO)//识别鼠标是否在“编辑框”之内
			{
				int sx = 0, sy = 0;//初始化运动时方块的位置
				while (true)
				{
					int _switch_ = 0;
					n = GetMouseMsg();
					switch (n.uMsg)
					{
					case WM_LBUTTONDOWN://左键单击退出地图编辑close
						if (n.x >= 0 && n.x <= CLOSE_RATIO && n.y >= 0 && n.y <= CLOSE_RATIO)
						{
							return 0;
						}
						break;
					case WM_RBUTTONDOWN://右键下落清除鼠标选中的方块
						_switch_ = 1;
						break;
					case WM_MOUSEMOVE:
						G_a_map_addit();//时刻更新编辑地图
						//判断鼠标是否在编辑区域之内
						if (n.x >= START_X + RATIO && n.x <= START_X + RATIO * (COLUMN - 1) && n.y >= START_Y + RATIO && n.y <= START_Y + RATIO * (LINE - 1))
						{

							L = (int)BLOCK_Pos_H(n.x - RATIO / 2 + 4);//矫正L
							H = (int)BLOCK_Pos_L(n.y + RATIO / 2 - 7);//矫正H
							sx = START_X + L * RATIO;//运动时方块的位置X
							sy = START_Y + H * RATIO;//运动时方块的位置Y
							putimage(sx, sy, &ch_image);

						}
						continue;
					}
					if (_switch_ == 1)
					{
						if (n.x >= START_X + RATIO && n.x <= START_X + RATIO * (COLUMN - 1) && n.y >= START_Y + RATIO && n.y <= START_Y + RATIO * (LINE - 1))
						{
							putimage(sx, sy, &image[map[page][H][L]]);

						}
						break;
					}
					if (H < LINE && H >= 0 && L < COLUMN && L >= 0 && n.x >= START_X && n.x < START_X + RATIO * COLUMN && n.y >= START_Y && n.y < START_Y + RATIO * LINE)
					{
						map[page][H][L] = num;
					}
				}
			}
			break;
		}

	}

}





//******************************************
//
//生成编辑地图
//
//******************************************
void G_a_map_addit(void)
{
	int iC = 0;
	IMAGE BK1;//地图的底色
	loadimage(&BK1, L"bk1.png", RATIO, RATIO);
	for (int i = 0; i < LINE; i++)
	{
		for (int j = 0; j < COLUMN; j++)
		{

			if (!(i > 0 && i < LINE - 1 && j > 0 && j < COLUMN - 1))
			{
				map[page][i][j] = 0;
			}

		}
	}
	for (int i = 0; i < LINE; i++)
	{

		for (int j = 0; j < COLUMN; j++)
		{
			if (map[page][i][j] == 5)
			{
				putimage(START_X + j * RATIO, START_Y + i * RATIO, &BK1);
			}

			putimage(START_X + j * RATIO, START_Y + i * RATIO, &image[map[page][i][j]]);
		}
	}
}





//******************************************
//
// 储存地图编辑文件
//
//******************************************
void map_addit_file(int iAdditPage)
{
	errno_t err;
	FILE* fp = NULL;
	err = fopen_s(&fp, "map_addit.txt", "r+");
	if (fp)
	{
		fseek(fp, 234L * iAdditPage, SEEK_SET);
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 12; j++)
			{
				fprintf(fp, "%d,", map[page][i][j]);
			}
			fprintf(fp, "\n");
		}
		fclose(fp);
	}
}






//******************************************
//
//计算玩家的位置对应的数字代码 
//
//******************************************
int man_Pos_num(void)
{
	return (map[page][(man.y - START_Y) / RATIO][(man.x - START_X) / RATIO]);
}





//******************************************
//
//人物移动后交换数组中的数字
//
//******************************************
void change(int H, int L)
{
	int ch;
	ch = map[page][H][L];
	map[page][H][L] = map[page][BLOCK_Pos_H(man.y)][BLOCK_Pos_L(man.x)];
	map[page][BLOCK_Pos_H(man.y)][BLOCK_Pos_L(man.x)] = ch;
}






//******************************************
//
//计算方块处于第几列
//
//******************************************
int BLOCK_Pos_L(int Sx)
{
	int L = (int)(Sx - START_X) / RATIO;//列
	return L;
}





//******************************************
//
//计算方块处于第几行
//
//******************************************
int BLOCK_Pos_H(int Sy)
{
	int H = (int)(Sy - START_Y) / RATIO;//行
	return H;
}
