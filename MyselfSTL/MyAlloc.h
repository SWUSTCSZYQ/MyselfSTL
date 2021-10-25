#pragma once
#include<new>

class MyAlloc
{
private:
	//处理内存不足的情况 out of memory
	static void* oom_malloc(size_t);
	static void* oom_realloc(void*, size_t n);
	static void (*_malloc_alloc_oom_handler)();

public:
	//分配和释放内存, 需要分配和释放的内存过大的时候使用（> _MAX_BYTE）
	
	static void* _allocate(size_t n)
	{
		void* result = malloc(n);
		if (result == nullptr)result = oom_malloc(n);
		return result;
	}

	static void _deallocate(void* p, size_t)
	{
		free(p);
	}

	static void* _reallocate(void* p, size_t,size_t new_sz)
	{
		void* result = realloc(p, new_sz);
		if (result == nullptr)result = oom_realloc(p, new_sz);
		return result;
	}

	//set_new_handler
	static void (*set_malloc_handler(void(*f)()))()
	{
		auto old = _malloc_alloc_oom_handler;
		_malloc_alloc_oom_handler = f;
		return old;
	}

public:
	//小块空间，使用内存池进行控制
	static void* allocate(size_t n);
	static void* reallocate(void* p, size_t old_sz, size_t new_sz);
	static void* deallocate(void* p, size_t);

private:

	//返回一个大小为n的对象，并可能加入大小为n的其他区块到free list
	static void* refill(size_t n);

	// 配置一块空间，可容纳nobjs个大小为size的区块
	static char* chunk_alloc(size_t size, int& nobjs);

	// 将内存对齐为8的倍数
	static size_t round_up(size_t bytes)
	{
		return (((bytes)+_ALIGN - 1) & ~(_ALIGN - 1));
	}

	//决定使用哪个区块
	static size_t FREELIST_INDEX(size_t bytes)
	{
		return (bytes + _ALIGN - 1) / _ALIGN - 1;
	}


private:
	static const int _ALIGN = 8; //小型区块的边界
	static const int _MAX_BYTE = 128;  //内存块上界
	static const int _NUM_FRELIST = _MAX_BYTE / _ALIGN;

	union obj {
		union obj* free_list_link; //指向下一个区块
		char client_data[1]; //存储本地内存块的首地址
	};

	static obj* volatile free_list[_NUM_FRELIST];

	// chunk allocation state
	static char* start_free;
	static char* end_free;
	static size_t heap_size;

};

char* MyAlloc::start_free = nullptr;
char* MyAlloc::end_free = nullptr;
size_t MyAlloc::heap_size = 0;

MyAlloc::obj* volatile MyAlloc::free_list[MyAlloc::_NUM_FRELIST] = {
	nullptr, nullptr, nullptr,nullptr,
	nullptr, nullptr, nullptr,nullptr,
	nullptr, nullptr, nullptr,nullptr,
	nullptr, nullptr, nullptr,nullptr
};



