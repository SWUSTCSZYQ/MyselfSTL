#pragma once
#include<new>

class MyAlloc
{
private:
	//�����ڴ治������ out of memory
	static void* oom_malloc(size_t);
	static void* oom_realloc(void*, size_t n);
	static void (*_malloc_alloc_oom_handler)();

public:
	//������ͷ��ڴ�, ��Ҫ������ͷŵ��ڴ�����ʱ��ʹ�ã�> _MAX_BYTE��
	
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
	//С��ռ䣬ʹ���ڴ�ؽ��п���
	static void* allocate(size_t n);
	static void* reallocate(void* p, size_t old_sz, size_t new_sz);
	static void* deallocate(void* p, size_t);

private:

	//����һ����СΪn�Ķ��󣬲����ܼ����СΪn���������鵽free list
	static void* refill(size_t n);

	// ����һ��ռ䣬������nobjs����СΪsize������
	static char* chunk_alloc(size_t size, int& nobjs);

	// ���ڴ����Ϊ8�ı���
	static size_t round_up(size_t bytes)
	{
		return (((bytes)+_ALIGN - 1) & ~(_ALIGN - 1));
	}

	//����ʹ���ĸ�����
	static size_t FREELIST_INDEX(size_t bytes)
	{
		return (bytes + _ALIGN - 1) / _ALIGN - 1;
	}


private:
	static const int _ALIGN = 8; //С������ı߽�
	static const int _MAX_BYTE = 128;  //�ڴ���Ͻ�
	static const int _NUM_FRELIST = _MAX_BYTE / _ALIGN;

	union obj {
		union obj* free_list_link; //ָ����һ������
		char client_data[1]; //�洢�����ڴ����׵�ַ
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



