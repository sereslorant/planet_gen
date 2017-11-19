#ifndef UTILS_HPP
#define UTILS_HPP

template<class T>
class array_slice
{
private:
	T *array;
	
public:
	
	T &operator[] (unsigned int index)
	{
		return array[index];
	}
	
	array_slice(T* p_array)
		:array(p_array)
	{}
}

#endif // UTILS_HPP
