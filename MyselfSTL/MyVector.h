#pragma once
#include<memory>
#include<algorithm>
#include<cstddef>
template<class T>
class MyVector
{
public:
	typedef T value_type;
	typedef value_type* pointer;
	typedef value_type* iterator;
	typedef value_type& reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

protected:
	//typedef simple_alloc<value_type, Alloc> data_allocator;
	iterator start; //空间的头
	iterator finish; //空间的尾
	iterator end_of_storage; //当前申请的空间的尾

	void insert_aux(iterator position, const T& value);
	void construct(iterator position, const T& value);
	void deallocate() {
		if (start)delete start;
	}

	void fill_initialize(size_type n, const T& value) {
		start = new T[n];
		finish = start + n;
		for (auto i = start; i != finish; ++i)
			*i = value;
		end_of_storage = end;
	}

public:
	iterator begin() { return begin; }
	iterator end() { return finish; }
	size_type size() const { return size_type(end() - start()); }
	size_type capacity() const {
		return size_type(end_of_storage - start());}
	bool empty() { return end() == start(); }
	reference operator[](size_type n) { return *(begin() + n); }

	//构造函数
	MyVector() :begin(0), end(0), end_of_storage(0) {}
	MyVector(int n, const T& value) { fill_initialize(n, value); }
	MyVector(long n, const T& value) { fill_initialize(n, value); }
	explicit MyVector(size_type n) { fill_initialize(n, T()); }

	~MyVector() {
		delete end();
		delete end_of_storage;
		deallocate();
	}

	reference front() { return *begin(); } //返回第一个元素
	reference back() { return *(end() - 1); } //返回最后一个元素
	void push_back(const T& value) {
		if (finish != end_of_storage)
		{
			construct(finish, value);
			++finish;
		}
		else insert_aux(end(), value);
	}

	//未完成
	void pop_back() {
		--finish;
		destory(finish);
	}

	iterator erase(iterator position) {
		if (position + 1 != end())
			std::copy(position + 1, finish, position);
		--finish;
		destory(finish);
		return position;
	}

	void insert(iterator position, const T& x) {
		insert_aux(position, x);
	}
	void insert(iterator position, size_type length, const T& x) {
		for (int i = 0; i < length; ++i)insert(position, x);
	}

	void resize(size_type new_size, const T& x) {
		if (new_size < size())
			erase(begin() + new_size, end());
		else insert(end(), new_size - size(), x);
	}

	void resize(size_type new_size) {
		resize(new_size, T());
	}
	void clear() { erase(begin(), end()); }

protected:
	//配置空间
	iterator allocate_and_fill(size_type n, const T& x);
};

