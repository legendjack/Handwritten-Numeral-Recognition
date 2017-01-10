#include <iostream>
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

Point clickPoint;
Mat matHandWriting;
bool cc = true;
void on_Mouse(int event, int x, int y, int flags, void*);

int main(int argc, char** argv)
{
	matHandWriting.create(80, 80, CV_8UC1);
	matHandWriting = Scalar::all(0);

	int intName[100];
	for (int i = 0; i < 100; i++)
	{
		intName[i] = i;
	}

	namedWindow("win", WINDOW_NORMAL);
	resizeWindow("win", 80, 80);
	setMouseCallback("win", on_Mouse, 0);

	int count = 0;
	while (count < 100)
	{
		stringstream ss;
		ss << intName[count];
		string stringName;
		ss >> stringName;
		imshow("win", matHandWriting);
		int key = waitKey(0);
		if (key == 32 && cc)
		{
			cc = false;
			imwrite(stringName+".png", matHandWriting);
			cout << count << "  ";
			count++;
		}
		else if (key == 32 && !cc)
		{
			cc = true;
			matHandWriting = Scalar::all(0);
			imshow("win", matHandWriting);
		}
		else if (key == 27)
		{
			break;
		}
	}

	return 0;
}

void on_Mouse(int event, int x, int y, int flags, void*)
{
	// 如果鼠标不在窗口中则返回
	if (x < 0 || x >= matHandWriting.cols || y < 0 || y >= matHandWriting.rows)
		return;

	// 如果鼠标左键被按下，获取鼠标当前位置；当鼠标左键按下并且移动时，绘制白线；
	if (event == EVENT_LBUTTONDOWN)
	{
		clickPoint = Point(x, y);
	}
	else if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON))
	{
		Point point(x, y);
		line(matHandWriting, clickPoint, point, Scalar::all(255), 6, 8, 0);
		clickPoint = point;
		imshow("win", matHandWriting);
	}
}
