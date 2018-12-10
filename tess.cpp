#include <iostream>
#include <utility>

class Lala
{
	public:
	static void b() {}
};

template <int N>
struct print_integers
{
	static constexpr bool small_enough = N < 10;

	template <bool c>
	static void do_it_h(typename std::enable_if<c>::type*);

	template <bool c>
	static void do_it_h(...)
	{ std::cout << "Too big" << std::endl; }

	static void do_it()
	{
		do_it_h<small_enough>(0);
	};
};

template <>
struct print_integers<0>
{
	template <bool c>
	static void do_it_h(typename std::enable_if<c>::type*)
	{
		std::cout << 0 << std::endl;	
	}

	static void do_it()
	{
		do_it_h<true>(0);
	};
};

template <int N>
template <bool c>
void print_integers<N>::do_it_h(typename std::enable_if<c>::type*)
{
	std::cout << N << std::endl;	
	print_integers<N-1>::do_it();
}

int main(int, char**)
{
	print_integers<4>::do_it();	
	print_integers<11>::do_it();
}
