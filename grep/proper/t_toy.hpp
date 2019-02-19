#pragma once

#include <typeinfo>
#include <string>

template <class T>
void print_type()
{
	std::cout << typeid(T).name() << std::endl;
}

template <class T>
int _print_type_for_list()
{
	std::cout << typeid(T).name() << ",";
}

template <class T>
std::string str()
{
	return typeid(T).name();
}



