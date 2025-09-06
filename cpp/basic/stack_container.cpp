#include <cassert>
#include <deque>
#include <iostream>
#include <vector>

template <typename T, template <typename> class Container = std::deque> class Stack
{
  private:
    Container<T> elements;

  public:
    void push(T const &elem);
    void pop();
    T const &top() const;
    bool empty() const
    {
        return elements.empty();
    }

    template <typename V, template <typename> class ContainerR>
    Stack<T, Container> &operator=(Stack<V, ContainerR> const &);

    template <typename, template <typename> class> friend class Stack;
};

template <typename T, template <typename> class Container> void Stack<T, Container>::push(T const &elem)
{
    elements.push_back(elem);
}

template <typename T, template <typename> class Container> void Stack<T, Container>::pop()
{
    assert(!elements.empty());
    elements.pop_back();
}

template <typename T, template <typename> class Container> T const &Stack<T, Container>::top() const
{
    assert(!elements.empty());
    return elements.back();
}

template <typename T, template <typename> class Container>
template <typename V, template <typename> class ContainerR>
Stack<T, Container> &Stack<T, Container>::operator=(Stack<V, ContainerR> const &s)
{
    elements.clear();
    elements.insert(elements.begin(), s.elements.begin(), s.elements.end());
    return *this;
}

int main()
{
    Stack<int> intStack;
    intStack.push(5);
    Stack<int, std::vector> intVStack;
    intVStack.push(1);
    intVStack.push(2);
    intVStack.push(3);
    intVStack.push(4);
    intVStack.push(5);
    intVStack.pop();
    intStack = intVStack;
    std::cout << intStack.top() << '\n';
}