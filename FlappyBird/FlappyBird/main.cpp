#include <iostream>
#include <graphics.h>
#include <vector>
#include <conio.h>
using namespace std;

constexpr auto swidth = 600;
constexpr auto sheight = 900;

// 背景和管道移动速度（单位：像素）
constexpr auto bkspeed = 4;
// 上下管道相差像素（单位：像素）
constexpr auto dert = 220;
// 管道生成速度（单位：帧）
constexpr auto addspeed = 70;

class Bird
{
public:
	Bird(IMAGE& img)
		:img(img)
	{
		rect.left = swidth / 2 - img.getwidth() / 2;
		rect.right = rect.left + img.getwidth();
		rect.top = sheight / 2 - img.getheight() / 2;
		rect.bottom = rect.top + img.getheight();
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
		if (rect.top >= sheight || rect.top <= 0)
		{
			return false;
		}
		rect.top += 8;
		rect.bottom += 8;

		putimage(rect.left, rect.top, &img);

		return true;
	}
	RECT& GetRect() { return rect; }

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
		x -= bkspeed;
		putimage(x, 0, &img);
		putimage(x + getwidth(), 0, &img);
	}

private:
	IMAGE& img;
	int x;
};

class PipeLineUp
{
public:
	PipeLineUp(IMAGE& img, int x, int y)
		:img(img)
	{
		rect.left = x;
		rect.right = rect.left + img.getwidth();
		rect.top = y;
		rect.bottom = sheight;
	}
	bool Show()
	{
		if (rect.right <= 0) { return false; }
		rect.left -= bkspeed;
		rect.right -= bkspeed;
		putimage(rect.left, rect.top, &img);
		return true;
	}
	RECT& GetRect() { return rect; }

	bool Duang(RECT& r2)
	{
		RECT r;
		r.left = rect.left - (r2.right - r2.left);
		r.right = rect.right;
		r.top = rect.top - (r2.bottom - r2.top);
		r.bottom = rect.bottom;

		return (r.left < r2.left&& r2.left <= r.right && r.top <= r2.top && r2.top <= r.bottom);
	}

protected:
	RECT rect;
	IMAGE& img;

};

class PipeLineDown : public PipeLineUp
{
public:
	PipeLineDown(IMAGE& img, int x, int y)
		:PipeLineUp(img, x, y)
	{
		rect.top = y - img.getheight();
		rect.bottom = y;
	}
};

int main()
{
	initgraph(swidth, sheight, EW_SHOWCONSOLE);
	setbkcolor(WHITE);
	cleardevice();

	IMAGE imgbird, imgbk, imgpipelineup, imgpipelinedown;
	loadimage(&imgbird, _T("../../images/bird.png"));
	loadimage(&imgbk, _T("../../images/bk.png"), swidth, sheight);
	loadimage(&imgpipelineup, _T("../../images/pipelineup.png"), 60, sheight);
	loadimage(&imgpipelinedown, _T("../../images/pipelinedown.png"), 60, sheight);
	
	BK bk = BK(imgbk);
	Bird b = Bird(imgbird);
	vector<PipeLineUp*> psup;
	vector<PipeLineDown*> psdown;

	bool is_live = true;

	int addpipe = 0;

	while (is_live)
	{
		BeginBatchDraw();

		bk.Show();
		b.Control();
		if (!b.Show()) { is_live = false; }
		Sleep(16);

		auto i = psup.begin();
		while (i != psup.end())
		{
			if (!(*i)->Show())
			{
				delete (*i);
				psup.erase(i);
				i = psup.begin();
			}
			else
			{
				if ((*i)->Duang(b.GetRect()))
				{
					is_live = false;
				}
				i++;
			}
		}
		auto j = psdown.begin();
		while (j != psdown.end())
		{
			if (!(*j)->Show())
			{
				delete (*j);
				psdown.erase(j);
				j = psdown.begin();
			}
			else
			{
				if ((*j)->Duang(b.GetRect()))
				{
					is_live = false;
				}
				j++;
			}
		}

		addpipe++;
		if (addpipe == addspeed)
		{
			addpipe = 0;

			int y = abs(rand()) % (sheight / 5 * 4 - dert);
			psup.push_back(new PipeLineUp(imgpipelineup, swidth, y + dert));
			psdown.push_back(new PipeLineDown(imgpipelinedown, swidth, y));
		}

		EndBatchDraw();
	}


	return 0;
}