#ifndef _Darray_
#define _Darray_

#include <iostream>
#include <cmath>

template <class T>
class Darray{
	// class data
	T* arr;
	// len of array
	int len;
	// allocated size
	int alloc_size;
	// is 
	bool allocated;

public:
	// constructor for dynamic array
	Darray();
	// destructor for dynamic array
	~Darray();
	// cop constructor for dynamic array
	Darray(const  Darray& obj);

public:
	// add elemnt to array
	void add(const T& obj);
	// operator =
	Darray<T>& operator=(const Darray<T>& rhs);
	// print all elemnts
	void print() const;
	// returns length of arr
	int get_len() const;
	// get element from index
	T get_at(int index) const;
	// set at position
	void set_at(int index, T data){
		arr[index] = data;
	}
	// remove NULL ptr in array
	void removeNull(){
		for (int i = 0; i < len; i++){
			if (arr[i] == NULL){
				int h = i;
				for (int i = h; i < len - 1; i++){
					arr[i] = arr[i + 1];
				}
				len--;
			}
		}
	}
private:
	// resize
	bool resize_help();
	// copy func
	void copy_from(const Darray<T>& other);
public:
	// free arr
	void free();
	// returns an integer if possible
	int make_number();
	// returns true if array can be converted to number
	bool is_number() const;
	// clear the array / set_size = 0 
	void clear(){
		delete[] arr;
		arr = NULL;
		alloc_size = 0;
		len = 0;
		allocated = false;
	}
	// returns true if the array is empty
	bool isEmpty(){
		if (len == 0){
			return true;
		}
		return false;
	}
};


template <class T>
inline Darray <T>::Darray(){
	arr = NULL;
	len = 0;
	alloc_size = 0;
	allocated = false;
}

template <class T>
inline bool Darray<T> ::resize_help(){
	allocated = true;
	if (len >= alloc_size){
		if (alloc_size == 0){
			alloc_size = 1;
		}
		allocated = false;
		alloc_size *= 2;

		T* temp = new(std::nothrow) T[alloc_size];
		if (!temp){
			return allocated;
		}
		for (int i = 0; i < len; i++){
			temp[i] = arr[i];
		}
		delete[]  arr;
		arr = temp;
		allocated = true;
	}
	return allocated;
}

template <class T>
inline Darray<T> :: ~Darray(){
	free();
}

template <class T>
inline void Darray<T> ::add(const  T& obj){
	resize_help();
	if (allocated == false){
		throw "Not enough memory";
	}
	arr[len++] = obj;
}






template <class T>
inline Darray<T>& Darray<T> ::  operator=(const Darray<T>& rhs){
	if (this != &rhs){
		free();
		copy_from(rhs);
	}
	return *this;
}


template<class T>
inline void Darray<T> ::print() const{
	for (int i = 0; i < len; i++){
		std::cout << arr[i];
	}
}

template<class T>
inline Darray<T> ::Darray(const  Darray& obj){
	copy_from(obj);
}

template<class T>
inline int Darray<T> ::get_len() const{
	return len;
}

template<class T>
inline T Darray<T>::get_at(int index) const{
	if (index < 0 && index >= len){
		throw "Index out of range -> < Darray<T>::get_at(int index)";
	}
	return arr[index];
}

template<class T>
inline void Darray<T> ::copy_from(const Darray<T>& other){
	this->arr = new T[other.alloc_size];
	for (int i = 0; i < other.len; i++){
		arr[i] = other.arr[i];
	}
	this->len = other.len;
	this->alloc_size = other.alloc_size;
	this->allocated = other.allocated;
}

template<class T>
inline void Darray<T> ::free(){
	delete[] arr;
	arr = NULL;
	len = 0;
	alloc_size = 0;
	allocated = false;
}

template<class T>
inline int Darray<T> ::make_number(){
	if (!is_number()){
		throw "Invalid number";
	}
	if (arr[0] == '-'){
		int degree = len - 1;
		int number = 0;
		int counter = degree - 1;
		for (int i = 1; i < len; i++){
			number = (int)(arr[i] - '0') * pow(10, counter) + number;
			counter--;
		}
		return number*(-1);
	}
	else{
		int degree = len - 1;
		int number = 0;
		for (int i = 0; i < len; i++){
			number = (int)(arr[i] - '0') * pow(10, degree) + number;
			degree--;
		}
		return number;
	}

}

template<class T>
inline bool Darray<T> ::is_number() const{
	bool status = true;
	for (int i = 0; i < len; i++){
		if (arr[i] >= '0' && arr[i] <= '9'){

		}
		else if (arr[i] == '-' || arr[i] == '+'){

		}
		else{
			status = false;
			break;
		}
	}
	return status;
}

#endif // !_DYNAMIC_ARR_
