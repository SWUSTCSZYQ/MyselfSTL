#include "MyVector.h"
#include<algorithm>
#include<memory>

template<class T>
void MyVector<T>::insert_aux(iterator position, const T& x)
{
	if (finish != end_of_storage) {
		construct(finish, , *(finish - 1)); //还有剩余空间，将最后一个值赋为其初值
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
		tyr{
			new_finish = uninitialized_copy(start, position, new_start); //将以前的值全部复制到新的空间中
			construct(new_finish, x);
			++new_finish;
			new_finish = uninitialized_copy(positon, finish, new_finish);//将安插点的原内容也拷贝过来
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
