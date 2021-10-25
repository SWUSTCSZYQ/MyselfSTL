#if 0
# include<new>
# define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
# include<iostream>
# define __THROW_BAD_ALLOC std::cout << "out of memory" << std::endl; exit(1)
#endif

#include "MyAlloc.h"

//默认为空，即什么也不做
void (*MyAlloc::_malloc_alloc_oom_handler)() = nullptr;


void* MyAlloc::oom_malloc(size_t n)
{
	void (*my_malloc_handler)();
	void* result;

	while (true) //不断尝试释放，配置
	{
		my_malloc_handler = _malloc_alloc_oom_handler;
		if (!my_malloc_handler)
			__THROW_BAD_ALLOC;
		(*my_malloc_handler)(); //调用处理例程，试图释放内存
		result = malloc(n);
		if (result)return result;
	}
}

void* MyAlloc::oom_realloc(void* p, size_t n)
{
	void (*my_malloc_handler)();
	void* result;

	while (true) //不断尝试释放，配置
	{
		my_malloc_handler = _malloc_alloc_oom_handler;
		if (!my_malloc_handler)
			__THROW_BAD_ALLOC;
		(*my_malloc_handler)(); //调用处理例程，试图释放内存
		result = realloc(p, n);
		if (result)return result;
	}
}

void* MyAlloc::allocate(size_t n)
{
	if (n > (size_t)_MAX_BYTE)
	{
		return _allocate(n);
	}
	obj* volatile* my_free_list = free_list + FREELIST_INDEX(n);
	obj* result = *my_free_list;
	if (result == nullptr)
	{
		//无可用的free list
		void* res = refill(round_up(n));
		return res;
	}
	// 调整free list
	*my_free_list = result->free_list_link;
	return result;
}

void* MyAlloc::deallocate(void* p, size_t n)
{
	if (n > (size_t)_MAX_BYTE)
	{
		_deallocate(p, n);
		return;
	}

	obj* q = (obj*) p;
	obj* volatile* my_free_list = free_list + FREELIST_INDEX(n);
	//回收
	q->free_list_link = *my_free_list;
	*my_free_list = q;
}


void* MyAlloc::reallocate(void* p, size_t old_sz, size_t new_sz)
{
	deallocate(p, old_sz);
	p = allocate(new_sz);
	return p;
}

void* MyAlloc::refill(size_t n)
{
	int nobjs = 20;
	char* chunk = chunk_alloc(n, nobjs); //内存池分配内存
	obj* volatile* my_free_list;
	obj* result;
	obj* current_obj, * next_obj;

	if (nobjs == 1) return chunk;
	my_free_list = free_list + FREELIST_INDEX(n);
	result = (obj*)chunk;

	*my_free_list = next_obj = (obj*)(chunk + n);
	for (int i = 1; ; i++)
	{
		current_obj = next_obj;
		next_obj = (obj*)((char*)next_obj + n);
		if (nobjs - 1 == i)
		{
			current_obj->free_list_link = nullptr;
			break;
		}
		current_obj->free_list_link = next_obj;
	}
	return result;
}

char* MyAlloc::chunk_alloc(size_t size, int& nobjs)
{
	size_t total_bytes = size * nobjs;
	size_t bytes_left = end_free - start_free; // 内存池剩余空间
	char* result;
	if (bytes_left >= total_bytes) //完全分配
	{
		result = start_free;
		start_free += total_bytes;
		return result;
	}
	else if (bytes_left >= size) //只能分配部分
	{
		nobjs = bytes_left / size;
		total_bytes = nobjs * size;
		result = start_free;
		start_free += total_bytes;
		return result;
	}
	else //一个区块的空间都无法提供
	{
		size_t bytes_to_get = 2 * total_bytes + round_up(heap_size >> 4);
		if (bytes_left > 0) 
		{
			obj* volatile* my_free_list = free_list + FREELIST_INDEX(bytes_left);
			((obj*)start_free)->free_list_link = *my_free_list;
			*my_free_list = (obj*)start_free;
		}

		start_free = (char*)malloc(bytes_to_get);
		if (start_free == nullptr) //堆空间不足， malloc申请失败
		{
			int i;
			obj* volatile* my_free_list, * p;
			for (i = size; i <= _MAX_BLOCKS; i += _ALIGN)
			{
				my_free_list = free_list + FREELIST_INDEX(i);
				p = *my_free_list;
				if (p != nullptr)
				{
					*my_free_list = p->free_list_link;
					start_free = (char*)p;
					end_free = start_free + 1;
					return (chunk_alloc(size, nobjs));
				}
			}
			//无空间分配，直接调用一级内存分配 然后报异常处理
			end_free = nullptr;
			start_free = (char*)_allocate(bytes_to_get);
		}
		heap_size += bytes_to_get;
		end_free = start_free + bytes_to_get;
		return (chunk_alloc(size, nobjs));
	}
}