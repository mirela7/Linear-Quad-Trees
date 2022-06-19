#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "LinearQuadTree.h"
#include "BpTree.h"
#include "StaticVector.h"

using namespace cv;
using namespace std;

void MENU()
{
	std::pair<int, int> NW, SE;
	char c;
	string input = "b64sqpng.png";
	string output = "img64_out.png";
	
	std::cout << "Default path to input image: b64sqpng.png\nDefault path to output image: img64_out.png\n";
	std::cout << "Press [0] to change input path. Press [s] to skip.\n";
	std::cin >> c;
	if (c == '0') {
		std::cout << "Input path: ";
		std::cin >> input;
	}
	else std::cout << "Input path set to default.\n";

	std::cout << "Press[0] to change output path. Press[s] to skip.\n";
	std::cin >> c;
	if (c == '0') {
		std::cout << "Output path: ";
		std::cin >> output;
	}
	else std::cout << "Output path set to default.\n";
	string outputname(output, 0, output.size() - 4);
	Mat img = imread(input, CV_8U);
	int nmbRows = img.rows, nmbCols = img.cols;
	if (nmbRows != nmbCols || log2(nmbRows) != (int)log2(nmbRows)) {
		std::cout << "Invalid image. The image must be square of 2^n x 2^n.";
	}
	LinearQuadTree* l = new LinearQuadTree(img);
	BpTree tree;
	for (auto& x : l->LQT) {
		tree.insert(x);
	}


	std::cout << "Window query processing mode entered.";
	int index = 0;
	while (true)
	{
		std::cout << "\nType coordinates of upper left window to inspect (indexRow, indexColumn): ";
		std::cin >> NW.first >> NW.second;

		std::cout << "Type coordinates of lower right window to inspect (indexRow, indexColumn): ";
		std::cin >> SE.first >> SE.second;

		if (NW.first < 0 || NW.second < 0 || SE.first < 0 || SE.second < 0 || NW.first >= nmbRows || SE.first >= nmbRows || NW.second > nmbCols || SE.second > nmbCols)
		{
			std::cout << "Invalid input. Skipped. \n";
			continue;
		}
		
		vector<pair<int, int>> w({ NW, {1, 2}, SE, {0, 0} });
		vector<LinearQuadTree::MortonBlock> sol = tree.windowQueryProcessing(w, nmbRows);
		Mat out = img.clone();
		cvtColor(out, out, COLOR_GRAY2BGR);

		for (auto& v : sol) {
			pair<int, int> poz = LinearQuadTree::MortonBlock::getRowAndColumnFromCombinedCode(v.bindex >> 4);
			int dep = v.bindex & 15;
			int len_block = 1 << ((int)log2(nmbRows) - dep);
			rectangle(out, Point(poz.second, poz.first), Point(poz.second + len_block - 1, poz.first + len_block - 1), Scalar(0, 0, 255), -1);
		}

		rectangle(out, Point(w[0].second, w[0].first), Point(w[2].second, w[2].first), Scalar(0, 255, 128));
		if (index == 0)
			imwrite(output, out);
		else imwrite(outputname + to_string(index) + ".png", out);

		std::cout << "Image saved.\n";
		std::cout << "Press [x] to close, [c] to continue. \n";
		std::cin >> c;
		if (c == 'x') return;
		index++;
		
	}

}



int main()
{
	MENU();
	/*Mat img = imread("b64sqpng.png", CV_8U);
	int dimImg = 32;
	
	imshow("Display Window", img);
	waitKey(0);
	LinearQuadTree* l = new LinearQuadTree(img);
	BpTree tree;
	for (auto &x : l->LQT) {
		tree.insert(x);
	}
	
	vector<pair<int, int>> w({ {2, 2}, {1, 2}, {10, 26}, {0, 0} });
	vector<LinearQuadTree::MortonBlock> sol = tree.windowQueryProcessing(w, 32);
	Mat out = img.clone(); 
	cvtColor(out, out, COLOR_GRAY2BGR);
	
	for (auto& v : sol) {
		pair<int, int> poz = LinearQuadTree::MortonBlock::getRowAndColumnFromCombinedCode(v.bindex>>4);
		int dep = v.bindex & 15;
		int len_block = 1 << ((int)log2(dimImg) - dep);
		rectangle(out, Point(poz.second, poz.first), Point(poz.second + len_block - 1, poz.first+len_block-1), Scalar(0, 0, 255), -1);
	}

	rectangle(out, Point(w[0].second, w[0].first), Point(w[2].second, w[2].first), Scalar(0, 255, 128));
	imwrite("img64_out.png", out);*/
	return 0;
}