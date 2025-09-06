#include "link.hpp"
#include <iostream>
#include <typeinfo>

// The template function will not be instantiated.

template <typename T> void printTypeOfE(T const &t)
{
    std::cout << typeid(t).name() << "\n";
}
