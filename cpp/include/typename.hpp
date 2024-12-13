#ifndef TYPENAME_HPP
#define TYPENAME_HPP

template <class T>
std::string
type_name() {
    typedef typename std::remove_reference<T>::type TR;
    std::string r = typeid(TR).name(); 
    if (std::is_const<TR>::value)
        r += " const";
    if (std::is_volatile<TR>::value)
        r += " volatile";
    if (std::is_lvalue_reference<T>::value)
        r += "&";
    else if (std::is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}

#endif 