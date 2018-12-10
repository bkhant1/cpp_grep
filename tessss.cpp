
#include <tuple>
#include <iostream>

template <class T> 
struct remove_const
{
	using type = T;	
};

template <class T>
struct remove_const<const T>:
	remove_const<T> {};

template <class T> 
struct is_tuple  
{ 
	template <class Tp> 
	static char test(
		typename std::tuple_element<
			0,
			typename remove_const<Tp>::type
		>::type*) {}; 
 
	template <class Tp> 
	static int test(...) {}; 
 
	static const bool result =  
		sizeof(test<T>(0)) == sizeof(char); 
};

int main(int, char**)
{
	std::cout << is_tuple<std::tuple<int, char>>::result << std::endl;
	std::cout << is_tuple<int>::result << std::endl;
	std::cout << is_tuple<const int>::result << std::endl;
	std::cout << is_tuple<const int&>::result << std::endl;
	std::cout << is_tuple<int*>::result << std::endl;
	std::cout << is_tuple<int* const>::result << std::endl;
}
