/*************************************************
	����������ѵ��֧����������labels�����˳��ı�ǩ
	img���ͼ��ͼ�����ǩ��˳��Ҫһһ��Ӧ
	ѵ������Ҫ��vector�ͱ���ת����Mat�ͣ����һ�Ҫ��
	��������
*************************************************/
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

//OpenCV3�������룺
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"

using namespace cv;
using namespace std;
using namespace cv::ml;

#define IMG_WIDTH  8
#define IMG_HEIGHT 8

// ���ڶ�ȡ�ļ����е�ͼ��
vector<Mat> readAllImage(cv::String pattern);
// ���µ���ͼ���С��ת��Ϊ����
void reshapeImg(vector<Mat> &img);

int main()
{
	cv::String pattern = "./trainset/*.jpg";
	vector<Mat> img = readAllImage(pattern);
	
	int labels[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,5 };
	int len = sizeof(labels) / sizeof(int);
	// ֱ�ӽ���ǩ����Mat����
	cv::Mat label(cv::Size(len, 1), CV_32SC1, labels);

	// ѵ��ͼƬ��С����
	reshapeImg(img);

	// Set up SVM's parameters
	// ����������
	Ptr<SVM> params = SVM::create();
	params->setType(SVM::C_SVC);
	params->setKernel(SVM::LINEAR);
	params->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));
	
	// ��vectorת��Mat��ʽ
	Mat train_mat;
	for (int i = 0; i < img.size(); i++)
	{
		train_mat.push_back(img[i]);
	}
	// ѵ��
	params->train(train_mat, ROW_SAMPLE, label);

	// ����������
	//params->save("numchar_svm.xml");

	// ����������
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
	// recursive=false��recursiveΪfalseʱ��
	// ��������ָ���ļ����ڷ���ģʽ���ļ���
	// ��recursiveΪtrueʱ����ͬʱ����ָ���ļ��е����ļ���
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
		// ��һ��, һ����Ϊ�ܼӿ�����Ż�ʱ�������ٶȡ�ƽ��ģ��Ȩ�ص�
		// ������������������������Ҳ���Բ���һ��ֱ��convertTo����
		//cv::normalize(img[i], img[i], 1.0, 0.0, cv::NormTypes::NORM_MINMAX, CV_32FC1);
		img[i].convertTo(img[i], CV_32FC1);
		// ������
		img[i] = img[i].reshape(0, 1);
	}
}