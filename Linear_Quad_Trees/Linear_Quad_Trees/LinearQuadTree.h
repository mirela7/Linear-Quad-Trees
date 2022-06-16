#pragma once
#include <iostream>
#include <bitset>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

#define MAX_BITS 32
#define zeros_(x) ( (x ^ (x - 1)) & x )
#define T(x) log2(zeros_(x))/2-1

class LinearQuadTree
{
public:
	LinearQuadTree();
	LinearQuadTree(Mat);

	class MortonBlock {
		bitset<MAX_BITS> row;
		bitset<MAX_BITS> column;

	public:
		bitset<MAX_BITS> depth;
		int code, dep;
		MortonBlock(int c): code(c){}
		MortonBlock(std::pair<int, int>, int depth, int);
		pair<int, int> getRowAndColumn();
		static pair<int, int> getRowAndColumnFromCombinedCode(long long);
		void createMortonCode(std::pair<int, int>);
		long long createFinalMortonCode();
	};
	
	vector<MortonBlock> LQT;
};
