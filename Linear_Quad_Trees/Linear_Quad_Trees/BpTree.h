#pragma once
#include "LinearQuadTree.h"
#include "StaticVector.h"
#include <stack>
class BpTree
{
private:
	typedef LinearQuadTree::MortonBlock MortonBlock;
	const static inline int branchFactor = 2;
	const static inline int minKeys = branchFactor - 1;
	const static inline int maxKeys = 2*branchFactor - 1;

	struct Node {
		std::vector<int> keys;
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
		struct ptr {
			StaticVector<Node*> children;
			NodeData data;

			ptr() : children(maxKeys+1) {
				
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

	BpTree();
	BpTree(std::vector<MortonBlock> blocks);
	void feed(std::vector<MortonBlock> blocks);
	Node* find(int key);
	void insert(MortonBlock b);
	void deconstruct();
	~BpTree();

	void DFS(BpTree::Node*, std::deque<BpTree::Node*>&);
	void displayLeavesInAscendingOrder();
	
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
	std::cout << "elements In Range: " << posFirst << " -> " << posLast << "\nCopying: ";
	StaticVector<T> vec(of.capacity);
	for (int i = posFirst; i <= posLast; i++)
	{
		std::cout << of.at(i) << " ";
		vec.push_back(of.at(i));
	}
	std::cout << "\nReveived:\n";
	for (int i = 0; i < vec.size; i++)
		std::cout << vec.at(i) << " ";
	std::cout << "\n-----------\n";
	return vec;
}
