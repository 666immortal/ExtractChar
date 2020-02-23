/*************************************************/
/*
  �������Դ��Ľ��ĵط���
  1����Ƶ�е�С��λ��ֻ����λ����������λ
  2���������������б�������һЩ���������ԣ������϶�Ϊ
       ���ܵ���Ƶ�����ȵ�Ӱ�죬�����������������Ƚ��ͣ���
	   �����������Щ����
  3��ʦ�ֽ����þ���ķ���ʵ���ַ�ʶ��
*/
/*************************************************/

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

//OpenCV3�������룺
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"

using namespace cv;
using namespace std;
// ����ѧϰ�����ռ�
using namespace ml;

// ����Ƶ����ȡ�����ַ�����ȡ��ע�� MOVIE_PLAY
// ��ͼ������ȡ�����ַ�����ע�͵�    MOVIE_PLAY
#define MOVIE_PLAY

// ������ͼ��Ŀ��
#define IMG_WIDTH  8
#define IMG_HEIGHT 8

// ��ȡ�����ַ����ռ�λ���ź��򣺴����ң����ϵ���
bool outLineSort(const vector<cv::Point>& former, const vector<cv::Point>& latter);
// �Ƴ�����Ҫ�Ĳ��֣�������Ϊ����Ҫ�Ĳ���
void removeBlock(vector<vector<cv::Point> >& pointSet);
// ͨ��svm�ж��ַ�����
bool getCharFromImg(Mat &sample, char &rst);

// ָ��֧����������ָ��
Ptr<ml::SVM> params;

int main(int argc, char* argv[])
{	
	// ��ʼ��֧��������������������ģ��
	params = ml::SVM::load("numchar_svm.xml");

#ifdef MOVIE_PLAY
	// ��ȡ��Ƶ�ı���
	VideoCapture cap;
	if (argc == 2)  // ��������������в�������򿪲���ָ������Ƶ
		cap.open(argv[1]);
	else
		// ��Ĭ����Ƶ
		cap.open("patient.avi");
	
	Mat frame;
	// ����ͼ����ʾ����
	namedWindow("SHOW", WINDOW_AUTOSIZE);

	while (cap.isOpened())
	{
		// ����Ƶ�ж�ȡһ֡
		cap >> frame;

		// �����Ƶ������
		if (frame.empty())
			break;

#else
	Mat frame = imread("3.jpg");
#endif // MOVIE_PLAY
		// ��ȡROI
		Mat ROI = frame(Rect(749, 803, 45, 66));
		Mat img_gray;
		// �Ҷ�
		cvtColor(ROI, img_gray, COLOR_BGR2GRAY);
		// ��ֵ��
		threshold(img_gray, img_gray, 100, 255, THRESH_BINARY);
		//imwrite("pic.jpg", ROI);

		// ��ȡ����
		vector<vector<cv::Point> > contours;
		vector<Vec4i> hierarchy;
		findContours(img_gray, contours, hierarchy,
			RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		// ��������������
		sort(contours.begin(), contours.end(), outLineSort);
		// ����������ɸѡ
		removeBlock(contours);

		// �������������ÿһ�еĽ��
		vector<Mat> imgCollect[3];

		for (int i = 0; i < contours.size(); i++)
		{
			// ���������ı߽�
			Rect rect = boundingRect(Mat(contours[i]));
			rectangle(ROI, rect.tl(), rect.br(), Scalar(255, 0, 0));
			Mat roi = ROI(rect);

			// ���ÿһ�����
			/*char tmp[50];
			sprintf(tmp, "numchar%d.jpg", i);
			imwrite(tmp, roi);*/
			
			// �ж��ַ���λ�ô�ŵ���Ӧ��������
			if (rect.tl().y < 16)
				imgCollect[0].push_back(roi);
			else if (rect.tl().y < 32)
				imgCollect[1].push_back(roi);
			else
				imgCollect[2].push_back(roi);
		}

		cv::String num_string[3];
		// ƴ������
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < imgCollect[i].size(); j++)
			{
				char temp;
				if (getCharFromImg(imgCollect[i][j], temp))
				{
					num_string[i] += temp;
				}
			}
			// ������һ��Ӧ���ǿ���ȥ����
			num_string[i] += '\0';
		}
				
#ifdef MOVIE_PLAY
		imshow("SHOW", frame);
		cout << num_string[0] << "\t";
		cout << num_string[1] << "\t";
		cout << num_string[2] << "\t";
		cout << endl;
		waitKey(20);
	}
#else
		cout << num_string[0] << endl;
		cout << num_string[1] << endl;
		cout << num_string[2] << endl;
#endif // MOVIE_PLAY

		waitKey(0);
	return 0;
}

bool outLineSort(const vector<cv::Point>& former, const vector<cv::Point>& latter)
{
	int former_level, latter_level;
	int center_y_former, center_y_latter;

	// ��ȡ�ַ������������
	Rect rect_fomer = boundingRect(Mat(former));
    center_y_former = (rect_fomer.tl().y + rect_fomer.br().y) / 2;

	Rect rect_latter = boundingRect(Mat(latter));
	center_y_latter = (rect_latter.tl().y + rect_latter.br().y) / 2;

	former_level = center_y_former / 17;
	latter_level = center_y_latter / 17;

	if (former_level < latter_level)
		return true;
	else if (former_level == latter_level && former[0].x < latter[0].x)
		return true;
	else
		return false;
}

void removeBlock(vector<vector<cv::Point> >& pointSet)
{
	for (vector<vector<cv::Point> >::iterator iter = pointSet.begin(); iter != pointSet.end();)
	{
		// ��ȡ�ַ������������
		Rect rect = boundingRect(Mat(*iter));
		int center_y = (rect.tl().y + rect.br().y) / 2;
		if (center_y > 32 && center_y < 50)
		{
			// ������ڵ����У����������Ƴ�
			iter = pointSet.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

bool getCharFromImg(Mat &sample, char &rst)
{
	// �������һ�����⣬�ȻҶ�����ȡroi���ַ����򣩺�����ȡroi�ٻҶȣ�svm�жϵĽ����һ��
	// Ӧ������ȡroi�ٻҶ�
	cvtColor(sample, sample, COLOR_BGR2GRAY);
	// ������С
	resize(sample, sample, cv::Size(IMG_WIDTH, IMG_HEIGHT), (0, 0), (0, 0), INTER_AREA);
	// �ı����ݸ�ʽ
	sample.convertTo(sample, CV_32FC1);
	// ת��Ϊ����
	sample = sample.reshape(0, 1);

	// ����֧������������Ԥ��
	int result = (int)params->predict(sample);
	
	// ����Ԥ���������ַ�
	if (result < 10)
	{
		rst = result + '0';
		return true;
	}
	else if (result == 10)
	{
		rst = '-';
		return true;
	}
	else if (result == 13)
	{
		rst = '.';
		return true;
	}
	else
		return false;
}