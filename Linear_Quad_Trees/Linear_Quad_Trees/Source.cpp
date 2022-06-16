#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "LinearQuadTree.h"

using namespace cv;
using namespace std;

int main()
{

	/*
	Mat image = Mat::zeros(300, 600, CV_8UC3);
	//circle(image, Point(250, 150), 100, Scalar(0, 255, 128), -100);
	//circle(image, Point(350, 150), 100, Scalar(255, 255, 255), -100);
	auto firts_l = image.ptr(0);

	imshow("Display Window", image);
	waitKey(0);*/

	Mat image = Mat::zeros(16, 16, CV_8U);
	rectangle(image, Point(2, 2), Point(7, 7), cv::Scalar(255), -1);
	rectangle(image, Point(8, 8), Point(15, 15), cv::Scalar(255), -1);
	bitwise_not(image, image);
	imshow("Display Window", image);
	waitKey(0);
	//int val = image.at<unsigned char>(1, 1);
	LinearQuadTree* l = new LinearQuadTree(image);
	for (auto &el : l->LQT) {
		cout << el.code << " " << el.dep << '\n';
	}
	return 0;
}