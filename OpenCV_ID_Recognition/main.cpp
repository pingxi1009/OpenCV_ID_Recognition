#include <opencv.hpp>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>

using namespace std;
using namespace cv;

const int DifMax = 40;	// �������ɫƫ
const int origin_R = 2;	// ԭʼ���ƹ̶�RGB
const int origin_G = 85;
const int origin_B = 196;

int main()
{
	Mat originImg;

	originImg = imread("test8.jpg", IMREAD_COLOR);	// ��ȡͼ��
	if (originImg.empty())	// �ж϶�ȡͼƬ�Ƿ�ɹ�
	{
		cout << "ͼ���ʧ��" << endl;
		return -1;
	}

	//imshow("ԭͼ", originImg);	// ��ʾͼƬ
	cout << "width = " << originImg.rows << "   colum = " << originImg.cols << endl;

	Mat resizeImgOri;	// �޸�ͼƬΪ�̶��ߴ�
	if (originImg.cols > 640)
	{
		resize(originImg, resizeImgOri, Size(640, 640 * originImg.rows / originImg.cols));
	}
	imshow("�ߴ�任���ͼ", resizeImgOri);

	Mat resizeImg = resizeImgOri.clone();

	Mat grayImg, gussImg;
	cvtColor(resizeImg, grayImg, COLOR_RGB2GRAY);		// �һ�

#if 1		// ��һ�ַ����������ԱȽϺã������Ƽ�

	GaussianBlur(grayImg, gussImg, Size(3, 3), 3, 0);	//��˹�����˲�
	Mat candyImage;
	//Canny(gussImg, candyImage, 250, 70, 3);			// ��Ե���
	Canny(gussImg, candyImage, 300, 200, 3);			// ��Ե���
	imshow("��Ե�����ͼ", candyImage);

	// ��̬ѧ�Ĵ���
	Mat dilateImg, erodeImg;
	Point m_point(-1, -1);
	Mat eleMentX = getStructuringElement(MORPH_RECT, Size(19, 1));	// ������̬ѧ�����ڴ�С
	dilate(candyImage, dilateImg, eleMentX, m_point, 2);	// ������Ͳ���
	erode(dilateImg, erodeImg, eleMentX, m_point, 4);		// ���ж�θ�ʴ
	//imshow("һ����̬ѧ����֮��", erodeImg);

	Mat eleMentY = getStructuringElement(MORPH_RECT, Size(1, 15));	// ������̬ѧ�����ڴ�С
	dilate(erodeImg, dilateImg, eleMentX, m_point, 2);
	erode(dilateImg, erodeImg, eleMentY, m_point, 1);

	//imshow("������̬ѧ����֮�� ����ǰ", erodeImg);
	dilate(erodeImg, dilateImg, eleMentY, m_point, 2);
	//erode(dilateImg, erodeImg, eleMentY, m_point, 1);
	//dilate(erodeImg, dilateImg, eleMentX, m_point, 2);
	//imshow("������̬ѧ����֮�� ���ͺ�", dilateImg);

	Mat blurrImg;
	medianBlur(dilateImg, blurrImg, 15);
	medianBlur(blurrImg, blurrImg, 15);
	imshow("������̬ѧ����֮��", blurrImg);

#else	// �ڶ��ַ�����ֻ��ʶ�������ض���ͼƬ�������Խϲ���Ƽ���������

	// ��ֵ������
	int img_R, img_G, img_B;	// RGB����ͨ����ֵ
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

	Mat blurrImg;
	//Mat eleMent = getStructuringElement(MORPH_RECT, Size(2, 2));	// ������̬ѧ�����ڴ�С
	dilate(resizeImg, blurrImg, eleMent);	// ������Ͳ���
	dilate(resizeImg, blurrImg, eleMent);	// ������Ͳ���
	dilate(resizeImg, blurrImg, eleMent);	// ������Ͳ���
	dilate(resizeImg, blurrImg, eleMent);	// ������Ͳ���

	erode(blurrImg, blurrImg, eleMent);	// ���ж�θ�ʴ
	erode(blurrImg, blurrImg, eleMent);	// ���ж�θ�ʴ
	erode(blurrImg, blurrImg, eleMent);	// ���ж�θ�ʴ
	

	//imshow("��ֵ�������ͼ", resizeImg);
	imshow("��̬ѧ�����ͼƬ2", blurrImg);

#endif

	if (blurrImg.channels() != 1)	// ����Ѿ��ǻһ���ͼ������Ҫ�һ�
	{
		cvtColor(blurrImg, blurrImg, COLOR_RGB2GRAY);
	}

	// �ҵ�����
	Mat coutourImg, inImg;
	coutourImg = blurrImg.clone();
	vector<vector<Point> > contours;
	coutourImg.convertTo(inImg, CV_8UC1);	// ��ͼƬת��Ϊ�̶���ʽ
	findContours(inImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);	// �ҵ���״

	// ��������
	drawContours(coutourImg, contours, -1, Scalar(0, 255, 0), 1);

	// ������ʾΪһ������ ������ȡ
	Mat roiImg;
	vector<Point> rectPoint;
	for (int i = 0 ; i < contours.size() ; i++)	// ���������ҵ��Ľ�����ҵ����ϳ��������ľ���
	{
		Rect curRect = boundingRect(Mat(contours[i]));
		cout << "contours " << i << " height = " << curRect.height << "   width = " << curRect.width << endl;

		// ���εĳ���ȣ�1.8-8��&& ���������3000-55000��
		if ((float)curRect.width / curRect.height >= 2.0 && (float)curRect.width / curRect.height <= 8)
		{
			cout << "=====Area = " << curRect.width * curRect.height << "=====" << endl;
			if (((curRect.width * curRect.height) > 4800) && (curRect.width * curRect.height) < 55000 )
			{
				cout << "-----------------�ҵ�����������-------------" << endl;
				cout << "R.x = " << curRect.x << "  R.y = " << curRect.y << endl;
				rectangle(coutourImg, curRect, Scalar(0, 0, 255), 3);
				roiImg = resizeImgOri(curRect);	// �ҵ��˳���λ��
				imshow("��̬ѧ�����ͼƬ3", roiImg);	// �ҵ��˳���λ��
			}
			
		}
	}
	
	if (!roiImg.data)
	{
		cout << "---------------------------------------" << endl;
		cout << "----------δ�ҵ�ƥ��ľ��Σ���---------" << endl;
		cout << "---------------------------------------" << endl;
		waitKey(0);
		return -1;
	}

	Mat canImg;
	Canny(roiImg, canImg, 450, 120, 3);
	//imshow("��̬ѧ�����ͼƬ4", canImg);

	// ��ֵ��
	Mat roiThreadImg;
	threshold(canImg, roiThreadImg, 50, 255, THRESH_BINARY);
	imshow("��̬ѧ�����ͼƬ5", roiThreadImg);
	
	// �ָ��ַ�
	// 1����ֱ�����ϵ�ͶӰ
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
			uchar m_temple = roiThreadImg.at<uchar>(j, i);	// y���x��Ҫת��һ�£���Ȼ�����ڴ�й©������
			//shuzhiShadow[i] = 0;
			//cout << "-Debug-" << roiThreadImg.at<uchar>(i, j) << endl;
			if (m_temple > 0)
			{
				shuzhiShadow[i] += 1;
				jishuconut++;	// ͳ��ÿһ�е��ж�����ֵ�����ص�
			}
		}
	}

	cout << "youduoshao  = " << jishuconut << endl;
	// ����������˲�
	for (int i = 0 ; i < roiCol - 1 ; i++)
	{
		if (shuzhiShadow[i] >= 4)	// һ�е���4����������Ϊ����Ч�ַ�
		{
			shuzhiShadow[i] = 1;
		}
		else if(shuzhiShadow[i] <= 3)	// һ�е�����3��������Ϊ������Ч�ַ�
		{
			shuzhiShadow[i] = 0;
		}
	}

	// ȷ���ַ���λ��
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

	// ��¼�����ַ��Ŀ��
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

	// ��ȡ�����ַ�����С���
	strWideTemp = strWide;
	int Img_Mix_Wide = 0;	// ��ʼ�����Ƶ���С���
	sort(strWideTemp.begin(), strWideTemp.end());
	int indexTemp = 0;
	for (vector<double>::reverse_iterator itor = strWideTemp.rbegin(); itor != strWideTemp.rend() ; itor++)
	{
		if (indexTemp < 6)
		{
			indexTemp++;
			cout << "��� = " << *itor << endl;
		}
		else 
		{
			cout << "��� = " << *itor << endl;
			Img_Mix_Wide = (int)*itor;
			break;
		}
	}
	
	// �����г�����С��ȵ���Ч�ַ��浽һ��vector��
	vector <double> endWide;
	for (vector<double>::iterator itor = strWide.begin(); itor != strWide.end(); itor++)
	{
		if (*itor >= Img_Mix_Wide)
		{
			endWide.push_back(*itor);
		}
	}

	// ��ȡ��Ч�ַ���ʼ��x��λ��
	for (int i = 0, j = 0; i < count; i++)
	{
		if (roiWides[i] >= Img_Mix_Wide)
		{
			positionX1[j] = positionReal[i];
			positionX2[j] = positionEmpty[i];
			++j;
		}
	}

	
	// �������е���Ч�ַ�������˳������
	cout << "=======�ó�����Ч���εĿ��=======" << endl;
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

	// �þ����㷨 K-means�����㷨ʵ�ַֿ��ַ� 2019-12-11

	// DBSCAN������

	// 13�ո��£�����

	// 14����

	// 15  Ҫ��ʱ

	waitKey(0);
	return 0;
}