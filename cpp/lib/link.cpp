#include <iostream>
#include <typeinfo>
#include "link.hpp"

// The template function will not be instantiated.

template<typename T>
void printTypeOfE(T const& t) {
	std::cout << typeid(t).name() << "\n";
}



