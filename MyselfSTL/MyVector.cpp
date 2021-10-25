#include "MyVector.h"
#include<algorithm>
#include<memory>

template<class T>
void MyVector<T>::insert_aux(iterator position, const T& x)
{
	if (finish != end_of_storage) {
		construct(finish, *(finish - 1)); //����ʣ��ռ䣬�����һ��ֵ��Ϊ���ֵ
		++finish;
		T x_copy = x;
		std::copy(position, finish - 2, finish - 1);
		*position = x_copy;
	}
	else
	{
		const size_type old_size = size();
		const size_type len = ole_size != 0 ? 2 * old_size : 1;

		iterator new_start = new T[len];
		iterator new_finish = new_start;
		try{
			new_finish = uninitialized_copy(start, position, new_start); //����ǰ��ֵȫ�����Ƶ��µĿռ���
			construct(new_finish, x);
			++new_finish;
			new_finish = uninitialized_copy(positon, finish, new_finish);//��������ԭ����Ҳ��������
		}
		catch (...) {
			destory(new_start, new_finish);
			delete new_start[];
			throw;
		}
		destroy(begin(), end());
		deallocate();
		start = new_start;
		finish = new_finish;
		end_of_storage = new_start + len;
	}
}