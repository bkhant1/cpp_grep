#include <assert.hpp>
#include <grep.hpp>
#include <vector>

using iVec = std::vector<int>;
using ivVec = std::vector<std::vector<int>>;

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

void g_helper_tests()
{
	ASSERT(is_iterator<iVec::iterator>::value);		
	ASSERT(!is_iterator<equal_to<int>>::value);
	
	ASSERT(!is_tuple<int>::value);
	ASSERT(is_tuple<std::tuple<char>>::value);

	IS_SAME(
		check_iterability<iVec>::type,
		Iterability::runtime_iterable
	);
	
	IS_SAME(
		check_iterability<int>::type,
		Iterability::not_iterable
	);

	IS_SAME(
		(check_iterability<std::tuple<int, char>>::type),
		Iterability::compiletime_iterable
	);
}

void g_test_no_options()
{
	std::vector<std::vector<int>> vec_of_vec = 
	{
		{1,2},
		{-1,3},
		{9,10},
		{1,9}
	};
	std::list<int*> grep_result_only_runtime = 
		grep<int>(
			vec_of_vec,
			equal_to<int>(1)
		);
	std::list<int*> expected_result_only_runtime 
	{
		&(vec_of_vec[0][0]),
		&(vec_of_vec[3][0])
	};
	ASSERT(grep_result_only_runtime == expected_result_only_runtime);

	std::tuple<int, char, float> tup 
	{
		1,
		'x',
		0.3f
	};
	std::list<char*> grep_result_only_compiletime =
		grep<char>(
			tup,
			equal_to<char>('x')
		);	
	std::list<char*> expected_result_only_compiletime
	{
		&std::get<1>(tup)
	};
	ASSERT(grep_result_only_compiletime == expected_result_only_compiletime);

	using isVi_t = std::tuple<int, std::string, iVec>;
	isVi_t in_mix_one 
	{
		5,
		"Bonjour",
		{4,3,2,1}
	};
	std::tuple<iVec, char, isVi_t> a_mix_one
	{
		{1,2,3,4,6},
		'B',
		in_mix_one
	};
	auto equal_to_char_B = equal_to<char>('B');
	std::list<char*> grep_result_mix = 
		grep<
			char,
			decltype(a_mix_one),
			equal_to<char>& // forcing the reference here
		>(a_mix_one, equal_to_char_B);
	std::list<char*> expected_result_mix 
	{
		&(std::get<1>(a_mix_one)),
		&(std::get<1>(
			std::get<2>(a_mix_one)
		)[0])
	};	
	ASSERT(grep_result_mix == expected_result_mix);
}

int main(int, char**)
{
	SAY("Helpers tests");
	g_helper_tests();	

	SAY("Grep test");
	g_test_no_options();


	using isVi_t = std::tuple<int, std::string, iVec>;

	isVi_t in_complex_one 
	{
		5,
		"Bonjour",
		{4,3,2,1}
	};

	std::tuple
	<
		iVec,
		char,
		isVi_t
	>
	a_complex_one
	{
		{1,2,3,4,6},
		'B',
		in_complex_one
	};

	auto equal_to_char_B = equal_to<char>('B');


	return 0;
}
