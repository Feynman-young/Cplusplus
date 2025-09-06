#ifndef LINK_HPP
#define LINK_HPP

#include <iostream>
#include <typeinfo>

// declaration of template
template <typename T> void printTypeOfI(T const &t)
{
    std::cout << typeid(t).name() << "\n";
}

template <typename T> void printTypeOfE(T const &t);

#endif // !LINK_HPP
