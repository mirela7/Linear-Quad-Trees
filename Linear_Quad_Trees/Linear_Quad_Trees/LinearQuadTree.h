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
		long long bindex = 0;
		MortonBlock() { code = 0; dep = 0; }
		MortonBlock(int c): code(c){}
		MortonBlock(std::pair<int, int>, int depth, int);
		MortonBlock(const MortonBlock&);
		pair<int, int> getRowAndColumn();
		static pair<int, int> getRowAndColumnFromCombinedCode(long long);
		static long long getCode(pair<int, int>);
		void createMortonCode(std::pair<int, int>);
		long long createFinalMortonCode();

		friend std::ostream& operator<<(std::ostream& g, MortonBlock b);

	};
	
	vector<MortonBlock> LQT;
};

