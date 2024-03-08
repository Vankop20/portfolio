#include <iostream>
#include <cstdlib>
//#include <conio.h>
#include <fstream>
#include <ctime>

using namespace std;

bool GO, ff;
int W=21, H=11, x, y, X, Y, score, xy[100][2]={}, l=0;
int r;
enum eDirection {STOP=0, UP, RIGHT, LEFT, DOWN};
eDirection dir;

void setup()
{
	srand(time(0));
	ifstream in;
	in.open("max.txt");
	in>>r;
	in.close();
	GO=0;
	dir=STOP;
	x=W/2;
	y=H/2;
	X=abs(rand()%(W-1))+1;
	Y=abs(rand()%(H-1))+1;
	score=0;
}
void draw()
{
	system("cls");
	for(int i=0; i<W; i++)
	{
		cout<<"#";
	}
	cout<<endl;
	for(int i=0; i<H; i++)
	{
		for(int j=0; j<W; j++)
		if(j==0 || j==W-1)
		{
			cout<<"#";
		}
		else
		{
			if(i==y && j==x)
			{
				cout<<"N";
			}
			else
			{
				if(i==Y && j==X)
				{
					cout<<"0";
				}
				else
				{
					bool f=1;
					for(int k=1; k<l; k++)
					{
						if(xy[k][0]==j && xy[k][1]==i)
						{
							cout<<'z';
							f=0;
						}
					}
					if(f)
						cout<<' ';
				}
			}
		}
		cout<<endl;
	}
	for(int i=0; i<W; i++)
	{
		cout<<"#";
	}
	cout<<endl;
	cout<<"Score:"<<score<<endl;
	cout<<"Record:"<<r<<endl;
}

void input()
{
	if(kbhit)
	{
		switch(getch())
		{
			case 'w':
				dir=UP;
				break;
			case 'a':
				dir=LEFT;
				break;
			case 'd':
				dir=RIGHT;
				break;
			case 's':
				dir=DOWN;
				break;
			case 'q':
				GO=1;
				break;
		}
	}
}

void logic()
{
	xy[0][0]=x;
	xy[0][1]=y;
	int px=xy[0][0], py=xy[0][1], p2x, p2y;
	for(int i=1; i<l; i++)
	{
		p2x=xy[i][0];
		p2y=xy[i][1];
		xy[i][0]=px;
		xy[i][1]=py;
		px=p2x;
		py=p2y;
	}
	switch(dir)
	{
		case LEFT:
			x--;
			break;
		case RIGHT:
			x++;
			break;
		case UP:
			y--;
			break;
		case DOWN:
			y++;
			break;
	} 
	if(x==W)
	x=0;
	if(y==H)
	y=0;
	if(x<0)
	x=W;
	if(y<0)
	y=H;	
	for(int k=0; k<l; k++)
	{
		if(x==xy[k][0] && y==xy[k][1])
		GO=1;
		ofstream out;
		out.open("max.txt");
		if(score>r)
		{
			out<<score;
			ff=1;
		}
		else
		{
			out<<r;
			ff=0;
		}
		out.close();
	}
	if(x==X && y==Y)
	{
		score+=100;
		X=(rand()%(W-1));
		Y=(rand()%(H-1));
		l++;
	}
}

int main()
{
	setup();
	while(!GO)
	{
		draw();
		input();
		logic();
	}
	while(GO)
	{
		system("cls");
		if(ff)
		cout<<"New record:"<<score;
		else
		cout<<"You score:"<<score;
	}
}
