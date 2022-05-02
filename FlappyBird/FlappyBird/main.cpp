#include <iostream>
#include <graphics.h>
#include <vector>
#include <conio.h>
using namespace std;

constexpr auto swidth = 600;
constexpr auto sheight = 900;

class Bird
{
public:
	Bird(IMAGE& img)
		:img(img)
	{
		rect.left = swidth / 2 - img.getwidth() / 2;
		rect.right = rect.left + img.getwidth();
		rect.top = sheight / 2 - img.getheight() / 2;
		rect.bottom = rect.top - img.getheight();
	}

	void Control()
	{
		if (_kbhit())
		{
			if (_getch() == 0x20)
			{
				rect.top -= 64;
				rect.bottom -= 64;
			}
		}
	}

	bool Show()
	{
		if (rect.top >= sheight)
		{
			return false;
		}
		rect.top += 8;
		rect.bottom += 8;

		putimage(rect.left, rect.top, &img);

		return true;
	}

private:
	RECT rect;
	IMAGE& img;

};

class BK
{
public:
	BK(IMAGE& img)
		:img(img),x(0)
	{
		
	}
	void Show()
	{
		if (x == -img.getwidth())
		{
			x = 0;
		}
		x -= 4;
		putimage(x, 0, &img);
		putimage(x + getwidth(), 0, &img);
	}

private:
	IMAGE& img;
	int x;
};

int main()
{
	initgraph(swidth, sheight, EW_SHOWCONSOLE);
	setbkcolor(WHITE);
	cleardevice();

	IMAGE imgbird, imgbk, imgpipeline;
	loadimage(&imgbird, _T("E:\\Code\\VSGit\\FlappyBird\\images\\bird.png"));
	loadimage(&imgbk, _T("E:\\Code\\VSGit\\FlappyBird\\images\\bk.png"), swidth, sheight);
	loadimage(&imgpipeline, _T("E:\\Code\\VSGit\\FlappyBird\\images\\pipeline.png"));
	
	BK bk = BK(imgbk);
	Bird b = Bird(imgbird);
	//vector<> ps;

	bool is_live = true;

	while (is_live)
	{
		BeginBatchDraw();

		bk.Show();
		b.Control();
		if (!b.Show()) { is_live = false; }
		Sleep(16);

		EndBatchDraw();
	}


	return 0;
}