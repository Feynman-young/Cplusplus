#include <iostream>
#include <vector>
#include <deque>
#include <bitset>
#include <array>

// Clarify that an identifier inside a template is a type
// print elements of an STL container
template<typename T>
void printContainer(T const& c) {
	typename T::const_iterator pos;
	typename T::const_iterator end(c.end());
	for (pos = c.begin(); pos != end; pos++) {
		std::cout << *pos << ' ';
	}
	std::cout << '\n';
}

// initialization
class Construct {
public:
	Construct() {
		std::cout << "Default Construct Func()" << std::endl;
	}
};

// zero initialization
template<typename T>
void foo() {
	T x{};
	x = T();
}

template<typename T, int N, int M>
bool less(T (&a) [N], T(&b) [M]) {
    for (int i = 0; i < N && i < M; i++) {
		if (a[i] < b[i]) return true;
		if (b[i] < a[i]) return false;
    }
	return N < M;
}

template<typename T>
struct CArray;	// primary template

template<typename T, std::size_t SZ>
struct CArray<T[SZ]> { // // partial specialization for arrays of known bounds
	static void print() {
		std::cout << "print() for T[" << SZ << "]\n";
	}
};

template<typename T, std::size_t SZ>
struct CArray<T(&)[SZ]> {
	// partial specialization for references to arrays of known bounds
	static void print() {
		std::cout << "print() for T(&)[" << SZ << "]\n";
	}
};

template<typename T>
struct CArray<T[]> { // partial specialization for arrays of unknown bounds
	static void print() {
		std::cout << "print() for T[]\n";
	}
};

template<typename T>
struct CArray<T(&)[]> { 
	// partial specialization for references to arrays of unknown bounds
	static void print() {
		std::cout << "print() for T(&)[]\n";
	}
};

template<typename T>
struct CArray<T*> {
	static void print() {
		std::cout << "print() fot T*\n";
	}
};

template<typename T1, typename T2, typename T3>
void func_t(int a1[7], int a2[], int(&a3)[42], int(&x0)[], T1 x1, T2& x2, T3&& x3) {
	CArray<decltype(a1)>::print();
	CArray<decltype(a2)>::print();
	CArray<decltype(a3)>::print();
	CArray<decltype(x0)>::print();
	CArray<decltype(x1)>::print();
	CArray<decltype(x2)>::print();
	CArray<decltype(x3)>::print();
}

template<typename T>
class Stack {
private:
	std::deque<T> elements;
public:
	Stack() = default;

	void push(T const& elem);
	void pop();
	T const& top() const;
	bool empty() const { return elements.empty(); }
	void print(std::ostream& stream) const;

	friend std::ostream& operator<< (std::ostream& stream, Stack<T> const& s) {
		s.print(stream);
		return stream;
	}

	template<typename V>
	Stack& operator = (Stack<V> const&);
};

template<typename T>
void Stack<T>::push(T const& elem) {
	elements.push_back(elem);
}

template<typename T>
void Stack<T>::pop() {
	elements.pop_back();
}

template<typename T>
T const& Stack<T>::top() const {
	return elements.back();
}

template<typename T>
void Stack<T>::print(std::ostream& stream) const {
	for (T const& elem : elements) {
		stream << elem << " ";
	}
}

template<typename T>
template<typename V>
Stack<T>& Stack<T>::operator = (Stack<V> const& s) {
	Stack<V> tmp(s);
	elements.clear();
	while (!tmp.empty()) {
		elements.push_front(tmp.top());
		tmp.pop();
	}
	return *this;
}

class BoolString {
private:
	std::string value;
public:
	BoolString(std::string const& s) : value(s) {}

	template<typename T = std::string>
	T get() const { return value; }
};

template<>
inline bool BoolString::get<bool>() const {
	return value == "true" || value == "1" || value == "on";
}

template<unsigned long N>
void printBitSet(std::bitset<N> const& bs) {
	std::cout << bs.template to_string<char, std::char_traits<char>, std::allocator<char>>();
}

template<typename T = long double>
constexpr T PI{ 3.14159265358 };

template<typename T> T val{};

template<int N>
std::array<int, N> arr{};

template<auto N>
constexpr decltype(N) dval = N;

int main() {
	std::vector<int> v{1, 2, 3, 4, 5};
	printContainer(v);

	Construct c1;
	Construct c2{};
	Construct c3 = {};
	Construct c4 = Construct();

	int a[] = { 1, 2, 3};
	int b[] = { 1, 2, 3, 4, 5 };
	std::cout << less(a, b) << std::endl;
	std::cout << less("abc", "abcd") << std::endl;

	int c[42];
	CArray<decltype(c)>::print();

	int x[] = {0, 8, 15};
	CArray<decltype(x)>::print();
	func_t(c, c, c, x, x, x, x);

	Stack<int> intStack;
	Stack<float> floatStack;

	intStack.push(1);
	intStack.push(2);
	intStack.push(3);

	floatStack = intStack;
	std::cout << floatStack << std::endl;

	std::cout << std::boolalpha;
	BoolString boolString("on");
	std::cout << boolString.get() << std::endl;
	std::cout << boolString.get<bool>() << std::endl;

	std::bitset<8> bs("10011100");
	printBitSet(bs); 
	
	// The lambda is transformed into a class definition.
	auto lambda = [] (auto x, auto y) { return x + y; };

	std::cout << "\n" << PI<float> << "\n";
	std::cout << PI<double> << "\n";
	std::cout << PI<> << "\n";

	val<long> = 42;
	std::cout << val<long> << "\n";

	std::cout << dval<'c'> << "\n";
}