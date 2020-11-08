#define _CRT_SECURE_NO_WARNINGS 1
#include<stdio.h>			
#include<time.h>			
#include<windows.h>		
#include<stdlib.h>			
#include<conio.h>	      //���ܼ������� 

#define U 1               //�ߵ���������
#define D 2
#define L 3
#define R 4           


/*********����ȫ�ֱ��� *******/ 
typedef struct listnode                   //����������ڵ� 
{
	int x;
	int y;
	struct listnode*next;
}snake;

int score=0,add=10;			//�ܵ÷���ÿ�γ�ʳ��÷�
int status,sleeptime=200;	//��ǰ��״̬��ÿ�����е�ʱ����
snake *head, *food;			//��ͷָ�룬ʳ��ָ��
snake *q;					//�����ߵ�ʱ���õ���ָ��
int endgamestatus=0;		//��Ϸ�����������1��ײ��ǽ��2��ҧ���Լ���3�������˳���Ϸ��
HANDLE hOut;				//����̨���


/***********��������************/ 
void gotoxy(int x,int y);   //���ù��λ��
void createMap();           //���Ƶ�ͼ
void initializesnake();    //��ʼ������������
void createfood();          //�������������ʳ��
int biteself();             //�ж��Ƿ�ҧ�����Լ�
void cantcrosswall();       //������ײǽ�����
void snakemove();           //������ǰ������
void speedup();				//����
void speeddown();			//����
void keyboardControl();     //���Ƽ��̰���
void endgame();             //��Ϸ����



/*********��Ϸ�ľ�̬��ʼ��***********/ 
void gotoxy(int x,int y)		//���ù��λ��												 
{
	COORD pos;
	pos.X=x;
	pos.Y=y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
} 

void createmap()                 //��ӡ����ͼ 
{
	gotoxy(68,8);
	printf("��Ϊǽ��ײ����gg"); 
	gotoxy(68,9);
	printf("��Ϊ��ʵ���Ե���++");
	int i,j;
	for(i=0;i<62;i+=2)           //��ӡ�������� 
	{
		gotoxy(i,0);
		printf("��");
		gotoxy(i,26);
		printf("��");
	}
	for(j=1;j<26;j++)			 //��ӡ�������� 
	{
		gotoxy(0,j);
		printf("��");
		gotoxy(59,j);
		printf("��");
	}

}

//����һ����̬���� 
void initializesnake()
{
    snake *tail;
    int i;
    tail=(snake*)malloc(sizeof(snake));     //����β��ʼ��ͷ�巨����x,y�趨��ʼ��λ��
    tail->x=24;         	//�ߵĳ�ʼλ�ã�24,5��
    tail->y=5;
    tail->next=NULL;
    for(i=1;i<=4;i++)       //������������Ϊ5
    {
        head=(snake*)malloc(sizeof(snake)); //��ʼ����ͷ
        head->next=tail;        //��ͷ����һλΪ��β
        head->x=24+2*i;         //������ͷλ��
        head->y=5;
        tail=head;              //��ͷ�����β��Ȼ���ظ�ѭ��
    }
    while(tail!=NULL)		//��ͷ��β���������
    {
        gotoxy(tail->x,tail->y);
        printf("��");       //�����������ʹ�á����
        tail=tail->next;    //��ͷ�����ϣ������ͷ����һλ��һֱ�������β
    }
}

//�������һ��ʳ��
void createfood()
{
    snake *food_1;
    srand((unsigned)time(NULL));        	//��ʼ�������
    food_1=(snake*)malloc(sizeof(snake));   
    food_1->x=8%52+2; food_1->y=20%24+1;
    while((food_1->x%2)!=0)    				//��֤��Ϊż����ʹ��ʳ��������ͷ���䣬Ȼ��ʳ����������������
    {
        food_1->x=rand()%52+2;              //ʳ���������
    }
    food_1->y=rand()%24+1;
    q=head;
    while(q->next==NULL)
    {
        if(q->x==food_1->x && q->y==food_1->y) //�ж������Ƿ���ʳ���غ�
        {
            free(food_1);               //��������ʳ���غϣ���ô�ͷ�ʳ��ָ��
            createfood();               //���´���ʳ��
        }
        q=q->next;
    }
    gotoxy(food_1->x,food_1->y);
    food=food_1;
    printf("��");           //���ʳ��
}

/*********����Ϊ��̬�߼��ı�д *******/

void keyboardcontrol()                                   //ʵ�ּ��̿��� 
{
	status = R;
	while(1)
    {
	if(GetAsyncKeyState(VK_UP) && status!=D)             //GetAsyncKeyState���������жϺ�������ʱָ���������״̬
    {
        status=U;                                        //����߲�������ǰ����ʱ�򣬰��ϼ���ִ������ǰ������
    }
        else if(GetAsyncKeyState(VK_DOWN) && status!=U)  //����߲�������ǰ����ʱ�򣬰��¼���ִ������ǰ������
    {
        status=D;
    }
	else if(GetAsyncKeyState(VK_LEFT)&& status!=R)       //����߲�������ǰ����ʱ�򣬰������ִ������ǰ��
	{
        status=L;
    }
    else if(GetAsyncKeyState(VK_RIGHT)&& status!=L)      //����߲�������ǰ����ʱ�򣬰��Ҽ���ִ������ǰ��
    {
       status=R;
    }
    Sleep(sleeptime);
    snakemove();
	}		 
}

