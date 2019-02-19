#include <typeinfo>
#include <vector> 
#include <list>
#include <iterator>
#include <iostream>
#include <tuple>
#include <t_values.hpp>
#include <t_types.hpp>
#include <t_toy.hpp>
#include <t_codegen.hpp>
#include <assert.hpp>

template <class T> 
struct get_begin_return_type_if_begin_exist  
{ 
	template <class Tp> 
	static
	//typename get_return_type<decltype(&Tp::begin)>::type
	decltype(std::declval<Tp>().begin())
	test(decltype(std::declval<Tp>().begin())*) 
	{
		return &Tp::begin;
	}; 
 
	template <class Tp> 
	/* Should be false_type instead of int */
	static int test(...) {}; 
 
	using type = decltype(test<T>(0)); 
};

DEFINE_TYPE_PROPERTY(decltype(std::declval<Tp>().begin())*, has_begin);
DEFINE_TYPE_PROPERTY(decltype(std::declval<Tp>().end())*, has_end);
DEFINE_TYPE_PROPERTY(typename std::iterator_traits<Tp>::iterator_category*, is_iterator);

template <class T> 
struct is_tuple  
{ 
	template <class Tp> 
	static char test(
		typename std::tuple_element<
			0,
			// Remove const to avoid a not in "immediate context"
			// error, which is not SFINAE failure (error happens 
			// somewhere in tuple_element implementation)
			typename remove_const<Tp>::type
		>::type*) {}; 
 
	template <class Tp> 
	static int test(...) {}; 
 
	static const bool result =  
		sizeof(test<T>(0)) == sizeof(char); 
};

template <bool cond, typename If, typename Else>
struct if_then_else 
{
	using type = Else;
};

template <typename If, typename Else>
struct if_then_else<true, If, Else>
{
	using type = If;
};

struct Iterability
{
	struct not_iterable {};
	struct runtime_iterable {};
	struct compiletime_iterable {};
};

template <typename T> 
struct check_iterability
{
	static constexpr bool runtime_iterable = 
		has_begin<T>::result &&
		has_end<T>::result &&
		is_iterator<
			typename get_begin_return_type_if_begin_exist<T>::type
		>::result;

	static constexpr bool compiletime_iterable = 
		is_tuple<T>::result;

	using type = typename if_then_else<
		runtime_iterable,
		Iterability::runtime_iterable,
		typename if_then_else<
			compiletime_iterable,
			Iterability::compiletime_iterable,
			Iterability::not_iterable
		>::type
	>::type;
};
// } MetaP library

template<class T, class S, typename P>
std::list<T*> grep(S& s, P f);

template <bool apply>
struct apply_predicate_or_false
{
	template <typename F, typename S>
	static bool predict(F f, S s)
	{ return f(s); } 
};

template <>
struct apply_predicate_or_false<false>
{
	template <typename F, typename S>
	constexpr static bool predict(F f, S s)
	{ return false; } 
};

template <bool condition>
struct insert_in_list_if 
{
	template <typename L, typename E>
	static void insert_if(L& ret, E& e, bool c)
	{
		if (c) ret.insert(ret.end(), &e);
	}
};

template <>
struct insert_in_list_if<false> 
{
	template <typename L, typename E>
	static void insert_if(L, E, bool) {};
};

template<class T, class S, typename P>
struct grep_helper_base
{
	grep_helper_base(S& s, P f):
		m_f(f), m_s(s) {};

	bool predicate_on_s()
	{
		return apply_predicate_or_false<
			is_substituable<
				typename at<
					typename get_parameters_type_list<P>::type,
					0
				>::type,
				S
			>::value
		>::template predict<P, S>(m_f, m_s);
	}

	P f() { return m_f; };
	S& s() { return m_s; };

	P m_f;
	S& m_s;	
};

template<class T, class S, typename P, class S_iterability>
struct grep_helper
{
	// Need to provide iterability
	//std::static_assert<false>;
};

template <typename T, typename P>
struct splice_grep_result_in
{
	splice_grep_result_in(std::list<T*>& l, P p):
		m_l(l), m_p(p) {}
	
	template <typename S>
	bool operator() (S& s)
	{
		m_l.splice(m_l.end(), grep<T,S,P>(s, m_p));
	}

	std::list<T*>& m_l;
	P m_p;	
};

