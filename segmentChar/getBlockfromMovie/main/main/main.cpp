#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

//判断标准的定义
bool outLineSort(const vector<cv::Point>& former, const vector<cv::Point>& latter);
void removeBlock(vector<vector<cv::Point> >& pointSet);

int main()
{
	/*Mat imgSrc = imread("1.jpg", 1);
	imshow("sourceImg", imgSrc);*/

	//namedWindow("Windows");
	VideoCapture cap;
	cap.open("patient.avi");
	Mat frame;
	int n = 0;

	while (true)
	{
		cap >> frame;

		if (frame.empty())
			break;
		n++;

		Mat ROI = frame(Rect(749, 803, 45, 66));
		Mat img_gray;
		cvtColor(ROI, img_gray, COLOR_BGR2GRAY);
		threshold(img_gray, img_gray, 100, 255, THRESH_BINARY);
		//imshow("thresholdImg", img_gray);

		vector<vector<cv::Point> > contours;
		vector<Vec4i> hierarchy;
		findContours(img_gray, contours, hierarchy,
			RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		vector<Mat> imgCollect;
		//Mat img;

		sort(contours.begin(), contours.end(), outLineSort);
		removeBlock(contours);

		for (int i = 0; i < contours.size(); i++)
		{
			Rect rect = boundingRect(Mat(contours[i]));
			rectangle(ROI, rect.tl(), rect.br(), Scalar(255, 0, 0));
			Mat roi = ROI(rect);

			imgCollect.push_back(roi);
		}

		/*char name[50];
		sprintf(name, "%d.jpg", n);
		imwrite(name, ROI);*/

		char name[50];
		for (int i = 0; i < imgCollect.size(); i++)
		{
			sprintf(name, "%d - img%d.jpg", n, i);
			imwrite(name, imgCollect[i]);
		}

		//imshow("Windows", ROI);
		waitKey(30);
	}

	return 0;
}

bool outLineSort(const vector<cv::Point>& former, const vector<cv::Point>& latter)
{
	int former_level, latter_level;
	former_level = former[0].y / 15;
	latter_level = latter[0].y / 15;
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
		if ((*iter)[0].y >35 && (*iter)[0].y < 50)
		{
			iter = pointSet.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}