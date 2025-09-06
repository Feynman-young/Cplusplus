#include "typename.hpp"
#include <functional>
#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>

// Callables
// Pointer to functions
void func(int i)
{
    std::cout << "Func called for: " << i << '\n';
}

// Class types with an operator (), including lambdas;
class FuncObj
{
  public:
    void operator()(int i) const
    {
        std::cout << "FuncObj::() called for: " << i << "\n";
    }
};

template <typename Iter, typename Callable> void foreach (Iter current, Iter end, Callable op)
{
    while (current != end)
    {
        op(*current);
        ++current;
    }
}

class Func
{
  public:
    void member_func(int i) const
    {
        std::cout << "Func::mem_func for: " << i << '\n';
    }
};

// Use std::invoke to call member functions.
template <typename Iter, typename Callable, typename... Args>
void foreach (Iter current, Iter end, Callable op, Args const &...args)
{
    while (current != end)
    {
        std::invoke(op, args..., *current);
        ++current;
    }
}

// Support move semantics by perfect forwarding
template <typename Callable, typename... Args> decltype(auto) call(Callable &&op, Args &&...args)
{
    if constexpr (!std::is_same_v<std::invoke_result_t<Callable, Args...>, void>)
    {
        return std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...);
    }
    else
    {
        std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...);
    }
}

// type_traits
template <typename T> class C
{
    static_assert(!std::is_same_v<std::remove_cv_t<T>, void>, "Invalid void type");

  public:
    template <typename V> void f(V &&v)
    {
        if constexpr (std::is_reference_v<T>)
        {
        }
        if constexpr (std::is_convertible_v<std::decay_t<V>, T>)
        {
        }
        if constexpr (std::has_virtual_destructor_v<V>)
        {
        }
    }
};

template <typename T> void printAddress(T &&x)
{
    auto p = std::addressof(x);
    std::cout << p << '\n';
}

template <typename T> void paramIsReference(T)
{
    std::cout << "T is reference: " << std::is_reference_v<T> << std::endl;
}

template <typename T, T Z = T{}> class RefMem
{
  private:
    T m;

  public:
    RefMem() : m(Z)
    {
    }
};

int null = 0;

template <typename T> class Cont
{
  private:
    T *elem;

  public:
    template <typename D = T> std::conditional<std::is_move_constructible_v<D>, D &&, D &>::type foo()
    {
        std::cout << type_name<D>() << '\n';
        if constexpr (std::is_move_constructible_v<D>)
        {
            return std::move(*new D{});
        }
        else
        {
            static D d{};
            return d;
        }
    }
};

struct Node
{
    std::string value;
    Cont<Node> next;
};

int main()
{
    std::vector<int> n{1, 2, 3, 4, 5};
    foreach (n.cbegin(), n.cend(), func)
        ;
    foreach (n.cbegin(), n.cend(), FuncObj())
        ;
    foreach (n.cbegin(), n.cend(), [](int i) { std::cout << "Lambda for: " << i << '\n'; })
        ;

    Func f;
    foreach (n.cbegin(), n.cend(), &Func::member_func, f)
        ;
    foreach (n.cbegin(), n.cend(), [](std::string const &s, int i) { std::cout << s << i << '\n'; }, "value: ")
        ;

    C<int> c;
    c.f(2.1);

    std::cout << type_name<std::remove_const_t<std::remove_reference_t<int const &>>>() << "\n";
    // A reference is not a const => int const
    std::cout << type_name<std::remove_reference_t<std::remove_const_t<int const &>>>() << "\n";

    std::cout << type_name<std::make_unsigned<int>::type>() << '\n';
    std::cout << type_name<std::add_rvalue_reference_t<int>>() << '\n';
    std::cout << type_name<std::add_rvalue_reference_t<int const>>() << '\n';
    std::cout << type_name<std::add_rvalue_reference_t<int &>>() << '\n';

    std::cout << std::is_assignable_v<int, int> << '\n';

    std::string s = "hello";
    printAddress(&s);
    printAddress(2);

    std::cout << type_name<decltype(1 ? std::declval<int>() : std::declval<double>())>() << '\n';
    std::cout << type_name<decltype(1 ? std::declval<double>() : std::declval<int>())>() << '\n';
    std::cout << type_name<decltype(1 ? std::declval<int>() : std::declval<float>())>() << '\n';

    int i = 0;
    int &ref = i;
    paramIsReference(i);
    paramIsReference(ref);
    paramIsReference<int &>(i);
    paramIsReference<int &>(ref);

    RefMem<int> rm1, rm2;
    rm1 = rm2;
    // RefMem<int&> rm3;
    // RefMem<int&, 0> rm4;
    RefMem<int &, null> rm5, rm6;
    // rm5 = rm6;
}