#include <iostream>
#include <opencv.hpp>
#include <opencv2\ml\ml.hpp>

using namespace cv;
using namespace std;

Mat matHandWriting;
Point clickPoint;
bool boolRecognize = true;

void on_Mouse(int event, int x, int y, int flags, void*);

void helpText()
{
	cout << "操作提示：" << endl;
	cout << "\t在窗口写数字，写完后按【空格键】识别，控制台输出识别结果" << endl;
	cout << "\t再次按【空格键】清除所写内容，然后进行下一次书写\n" << endl;
	cout << "识别结果：";
}

int main()
{
    FileStorage fsClassifications("classifications.xml", FileStorage::READ);  // 读取 classifications.xml 分类文件

    if (fsClassifications.isOpened() == false) {
        cout << "ERROR, 无法打开classifications.xml\n\n";
		system("pause");
        return 0;
    }

	Mat matClassificationInts;
    fsClassifications["classifications"] >> matClassificationInts;  // 把 classifications.xml 中的 classifications 读取进Mat变量
    fsClassifications.release();  // 关闭文件

    FileStorage fsTrainingImages("images.xml", FileStorage::READ);  // 打开训练图片文件

    if (fsTrainingImages.isOpened() == false) {
        cout << "ERROR, 无法打开images.xml\n\n";
		system("pause");
        return 0;
    }

	// 读取训练图片数据（从images.xml中）
    Mat matTrainingImagesAsFlattenedFloats;  // we will read multiple images into this single image variable as though it is a vector
    fsTrainingImages["images"] >> matTrainingImagesAsFlattenedFloats;  // 把 images.xml 中的 images 读取进Mat变量
    fsTrainingImages.release();

	// 训练
    Ptr<ml::KNearest> kNearest(ml::KNearest::create());  // 实例化 KNN 对象

	// 最终调用train函数，注意到两个参数都是Mat类型（单个Mat），尽管实际上他们都是多张图片或多个数
    kNearest->train(matTrainingImagesAsFlattenedFloats, ml::ROW_SAMPLE, matClassificationInts);

    // 测试
	matHandWriting.create(Size(160, 160), CV_8UC1);
	matHandWriting = Scalar::all(0);
	imshow("手写数字", matHandWriting);
	setMouseCallback("手写数字", on_Mouse, 0);

	helpText();
	///////////////////////////////////////////////////////////////////////////////
	while (true)
	{
		int key = waitKey(0);
		if (key == 32 && boolRecognize)
		{
			boolRecognize = false;

			Mat matROICopy = matHandWriting.clone();
			vector<vector<Point>> contours;
			vector<Vec4i> hierarchy;
			findContours(matHandWriting, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
			Rect boundRect = boundingRect(contours[0]);
			Mat numImage = matROICopy(boundRect).clone();
			resize(numImage, numImage, Size(40, 40));

			/* 这里计算目标图像的HOG特征（方向梯度直方图特征）代替之前用的灰度特征 */
			HOGDescriptor *hog = new HOGDescriptor(Size(40, 40), Size(16, 16), Size(8, 8), Size(8, 8), 9);
			vector<float> descriptors;
			hog->compute(numImage, descriptors);
			Mat matROIFlattenedFloat(1, (int)(descriptors.size()), CV_32FC1, descriptors.data());

			Mat matCurrentChar(0, 0, CV_32F);  // findNearest的结果保存在这里
			
			// 最终调用 findNearest 函数
			kNearest->findNearest(matROIFlattenedFloat, 1, matCurrentChar);

			//float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);
			int intCurrentChar = (int)matCurrentChar.at<float>(0, 0);
			cout << intCurrentChar << "  ";
		}
		else if (key == 32 && !boolRecognize)
		{
			boolRecognize = true;
			matHandWriting = Scalar::all(0);
			imshow("手写数字", matHandWriting);
		}
		else if (key == 27)
			break;
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
		line(matHandWriting, clickPoint, point, Scalar::all(255), 12, 8, 0);
		clickPoint = point;
		imshow("手写数字", matHandWriting);
	}
}