void snakemove()	//��ǰ��,��U,��D,��L,��R
{
	snake * nexthead;
    cantcrosswall();
    nexthead=(snake*)malloc(sizeof(snake));		//Ϊ��һ�����ٿռ�
    if(status==U)
    {
        nexthead->x=head->x;             //����ǰ��ʱ��x���겻����y����-1
        nexthead->y=head->y-1;
        nexthead->next=head;
        head=nexthead;
        q=head;                          //ָ��qָ����ͷ
        if(nexthead->x==food->x && nexthead->y==food->y)	   //�����һ����ʳ�� ��һ��λ�õ������ʳ���������ͬ
        {
            while(q!=NULL)
            {
                gotoxy(q->x,q->y);
                printf("��");       //ԭ��ʳ���λ�ã��ӡ񻻳ɡ�
                q=q->next;          //ָ��qָ����������һλҲִ��ѭ����Ĳ���
            }
            score=score+add;        //����һ��ʳ����ܷ��ϼ���ʳ��ķ�
			speedup();
            createfood();           //����ʳ��
        }
        else                        
        {
            while(q->next->next!=NULL)	//���û����ʳ��
            {
                gotoxy(q->x,q->y);
                printf("��");           //��������ǰ�ߣ������ǰλ�õ�����
                q=q->next;              //���������������
            }
            gotoxy(q->next->x,q->next->y);  //���������ѭ����qָ����β����β����һλ���������߹�ȥ��λ��
            printf("  ");
            free(q->next);      //�������֮���ͷ�ָ����һλ��ָ��
            q->next=NULL;       //ָ����һλָ���
        }
    }
    if(status==D)
    {
        nexthead->x=head->x;        //����ǰ��ʱ��x���겻����y����+1
        nexthead->y=head->y+1;
        nexthead->next=head;
        head=nexthead;
        q=head;
        if(nexthead->x==food->x && nexthead->y==food->y)  //��ʳ��
        {
            
            while(q!=NULL)
            {
                gotoxy(q->x,q->y);
                printf("��");
                q=q->next;
            }
            score=score+add;
			speedup();
            createfood();
        }
        else                               //û��ʳ��
        {
            while(q->next->next!=NULL)
            {
                gotoxy(q->x,q->y);
                printf("��");
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
        nexthead->x=head->x-2;        //����ǰ��ʱ��x���������ƶ�-2��y���겻��
        nexthead->y=head->y;
        nexthead->next=head;
        head=nexthead;
        q=head;
        if(nexthead->x==food->x && nexthead->y==food->y)//��ʳ��
        {
            while(q!=NULL)
            {
                gotoxy(q->x,q->y);
                printf("��");
                q=q->next;
            }
            score=score+add;
			speedup();
            createfood();
        }
        else                                //û��ʳ��
        {
            while(q->next->next!=NULL)
            {
                gotoxy(q->x,q->y);
                printf("��");
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
        nexthead->x=head->x+2;        //����ǰ��ʱ��x���������ƶ�+2��y���겻��
        nexthead->y=head->y;
        nexthead->next=head;
        head=nexthead;
        q=head;
        if(nexthead->x==food->x && nexthead->y==food->y)//��ʳ��
        {
            while(q!=NULL)
            {
                gotoxy(q->x,q->y);
                printf("��");
                q=q->next;
            }
            score=score+add;
			speedup();
            createfood();
        }
        else                                         //û��ʳ��
        {
            while(q->next->next!=NULL)
            {
                gotoxy(q->x,q->y);
                printf("��");
                q=q->next;        
            }
            gotoxy(q->next->x,q->next->y);
            printf("  ");
            free(q->next);
            q->next=NULL;
        }
    }
    if(biteself()==1)       //�ж��Ƿ��ҧ���Լ�
    {
        endgamestatus=2;
        endgame();
    }
}
		
int biteself()         //ҧ�Լ� 
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

void cantcrosswall()         //ײǽ 
{  
    if(head->x==0 || head->x==62 ||head->y==0 || head->y==26) 
    {
        endgamestatus=1;       
        endgame();             
    }
}

/**
 *  ���٣��߳Ե�ʳ����Զ����٣����Ұ�F1�����
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
 *  ���٣���F2�����
 */
void speeddown()
{
	if(sleeptime<350)               //���ʱ����С��350
    {
        sleeptime=sleeptime+30;     //ʱ��������30
       // add=add-2;                  //ÿ��һ��ʳ��ĵ÷ּ�2
 
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
        printf("��Ϸ����");
    }
    gotoxy(43,12);
    printf("��ĵ÷�Ϊ%d",score);
}

//�������ع�꺯��
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
		createmap();        //������ͼ
    	initializesnake();        //��ʼ������
    	HideCursor();
    	createfood();       //����ʳ��
    	keyboardcontrol();	//��������
	return 0;
}
