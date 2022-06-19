#pragma once
#include "LinearQuadTree.h"
#include "StaticVector.h"
#include <stack>
class BpTree
{
private:
	typedef LinearQuadTree::MortonBlock MortonBlock;
	const static inline int branchFactor = 5;
	const static inline int minKeys = branchFactor - 1;
	const static inline int maxKeys = 2*branchFactor - 1;

	struct Node {
		std::vector<long long> keys;
		Node* parent;

		enum class State : char
		{
			INTERNAL = 'i',
			LEAF = 'l'
		}role;

		struct NodeData {
			StaticVector<MortonBlock> dataBlocks;
			Node* next;

			NodeData() : dataBlocks(maxKeys+1) {
				next = nullptr;
			}
		};
		union ptr {
			StaticVector<Node*> children;
			NodeData data;

			ptr() {
				new(&children) StaticVector<Node*>(maxKeys + 1);
				new (&data) NodeData;
				
			};
			ptr(int) {

			};
			~ptr() {};
		} links;

		Node(State r = State::INTERNAL, int key = -1, MortonBlock value = -1, Node* dad = NULL) : links() {
			role = r;
			keys.reserve(maxKeys);
			/*if(key != -1)
				keys.push_back(key);
			if (role == State::LEAF && value.code != -1) {
				links.data.dataBlocks.push_back(value);
			}*/
			parent = dad;
		}
		void addValKey(MortonBlock b) {
			std::lower_bound(keys.begin(), keys.end(), b.code);
		}
		bool isFull() {
			return keys.size() == maxKeys;
		}
		friend std::ostream& operator<<(std::ostream& g, Node& n)
		{
			g << "[" << (char)n.role << "]\n{";
			for (auto& it : n.keys)
				g << it << " ";
			g << "}";
			if (n.role == State::LEAF) {
				g << "(";
				for (int i = 0; i < n.keys.size(); i++)
					g << n.links.data.dataBlocks[i].code << " ";
				g << ")";
			}
			return g;
		}
	}*root;

	

	template<class T>
	std::vector<T> elementsInRange(std::vector<T>& of,int posFirst, int posLast);

	template<class T>
	StaticVector<T> elementsInRange(StaticVector<T>& of, int posFirst, int posLast);

	void moveChildren(Node* from, Node* newParent, int start, int to);

public:
	struct dummy_iterator {
		BpTree::Node* in;
		int index = 0;

		dummy_iterator(Node* nod, int idx = 0) {
			in = nod;
			index = idx;
		}
		dummy_iterator(const dummy_iterator& it) {
			in = it.in;
			index = it.index;
		}

		dummy_iterator operator++() {
			if (in->links.data.next) {
				if (index < in->links.data.dataBlocks.size - 1)
					index++;
				else {
					in = in->links.data.next;
					index = 0;
				}
			}
			else index++;
			return *this;
		}

		bool isValid() {
			if (index >= 0 && index < in->keys.size())
				return true;
			return false;
		}

		MortonBlock operator*() {
			return in->links.data.dataBlocks.at(index);
		}

		bool operator!=(dummy_iterator& other) {
			return !(in == other.in && index == other.index);
		}
		bool operator==(dummy_iterator& other) {
			return in == other.in && index == other.index;
		}
	};

	dummy_iterator begin();
	dummy_iterator end();


	BpTree();
	BpTree(std::vector<MortonBlock> blocks);
	dummy_iterator leftest();
	dummy_iterator rightest();
	dummy_iterator find(long long key);
	void insert(MortonBlock b);
	void deconstruct();
	~BpTree();

	void DFS(BpTree::Node*, std::deque<BpTree::Node*>&);
	void displayLeavesInAscendingOrder();
	
	static bool enclosed(vector<pair<int, int>> w, long long b, int len) { 
		// is b enclosed in w?
		// w -> NW, NE, SE, SW
		pair<int, int> poz = LinearQuadTree::MortonBlock::getRowAndColumnFromCombinedCode((b>>4));
		if (poz.first >= w[0].first && poz.first <= w[2].first && poz.first + len -1 >= w[0].first && poz.first + len -1<= w[2].first)
			if (poz.second >= w[0].second && poz.second <= w[2].second && poz.second + len -1 >= w[0].second && poz.second + len -1 <= w[2].second)
				return true;
		return false;
	}

