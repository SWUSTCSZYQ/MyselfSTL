#pragma once

#include<new.h>
#include "type_trivial.h"

template<typename T1, typename T2>
inline void construct(T1* p, T2& value)
{
	new (p) T1(value);
}

template<typename T>
inline void destroy(T* pointer)
{
	pointer->~T();
}

template<typename ForwardIterator>
inline void destory(ForwardIterator first, ForwardIterator last)
{
	_destory(first, last);
}

template<typename ForwardIterator, typename T>
inline void _destory(ForwardIterator first, ForwardIterator last, T*)
{
	//typedef typename 
}

template<typename ForwardIterator>
inline void _destory_aux(ForwardIterator first, ForwardIterator last, _true_type)
{

}

template<typename ForwardIterator>
inline void _destory_aux(ForwardIterator first, ForwardIterator last, _false_type)
{
	while (first < last)
	{
		destory(&*first);
		++first;
	}
}