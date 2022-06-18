#include "LinearQuadTree.h"

LinearQuadTree::LinearQuadTree()
{
	LQT.clear();
}

LinearQuadTree::LinearQuadTree(Mat img)
{
	int rows = img.rows;
	int cols = img.cols;
	int size = rows;
	for (int i = 0; i < rows*cols; i+=4) {
		auto poz1 = MortonBlock::getRowAndColumnFromCombinedCode(i);
		bool ok = true;
		if (img.at<unsigned char>(poz1.first, poz1.second) != 0)
			ok = false;
		for (int j = 1; j <= 3; j++) {
			auto poz2 = MortonBlock::getRowAndColumnFromCombinedCode(i+j);
			if (img.at<unsigned char>(poz1.first, poz1.second) != img.at<unsigned char>(poz2.first, poz2.second)) {
				ok = false;
				break;
			}
		}
		if (ok == true) {
			if (img.at<unsigned char>(poz1.first, poz1.second) == 0)
				LQT.emplace_back(poz1, log2(size) - 1, i);
		}
		else {
			for (int j = 0; j <= 3; j++) {
				auto poz = MortonBlock::getRowAndColumnFromCombinedCode(i + j);
				if (img.at<unsigned char>(poz.first, poz.second) == 0)
					LQT.emplace_back(poz, log2(size)-1, i+j);
			}
		}

		// can we collapse the stack?
		if (ok == true && LQT.size() >= 4 && (LQT.back().code + 4) % 4 == 0) {
			int number_collapse = T(LQT.back().code+4);

			// condense the T-steps
			for (int L = 1; L <= number_collapse; L++) {
				int len = LQT.size()-1;
				ok = true; // can compress?
				for (int l = 0; l < 3; l++) {
					if (LQT[len - l].code - LQT[len - l - 1].code != 1 << (2 * L)) { // cannot compress, size incompatibility
						ok = false;
						break;
					}
				}
				if (ok == true) {
					MortonBlock compressed = LQT[len - 3];
					compressed.dep -= 1; compressed.depth = bitset<MAX_BITS>(compressed.dep);
					// pop the kids
					for (int i = 0; i < 4; i++, LQT.pop_back());

					// add the new value:
					LQT.push_back(compressed);
				}
				else
					break;
			}
		}
		
	}
}

LinearQuadTree::MortonBlock::MortonBlock(std::pair<int, int> pos, int d, int c)
{
	row = row | bitset<MAX_BITS>(pos.first);
	column = column | bitset<MAX_BITS>(pos.second);

	depth = bitset<MAX_BITS>(d);
	code = c;
	dep = d;
}

LinearQuadTree::MortonBlock::MortonBlock(const MortonBlock& cpy)
{
	for (int i = 0; i < MAX_BITS; ++i) {
		row[i] = cpy.row[i];
		column[i] = cpy.column[i];
		depth[i] = cpy.depth[i];
	}
	code = cpy.code;
	dep = cpy.dep;
}


pair<int, int> LinearQuadTree::MortonBlock::getRowAndColumnFromCombinedCode(long long cod)
{
	bitset<MAX_BITS> r, c;
	int last_poz = 0;
	for (; cod != 0; cod /= 4) {
		c[last_poz] = cod % 2;
		r[last_poz] = cod / 2 % 2;
		last_poz++;
	}

	return pair<int, int>(r.to_ulong(), c.to_ulong());
}

std::ostream& operator<<(std::ostream& g, LinearQuadTree::MortonBlock b)
{
	g << "MBlock[c: " << b.code << " dep: " << b.dep << "]";
	return g;
}
