#include <iostream>
#include <typeinfo>
#include <tuple>

struct false_ { static bool const value = false; };
struct true_ { static bool const value = true; };

template <typename T>
struct is_integral: false_ {};

template <>
struct is_integral<char>: true_ {};
template <>
struct is_integral<int>: true_ {};

struct print_type 
{
	template <class T>
	bool operator() (T)
	{
		std::cout << typeid(T).name() << std::endl;
	}
};

struct stream_value
{
	template <class T>
	bool operator() (T t)
	{
		std::cout << t << std::endl;
	}
};

bool print_type_f(int x) { std::cout << "int" << std::endl; }

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

template <int... R1, int ...R2>
struct cat_range<int_range<R1...>, int_range<R2...>>
{
	using type = int_range<R1..., R2...>;
};

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

template <typename... Elems>
struct type_list {};

template <typename Head, typename... Elems, typename F>
void execute_impl(Head h, F f);

template <typename Head>
struct foreach {};

template <typename Head>
struct foreach<type_list<Head>>
{
        template <typename F>
        static void execute(F f)
        {
        	f(Head());
        }
};

template <typename Head, typename... Elems>
struct foreach<type_list<Head, Elems...>>
{
	template <typename F>
	static void execute(F f)
	{
		execute_impl<F, Head, Elems...>(Head(), f);
	}
};

template <typename F, typename Head, typename... Elems>
void execute_impl(Head h, F f)
{
	f(h);
	foreach<type_list<Elems...>>::execute(f);
}

template <typename L, typename N>
struct foreach_tuple_type_impl;

template <typename L, int... N>
struct foreach_tuple_type_impl<L, int_range<N...>>  
{	
	template <typename F>
	static void apply(F f) 
	{
		bool _[] = { ((bool) f(
			typename std::tuple_element<N,L>::type()
		))...} ;
	}
};

template <typename L>
struct foreach_tuple_type
{
	template <typename F>
	static void apply(F f)
	{
		foreach_tuple_type_impl<
			L,
			typename make_range<
				std::tuple_size<L>::value-1
			>::type
		>::apply(f);
	}
};

template <typename L, typename N>
struct foreach_tuple_value_impl;

template <typename L, int... N>
struct foreach_tuple_value_impl<L, int_range<N...>>  
{	
	template <typename F>
	static void apply(F f, const L& l) 
	{
		bool _[] = { ((bool) f(
			std::get<N>(l)	
		))...} ;
	}

	template <bool c, typename El, typename F>
	static typename std::enable_if<c, bool>::type f_if(El el, F f)
	{
		return f(el);
	}

	template <bool c, typename El, typename F>
	static typename std::enable_if<!c, bool>::type f_if(El el, F f)
	{
		std::cout << "No possible to pass this type to f" << std::endl;
		return false;
	}

	template <typename F>
	static void try_apply(F f, const L& l)
	{
		bool _[] = 
		{ ((bool) 
			f_if<
				is_integral<typename std::tuple_element<N,L>::type>::value
			>(std::get<N>(l), f)
		)...} ;
	}
};

template <typename L>
struct foreach_tuple_value
{
	foreach_tuple_value(const L& l):
		m_l(l) {}
	
	template <typename F>
	void apply(F f) 
	{
		foreach_tuple_value_impl<
			L,
			typename make_range<
				std::tuple_size<L>::value-1
			>::type
		>::apply(f, m_l);
	}

	template <typename F>
	void try_apply(F f) 
	{
		foreach_tuple_value_impl<
			L,
			typename make_range<
				std::tuple_size<L>::value-1
			>::type
		>::try_apply(f, m_l);
	}

	const L& m_l;
};

int main(int argc, char* argv[])
{
	using my_list = type_list<int, char, bool>;
	std::tuple<int,char,bool> x  {5, 'X', true};
	foreach_tuple_type<decltype(x)>::apply(print_type());
	foreach_tuple_value<decltype(x)>(x).apply(stream_value());

	class A {};

	std::tuple<int, int, A> y {1, 2, A() };
	foreach_tuple_value<decltype(y)>(y).try_apply(stream_value());
	foreach<my_list>::execute(print_type());
}