template <class T, class S, typename P>
struct grep_helper<T,S,P, Iterability::compiletime_iterable>:
	grep_helper_base<T,S,P>
{
	using base = grep_helper_base<T,S,P>;

	grep_helper(S& s, P f): base(s,f) {};

	std::list<T*> operator() ()
	{
		std::list<T*> ret;

		insert_in_list_if<
			std::is_same<T,S>::value
		>::insert_if(ret, &base::s(), base::predicate_on_s());

		// Apply should insert the result of grep on x in ret
		foreach_tuple_value<S>(base::s()).apply(
			// Ugly to pass f here, should be passing something like
			// [](auto x) { grep<T>(x, base::f()) }
			// But generic lambda requires c++14 and IDK about bind
			splice_grep_result_in<T, P>(ret, base::f())
			);
		
		return ret;
	}
};

template<class T, class S, typename P>
struct grep_helper<T,S,P, Iterability::runtime_iterable>:
	grep_helper_base<T,S,P>
{
	using base = grep_helper_base<T,S,P>;

	grep_helper(S& s, P f): base(s,f) {};

	std::list<T*> operator() ()
	{
		std::list<T*> ret;

		insert_in_list_if<
			std::is_same<T,S>::value
		>::insert_if(ret, &base::s(), base::predicate_on_s());

		for (auto& x: base::s())
			ret.splice(ret.end(), grep<T,decltype(x),P>(x, base::f()));

		return ret;
	}
};

void typeof(std::vector<int>) {}

template<class T, class S, typename P>
struct grep_helper<T,S,P,Iterability::not_iterable>:
	grep_helper_base<T,S,P>
{
	using base = grep_helper_base<T,S,P>;

	grep_helper(S& s, P f): base(s,f) {};

	std::list<T*> operator() ()
	{
		std::list<T*> ret;

		insert_in_list_if<
			is_substituable<T,S>::value
		>::insert_if(ret, base::s(), base::predicate_on_s());

		return ret;
	}	
};

// TODO: you should be able to choose what kind
// of return you want (copy/const ref/ref/ptr)
template<class T, class S, typename P>
std::list<T*> grep(S& s, P f)
{
	// Is S iterable ?
	using s_check_iterability = typename check_iterability<S>::type;
	return grep_helper<
			T, S, P,
			s_check_iterability
		>(s, f)();
}

// Tests helpers
class HasBegin
{
	using ret_begin_t = int;
	public:
	void begin() {};
};

struct HasBeginAndEnd
{
	public:
	void begin() {};
	void end() {};	
};

struct HasBeginAndEndBeginReturnIterator
{
	public:
	std::vector<int>::iterator begin() {};
	void end() {};	
};

class HasParentBegin: public HasBegin
{
};

class NotAsBegin
{
	public:
	void end() {};
};

template <typename T>
struct X
{
	using type = T;
};

// Id metafunction
template <typename T>
struct identity
{
	using type = T;
};

int my_function(char) {}
const HasBegin* my_function_2() {}

template <typename T>
void print_grep_result(const std::list<T*>& result)
{
	std::cout << "{";
	for (auto y : result) { std::cout << " " <<*y << ","; }; // { 1, 1, }
	std::cout << "}" << std::endl;
}

struct equal_1_f
{
	equal_1_f():m_nb_evaluations(0) {};

	bool operator() (int x) {return x==1;}

	int m_nb_evaluations;
};

bool equal_2(int x) {
	return x == 1;
}

template <typename Tp>
void my_func(typename std::tuple_element<0, Tp>::type*) {}

template<typename T>
struct equal_to{

	equal_to(T x):m_x(x), m_nb_eval(0) {}
	
	bool operator()(T x) {
		m_nb_eval = m_nb_eval + 1;
		return m_x==x;
	}
	
	int m_nb_eval;
	T m_x;
};

#include <chrono>

void has_begin_tests()
{
	ASSERT(has_begin<HasBegin>::result == 1);
	ASSERT(has_begin<HasParentBegin>::result == 1);
	ASSERT(has_begin<NotAsBegin>::result == 0);
}

