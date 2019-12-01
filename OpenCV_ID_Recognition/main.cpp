#include <opencv.hpp>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

using namespace std;
using namespace cv;

const int DifMax = 40;	// �������ɫƫ
const int origin_R = 2;	// ԭʼ���ƹ̶�RGB
const int origin_G = 85;
const int origin_B = 196;

int main()
{
	Mat originImg;
	int img_R, img_G, img_B;	// RGB����ͨ����ֵ

	originImg = imread("test.jpg", IMREAD_COLOR);	// ��ȡͼ��
	if (originImg.empty())	// �ж϶�ȡͼƬ�Ƿ�ɹ�
	{
		cout << "ͼ���ʧ��" << endl;
		return -1;
	}

	//imshow("ԭͼ", originImg);	// ��ʾͼƬ
	cout << "width = " << originImg.rows << "   colum = " << originImg.cols << endl;

	Mat resizeImgOri;
	if (originImg.cols > 640)
	{
		resize(originImg, resizeImgOri, Size(640, 640 * originImg.rows / originImg.cols));
	}
	imshow("�ߴ�任���ͼ", resizeImgOri);

	Mat resizeImg = resizeImgOri.clone();

	Mat grayImg, gussImg;
	cvtColor(resizeImg, grayImg, COLOR_RGB2GRAY);	// �һ�

#if 0

	GaussianBlur(grayImg, gussImg, Size(3, 3), 3, 0); //��˹�����˲�

	Mat candyImage;
	Canny(gussImg, candyImage, 300, 250, 3);	// ��Ե���
	imshow("��Ե�����ͼ", candyImage);

	// ��̬ѧ�Ĵ���
	Mat binOriImg;
	Mat eleMent = getStructuringElement(MORPH_RECT, Size(6, 3));	// ������̬ѧ�����ڴ�С
	dilate(candyImage, binOriImg, eleMent);	// ������Ͳ���
	dilate(candyImage, binOriImg, eleMent);	// ������Ͳ���
	dilate(candyImage, binOriImg, eleMent);	// ������Ͳ���
	dilate(candyImage, binOriImg, eleMent);	// ������Ͳ���
	dilate(candyImage, binOriImg, eleMent);	// ������Ͳ���

	erode(binOriImg, binOriImg, eleMent);	// ���ж�θ�ʴ
	erode(binOriImg, binOriImg, eleMent);	// ���ж�θ�ʴ
	
	Mat eleMent1 = getStructuringElement(MORPH_RECT, Size(12, 6));	// ������̬ѧ�����ڴ�С
	dilate(binOriImg, candyImage, eleMent1);	// ������Ͳ���
	dilate(binOriImg, candyImage, eleMent1);	// ������Ͳ���
	dilate(binOriImg, candyImage, eleMent1);	// ������Ͳ���
	dilate(binOriImg, candyImage, eleMent1);	// ������Ͳ���

	imshow("��̬ѧ�����ͼƬ1", binOriImg);
	imshow("��̬ѧ�����ͼƬ3", candyImage);
#endif

	// ��ֵ������
	Mat eleMent = getStructuringElement(MORPH_RECT, Size(8, 4));	// ������̬ѧ�����ڴ�С
	for (int i = 0 ; i < resizeImg.rows ; i++)
	{
		for (int j = 0 ; j < resizeImg.cols ; j++)
		{
			img_B = resizeImg.at<Vec3b>(i, j)[0];	// ��ȡͼƬ����ͨ����ֵ
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
	//Mat eleMent = getStructuringElement(MORPH_RECT, Size(2, 2));	// ������̬ѧ�����ڴ�С
	dilate(resizeImg, binOriImg, eleMent);	// ������Ͳ���
	dilate(resizeImg, binOriImg, eleMent);	// ������Ͳ���
	dilate(resizeImg, binOriImg, eleMent);	// ������Ͳ���
	dilate(resizeImg, binOriImg, eleMent);	// ������Ͳ���

	erode(binOriImg, binOriImg, eleMent);	// ���ж�θ�ʴ
	erode(binOriImg, binOriImg, eleMent);	// ���ж�θ�ʴ
	erode(binOriImg, binOriImg, eleMent);	// ���ж�θ�ʴ
	

	//imshow("��ֵ�������ͼ", resizeImg);
	imshow("��̬ѧ�����ͼƬ2", binOriImg);

	cvtColor(binOriImg, binOriImg, COLOR_RGB2GRAY);


	Mat coutourImg, inImg;
	coutourImg = binOriImg.clone();
	vector<vector<Point> > contours;
	coutourImg.convertTo(inImg, CV_8UC1);
	findContours(inImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);	// �ҵ���״

	// ��������
	drawContours(coutourImg, contours, -1, Scalar(0, 255, 0), 1);

	// ������ʾΪһ������ ������ȡ
	Mat roiImg;
	vector<Point> rectPoint;
	for (int i = 0 ; i < contours.size() ; i++)
	{
		Rect curRect = boundingRect(Mat(contours[i]));
		cout << "contours " << i << " height = " << curRect.height << "   width = " << curRect.width << endl;
		if ((float)curRect.width / curRect.height >= 2.2 && (float)curRect.width / curRect.height <= 3.6 && 
			((curRect.width * curRect.height) > 10000) && (curRect.width * curRect.height) < 15000)
		{
			cout << "-----------------�ҵ�����������-------------" << endl;
			cout << "R.x = " << curRect.x << "  R.y = " << curRect.y << endl;
			rectangle(coutourImg, curRect, Scalar(0, 0, 255), 3);
			roiImg = resizeImgOri(curRect);	// �ҵ��˳���λ��
		}
	}
	imshow("��̬ѧ�����ͼƬ3", roiImg);	// �ҵ��˳���λ��

	Mat canImg;
	Canny(roiImg, canImg, 450, 120, 3);
	//imshow("��̬ѧ�����ͼƬ4", canImg);

	// ��ֵ��
	Mat roiThreadImg;
	threshold(canImg, roiThreadImg, 50, 255, THRESH_BINARY);
	imshow("��̬ѧ�����ͼƬ5", roiThreadImg);
	

	
	waitKey(0);
	return 0;
}