
#include <iostream>
#include <vector>
#include <type_traits>

#define IVEC(...) std::vector<__VA_ARGS__>

template <typename T>
struct m { typename T::aaaa x; };

struct A
{
	int lol() {return 0;}
};

template <typename A, typename Ts>
using FunctionType = decltype(std::declval<A>().lol())(*)(Ts);

FunctionType<A, int> my_func() {};

int main(int, char**)
{
	int x;
	decltype(x) z;
	m<decltype(int)> y;
}
