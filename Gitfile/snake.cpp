#define _CRT_SECURE_NO_WARNINGS 1
#include<stdio.h>			
#include<time.h>			
#include<windows.h>		
#include<stdlib.h>			
#include<conio.h>	      //接受键盘输入 

#define U 1               //蛇的上下左右
#define D 2
#define L 3
#define R 4           


/*********定义全局变量 *******/ 
typedef struct listnode                   //定义蛇链表节点 
{
	int x;
	int y;
	struct listnode*next;
}snake;

int score=0,add=10;			//总得分与每次吃食物得分
int status,sleeptime=200;	//蛇前进状态，每次运行的时间间隔
snake *head, *food;			//蛇头指针，食物指针
snake *q;					//遍历蛇的时候用到的指针
int endgamestatus=0;		//游戏结束的情况，1：撞到墙；2：咬到自己；3：主动退出游戏。
HANDLE hOut;				//控制台句柄


/***********函数声明************/ 
void gotoxy(int x,int y);   //设置光标位置
void createMap();           //绘制地图
void initializesnake();    //初始化蛇身，画蛇身
void createfood();          //创建并随机出现食物
int biteself();             //判断是否咬到了自己
void cantcrosswall();       //设置蛇撞墙的情况
void snakemove();           //控制蛇前进方向
void speedup();				//加速
void speeddown();			//减速
void keyboardControl();     //控制键盘按键
void endgame();             //游戏结束



/*********游戏的静态初始化***********/ 
void gotoxy(int x,int y)		//设置光标位置												 
{
	COORD pos;
	pos.X=x;
	pos.Y=y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
} 

void createmap()                 //打印出地图 
{
	gotoxy(68,8);
	printf("□为墙，撞到就gg"); 
	gotoxy(68,9);
	printf("●为果实，吃到身长++");
	int i,j;
	for(i=0;i<62;i+=2)           //打印上下网格 
	{
		gotoxy(i,0);
		printf("□");
		gotoxy(i,26);
		printf("□");
	}
	for(j=1;j<26;j++)			 //打印左右网格 
	{
		gotoxy(0,j);
		printf("□");
		gotoxy(59,j);
		printf("□");
	}

}

//创造一个静态的蛇 
void initializesnake()
{
    snake *tail;
    int i;
    tail=(snake*)malloc(sizeof(snake));     //从蛇尾开始，头插法，以x,y设定开始的位置
    tail->x=24;         	//蛇的初始位置（24,5）
    tail->y=5;
    tail->next=NULL;
    for(i=1;i<=4;i++)       //设置蛇身，长度为5
    {
        head=(snake*)malloc(sizeof(snake)); //初始化蛇头
        head->next=tail;        //蛇头的下一位为蛇尾
        head->x=24+2*i;         //设置蛇头位置
        head->y=5;
        tail=head;              //蛇头变成蛇尾，然后重复循环
    }
    while(tail!=NULL)		//从头到尾，输出蛇身
    {
        gotoxy(tail->x,tail->y);
        printf("★");       //输出蛇身，蛇身使用★组成
        tail=tail->next;    //蛇头输出完毕，输出蛇头的下一位，一直输出到蛇尾
    }
}

//随机生成一个食物
void createfood()
{
    snake *food_1;
    srand((unsigned)time(NULL));        	//初始化随机数
    food_1=(snake*)malloc(sizeof(snake));   
    food_1->x=8%52+2; food_1->y=20%24+1;
    while((food_1->x%2)!=0)    				//保证其为偶数，使得食物能与蛇头对其，然后食物会出现在网格线上
    {
        food_1->x=rand()%52+2;              //食物随机出现
    }
    food_1->y=rand()%24+1;
    q=head;
    while(q->next==NULL)
    {
        if(q->x==food_1->x && q->y==food_1->y) //判断蛇身是否与食物重合
        {
            free(food_1);               //如果蛇身和食物重合，那么释放食物指针
            createfood();               //重新创建食物
        }
        q=q->next;
    }
    gotoxy(food_1->x,food_1->y);
    food=food_1;
    printf("●");           //输出食物
}

/*********以下为动态逻辑的编写 *******/

void keyboardcontrol()                                   //实现键盘控制 
{
	status = R;
	while(1)
    {
	if(GetAsyncKeyState(VK_UP) && status!=D)             //GetAsyncKeyState函数用来判断函数调用时指定虚拟键的状态
    {
        status=U;                                        //如果蛇不是向下前进的时候，按上键，执行向上前进操作
    }
        else if(GetAsyncKeyState(VK_DOWN) && status!=U)  //如果蛇不是向上前进的时候，按下键，执行向下前进操作
    {
        status=D;
    }
	else if(GetAsyncKeyState(VK_LEFT)&& status!=R)       //如果蛇不是向右前进的时候，按左键，执行向左前进
	{
        status=L;
    }
    else if(GetAsyncKeyState(VK_RIGHT)&& status!=L)      //如果蛇不是向左前进的时候，按右键，执行向右前进
    {
       status=R;
    }
    Sleep(sleeptime);
    snakemove();
	}		 
}

