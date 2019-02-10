

template <typename ...Args>
struct void_
{
	using type = void;
};

template <typename T>
struct remove_ref
{
	using type = T;
};
template <typename T>
struct remove_ref<T&>
{
	using type = T;
};

template <typename T>
struct remove_const
{
	using type = T;
};
template <typename T>
struct remove_const<const T>
{
	using type = T;
};


template <typename T>
struct get_return_type { /* static_assert<false> */ };

template <typename T1, typename T2>
struct is_substituable:
	std::is_same<
		typename remove_ref<
			typename remove_const<T1>::type
		>::type,
		typename remove_ref<
			typename remove_const<T2>::type
		>::type
	> {};


// First argument is a type that can only exist if a class
// Tp (Tp == T) .
#define DEFINE_TYPE_PROPERTY(MEMBER_TYPE_FROM_T, NAME) \
template <class T> \
struct NAME  \
{ \
	template <class Tp> \
	static char test(MEMBER_TYPE_FROM_T) {}; \
 \
	template <class Tp> \
	static int test(...) {}; \
 \
	static int type_of(int){};\
\
	static const bool result =  \
		sizeof(test<T>(0)) == sizeof(char); \
}

