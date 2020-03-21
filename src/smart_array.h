#ifndef __SMART_ARRAY_H__
#define __SMART_ARRAY_H__

#include <memory>

/// Smart array class.
/** The smart array class is a template class for specialized arrays that remember their size. */
template <class T>
class smart_array
{
	public:
		smart_array(int count); ///< Create an array with the specified number of elements.
		smart_array(const smart_array<T>& a); ///< Create a copy of another smart_array
		smart_array(std::initializer_list<T> items); ///< Create a smart_array with an initializer, similar to the normal way of creating an array with initializers {1,2,3}
		std::size_t length(); ///< Get the number of elements in the array
		T get(int i) const; ///< Return an item from the array.
		T& operator[](int i); ///< Return (by reference) an item of the array.
		T* array(); ///< Return the raw array.
	private:
		std::unique_ptr<T[]> data; ///< The array, unique_ptr automatically performs delete
		std::size_t l; ///< The number of elements in the array
};


template <class T>
smart_array<T>::smart_array(int count)
{
	l = count;
	data = std::unique_ptr<T[]>(new T[l]);
}

template <class T>
smart_array<T>::smart_array(const smart_array<T>& a)
{
	l = a.l;
	data = std::unique_ptr<T[]>(new T[l]);
    for (unsigned int i = 0; i < a.l; i++)
	{
		data[i] = a.data[i];
	}
}

template <class T>
T& smart_array<T>::operator [] (int i)
{
	return data[i];
}


template <class T>
T smart_array<T>::get(int i) const
{
	return data[i];
}

template <class T>
smart_array<T>::smart_array(std::initializer_list<T> items)
{
	l = items.size();
	data = std::unique_ptr<T[]>(new T[l]);
	int index = 0;
	for (const T* i = items.begin(); i != items.end(); i++)
	{
		data[index++] = *i;
	}
}

template <class T>
T* smart_array<T>::array()
{
	return data.get();
}

template <class T>
std::size_t smart_array<T>::length()
{
	return l;
}

#endif

