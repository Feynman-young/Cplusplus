#include <iostream>
#include <vector>

// template metaprogramming
template <unsigned P, unsigned D> struct DoIsPrime
{
    static constexpr bool value = (P % D != 0) && DoIsPrime<P, D - 1>::value;
};

template <unsigned P> struct DoIsPrime<P, 2>
{
    static constexpr bool value = (P % 2 != 0);
};

template <unsigned P> struct IsPrime
{
    static constexpr bool value = DoIsPrime<P, P / 2>::value;
};

// constexpr
constexpr bool doIsPrime(unsigned p, unsigned d)
{
    return d != 2 ? (p % d != 0) && (doIsPrime(p, d - 1)) : (p % 2 != 0);
}

constexpr bool isPrime(unsigned p)
{
    return p < 4 ? (p > 1) : doIsPrime(p, p / 2);
}

constexpr bool isPrimeWithLoop(unsigned p)
{
    for (unsigned d = 2; d < p / 2; ++d)
    {
        if (p % d == 0)
        {
            return false;
        }
    }
    return p > 1;
}

// Use partial specialization to select at compile time between different implementations
template <int SZ, bool = isPrime(SZ)> struct Helper
{
};

// SFINAE: Substitution Failure Is Not An Error
// Number of elements in a raw array:
template <typename T, unsigned N> std::size_t len(T (&)[N])
{
    return N;
}

// Number of elements for a type having size_type:
template <typename T> auto len(T const &t) -> decltype((void)(t.size()), t.size())
{
    return t.size();
}

// fallback for all other types
std::size_t len(...)
{
    return 0;
}

template <typename T, typename... Args> void print(T const &head, Args const &...args)
{
    std::cout << head << '\n';
    if constexpr (sizeof...(args) > 0)
    {
        print(args...);
    }
}

template <typename T> void foo(T t)
{
    if constexpr (std::is_integral_v<T>)
    {
        if (t > 0)
        {
            foo(t - 1);
        }
    }
    else
    {
        undeclaredFunc(t);
        static_assert(!std::is_integral_v<T>, "No Integral");
    }
}

int main()
{
    std::cout << IsPrime<9>::value << "\n";
    std::cout << isPrime(9) << "\n";
    // const bool b = isPrimeWithLoop(9); // Evaluated at compile time.
    std::cout << isPrimeWithLoop(9) << "\n";

    int a[10];
    std::cout << len(a) << "\n";
    std::cout << len("hello") << "\n";
    std::vector<int> v{1, 2, 3, 4, 5, 6};
    std::cout << len(v) << "\n";
    int *p = nullptr;
    std::cout << len(p) << "\n";
    std::allocator<int> m;
    std::cout << len(m) << "\n";
    print('a', 'b', 'c');
    foo(42);
}