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

	imshow("ԭͼ", originImg);	// ��ʾͼƬ
	cout << "width = " << originImg.rows << "   colum = " << originImg.cols << endl;

	Mat resizeImg;
	if (originImg.cols > 640)
	{
		resize(originImg, resizeImg, Size(640, 640 * originImg.rows / originImg.cols));
	}
	imshow("�ߴ�任���ͼ", resizeImg);

	Mat grayImg, gussImg;
	cvtColor(resizeImg, grayImg, COLOR_RGB2GRAY);	// �һ�
	GaussianBlur(grayImg, gussImg, Size(3, 3), 3, 0); //��˹�����˲�

	Mat candyImage;
	Canny(gussImg, candyImage, 500, 200, 3);	// ��Ե���
	imshow("��Ե�����ͼ", candyImage);

	// ��̬ѧ�Ĵ���
	Mat binOriImg;
	Mat eleMent = getStructuringElement(MORPH_RECT, Size(2, 2));	// ������̬ѧ�����ڴ�С
	dilate(candyImage, binOriImg, eleMent);	// ������Ͳ���
	dilate(candyImage, binOriImg, eleMent);	// ������Ͳ���
	dilate(candyImage, binOriImg, eleMent);	// ������Ͳ���

	erode(binOriImg, binOriImg, eleMent);	// ���ж�θ�ʴ
	erode(binOriImg, binOriImg, eleMent);	// ���ж�θ�ʴ
	erode(binOriImg, binOriImg, eleMent);	// ���ж�θ�ʴ

	imshow("��̬ѧ�����ͼƬ1", binOriImg);

	// ��ֵ������
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

	//Mat binOriImg;
	//Mat eleMent = getStructuringElement(MORPH_RECT, Size(2, 2));	// ������̬ѧ�����ڴ�С
	dilate(resizeImg, binOriImg, eleMent);	// ������Ͳ���
	dilate(resizeImg, binOriImg, eleMent);	// ������Ͳ���
	dilate(resizeImg, binOriImg, eleMent);	// ������Ͳ���

	erode(binOriImg, binOriImg, eleMent);	// ���ж�θ�ʴ
	erode(binOriImg, binOriImg, eleMent);	// ���ж�θ�ʴ
	erode(binOriImg, binOriImg, eleMent);	// ���ж�θ�ʴ


	//imshow("��ֵ�������ͼ", resizeImg);
	imshow("��̬ѧ�����ͼƬ2", binOriImg);

	

	
	waitKey(0);
	return 0;
}