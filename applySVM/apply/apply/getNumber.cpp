/*************************************************/
/*
  本程序仍待改进的地方：
  1、视频中的小数位不只有两位，而是有三位
  2、其他机器上运行本程序有一些结果结果不对（初步认定为
       是受到视频清晰度的影响，在其他机器中清晰度降低，检
	   测出的轮廓有些不对
  3、师兄建议用卷积的方法实现字符识别
*/
/*************************************************/

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

//OpenCV3需额外加入：
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"

using namespace cv;
using namespace std;
// 机器学习命名空间
using namespace ml;

// 从视频中提取数字字符，则取消注释 MOVIE_PLAY
// 从图像中提取数字字符，则注释掉    MOVIE_PLAY
#define MOVIE_PLAY

// 调整后图像的宽高
#define IMG_WIDTH  8
#define IMG_HEIGHT 8

// 将取出的字符按空间位置排好序：从左到右，由上到下
bool outLineSort(const vector<cv::Point>& former, const vector<cv::Point>& latter);
// 移除不需要的部分：第三行为不需要的部分
void removeBlock(vector<vector<cv::Point> >& pointSet);
// 通过svm判断字符内容
bool getCharFromImg(Mat &sample, char &rst);

// 指向支持向量机的指针
Ptr<ml::SVM> params;

int main(int argc, char* argv[])
{	
	// 初始化支持向量机，导入向量机模板
	params = ml::SVM::load("numchar_svm.xml");

#ifdef MOVIE_PLAY
	// 读取视频的变量
	VideoCapture cap;
	if (argc == 2)  // 如果有输入命令行参数，则打开参数指定的视频
		cap.open(argv[1]);
	else
		// 打开默认视频
		cap.open("patient.avi");
	
	Mat frame;
	// 声明图像显示窗口
	namedWindow("SHOW", WINDOW_AUTOSIZE);

	while (cap.isOpened())
	{
		// 从视频中读取一帧
		cap >> frame;

		// 如果视频读完了
		if (frame.empty())
			break;

#else
	Mat frame = imread("3.jpg");
#endif // MOVIE_PLAY
		// 提取ROI
		Mat ROI = frame(Rect(749, 803, 45, 66));
		Mat img_gray;
		// 灰度
		cvtColor(ROI, img_gray, COLOR_BGR2GRAY);
		// 二值化
		threshold(img_gray, img_gray, 100, 255, THRESH_BINARY);
		//imwrite("pic.jpg", ROI);

		// 提取轮廓
		vector<vector<cv::Point> > contours;
		vector<Vec4i> hierarchy;
		findContours(img_gray, contours, hierarchy,
			RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		// 对轮廓进行排序
		sort(contours.begin(), contours.end(), outLineSort);
		// 对轮廓进行筛选
		removeBlock(contours);

		// 三个容器各存放每一行的结果
		vector<Mat> imgCollect[3];

		for (int i = 0; i < contours.size(); i++)
		{
			// 划定轮廓的边界
			Rect rect = boundingRect(Mat(contours[i]));
			rectangle(ROI, rect.tl(), rect.br(), Scalar(255, 0, 0));
			Mat roi = ROI(rect);

			// 存放每一个结果
			/*char tmp[50];
			sprintf(tmp, "numchar%d.jpg", i);
			imwrite(tmp, roi);*/
			
			// 判断字符的位置存放到相应的容器中
			if (rect.tl().y < 16)
				imgCollect[0].push_back(roi);
			else if (rect.tl().y < 32)
				imgCollect[1].push_back(roi);
			else
				imgCollect[2].push_back(roi);
		}

		cv::String num_string[3];
		// 拼凑数字
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
			// 下面这一行应该是可以去掉的
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

	// 获取字符块的纵向中心
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
		// 获取字符块的纵向中心
		Rect rect = boundingRect(Mat(*iter));
		int center_y = (rect.tl().y + rect.br().y) / 2;
		if (center_y > 32 && center_y < 50)
		{
			// 如果处在第三行，从容器中移除
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
	// 这里存在一个问题，先灰度再提取roi（字符区域）和先提取roi再灰度，svm判断的结果不一致
	// 应该先提取roi再灰度
	cvtColor(sample, sample, COLOR_BGR2GRAY);
	// 调整大小
	resize(sample, sample, cv::Size(IMG_WIDTH, IMG_HEIGHT), (0, 0), (0, 0), INTER_AREA);
	// 改变数据格式
	sample.convertTo(sample, CV_32FC1);
	// 转化为向量
	sample = sample.reshape(0, 1);

	// 调用支持向量机进行预测
	int result = (int)params->predict(sample);
	
	// 根据预测结果返回字符
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