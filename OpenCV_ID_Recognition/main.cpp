#include <opencv.hpp>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

using namespace std;
using namespace cv;

const int DifMax = 40;	// 设置最大色偏
const int origin_R = 2;	// 原始车牌固定RGB
const int origin_G = 85;
const int origin_B = 196;

int main()
{
	Mat originImg;

	int img_R, img_G, img_B;	// RGB各个通道的值

	originImg = imread("test.jpg", IMREAD_COLOR);	// 读取图像
	if (originImg.empty())	// 判断读取图片是否成功
	{
		cout << "图像打开失败" << endl;
		return -1;
	}

	imshow("原图", originImg);	// 显示图片
	cout << "width = " << originImg.rows << "   colum = " << originImg.cols << endl;

	Mat resizeImg;
	if (originImg.cols > 640)
	{
		resize(originImg, resizeImg, Size(640, 640 * originImg.rows / originImg.cols));
	}
	imshow("尺寸变换后的图", resizeImg);

	Mat grayImg, gussImg;
	cvtColor(resizeImg, grayImg, COLOR_RGB2GRAY);	// 灰化
	GaussianBlur(grayImg, gussImg, Size(3, 3), 3, 0); //高斯函数滤波

	Mat candyImage;
	Canny(gussImg, candyImage, 500, 200, 3);	// 边缘检测
	imshow("边缘检测后的图", candyImage);

	// 形态学的处理
	Mat binOriImg;
	Mat eleMent = getStructuringElement(MORPH_RECT, Size(2, 2));	// 设置形态学处理窗口大小
	dilate(candyImage, binOriImg, eleMent);	// 多次膨胀操作
	dilate(candyImage, binOriImg, eleMent);	// 多次膨胀操作
	dilate(candyImage, binOriImg, eleMent);	// 多次膨胀操作

	erode(binOriImg, binOriImg, eleMent);	// 进行多次腐蚀
	erode(binOriImg, binOriImg, eleMent);	// 进行多次腐蚀
	erode(binOriImg, binOriImg, eleMent);	// 进行多次腐蚀

	imshow("形态学处理后图片1", binOriImg);

	// 二值化处理
	for (int i = 0 ; i < resizeImg.rows ; i++)
	{
		for (int j = 0 ; j < resizeImg.cols ; j++)
		{
			img_B = resizeImg.at<Vec3b>(i, j)[0];	// 获取图片各个通道的值
			img_G = resizeImg.at<Vec3b>(i, j)[1];
			img_R = resizeImg.at<Vec3b>(i, j)[2];

			if ((abs(origin_B - img_B) < DifMax) && (abs(origin_B - img_B) < DifMax) && (abs(origin_B - img_B) < DifMax))
			{
				resizeImg.at<Vec3b>(i, j)[0] = 255;
				resizeImg.at<Vec3b>(i, j)[1] = 255;
				resizeImg.at<Vec3b>(i, j)[2] = 255;
			}
			else
			{
				resizeImg.at<Vec3b>(i, j)[0] = 0;
				resizeImg.at<Vec3b>(i, j)[1] = 0;
				resizeImg.at<Vec3b>(i, j)[2] = 0;
			}
		}
	}

	//Mat binOriImg;
	//Mat eleMent = getStructuringElement(MORPH_RECT, Size(2, 2));	// 设置形态学处理窗口大小
	dilate(resizeImg, binOriImg, eleMent);	// 多次膨胀操作
	dilate(resizeImg, binOriImg, eleMent);	// 多次膨胀操作
	dilate(resizeImg, binOriImg, eleMent);	// 多次膨胀操作

	erode(binOriImg, binOriImg, eleMent);	// 进行多次腐蚀
	erode(binOriImg, binOriImg, eleMent);	// 进行多次腐蚀
	erode(binOriImg, binOriImg, eleMent);	// 进行多次腐蚀


	//imshow("二值化处理后图", resizeImg);
	imshow("形态学处理后图片2", binOriImg);

	

	
	waitKey(0);
	return 0;
}