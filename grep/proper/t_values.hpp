#pragma once

//
// Booleans
//

// false value
struct false_
{
	static const bool value = false;
};

// true value
struct true_
{
	static const bool value = true;
};

//
// Integer range
//

// the range
template <int ...N>
struct int_range 
{
	static int _print_1(int n)
	{ std::cout << n << ","; }

	static void print()
	{
		int _[] = {_print_1(N)... };
		std::cout << std::endl;
	}
};

template <typename R1, typename R2>
struct cat_range {};

// concatenate ranges
template <int... R1, int ...R2>
struct cat_range<int_range<R1...>, int_range<R2...>>
{
	using type = int_range<R1..., R2...>;
};

// helper to make the range
template <int N>
struct make_range
{
	using type = typename cat_range<
			typename make_range<N-1>::type,
			int_range<N>
			>::type;
};

template <>
struct make_range<0>
{
	using type = int_range<0>;
};

