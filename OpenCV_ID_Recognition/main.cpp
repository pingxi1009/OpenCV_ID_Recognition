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

	//imshow("原图", originImg);	// 显示图片
	cout << "width = " << originImg.rows << "   colum = " << originImg.cols << endl;

	Mat resizeImgOri;
	if (originImg.cols > 640)
	{
		resize(originImg, resizeImgOri, Size(640, 640 * originImg.rows / originImg.cols));
	}
	imshow("尺寸变换后的图", resizeImgOri);

	Mat resizeImg = resizeImgOri.clone();

	Mat grayImg, gussImg;
	cvtColor(resizeImg, grayImg, COLOR_RGB2GRAY);	// 灰化

#if 0

	GaussianBlur(grayImg, gussImg, Size(3, 3), 3, 0); //高斯函数滤波

	Mat candyImage;
	Canny(gussImg, candyImage, 300, 250, 3);	// 边缘检测
	imshow("边缘检测后的图", candyImage);

	// 形态学的处理
	Mat binOriImg;
	Mat eleMent = getStructuringElement(MORPH_RECT, Size(6, 3));	// 设置形态学处理窗口大小
	dilate(candyImage, binOriImg, eleMent);	// 多次膨胀操作
	dilate(candyImage, binOriImg, eleMent);	// 多次膨胀操作
	dilate(candyImage, binOriImg, eleMent);	// 多次膨胀操作
	dilate(candyImage, binOriImg, eleMent);	// 多次膨胀操作
	dilate(candyImage, binOriImg, eleMent);	// 多次膨胀操作

	erode(binOriImg, binOriImg, eleMent);	// 进行多次腐蚀
	erode(binOriImg, binOriImg, eleMent);	// 进行多次腐蚀
	
	Mat eleMent1 = getStructuringElement(MORPH_RECT, Size(12, 6));	// 设置形态学处理窗口大小
	dilate(binOriImg, candyImage, eleMent1);	// 多次膨胀操作
	dilate(binOriImg, candyImage, eleMent1);	// 多次膨胀操作
	dilate(binOriImg, candyImage, eleMent1);	// 多次膨胀操作
	dilate(binOriImg, candyImage, eleMent1);	// 多次膨胀操作

	imshow("形态学处理后图片1", binOriImg);
	imshow("形态学处理后图片3", candyImage);
#endif

	// 二值化处理
	Mat eleMent = getStructuringElement(MORPH_RECT, Size(8, 4));	// 设置形态学处理窗口大小
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

	Mat binOriImg;
	//Mat eleMent = getStructuringElement(MORPH_RECT, Size(2, 2));	// 设置形态学处理窗口大小
	dilate(resizeImg, binOriImg, eleMent);	// 多次膨胀操作
	dilate(resizeImg, binOriImg, eleMent);	// 多次膨胀操作
	dilate(resizeImg, binOriImg, eleMent);	// 多次膨胀操作
	dilate(resizeImg, binOriImg, eleMent);	// 多次膨胀操作

	erode(binOriImg, binOriImg, eleMent);	// 进行多次腐蚀
	erode(binOriImg, binOriImg, eleMent);	// 进行多次腐蚀
	erode(binOriImg, binOriImg, eleMent);	// 进行多次腐蚀
	

	//imshow("二值化处理后图", resizeImg);
	imshow("形态学处理后图片2", binOriImg);

	cvtColor(binOriImg, binOriImg, COLOR_RGB2GRAY);


	Mat coutourImg, inImg;
	coutourImg = binOriImg.clone();
	vector<vector<Point> > contours;
	coutourImg.convertTo(inImg, CV_8UC1);
	findContours(inImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);	// 找到形状

	// 画出轮廓
	drawContours(coutourImg, contours, -1, Scalar(0, 255, 0), 1);

	// 轮廓表示为一个矩形 车牌提取
	Mat roiImg;
	vector<Point> rectPoint;
	for (int i = 0 ; i < contours.size() ; i++)
	{
		Rect curRect = boundingRect(Mat(contours[i]));
		cout << "contours " << i << " height = " << curRect.height << "   width = " << curRect.width << endl;
		if ((float)curRect.width / curRect.height >= 2.2 && (float)curRect.width / curRect.height <= 3.6 && 
			((curRect.width * curRect.height) > 10000) && (curRect.width * curRect.height) < 15000)
		{
			cout << "-----------------找到！！！！！-------------" << endl;
			cout << "R.x = " << curRect.x << "  R.y = " << curRect.y << endl;
			rectangle(coutourImg, curRect, Scalar(0, 0, 255), 3);
			roiImg = resizeImgOri(curRect);	// 找到了车牌位置
		}
	}
	imshow("形态学处理后图片3", roiImg);	// 找到了车牌位置

	Mat canImg;
	Canny(roiImg, canImg, 450, 120, 3);
	//imshow("形态学处理后图片4", canImg);

	// 二值化
	Mat roiThreadImg;
	threshold(canImg, roiThreadImg, 50, 255, THRESH_BINARY);
	imshow("形态学处理后图片5", roiThreadImg);
	

	
	waitKey(0);
	return 0;
}