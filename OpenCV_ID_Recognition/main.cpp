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

	originImg = imread("test6.jpg", IMREAD_COLOR);	// 读取图像
	if (originImg.empty())	// 判断读取图片是否成功
	{
		cout << "图像打开失败" << endl;
		return -1;
	}

	//imshow("原图", originImg);	// 显示图片
	cout << "width = " << originImg.rows << "   colum = " << originImg.cols << endl;

	Mat resizeImgOri;	// 修改图片为固定尺寸
	if (originImg.cols > 640)
	{
		resize(originImg, resizeImgOri, Size(640, 640 * originImg.rows / originImg.cols));
	}
	imshow("尺寸变换后的图", resizeImgOri);

	Mat resizeImg = resizeImgOri.clone();

	Mat grayImg, gussImg;
	cvtColor(resizeImg, grayImg, COLOR_RGB2GRAY);		// 灰化

#if 1		// 第一种方法，兼容性比较好，及其推荐

	GaussianBlur(grayImg, gussImg, Size(3, 3), 3, 0);	//高斯函数滤波
	Mat candyImage;
	Canny(gussImg, candyImage, 300, 200, 3);			// 边缘检测
	imshow("边缘检测后的图", candyImage);

	// 形态学的处理
	Mat dilateImg, erodeImg;
	Point m_point(-1, -1);
	Mat eleMentX = getStructuringElement(MORPH_RECT, Size(19, 1));	// 设置形态学处理窗口大小
	dilate(candyImage, dilateImg, eleMentX, m_point, 2);	// 多次膨胀操作
	erode(dilateImg, erodeImg, eleMentX, m_point, 4);		// 进行多次腐蚀
	imshow("一次形态学处理之后", erodeImg);

	Mat eleMentY = getStructuringElement(MORPH_RECT, Size(1, 15));	// 设置形态学处理窗口大小
	dilate(erodeImg, dilateImg, eleMentX, m_point, 2);
	erode(dilateImg, erodeImg, eleMentY, m_point, 1);

	imshow("二次形态学处理之后 膨胀前", erodeImg);
	dilate(erodeImg, dilateImg, eleMentY, m_point, 2);
	//erode(dilateImg, erodeImg, eleMentY, m_point, 1);
	//dilate(erodeImg, dilateImg, eleMentX, m_point, 2);
	imshow("二次形态学处理之后 膨胀后", dilateImg);

	Mat blurrImg;
	medianBlur(dilateImg, blurrImg, 15);
	medianBlur(blurrImg, blurrImg, 15);
	imshow("三次形态学处理之后", blurrImg);

#else	// 第二种方法，只能识别少数特定的图片，兼容性较差，不推荐这样处理

	// 二值化处理
	int img_R, img_G, img_B;	// RGB各个通道的值
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

	Mat blurrImg;
	//Mat eleMent = getStructuringElement(MORPH_RECT, Size(2, 2));	// 设置形态学处理窗口大小
	dilate(resizeImg, blurrImg, eleMent);	// 多次膨胀操作
	dilate(resizeImg, blurrImg, eleMent);	// 多次膨胀操作
	dilate(resizeImg, blurrImg, eleMent);	// 多次膨胀操作
	dilate(resizeImg, blurrImg, eleMent);	// 多次膨胀操作

	erode(blurrImg, blurrImg, eleMent);	// 进行多次腐蚀
	erode(blurrImg, blurrImg, eleMent);	// 进行多次腐蚀
	erode(blurrImg, blurrImg, eleMent);	// 进行多次腐蚀
	

	//imshow("二值化处理后图", resizeImg);
	imshow("形态学处理后图片2", blurrImg);

#endif

	if (blurrImg.channels() != 1)	// 如果已经是灰化的图像，则不需要灰化
	{
		cvtColor(blurrImg, blurrImg, COLOR_RGB2GRAY);
	}

	// 找到轮廓
	Mat coutourImg, inImg;
	coutourImg = blurrImg.clone();
	vector<vector<Point> > contours;
	coutourImg.convertTo(inImg, CV_8UC1);	// 将图片转化为固定格式
	findContours(inImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);	// 找到形状

	// 画出轮廓
	drawContours(coutourImg, contours, -1, Scalar(0, 255, 0), 1);

	// 轮廓表示为一个矩形 车牌提取
	Mat roiImg;
	vector<Point> rectPoint;
	for (int i = 0 ; i < contours.size() ; i++)	// 遍历所有找到的结果，找到符合车牌特征的矩形
	{
		Rect curRect = boundingRect(Mat(contours[i]));
		cout << "contours " << i << " height = " << curRect.height << "   width = " << curRect.width << endl;

		// 矩形的长宽比（1.8-8）&& 矩形面积（3000-55000）
		if ((float)curRect.width / curRect.height >= 1.8 && (float)curRect.width / curRect.height <= 8 && 
			((curRect.width * curRect.height) > 3000) && (curRect.width * curRect.height) < 55000)
		{
			cout << "-----------------找到！！！！！-------------" << endl;
			cout << "R.x = " << curRect.x << "  R.y = " << curRect.y << endl;
			rectangle(coutourImg, curRect, Scalar(0, 0, 255), 3);
			roiImg = resizeImgOri(curRect);	// 找到了车牌位置
			imshow("形态学处理后图片3", roiImg);	// 找到了车牌位置
		}
	}
	
	if (!roiImg.data)
	{
		cout << "---------------------------------------" << endl;
		cout << "----------未找到匹配的矩形！！---------" << endl;
		cout << "---------------------------------------" << endl;
		waitKey(0);
		return -1;
	}

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