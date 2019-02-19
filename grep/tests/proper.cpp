#include <assert.hpp>
#include <t_toy.hpp>
#include <t_types.hpp>
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

int fct_return_int() { return 0; }

const int& fct_return_const_int_ref(int& x) { return x; }

void fct_take_int(int) {}

int fct_take_const_char_ref(const char&) {}

struct fctor_take_i_return_c {
	char operator()(int) { return 'x'; }
};

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
void test_member_function_type()
{
	IS_SAME(
		begin_type<HasBegin>::type,
		int*(HasBegin::*)(void)
	);
	
	IS_SAME(
		begin_type<int>::type,
		false_
	);
}

HAS_MEMBER_FUNCTION(begin, has_begin);
void t_has_member_function_test()
{
	ASSERT(has_begin<HasBegin>::value);
	ASSERT(!has_begin<int>::value);
}

int main(int, char**)
{
	SAY("Toy tests");
	t_toy_tests();
	SAY("Types tests");
	t_type_list_test();
	t_is_substituable_test();
	t_get_return_type_test();
	t_get_parameter_type_list_test();
	t_has_member_function_test();
}

