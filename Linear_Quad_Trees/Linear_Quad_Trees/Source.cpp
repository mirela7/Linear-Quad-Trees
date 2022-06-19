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
	Mat img = imread("img.png", CV_8U);
	int dimImg = 32;
	//bitwise_not(img, img);
	imshow("Display Window", img);
	waitKey(0);
	LinearQuadTree* l = new LinearQuadTree(img);
	for (auto& el : l->LQT) {
		pair<int, int> poz = LinearQuadTree::MortonBlock::getRowAndColumnFromCombinedCode(el.bindex >> 4);
		int dep = el.bindex & 15;
		int len_block = 1 << ((int)log2(dimImg) - dep);

		cout << el.bindex << " " << el.dep <<" " <<"("<<poz.first<<", "<<poz.second<<")"<< "(" << poz.first+len_block << ", " << poz.second+len_block << ")" << '\n';
	}

	BpTree tree;
	for (auto &x : l->LQT) {
		tree.insert(x);
	}
	tree.displayLeavesInAscendingOrder();
	vector<pair<int, int>> w({ {8, 0}, {1, 2}, {15,17}, {0, 0} });
	vector<LinearQuadTree::MortonBlock> sol = tree.windowQueryProcessing(w, 32);
	cout <<'\n' << sol.size() << '\n';
	for (auto& v : sol) {
		cout << v.bindex << " " << v.code << " " << v.dep << "\n";
	}

	Mat out = img.clone(); 
	cvtColor(out, out, COLOR_GRAY2BGR);
	
	for (auto& v : sol) {
		pair<int, int> poz = LinearQuadTree::MortonBlock::getRowAndColumnFromCombinedCode(v.bindex>>4);
		int dep = v.bindex & 15;
		int len_block = 1 << ((int)log2(dimImg) - dep);

		rectangle(out, Point(poz.second, poz.first), Point(poz.second + len_block - 1, poz.first+len_block-1), Scalar(0, 0, 255), -1);
	}

	rectangle(out, Point(w[0].second, w[0].first), Point(w[2].second, w[2].first), Scalar(0, 255, 128));
	imwrite("img_out.png", out);


	//BpTree tree;
	//vector<int> cds;
	//for (int i = 1; i <= 12 * 12; i++)
	//	cds.push_back(i);
	/*vector<int> cds = {3, 10, 20, 4, 5, 21, 6, 25, 11, 22};
	//vector<int> cds = { 1, 3, 7, 4};
	int i = 0;
	for (auto& code : cds)
	{
		LinearQuadTree::MortonBlock mb({i, i}, code, code);
		tree.insert(mb);
		/*std::cout << tree;
		sd::cout << "\n-----\n";
		
		std::cout << "\n";*/
		//tree.displayLeavesInAscendingOrder();
	//	i++;
	//}
	//std::cout << tree;
	//std::cout << "\n\n";
	//tree.displayLeavesInAscendingOrder();
	/*
	BpTree::dummy_iterator en = tree.find(22);
	std::cout << *en;
	for (auto it = tree.find(5); it != en; ++it)
		std::cout << *it << "\n";*/
	//std::cout << *tree.leftest() << "\n" << *tree.rightest();
	return 0;
}