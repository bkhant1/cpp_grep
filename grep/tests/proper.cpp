#include <assert.hpp>
#include <t_toy.hpp>
#include <t_types.hpp>
#include <t_codegen.hpp>
#include <iostream>
#include <vector>

//
// Some definitions used accross all tests
// 

class HasBegin
{
	using ret_begin_t = int;
	public:
	int* begin() {};
	HasBegin& mbr_fct_return_this_ref() { return *this; }
	void mbr_fct_return_void(int, char) {}

	void mbr_fct_take_i_c_ivec(
		int, char, std::vector<int>) {}	
};

class HasParentBegin: public HasBegin {};

class HasSum_i_i
{
	public:
	int sum(int, int) { return 0; }
};

class HasNestedType
{
	using nested_type = int;
};

int fct_return_int() { return 0; }

const int& fct_return_const_int_ref(int& x) { return x; }

void fct_take_int(int) {}

int fct_take_const_char_ref(const char&) {}

struct fctor_take_i_return_c {
	char operator()(int) { return 'x'; }
};

struct insert_as_string_in
{
	insert_as_string_in(std::vector<std::string>& l):
		m_l(l) {}

	template <typename X>
	bool operator() (X x)
	{
		m_l.insert(m_l.end(), std::to_string(x));
	}

	std::vector<std::string>& m_l;
};

//
// t_toy tests
//

void t_toy_tests()
{
	ASSERT(str<int>() == "i");
	ASSERT(str<char>() == "c");
	ASSERT(str<std::vector<int>>() == "St6vectorIiSaIiEE");
	ASSERT(str<HasBegin>() == "8HasBegin");
}

//
// t_types tests
//
void t_type_list_test()
{	
	using t_tl = type_list<int,char,const float&>;
	t_tl tl = t_tl();

	std::string tl_str = tl.str();
	ASSERT(tl_str == "i,c,f");

	IS_SAME(char, (at<t_tl, 1>::type))
	IS_SAME(const float&, (at<t_tl, 2>::type))
}

void t_is_substituable_test()
{
	ASSERT((is_substituable<int,int>::value));
	ASSERT((!is_substituable<int, char>::value));
	ASSERT((
		is_substituable<
			const int,
			int
		>::value
	));
	ASSERT((
		is_substituable<
			int&,
			int
		>::value
	));
	ASSERT((
		is_substituable<
			int&,
			const int
		>::value
	));
	ASSERT((
		is_substituable<
			const int&,
			int
		>::value
	));
}

void t_get_return_type_test()
{
	IS_SAME(
		get_return_type<
			decltype(&fct_return_int)
		>::type,
		int)
	IS_SAME(
		get_return_type<
			decltype(&fct_return_const_int_ref)
		>::type,
		const int&)
	IS_SAME(
		get_return_type<
			decltype(&HasBegin::mbr_fct_return_this_ref)	
		>::type,
		HasBegin&)
	// Doesnt work with void
	ASSERT(false);
	//IS_SAME(
	//	get_return_type<
	//		decltype(&HasBegin::mbr_fct_return_void)	
	//	>::type,
	//	void)
	IS_SAME(
		get_return_type<
			fctor_take_i_return_c
		>::type,
		char)
	// Doesnt work with lambdas
	ASSERT(false);
}

void t_get_parameter_type_list_test()
{
	IS_SAME(
		get_parameters_type_list<
			decltype(&fct_take_int)
		>::type,
		type_list<int>)			

	IS_SAME(
		get_parameters_type_list<
			decltype(&fct_take_const_char_ref)
		>::type,
		type_list<const char&>)

	IS_SAME(
		get_parameters_type_list<
			fctor_take_i_return_c
		>::type,
		type_list<int>)
	
	IS_SAME(
		get_parameters_type_list<
			decltype(&HasBegin::mbr_fct_take_i_c_ivec)
		>::type,
		(type_list<int, char, std::vector<int>>))

	// Doesnt work with lambdas
	ASSERT(false);
}

// TODO remove this is just to have the compiler to display the type
template<typename T>
struct m { typename T::this_hopefully_does_not_exist z; };

MEMBER_FUNCTION_TYPE(begin, begin_type);
MEMBER_FUNCTION_TYPE(sum, sum_i_i_type, int, int);
MEMBER_FUNCTION_TYPE(sum, sum_c_type, char);
void test_member_function_type()
{
	IS_SAME(
		begin_type<HasBegin>::type,
		int*(HasBegin::*)(void)
	);
	
	IS_SAME(
		begin_type<std::vector<int>>::type,
		decltype(
			std::declval<std::vector<int>>().begin())
			(std::vector<int>::*)()
		)
	
	IS_SAME(
		begin_type<int>::type,
		false_
	);

	IS_SAME(
		sum_i_i_type<HasSum_i_i>::type,
		int(HasSum_i_i::*)(int, int)
	);

	IS_SAME(
		sum_c_type<HasSum_i_i>::type,
		false_
	);

	IS_SAME(
		begin_type<HasBegin&>::type,
		int*(HasBegin::*)(void)
	);
}

MEMBER_FUNCTION_RETURN_TYPE(begin, begin_return_type);
void test_member_function_return_type()
{
	IS_SAME(begin_return_type<HasBegin>::type, int*);
	IS_SAME(begin_return_type<HasBegin&>::type, int*);
	IS_SAME(begin_return_type<int>::type, false_);
}

HAS_MEMBER_FUNCTION(begin, has_begin);
HAS_MEMBER_FUNCTION(end, has_end);
HAS_MEMBER_FUNCTION(sum, has_sum_i_i, int, int);
HAS_MEMBER_FUNCTION(sum, has_sum_c, char);
void t_has_member_function_test()
{
	ASSERT(has_begin<std::vector<int>>::value);
	ASSERT(
		has_begin<
			std::vector<
				std::vector<int>
			>
		>::value);

	ASSERT(has_begin<HasBegin>::value);

	ASSERT(has_begin<HasParentBegin>::value);

	ASSERT(!has_begin<int>::value);


	ASSERT(has_end<std::vector<int>>::value);

	ASSERT(has_sum_i_i<HasSum_i_i>::value);
	ASSERT(!has_sum_c<HasSum_i_i>::value);

}

NESTED_TYPE(iterator_category, get_iterator_category);
void t_has_nested_type()
{
	IS_SAME(
		get_iterator_category<
			std::iterator_traits<
				std::vector<int>::iterator
			>
		>::type,
		std::random_access_iterator_tag	
	);
}

void t_integer_range_test()
{
	ASSERT(
		(int_range<5,3,4,18>::str()
		==
		"5,3,4,18")
	);

	IS_SAME(
		make_range<5>::type,
		(int_range<0,1,2,3,4,5>)
	);

	IS_SAME(
		(cat_range<
			int_range<1,2>,
			int_range<3,4>
		>::type),
		(int_range<1,2,3,4>)
	);
}

void t_codegen_tests()
{
	std::tuple<int, char> tuple { 5, 'A' };

	std::vector<std::string> l;

	foreach_tuple_value<decltype(tuple)>(tuple).apply(
		insert_as_string_in(l));

	// std::to_string applies the same for int and char
	std::vector<std::string> result {"5", "65" };

	ASSERT(l == result);
}

int main(int, char**)
{
	SAY("Toys tests");
	t_toy_tests();
	SAY("Types tests");
	t_type_list_test();
	t_is_substituable_test();
	t_get_return_type_test();
	t_get_parameter_type_list_test();
	t_has_member_function_test();
	SAY("Values tests");
	t_integer_range_test();	
	SAY("Codegen tests");
	t_codegen_tests();
}

