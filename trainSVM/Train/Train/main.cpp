/*************************************************
	本程序用于训练支持向量机，labels存放有顺序的标签
	img存放图像，图像与标签的顺序要一一对应
	训练是需要把vector型变量转换成Mat型，而且还要求
	是行向量
*************************************************/
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

//OpenCV3需额外加入：
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"

using namespace cv;
using namespace std;
using namespace cv::ml;

#define IMG_WIDTH  8
#define IMG_HEIGHT 8

// 用于读取文件夹中的图像
vector<Mat> readAllImage(cv::String pattern);
// 重新调整图像大小并转换为向量
void reshapeImg(vector<Mat> &img);

int main()
{
	cv::String pattern = "./trainset/*.jpg";
	vector<Mat> img = readAllImage(pattern);
	
	int labels[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,5 };
	int len = sizeof(labels) / sizeof(int);
	// 直接将标签输入Mat变量
	cv::Mat label(cv::Size(len, 1), CV_32SC1, labels);

	// 训练图片大小调整
	reshapeImg(img);

	// Set up SVM's parameters
	// 创建向量机
	Ptr<SVM> params = SVM::create();
	params->setType(SVM::C_SVC);
	params->setKernel(SVM::LINEAR);
	params->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));
	
	// 将vector转成Mat格式
	Mat train_mat;
	for (int i = 0; i < img.size(); i++)
	{
		train_mat.push_back(img[i]);
	}
	// 训练
	params->train(train_mat, ROW_SAMPLE, label);

	// 保存向量机
	//params->save("numchar_svm.xml");

	// 测试向量机
	cv::String pattern_test = "./testset/*.jpg";
	vector<Mat> img_test = readAllImage(pattern_test);
	//imwrite("mypic0.jpg", img_test[0]);
	reshapeImg(img_test);

	float result;
	for (int i = 0; i < img_test.size(); i++)
	{
		result = params->predict(img_test[i]);
		cout << "No." << i << "\'s result: " << result << endl;
	}
}

vector<Mat> readAllImage(cv::String pattern)
{
	vector<cv::String> fn;
	// recursive=false当recursive为false时，
	// 仅仅遍历指定文件夹内符合模式的文件；
	// 当recursive为true时，会同时遍历指定文件夹的子文件夹
	glob(pattern, fn, false);  
	vector<Mat> images;
	size_t count = fn.size();           //number of png files in images folder
	for (size_t i = 0; i < count; i++)
	{
		cout << fn[i] << endl;
		images.emplace_back(cv::imread(fn[i], cv::IMREAD_GRAYSCALE));
	}
	return images;
}

void reshapeImg(vector<Mat> &img)
{
	for (int i = 0; i < img.size(); i++)
	{
		resize(img[i], img[i], cv::Size(IMG_WIDTH, IMG_HEIGHT), (0, 0), (0, 0), INTER_AREA);
		// 归一化, 一般认为能加快参数优化时的收敛速度、平衡模型权重等
		// 但如果你的数据量级本身相差不大，也可以不归一化直接convertTo即可
		//cv::normalize(img[i], img[i], 1.0, 0.0, cv::NormTypes::NORM_MINMAX, CV_32FC1);
		img[i].convertTo(img[i], CV_32FC1);
		// 向量化
		img[i] = img[i].reshape(0, 1);
	}
}