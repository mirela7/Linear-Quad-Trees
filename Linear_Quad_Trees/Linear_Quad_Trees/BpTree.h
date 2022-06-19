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
	
	static bool enclosed(vector<pair<int, int>> w, long long b, int len);
	static bool intersects(vector<pair<int, int>> w, long long b, int len);
	vector<MortonBlock> windowQueryProcessing(vector<pair<int, int>> w, int dimImg, long long bin = 0);

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
