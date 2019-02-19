#pragma once
#include <t_toy.hpp>
#include <t_values.hpp>
#include <type_traits>

//
// Types
//

// void
template <typename ...Args>
struct void_
{
	using type = void;
};

// type list
template <typename ...N>
struct type_list
{
	std::string str()
	{
		std::string all[] =
			{::str<N>()...};
		std::string ret;
		for (auto x: all)
		{
			ret += x + ",";
		}
		ret.resize(ret.size() - 1);
		return ret;
	}

	static void print()
	{
		int _[] = {_print_type_for_list<N>()...};
		std::cout << std::endl;
	}	
};

// type a position N in the list
template <typename List, int N>
struct at {/* static assert false */};

template <typename First, typename ...List, int N>
struct at<type_list<First, List...>, N>
{
	using type = typename at<type_list<List...>, N-1>::type; 
};

template <typename Head, typename ...List>
struct at<type_list<Head, List...>, 0>
{
	using type = Head;
};

//
// Type modification
//

template <typename T>
struct remove_ref
{
	using type = T;
};

template <typename T>
struct remove_ref<T&>
{
	using type = T;
};

template <typename T>
struct remove_const
{
	using type = T;
};

template <typename T>
struct remove_const<const T>
{
	using type = T;
};

//
// Type inspection
//

// true if T1 = T2 once const and reference removed
template <typename T1, typename T2>
struct is_substituable:
	std::is_same<
		typename remove_ref<
			typename remove_const<T1>::type
		>::type,
		typename remove_ref<
			typename remove_const<T2>::type
		>::type
	> {};

template <class T> 
struct get_callable_operator_type_if_exist  
{ 
	template <class Tp> 
	static
	decltype(&Tp::operator())
	test(void*) 
	{
		return &Tp::begin;
	}; 
 
	template <class Tp> 
	/* Should be false_type instead of int */
	static false_ test(...) {}; 
 
	using type = decltype(test<T>(0)); 
};

// get the return type of any callable
template <typename T>
struct get_return_type 
{
	using callable_operator_type = 
		typename get_callable_operator_type_if_exist<
			T
		>::type;

	using type = typename get_return_type<
		callable_operator_type
	>::type;	
};

template <typename R, typename ...Args>
struct get_return_type<R(*)(Args...)>
{
	using type = R;
};

template <typename R, typename C, typename ...Args>
struct get_return_type<R(C::*)(Args...)>
{
	using type = R;
};

// get the parameter type list of a callable
template <typename T>
struct get_parameters_type_list 
{
	using type = typename get_parameters_type_list<
		typename get_callable_operator_type_if_exist<T>::type
	>::type;
};

template <typename T>
struct get_parameters_type_list<T&>:
	get_parameters_type_list<T> {};

template <typename R, typename ...Args>
struct get_parameters_type_list<R(*)(Args...)>
{
	using type = type_list<Args...>;
};

template <typename R, typename C, typename ...Args>
struct get_parameters_type_list<R(C::*)(Args...)> 
	: get_parameters_type_list<R(*)(Args...)> {};

#define MEMBER_FUNCTION_TYPE(MEMBER_NAME, NAME_OF_META_FCT_TO_DEFINE) \
template <class T> \
struct NAME_OF_META_FCT_TO_DEFINE  \
{ \
	template <class Tp> \
	static decltype(&Tp::begin) \
	test(decltype(std::declval<Tp>().begin())*) {}; \
 \
	template <class Tp> \
	static false_ test(...) {}; \
 \
	using type = decltype(test<T>(0)); \
}

#define _CONCAT(x,y) x ## y
#define CONCAT(x,y) _CONCAT(x,y)

#define HAS_MEMBER_FUNCTION(MEMBER_NAME, NAME_OF_META_FCT_TO_DEFINE) \
MEMBER_FUNCTION_TYPE(MEMBER_NAME, CONCAT(CONCAT(NAME_OF_META_FCT_TO_DEFINE, __LINE__), __FILENAME__)); \
template <typename T> \
struct NAME_OF_META_FCT_TO_DEFINE \
{ \
	static const bool value = \
		!std::is_same< \
			typename CONCAT(CONCAT(NAME_OF_META_FCT_TO_DEFINE, __LINE__),__FILENAME__)<T>::type, \
			false_ \
		>::value; \
};
	

// First argument is a type that can only exist if a class
// Tp (Tp == T) .
#define DEFINE_TYPE_PROPERTY(MEMBER_TYPE_FROM_T, NAME) \
template <class T> \
struct NAME  \
{ \
	template <class Tp> \
	static char test(MEMBER_TYPE_FROM_T) {}; \
 \
	template <class Tp> \
	static int test(...) {}; \
 \
	static int type_of(int){};\
\
	static const bool result =  \
		sizeof(test<T>(0)) == sizeof(char); \
}

