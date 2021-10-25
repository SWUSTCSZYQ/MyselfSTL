#pragma once

#include <cstddef>

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag :public input_iterator_tag {};
struct bidirectional_iterator_tag :public forward_iterator_tag {};
struct random_access_iterator_tag :public bidirectional_iterator_tag {};

template<typename Category, class T, class Distance = ptrdiff_t,
	typename Pointer = T*, class Reference = T&>
	struct iterator
{
	typedef Category iterator_category;
	typedef T value_type;
	typedef Distance difference_type;
	typedef Pointer pointer;
	typedef Reference reference;
};

template<typename Iterator>
struct iterator_traits
{
	typedef typename Iterator::iterator_category iterator_category;
	typedef typename Iterator::value_type value_type;
	typedef typename Iterator::difference_type difference_type;
	typedef typename Iterator::pointer pointer;
	typedef typename Iterator::reference reference;
};


//针对原生指针进行特化
template<class T>
struct iterator_traits<T*>
{
	typename random_access_iterator_tag iterator_category;
	typename T value_type;
	typename ptrdiff_t difference_type;
	typename T* pointer;
	typename T& reference;
};

//针对pointer-to-const进行偏特化
template<class T>
struct iterator_traits<const T*>
{
	typename random_access_iterator_tag iterator_category;
	typename T value_type;
	typename ptrdiff_t difference_type;
	typename const T* pointer;
	typename const T& reference;
};

//决定迭代器的类型
template<typename Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&)
{
	typedef typename iterator_traits<Iterator>::iterator_category category;
	return category();
}

//返回value_type
template<typename Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&)
{
	return static_cast<typename iterator_traits<Iterator>::value_type*>(nullptr);
}

//决定distance_type
template<typename Iterator>
inline typename iterator_traits<Iterator>::distance_type*
distance_type(const Iterator&)
{
	return static_cast<typename iterator_traits<Iterator>::distance_type*>(nullptr);
}

template<typename InputIterator>
inline iterator_traits<InputIterator>::difference_type
_distance(InputIterator first, InputIterator last, input_iterator_tag)
{
	iterator_traits<InputIterator>::difference_type n = 0;
	while (first != last)
	{
		++n;
		++first;
	}
	return n;
}


template<typename RandomAccessIterator>
inline iterator_traits<RandomAccessIterator>::difference_type
_distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
{
	return last - first;
}

template<typename InputIterator>
inline iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
	typedef typename iterator_traits<InputIterator>::iterator_category category;
	return _distance(first, last, category());
}


template<typename InputIterator, typename Distance>
inline void _advance(InputIterator& i, Distance n, input_iterator_tag)
{
	if (i >= 0)
		while (n--)++i;
	else
		while (n++)--i;
}

template<typename RandomAccessIterator, typename Distance>
inline void _advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag)
{
	i += n;
}

template<typename InputIterator, typename Distance>
inline void advance(InputIterator& i, Distance n)
{
	_advance(i, n, iterator_category(i));
}