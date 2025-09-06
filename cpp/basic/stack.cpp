#include <cassert>
#include <deque>
#include <iostream>
#include <string>
#include <vector>

template <typename T> class Stack
{
  public:
    Stack() = default;
    Stack(T const &elem);

    void push(T const &elem);
    void pop();
    T const &top() const;
    bool empty() const
    {
        return elems.empty();
    }
    void print(std::ostream &stream) const;

    friend std::ostream &operator<<(std::ostream &stream, Stack<T> const &s)
    {
        s.print(stream);
        return stream;
    }

  private:
    std::vector<T> elems;
};

template <typename T> Stack<T>::Stack(const T &elem) : elems({elem})
{
}

template <typename T> void Stack<T>::push(T const &elem)
{
    elems.push_back(elem);
}

template <typename T> void Stack<T>::pop()
{
    elems.pop_back();
}

template <typename T> T const &Stack<T>::top() const
{
    return elems.back();
}

template <typename T> void Stack<T>::print(std::ostream &stream) const
{
    for (T const &elem : elems)
    {
        stream << elem << " ";
    }
}

template <> class Stack<std::string>
{
  public:
    void push(std::string const &elem);
    void pop();
    std::string const &top() const;
    bool empty() const
    {
        return elems.empty();
    }
    void print(std::ostream &stream) const;

    friend std::ostream &operator<<(std::ostream &stream, Stack<std::string> const &s)
    {
        s.print(stream);
        return stream;
    }

  private:
    std::deque<std::string> elems;
};

void Stack<std::string>::push(std::string const &elem)
{
    elems.push_back(elem);
}

void Stack<std::string>::pop()
{
    elems.pop_back();
}

std::string const &Stack<std::string>::top() const
{
    return elems.back();
}

void Stack<std::string>::print(std::ostream &stream) const
{
    for (std::string const &elem : elems)
    {
        stream << elem << " ";
    }
}

int main()
{
    Stack<int> intStack;
    Stack<std::string> stringStack;

    intStack.push(7);
    std::cout << intStack.top() << '\n';

    stringStack.push("Hello");
    std::cout << stringStack.top() << '\n';
    stringStack.pop();

    stringStack.push("Hello");
    stringStack.push("World");
    std::cout << stringStack << std::endl;

    Stack stack = 0;
    std::cout << stack.empty() << std::endl;
    std::cout << stack.top() << std::endl;
}