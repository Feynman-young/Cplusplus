#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>

template <typename T, std::size_t M> class Stack
{
  public:
    Stack();

    void push(T const &elem);
    void pop();
    [[nodiscard]] T const &top() const;
    [[nodiscard]] bool empty() const
    {
        return num == 0;
    }
    [[nodiscard]] std::size_t size() const
    {
        return num;
    }

  private:
    std::array<T, M> elems;
    std::size_t num;
};

template <typename T, std::size_t M> Stack<T, M>::Stack() : elems(), num(0)
{
}

template <typename T, std::size_t M> void Stack<T, M>::push(T const &elem)
{
    if (num >= M)
    {
        std::cerr << "Stack Overflow" << std::endl;
    }
    else
    {
        elems[num++] = elem;
    }
}

template <typename T, std::size_t M> void Stack<T, M>::pop()
{
    if (empty())
    {
        std::cerr << "Empty Stack" << std::endl;
    }
    --num;
}

template <typename T, std::size_t M> T const &Stack<T, M>::top() const
{
    if (empty())
    {
        std::cerr << "Empty Stack" << std::endl;
    }
    return elems[num - 1];
}

template <auto Val, typename T = decltype(Val)> T add(T x)
{
    return x + Val;
}

template <char const *name> class Message
{
};

extern char const s0[] = "hi";
char const s1[] = "hi";

int main()
{
    Stack<int, 20> int20Stack;
    Stack<int, 40> int40Stack;
    Stack<std::string, 40> stringStack;

    int20Stack.push(7);
    std::cout << int20Stack.top() << '\n';
    int20Stack.pop();

    stringStack.push("hello");
    std::cout << stringStack.top() << '\n';
    stringStack.pop();

    std::array<int, 5> s = {1, 2, 3, 4, 5};
    std::transform(s.begin(), s.end(), s.begin(), add<1>);
    for (int v : s)
        std::cout << v << " ";

    static char const s2[] = "hi";

    Message<s0> m0; // C++ 11
    Message<s1> m1; // C++ 14
    Message<s2> m2; // C++ 17
}