void snakemove()	//蛇前进,上U,下D,左L,右R
{
	snake * nexthead;
    cantcrosswall();
    nexthead=(snake*)malloc(sizeof(snake));		//为下一步开辟空间
    if(status==U)
    {
        nexthead->x=head->x;             //向上前进时，x坐标不动，y坐标-1
        nexthead->y=head->y-1;
        nexthead->next=head;
        head=nexthead;
        q=head;                          //指针q指向蛇头
        if(nexthead->x==food->x && nexthead->y==food->y)	   //如果下一个有食物 下一个位置的坐标和食物的坐标相同
        {
            while(q!=NULL)
            {
                gotoxy(q->x,q->y);
                printf("★");       //原来食物的位置，从●换成★
                q=q->next;          //指针q指向的蛇身的下一位也执行循环里的操作
            }
            score=score+add;        //吃了一个食物，在总分上加上食物的分
			speedup();
            createfood();           //创建食物
        }
        else                        
        {
            while(q->next->next!=NULL)	//如果没遇到食物
            {
                gotoxy(q->x,q->y);
                printf("★");           //蛇正常往前走，输出当前位置的蛇身
                q=q->next;              //继续输出整个蛇身
            }
            gotoxy(q->next->x,q->next->y);  //经过上面的循环，q指向蛇尾，蛇尾的下一位，就是蛇走过去的位置
            printf("  ");
            free(q->next);      //进行输出之后，释放指向下一位的指针
            q->next=NULL;       //指针下一位指向空
        }
    }
    if(status==D)
    {
        nexthead->x=head->x;        //向下前进时，x坐标不动，y坐标+1
        nexthead->y=head->y+1;
        nexthead->next=head;
        head=nexthead;
        q=head;
        if(nexthead->x==food->x && nexthead->y==food->y)  //有食物
        {
            
            while(q!=NULL)
            {
                gotoxy(q->x,q->y);
                printf("★");
                q=q->next;
            }
            score=score+add;
			speedup();
            createfood();
        }
        else                               //没有食物
        {
            while(q->next->next!=NULL)
            {
                gotoxy(q->x,q->y);
                printf("★");
                q=q->next;
            }
            gotoxy(q->next->x,q->next->y);
            printf("  ");
            free(q->next);
            q->next=NULL;
        }
    }
    if(status==L)
    {
        nexthead->x=head->x-2;        //向左前进时，x坐标向左移动-2，y坐标不动
        nexthead->y=head->y;
        nexthead->next=head;
        head=nexthead;
        q=head;
        if(nexthead->x==food->x && nexthead->y==food->y)//有食物
        {
            while(q!=NULL)
            {
                gotoxy(q->x,q->y);
                printf("★");
                q=q->next;
            }
            score=score+add;
			speedup();
            createfood();
        }
        else                                //没有食物
        {
            while(q->next->next!=NULL)
            {
                gotoxy(q->x,q->y);
                printf("★");
                q=q->next;        
            }
            gotoxy(q->next->x,q->next->y);
            printf("  ");
            free(q->next);
            q->next=NULL;
        }
    }
    if(status==R)
    {
        nexthead->x=head->x+2;        //向右前进时，x坐标向右移动+2，y坐标不动
        nexthead->y=head->y;
        nexthead->next=head;
        head=nexthead;
        q=head;
        if(nexthead->x==food->x && nexthead->y==food->y)//有食物
        {
            while(q!=NULL)
            {
                gotoxy(q->x,q->y);
                printf("★");
                q=q->next;
            }
            score=score+add;
			speedup();
            createfood();
        }
        else                                         //没有食物
        {
            while(q->next->next!=NULL)
            {
                gotoxy(q->x,q->y);
                printf("★");
                q=q->next;        
            }
            gotoxy(q->next->x,q->next->y);
            printf("  ");
            free(q->next);
            q->next=NULL;
        }
    }
    if(biteself()==1)       //判断是否会咬到自己
    {
        endgamestatus=2;
        endgame();
    }
}
		
int biteself()         //咬自己 
{
	    snake *self;            
	    self=head->next;        
	    while(self!=NULL)
	    {
	        if(self->x==head->x && self->y==head->y)    
	        {
	            return 1;       
	        }
	        self=self->next;
	    }
    return 0;
}

void cantcrosswall()         //撞墙 
{  
    if(head->x==0 || head->x==62 ||head->y==0 || head->y==26) 
    {
        endgamestatus=1;       
        endgame();             
    }
}

/**
 *  加速，蛇吃到食物会自动提速，并且按F1会加速
 */
 
void speedup()
{
	if(sleeptime>=50)
	{
		sleeptime=sleeptime-10;
	//	add=add+2;
 
    }
}
 
/**
 *  加速，按F2会减速
 */
void speeddown()
{
	if(sleeptime<350)               //如果时间间隔小于350
    {
        sleeptime=sleeptime+30;     //时间间隔加上30
       // add=add-2;                  //每吃一次食物的得分减2
 
    }
}

void endgame()               //the end
{
    system("cls");
    if(endgamestatus==1)
    {
		//Lostdraw();
		gotoxy(43,9);
		printf("GAME  OVER !");
    }
    else if(endgamestatus==2)
    {
        
        //Lostdraw();
        gotoxy(43,9);
        printf("GAME  OVER !");
    }
    else if(endgamestatus==3)
    {
		//Lostdraw();
        printf("游戏结束");
    }
    gotoxy(43,12);
    printf("你的得分为%d",score);
}

//定义隐藏光标函数
void HideCursor()
{
	CONSOLE_CURSOR_INFO cursor;    
	cursor.bVisible = FALSE;    
	cursor.dwSize = sizeof(cursor);    
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);    
	SetConsoleCursorInfo(handle, &cursor);
}

int main()
{
    	//system("cls");
    	HideCursor();
		createmap();        //创建地图
    	initializesnake();        //初始化蛇身
    	HideCursor();
    	createfood();       //创建食物
    	keyboardcontrol();	//按键控制
	return 0;
}
