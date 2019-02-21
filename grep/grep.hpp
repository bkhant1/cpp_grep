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

MEMBER_FUNCTION_RETURN_TYPE(begin, begin_return_type);
HAS_MEMBER_FUNCTION(begin, has_begin);
HAS_MEMBER_FUNCTION(end, has_end);
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
 
	static const bool value =  
		sizeof(test<T>(0)) == sizeof(char); 
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
		has_begin<T>::value &&
		has_end<T>::value &&
		is_iterator<
			typename begin_return_type<T>::type
		>::value;

	static constexpr bool compiletime_iterable = 
		is_tuple<T>::value;

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

