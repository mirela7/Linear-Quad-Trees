#include "BpTree.h"
void BpTree::moveChildren(Node* from, Node* newParent, int start, int to)
{
	for (int i = start; i <= to; ++i)
	{
		newParent->links.children.push_back(from->links.children[i]);
		from->links.children[i]->parent = newParent;
	}
}
BpTree::dummy_iterator BpTree::begin()
{
	return leftest();
}
BpTree::dummy_iterator BpTree::end()
{
	dummy_iterator righ = rightest();
	righ.index++;
	return righ;
}
BpTree::BpTree()
{
	root = nullptr;
}
BpTree::dummy_iterator BpTree::leftest()
{
	Node* act = root;
	while (act->role != BpTree::Node::State::LEAF)
		act = act->links.children[0];
	return dummy_iterator(act, 0);
}
BpTree::dummy_iterator BpTree::rightest()
{
	Node* act = root;
	while (act->role != BpTree::Node::State::LEAF)
		act = act->links.children[act->links.children.size-1];
	return dummy_iterator(act, act->keys.size()-1);
}
BpTree::dummy_iterator BpTree::find(int key)
{
	int indexInDescent;
	BpTree::Node* act = root;
	while (act->role != BpTree::Node::State::LEAF)
	{
		auto it = std::upper_bound(act->keys.begin(), act->keys.end(), key);
		indexInDescent = std::distance(act->keys.begin(), it);
		act = act->links.children[indexInDescent];
	}
	auto it = std::lower_bound(act->keys.begin(), act->keys.end(), key);
	indexInDescent = std::distance(act->keys.begin(), it);
	return dummy_iterator(act, indexInDescent);
}
void BpTree::insert(MortonBlock b)
{
	if (root == nullptr) {
		//root = new BpTree::Node(BpTree::Node::State::LEAF, b.code, b);
		root = new BpTree::Node(BpTree::Node::State::LEAF);
		root->keys.push_back(b.code);
		root->links.data.dataBlocks.push_back(b);
		root->links.data.next = nullptr;
		return;
	}
	BpTree::Node* act = root;
	int indexInDescent = 0;
	
	while (true) {
		if (act->isFull()) {
			int mid = act->keys.size() / 2;
			
			BpTree::Node* parent;
			if (act->parent) parent = act->parent;
			else {
				parent = new BpTree::Node(BpTree::Node::State::INTERNAL);
			}

			BpTree::Node::State newstate = act->role;
			BpTree::Node* left = new BpTree::Node(newstate);
			BpTree::Node* right = new BpTree::Node(newstate);
			left->parent = parent;
			right->parent = parent;
			
			left->keys = elementsInRange(act->keys, 0, mid - 1);
			right->keys = elementsInRange(act->keys, (act->role==BpTree::Node::State::LEAF) ? mid : (mid + 1), act->keys.size() - 1);
			parent->keys.insert(parent->keys.begin() + indexInDescent, act->keys[mid]);
			
			

			if (parent->role == BpTree::Node::State::INTERNAL) {
				parent->links.children.popAt(indexInDescent);
				parent->links.children.insert(indexInDescent, right);
				parent->links.children.insert(indexInDescent, left);
			}

			if (act->role == BpTree::Node::State::INTERNAL) {
				moveChildren(act, left, 0, mid);
				moveChildren(act, right, mid + 1, maxKeys);
			}
			else {
				left->links.data.dataBlocks = elementsInRange(act->links.data.dataBlocks, 0, mid-1);
				right->links.data.dataBlocks = elementsInRange(act->links.data.dataBlocks, mid, maxKeys-1);
				left->links.data.next = right;
				right->links.data.next = nullptr;
				if (indexInDescent > 0)
					parent->links.children[indexInDescent - 1]->links.data.next = left;
				if (0 <= indexInDescent && indexInDescent+2 < parent->links.children.size)
					right->links.data.next = parent->links.children[indexInDescent + 2];
			}

			if (act == root) root = parent;

			Node* todel = act;
			act = parent;
			delete todel;

		}
		
		if (act->role == BpTree::Node::State::LEAF) {

			auto it = std::lower_bound(act->keys.begin(), act->keys.end(), b.code);
			int pos = std::distance(act->keys.begin(), it);
			act->keys.insert(it, b.code);
			act->links.data.dataBlocks.insert(pos, b);
			return;
		}
		else {
			auto it = std::lower_bound(act->keys.begin(), act->keys.end(), b.code);
			indexInDescent = std::distance(act->keys.begin(), it);
			act = act->links.children[indexInDescent];
		}
	}
}

void BpTree::deconstruct()
{
	std::deque<Node*> dfs;
	dfs.push_back(root);
	DFS(root, dfs);
	while (!dfs.empty())
	{
		auto front = dfs.front();
		dfs.pop_front();
		delete front;
	}
}

BpTree::~BpTree()
{
	deconstruct();
}

void BpTree::DFS(BpTree::Node* act, std::deque<Node*>& deq)
{
	if (act->role == BpTree::Node::State::INTERNAL)
	{
		for (int i = 0; i < act->links.children.size; i++)
		{
			deq.push_back(act->links.children[i]);
			DFS(act->links.children[i], deq);
		}
	}
}

void BpTree::displayLeavesInAscendingOrder()
{
	Node* act = root;
	while (act->role != BpTree::Node::State::LEAF)
		act = act->links.children[0];
	while (act != nullptr)
	{
		std::cout << "(";
		for (int i = 0; i < act->links.data.dataBlocks.size; i++)
			std::cout << "<c: " << act->links.data.dataBlocks.at(i).code << " d" << act->links.data.dataBlocks.at(i).dep << "> ";
		std::cout << ")";
		act = act->links.data.next;
	}
}

std::ostream& operator<<(std::ostream& g, BpTree& tree)
{
	std::deque<BpTree::Node*> dfs;
	dfs.push_front(tree.root);
	tree.DFS(tree.root, dfs);
	while (!dfs.empty())
	{
		auto front = dfs.front();
		dfs.pop_front();
		g << *front << "\n";
	}
	return g;
}
