#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

//判断标准的定义
bool outLineSort(const vector<cv::Point>& former, const vector<cv::Point>& latter);

int main()
{
	Mat imgSrc = imread("1.jpg", 1);
	imshow("sourceImg", imgSrc);
	Mat img_gray;
	cvtColor(imgSrc, img_gray, COLOR_BGR2GRAY);
	threshold(img_gray, img_gray, 100, 255, THRESH_BINARY);
	imshow("thresholdImg", img_gray);

	vector<vector<cv::Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(img_gray, contours, hierarchy,
		RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<Mat> imgCollect;
	//Mat img;

	sort(contours.begin(), contours.end(), outLineSort);

	//for (int i = 0; i < contours.size(); i++)
	for (int i = 0; i < 16; i++)
	{
		Rect rect = boundingRect(Mat(contours[i]));
		rectangle(imgSrc, rect.tl(), rect.br(), Scalar(255, 0, 0));
		Mat roi = imgSrc(rect);

		//roi.convertTo(img, roi.type());
		imgCollect.push_back(roi);
	}

	char name[50];
	for (int i = 0; i < imgCollect.size(); i++)
	{
		sprintf(name, "img%d", i);
		imshow(name, imgCollect[i]);
	}
	
	imshow("contoursImg", imgSrc);
	waitKey(0);
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