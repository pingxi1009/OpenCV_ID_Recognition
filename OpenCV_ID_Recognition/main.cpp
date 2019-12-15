#include <opencv.hpp>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>

using namespace std;
using namespace cv;

const int DifMax = 40;	// 设置最大色偏
const int origin_R = 2;	// 原始车牌固定RGB
const int origin_G = 85;
const int origin_B = 196;

int main()
{
	Mat originImg;

	originImg = imread("test8.jpg", IMREAD_COLOR);	// 读取图像
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
	//Canny(gussImg, candyImage, 250, 70, 3);			// 边缘检测
	Canny(gussImg, candyImage, 300, 200, 3);			// 边缘检测
	imshow("边缘检测后的图", candyImage);

	// 形态学的处理
	Mat dilateImg, erodeImg;
	Point m_point(-1, -1);
	Mat eleMentX = getStructuringElement(MORPH_RECT, Size(19, 1));	// 设置形态学处理窗口大小
	dilate(candyImage, dilateImg, eleMentX, m_point, 2);	// 多次膨胀操作
	erode(dilateImg, erodeImg, eleMentX, m_point, 4);		// 进行多次腐蚀
	//imshow("一次形态学处理之后", erodeImg);

	Mat eleMentY = getStructuringElement(MORPH_RECT, Size(1, 15));	// 设置形态学处理窗口大小
	dilate(erodeImg, dilateImg, eleMentX, m_point, 2);
	erode(dilateImg, erodeImg, eleMentY, m_point, 1);

	//imshow("二次形态学处理之后 膨胀前", erodeImg);
	dilate(erodeImg, dilateImg, eleMentY, m_point, 2);
	//erode(dilateImg, erodeImg, eleMentY, m_point, 1);
	//dilate(erodeImg, dilateImg, eleMentX, m_point, 2);
	//imshow("二次形态学处理之后 膨胀后", dilateImg);

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
		if ((float)curRect.width / curRect.height >= 2.0 && (float)curRect.width / curRect.height <= 8)
		{
			cout << "=====Area = " << curRect.width * curRect.height << "=====" << endl;
			if (((curRect.width * curRect.height) > 4800) && (curRect.width * curRect.height) < 55000 )
			{
				cout << "-----------------找到！！！！！-------------" << endl;
				cout << "R.x = " << curRect.x << "  R.y = " << curRect.y << endl;
				rectangle(coutourImg, curRect, Scalar(0, 0, 255), 3);
				roiImg = resizeImgOri(curRect);	// 找到了车牌位置
				imshow("形态学处理后图片3", roiImg);	// 找到了车牌位置
			}
			
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
	
	// 分割字符
	// 1，竖直方向上的投影
	Mat roiCloneImg = roiThreadImg.clone();
	roiCloneImg.type();
	int roiCol = roiThreadImg.cols, roiRow = roiThreadImg.rows;
	//ushort m_temple;
	int shuzhiShadow[500] = { 0, };
	int jishuconut = 0;
	for (int i = 0 ; i < roiCol - 1 ; i++)
	{
		for (int j = 0 ; j < roiRow -1 ; j++)
		{
			uchar m_temple = roiThreadImg.at<uchar>(j, i);	// y轴和x轴要转换一下，不然容易内存泄漏！！！
			//shuzhiShadow[i] = 0;
			//cout << "-Debug-" << roiThreadImg.at<uchar>(i, j) << endl;
			if (m_temple > 0)
			{
				shuzhiShadow[i] += 1;
				jishuconut++;	// 统计每一列的有多少有值的像素点
			}
		}
	}

	cout << "youduoshao  = " << jishuconut << endl;
	// 对数组进行滤波
	for (int i = 0 ; i < roiCol - 1 ; i++)
	{
		if (shuzhiShadow[i] >= 4)	// 一列的有4个以上则认为是有效字符
		{
			shuzhiShadow[i] = 1;
		}
		else if(shuzhiShadow[i] <= 3)	// 一列的少于3个的则认为不是有效字符
		{
			shuzhiShadow[i] = 0;
		}
	}

	// 确认字符的位置
	int count = 0;
	int m_temple;
	int positionReal[50], positionEmpty[50];
	bool m_CountFlag = false;
	for (int i = 0 ; i < roiCol - 1 ; i++)
	{
		m_temple = shuzhiShadow[i];
		if (m_temple == 1 && !m_CountFlag)
		{
			m_CountFlag = true;
			positionReal[count] = i;
			continue;
		}
		if (m_temple == 0 && m_CountFlag)
		{
			m_CountFlag = false;
			positionEmpty[count] = i;
			count++;
		}
		if (i == roiCol - 2 && m_CountFlag)
		{
			m_CountFlag = false;
			positionEmpty[count] = i;
			count++;
		}
	}

	// 记录所有字符的宽度
	vector <double> strWide, strWideTemp;
	int roiWides[50] = { 0, };
	int positionX1[50] = {0, };
	int positionX2[50] = {0, };
	for (int i = 0 ,j = 0; i < count ; i++)
	{
		cout << "positionReal = " << positionReal[i] << "   positionEmpty = " << positionEmpty[i] << 
			" ------ distance = " << positionEmpty[i] - positionReal[i] << endl;
		roiWides[i] = positionEmpty[i] - positionReal[i];

		strWide.push_back(roiWides[i]);
	}

	// 获取车牌字符的最小宽度
	strWideTemp = strWide;
	int Img_Mix_Wide = 0;	// 初始化车牌的最小宽度
	sort(strWideTemp.begin(), strWideTemp.end());
	int indexTemp = 0;
	for (vector<double>::reverse_iterator itor = strWideTemp.rbegin(); itor != strWideTemp.rend() ; itor++)
	{
		if (indexTemp < 6)
		{
			indexTemp++;
			cout << "宽度 = " << *itor << endl;
		}
		else 
		{
			cout << "宽度 = " << *itor << endl;
			Img_Mix_Wide = (int)*itor;
			break;
		}
	}
	
	// 将所有超过最小宽度的有效字符存到一个vector中
	vector <double> endWide;
	for (vector<double>::iterator itor = strWide.begin(); itor != strWide.end(); itor++)
	{
		if (*itor >= Img_Mix_Wide)
		{
			endWide.push_back(*itor);
		}
	}

	// 获取有效字符开始的x轴位置
	for (int i = 0, j = 0; i < count; i++)
	{
		if (roiWides[i] >= Img_Mix_Wide)
		{
			positionX1[j] = positionReal[i];
			positionX2[j] = positionEmpty[i];
			++j;
		}
	}

	
	// 画出所有的有效字符，并按顺序排列
	cout << "=======得出的有效矩形的宽度=======" << endl;
	int indexI = 0;
	Mat licensenImg = Mat(Scalar(0));
	for (vector<double>::iterator itor = endWide.begin() ; itor != endWide.end() ; itor++)
	{
		cout << " [ " << *itor << " ] " << endl;
		Rect tempRect(positionX1[indexI], 0, (int)*itor, roiThreadImg.rows);
		cout << "PositionX = " << positionX1[indexI] << endl;
		indexI++;
		licensenImg = roiThreadImg(tempRect);
		imshow("result11" + indexI, licensenImg);
		ostringstream oss;
		oss << "licenseIma" << indexI << ".jpg";
		imwrite(oss.str(), licensenImg);

	}

	// 用聚类算法 K-means聚类算法实现分开字符 2019-12-11

	// DBSCAN聚类算

	// 13日更新！！！

	// 14更新

	// 15  要及时

	waitKey(0);
	return 0;
}