	static bool intersects(vector<pair<int, int>> w, long long b, int len) {
		// is b enclosed in w?
		// w -> NW, NE, SE, SW
		pair<int, int> poz = LinearQuadTree::MortonBlock::getRowAndColumnFromCombinedCode((b >> 4));
		int depth = b & 15;
		if (poz.second >= w[2].second)
			return false;
		if (poz.second + len <= w[0].second)
			return false;
		if (poz.first + len <= w[0].first)
			return false;
		if (poz.first >= w[2].first)
			return false;
		return true;
	}
	vector<MortonBlock> windowQueryProcessing(vector<pair<int, int>> w, int dimImg, long long bin=0) {
		stack<long long> S;
		vector<MortonBlock> sol;
		S.push(bin);
		while (!S.empty()) {
			long long b = S.top(); S.pop();
			int dep = b & 15;
			int len_block = 1 << ((int)log2(dimImg) - dep);
			if (enclosed(w, b, len_block))
			{
				
				pair<int, int> poz = LinearQuadTree::MortonBlock::getRowAndColumnFromCombinedCode((b >> 4));
				long long code1 = LinearQuadTree::MortonBlock::getCode({poz.first + len_block-1, poz.second + len_block-1});
				long long Mbmax = (code1 << 4) | 15;
				// retrieve blocks b - Mbmax
				auto it = find(b);
				if (it!=end() && !it.isValid())
					++it;
				while (it.isValid() && it!=end() && it.in->keys[it.index] < Mbmax) {
					sol.push_back(it.in->links.data.dataBlocks.at(it.index));
					++it;
				}
				

			}
			else {
				// search b in database

				pair<int, int> poz = LinearQuadTree::MortonBlock::getRowAndColumnFromCombinedCode((b >> 4));
				auto it = find(b);
				if (it.isValid() && it!=end() && it.in->keys[it.index] == b) {
					sol.push_back(it.in->links.data.dataBlocks.at(it.index));
					continue;
				}
				long long bCodeKid;
				//SE kid
				bCodeKid = ((LinearQuadTree::MortonBlock::getCode({ poz.first + len_block / 2, poz.second + len_block / 2 }) << 4) | (dep + 1));
				if (intersects(w, bCodeKid, len_block / 2)) {
					S.push(bCodeKid);
				}

				//SW kid
				bCodeKid = ((LinearQuadTree::MortonBlock::getCode({ poz.first + len_block / 2  , poz.second }) << 4) | (dep + 1));
				if (intersects(w, bCodeKid, len_block / 2)) {
					S.push(bCodeKid);
				}

				//NE kid
				bCodeKid = ((LinearQuadTree::MortonBlock::getCode({ poz.first, poz.second + len_block / 2 }) << 4) | (dep + 1));
				if (intersects(w, bCodeKid, len_block / 2)) {
					S.push(bCodeKid);
				}

				// NW kid
				bCodeKid = ((LinearQuadTree::MortonBlock::getCode({ poz.first, poz.second })<<4) | (dep+1));
				if (intersects(w, bCodeKid, len_block/2)) {
					S.push(bCodeKid);
				}
				

			}
		}
		return sol;
	}


	friend std::ostream& operator<<(std::ostream& g, BpTree& tree);
};

template<class T>
inline std::vector<T> BpTree::elementsInRange(std::vector<T>& of, int posFirst, int posLast)
{
	return std::vector<T>(of.begin() + posFirst, of.begin() + posLast+1);
}

template<class T>
StaticVector<T> BpTree::elementsInRange(StaticVector<T>& of, int posFirst, int posLast)
{
	StaticVector<T> vec(of.capacity);
	for (int i = posFirst; i <= posLast; i++)
	{
		vec.push_back(of.at(i));
	}
	return vec;
}
