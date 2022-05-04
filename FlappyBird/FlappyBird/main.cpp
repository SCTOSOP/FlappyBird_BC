// 2022_5_4:13_19
// By 马宇卓 Ma Yuzhuo
// This is an open source game project.
// EasyX library support is required.
// Visual Studio version 2022.

#include <iostream>
#include <graphics.h>
#include <vector>
#include <conio.h>
using namespace std;

constexpr auto screenwidth = 600;
constexpr auto screenheight = 900;

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
		// 屏幕正中间生成
		rect.left = screenwidth / 2 - img.getwidth() / 2;
		rect.right = rect.left + img.getwidth();
		rect.top = screenheight / 2 - img.getheight() / 2;
		rect.bottom = rect.top + img.getheight();
	}

	void Control()
	{
		// 按 空格（0x20） 跳跃
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
		if (rect.top >= screenheight || rect.top <= 0)
		{
			return false;
		}
		// 每帧下落
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
		// 两张背景并排以达循环播放
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
		rect.bottom = screenheight;
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

	// 碰撞算法
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
	initgraph(screenwidth, screenheight, EW_SHOWCONSOLE);

	IMAGE imgbird, imgbk, imgpipelineup, imgpipelinedown;
	loadimage(&imgbird, _T("../../images/bird.png"));
	loadimage(&imgbk, _T("../../images/bk.png"), screenwidth, screenheight);
	loadimage(&imgpipelineup, _T("../../images/pipelineup.png"), 60, screenheight);
	loadimage(&imgpipelinedown, _T("../../images/pipelinedown.png"), 60, screenheight);
	
	BK bk = BK(imgbk);
	Bird b = Bird(imgbird);
	// 下方管道和上方管道容器，考虑到上下管道不一定同时创建
	// 所以并未将上下容器合并，方便日后实现更多地图模式
	vector<PipeLineUp*> psup;
	vector<PipeLineDown*> psdown;

	// 控制游戏循环
	bool is_live = true;
	// 管道添加计数（单位：帧）
	int addpipe = 0;

	while (is_live)
	{
		BeginBatchDraw();

		bk.Show();
		b.Control();
		// bird飞出画面外即游戏结束
		if (!b.Show()) { is_live = false; }
		Sleep(16);

		// 循环遍历删除画面外的下方管道，顺便碰撞检测
		auto i = psup.begin();
		while (i != psup.end())
		{
			// 在画面外则删除管道，释放管道资源
			if (!(*i)->Show())
			{
				delete (*i);
				psup.erase(i);
				i = psup.begin();
			}
			// 否则碰撞检测，碰撞即游戏结束
			else
			{
				if ((*i)->Duang(b.GetRect()))
				{
					is_live = false;
				}
				i++;
			}
		}
		// 遍历上方管道，同上
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

		// 添加管道
		addpipe++;
		if (addpipe == addspeed)
		{
			addpipe = 0;

			// 随机高度
			int y = abs(rand()) % (screenheight / 5 * 4 - dert);
			psup.push_back(new PipeLineUp(imgpipelineup, screenwidth, y + dert));
			psdown.push_back(new PipeLineDown(imgpipelinedown, screenwidth, y));
		}

		EndBatchDraw();
	}


	return 0;
}