#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "LinearQuadTree.h"
#include "BpTree.h"
#include "StaticVector.h"

using namespace cv;
using namespace std;


union Test {
	StaticVector<int> stat;
	StaticVector<char> st2;

	Test(int a) : stat(2) {

	}
	Test(char c) : st2(5) {

	}
	~Test() {}
};


int main()
{
	/*
	Mat image = Mat::zeros(300, 600, CV_8UC3);
	//circle(image, Point(250, 150), 100, Scalar(0, 255, 128), -100);
	//circle(image, Point(350, 150), 100, Scalar(255, 255, 255), -100);
	auto firts_l = image.ptr(0);

	imshow("Display Window", image);
	waitKey(0);*/
	/*
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
	}*/


	/*std::vector<int> vec;
	vec.insert(std::lower_bound(vec.begin(), vec.end(), 1), 1);
	std::cout << vec[0];*/


	BpTree tree;
	//vector<int> cds = { 1, 3, 7, 4, 10, 11, 5, 2, 20, 25, 30, 6, 8 };
	//vector<int> cds = {3, 10, 20, 4, 5, 21, 6, 25, 11, 22};
	vector<int> cds = { 1, 3, 7, 4};
	int i = 0;
	for (auto& code : cds)
	{
		LinearQuadTree::MortonBlock mb({i, i}, code, code);
		tree.insert(mb);
		std::cout << tree;
		std::cout << "\n-----\n";
		
		std::cout << "\n";
		i++;
	}
	std::cout << tree;
	std::cout << "\n\n";
	tree.displayLeavesInAscendingOrder();
	return 0;
}