#pragma once

//
// Iteration
//

template <typename L, typename N>
struct foreach_tuple_value_impl;

template <typename L, int... N>
struct foreach_tuple_value_impl<L, int_range<N...>>  
{	
	template <typename F>
	static void apply(F f, L& l) 
	{
		bool _[] = { ((bool) f(
			std::get<N>(l)	
		))...} ;
	}

};

// build an "iterator" over the values of a tuple.
// apply can be called to apply a function to those values.
template <typename L>
struct foreach_tuple_value
{
	foreach_tuple_value(L& l):
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

	L& m_l;
};