int main(int argc, char* argv[])
{	
	using IVec = std::vector<int>;
	using IIVec = std::vector<std::vector<int>>;
	has_begin_tests();
	std::cout << "Is iterator tests" << std::endl;
	std::cout << is_iterator<NotAsBegin>::result << std::endl; // 0
	std::cout << is_iterator<IVec::iterator>::result << std::endl; // 1

	std::cout << "If then else tests" << std::endl;
	print_type<
		if_then_else<true, X<int>, X<char>>::type::type>(); // i
	print_type<
		if_then_else<false, X<int>, X<char>>::type::type>(); // c

	std::cout << "Get return type test" << std::endl;
	print_type<
		get_return_type<decltype(&my_function)>::type>(); // i 
	print_type<
		get_return_type<decltype(&my_function_2)>::type>(); // PK8HasBegin

	std::cout << "Conditional get return type test" << std::endl;
	print_type<
		typename get_begin_return_type_if_begin_exist<HasBegin>::type
	>(); // v
	print_type<
		typename get_begin_return_type_if_begin_exist<IVec>::type
	>(); // Complex shit with vector and iterator in the name
	
	std::cout << "Is iterable test" << std::endl;
	print_type<check_iterability<IVec>::type>(); // run time iterable
	print_type<check_iterability<int>::type>(); // not iterable
	print_type<check_iterability<HasBeginAndEnd>::type>(); // not iterable
	print_type<check_iterability<HasBeginAndEndBeginReturnIterator>::type>(); // run time iterable

	std::cout << "Get parameter type_list test" << std::endl;
	struct MyClass 
	{
		void method1(int) {}
		void method2(int, char, bool) {}
	};
	get_parameters_type_list<bool(*)(int)>::type::print(); // i,
	get_parameters_type_list<bool(*)(int, char, bool)>::type::print(); // i,c,b
	get_parameters_type_list<bool(MyClass::*)(bool)>::type::print(); // b,
	get_parameters_type_list<decltype(&MyClass::method1)>::type::print(); // i,
	get_parameters_type_list<decltype(&MyClass::method2)>::type::print(); // i,c,b

	// is_tuple test
	std::cout << "Is tuple test" << std::endl;
	std::cout << is_tuple<std::tuple<int, char>>::result << std::endl;
	std::cout << is_tuple<int>::result << std::endl;
	std::cout << is_tuple<const int>::result << std::endl;

	// is_substituable test
	std::cout << "is_substituable test " << std::endl;
	std::cout << is_substituable<int, int>::value << std::endl;
	std::cout << is_substituable<int&, int>::value << std::endl;
	std::cout << is_substituable<int, int&>::value << std::endl;
	std::cout << is_substituable<const int, int&>::value << std::endl;
	

	std::cout << "Grep test" << std::endl;
	// grep test
	std::vector<std::vector<int>> a = 
	{
		{1,2},
		{-1,3},
		{9,10},
		{1,9}
	};

	// With function pointer
	std::list<int*> result = grep<int>(a, equal_2);
	std::cout << "{";
	for (auto y : result) { std::cout << " " <<*y << ","; }; // { 1, 1, }
	std::cout << "}" << std::endl;
	// With functor
	std::list<int*> result_functor = grep<int>(a, equal_1_f());
	std::cout << "{";
	for (auto y : result_functor) { std::cout << " " <<*y << ","; }; // { 1, 1, }
	std::cout << "}" << std::endl;

	// Grep with a tuple
	std::tuple<int, char, float> tup {1, 'x', 0.3f};

	std::list<char*> result_tuple = grep<char>(tup, equal_to<char>('x') );	
	std::cout << "{";
	char& ref_x = std::get<1>(tup);
	for (auto y : result_tuple) {
		std::cout << " " << *y << ",";
	}; // { 1, 1, }
	std::cout << "}" << std::endl;

	using isVi_t = std::tuple<int, std::string, IVec>;

	isVi_t in_complex_one 
	{
		5,
		"Bonjour",
		{4,3,2,1}
	};

	std::tuple
	<
		IVec,
		char,
		isVi_t
	>
	a_complex_one
	{
		{1,2,3,4,6},
		'B',
		in_complex_one
	};

	std::list<int*> result_complex = grep<int>(a_complex_one, equal_to<int>(6));
	print_grep_result(result_complex); // { 6 }
//
//	result_complex = grep<int>(a_complex_one, equal_to<int>(1));
//	print_grep_result(result_complex); // {1, 1}	

	auto equal_to_char_B = equal_to<char>('B');
	IVec& my_vec = std::get<0>(a_complex_one);
	my_vec.push_back(1);
	std::list<char*> result_complex_2 = 
		grep<
			char,
			decltype(a_complex_one),
			equal_to<char>&
		>(a_complex_one, equal_to_char_B);

	std::cout << equal_to_char_B.m_nb_eval << std::endl;
	print_grep_result(result_complex_2); // { B, B} (as string is iterable)
	return 0;
}
