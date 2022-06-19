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
BpTree::dummy_iterator BpTree::find(long long key)
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
	//if (*it != key)
	//	return dummy_iterator(act, -1);
	indexInDescent = std::distance(act->keys.begin(), it);
	return dummy_iterator(act, indexInDescent);
}
void BpTree::insert(MortonBlock b)
{
	if (root == nullptr) {
		//root = new BpTree::Node(BpTree::Node::State::LEAF, b.code, b);
		root = new BpTree::Node(BpTree::Node::State::LEAF);
		root->keys.push_back(b.bindex);
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

			auto it = std::lower_bound(act->keys.begin(), act->keys.end(), b.bindex);
			int pos = std::distance(act->keys.begin(), it);
			act->keys.insert(it, b.bindex);
			act->links.data.dataBlocks.insert(pos, b);
			return;
		}
		else {
			auto it = std::lower_bound(act->keys.begin(), act->keys.end(), b.bindex);
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
			std::cout << "<MD: " << act->links.data.dataBlocks.at(i).code << " d" << act->links.data.dataBlocks.at(i).dep << " BC:" << act->links.data.dataBlocks.at(i).bindex << "> ";
		std::cout << ")";
		act = act->links.data.next;
	}
}

bool BpTree::enclosed(vector<pair<int, int>> w, long long b, int len) {
	// is b enclosed in w?
	// w -> NW, NE, SE, SW
	pair<int, int> poz = LinearQuadTree::MortonBlock::getRowAndColumnFromCombinedCode((b >> 4));
	if (poz.first >= w[0].first && poz.first <= w[2].first && poz.first + len - 1 >= w[0].first && poz.first + len - 1 <= w[2].first)
		if (poz.second >= w[0].second && poz.second <= w[2].second && poz.second + len - 1 >= w[0].second && poz.second + len - 1 <= w[2].second)
			return true;
	return false;
}

bool BpTree::intersects(vector<pair<int, int>> w, long long b, int len) {
	// is b intersecting w?
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
vector<LinearQuadTree::MortonBlock> BpTree::windowQueryProcessing(vector<pair<int, int>> w, int dimImg, long long bin) {
	stack<long long> S;
	vector<LinearQuadTree::MortonBlock> sol;
	S.push(bin);
	while (!S.empty()) {
		long long b = S.top(); S.pop();
		int dep = b & 15;
		int len_block = 1 << ((int)log2(dimImg) - dep);
		if (enclosed(w, b, len_block))
		{

			pair<int, int> poz = LinearQuadTree::MortonBlock::getRowAndColumnFromCombinedCode((b >> 4));
			long long code1 = LinearQuadTree::MortonBlock::getCode({ poz.first + len_block - 1, poz.second + len_block - 1 });
			long long Mbmax = (code1 << 4) | 15;
			// retrieve blocks b - Mbmax
			auto it = find(b);
			if (it != end() && !it.isValid())
				++it;
			while (it.isValid() && it != end() && it.in->keys[it.index] < Mbmax) {
				sol.push_back(it.in->links.data.dataBlocks.at(it.index));
				++it;
			}


		}
		else {
			// search b in database

			pair<int, int> poz = LinearQuadTree::MortonBlock::getRowAndColumnFromCombinedCode((b >> 4));
			auto it = find(b);
			if (it.isValid() && it != end() && it.in->keys[it.index] == b) {
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
			bCodeKid = ((LinearQuadTree::MortonBlock::getCode({ poz.first, poz.second }) << 4) | (dep + 1));
			if (intersects(w, bCodeKid, len_block / 2)) {
				S.push(bCodeKid);
			}


		}
	}
	return sol;
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
