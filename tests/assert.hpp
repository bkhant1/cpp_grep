#pragma once

#include <iostream>
#include <type_traits>

template<typename T>
struct argument_type; 
template<typename T, typename U> 
struct argument_type<T(U)> 
{
	using type = U;
};

#define ASSERT(COND) \
	if (!(COND)) \
		std::cout << "Test failed at line " \
			<< __LINE__  \
			<< " in "  \
			<< __FILE__  << std::endl; \
	else \
		std::cout << "OK (line " \
			<< __LINE__\
			<< ")" \
			<< std::endl;
		
#define IS_SAME(T1, T2) \
	{ \
		static_assert( \
			std::is_same< \
				argument_type<void(T1)>::type, \
				argument_type<void(T2)>::type \
			>::value \
		, "Unmatching types"); \
	}

#define SAY(STREAM) \
	std::cout << STREAM << std::endl;
