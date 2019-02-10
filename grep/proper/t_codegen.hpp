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
				true	
			>(std::get<N>(l), f)
		)...} ;
	}
};

// "iterate" over the values of a tuple.
// Apply can be called to apply a function to those values
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

	L& m_l;
};

