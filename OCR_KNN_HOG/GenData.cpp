#include <iostream>
#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
	Mat matClassificationInts;				// 保存我们感兴趣的字符，0~9
    Mat matTrainingImagesAsFlattenedFloats;	// 保存训练图片中所有单个字符ROI

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			matClassificationInts.push_back(i);
		}
	}

	int intName[100];
	for (int i = 0; i < 100; i++)
	{
		intName[i] = i;
	}

	for (int i = 0; i < 100; i++)
	{
		stringstream ss;
		ss << intName[i];
		string stringName;
		ss >> stringName;
		Mat matROI = imread("numRecognize\\"+stringName+".png", 0);
		Mat matROICopy = matROI.clone();
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(matROI, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		Rect boundRect = boundingRect(contours[0]);
		Mat numImage = matROICopy(boundRect).clone();
		resize(numImage, numImage, Size(40, 40));

		/* 这里计算目标图像的HOG特征（方向梯度直方图特征）代替之前用的灰度特征 */
		HOGDescriptor *hog = new HOGDescriptor(Size(40, 40), Size(16, 16), Size(8, 8), Size(8, 8), 9);
		vector<float> descriptors;
		hog->compute(numImage, descriptors);
		Mat dst(1, (int)(descriptors.size()), CV_32FC1, descriptors.data());

		matTrainingImagesAsFlattenedFloats.push_back(dst);
	}

	// 保存分类文件为 classifications.xml
    FileStorage fsClassifications("classifications.xml", FileStorage::WRITE);

    if (fsClassifications.isOpened() == false) {
		cout << "ERROR: 无法打开训练分类文件classifications.xml\n\n";
		system("pause");
		return 0;
    }

    fsClassifications << "classifications" << matClassificationInts;        // write classifications into classifications section of classifications file
    fsClassifications.release();                                            // close the classifications file

	// 保存训练图片文件为 images.xml
    FileStorage fsTrainingImages("images.xml", FileStorage::WRITE);

    if (fsTrainingImages.isOpened() == false) {
        cout << "ERROR: 无法打开训练图片文件images.xml\n\n";
		system("pause");
		return 0;
    }

    fsTrainingImages << "images" << matTrainingImagesAsFlattenedFloats;      // write training images into images section of images file
    fsTrainingImages.release(); 

	cout << "成功生成xml文件！\n" << endl;
	//system("pause");
	return 0;
}