#pragma once
#include <vector>
#include <iostream>

template<class T>
class StaticVector
{
private:
	T* vec;
public:
	int capacity = 0;
	int size = 0;
	
	StaticVector(const StaticVector<T>& cpy) {
		if (this == &cpy) return;
		else if(capacity != cpy.capacity) {
			if (capacity != 0) delete[] vec;
			capacity = cpy.capacity;
			vec = new T[capacity+5];
			size = cpy.size;
		}
		size = cpy.size;
		for (int i = 0; i < size; i++)
			vec[i] = cpy.vec[i];
	}

	StaticVector(int alloc) {
		vec = new T[alloc];
		capacity = alloc;
		size = 0;
	};
	StaticVector() {
		vec = nullptr;
		capacity = 0;
		size = 0;
	}

	void reserve(int alloc) {
		if (vec) delete[] vec;
		capacity = alloc;
		vec = new T[alloc];
		size = 0;
	}
	
	void insert(int pos, T who) {
		if (pos > size) {
			std::cout << "illegal insertion";
		}
		for (int i = size; i > pos; --i)
			vec[i] = vec[i - 1];
		vec[pos] = who;
		++size;
	}

	T& operator[](int pos) {
		if (pos < 0 || pos >= size) std::cout << "index out of bounds";
		return vec[pos];
	}

	T at(int pos) {
		if (pos < 0 || pos >= size) std::cout << "index out of bounds";
		return vec[pos];
	}

	StaticVector<T>& operator=(StaticVector<T> cpy) {
		if (this == &cpy) return *this;
		if (capacity != cpy.capacity) {
			if (capacity != 0) delete[] vec;
			capacity = cpy.capacity;
			vec = new T[capacity];
			size = cpy.size;
		}
		size = cpy.size;
		for (int i = 0; i < size; i++)
			vec[i] = cpy.vec[i];
		return *this;
	}

	void push_back(T who) {
		if (size < capacity)
			vec[size++] = who;
		else std::cout << "illegal push_back";
	}

	void popAt(int pos) {
		if (pos >= size || pos < 0) return;
		if (pos == size - 1) { size--; return; }

		for (int i = pos; i < size - 1; i++)
			vec[i] = vec[i + 1];
		size--;
	}

	~StaticVector() {
		if(vec)
			delete[] vec;
	}
};

