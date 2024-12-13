#include <iostream>
#include <functional>		// For std::cref()

// Lvalue && Rvalue

template<typename T>
void printByValue(T arg) {
	std::cout << typeid(T).name() << ": " << arg << "\n";
}

std::string hello() {
	return "Hello";
}

template<typename T>
void printByReference(T& arg) {
	std::cout << typeid(T).name() << ": " << arg << "\n";
}

void modifyConst(const int* p) {
	int* tmp = const_cast<int*>(p);
	*tmp = 42;
}

template<typename T>
// requires (!std::is_const_v<T>)
void outR(T& arg) {
	if (std::is_array<T>::value) {
		std::cout << "Got array of " << std::extent<T>::value << " elements\n";
	}
}

template<typename T>
void passR(T&& arg) {
	T x; // For passed lvalues, x is a reference, which requires an initializer;
	std::cout << typeid(T).name() << "\n";
}

void printString(std::string const& s) {
	std::cout << s << "\n";
}

template<typename T>
void printT(T arg) {
	printString(arg); // Might convert arg back to std::string
}

template<typename T>
T retR(T&& p) { // p is a forwarding reference
	return T{}; // OOPS: returns by reference when called for lvalue;
}

template<typename T>
T retV(T p) { // p might be a reference
	return T{}; // OOPS: returns a reference if T is a reference
}

// For more safe, there are two options
// 1. Convert type T to a nonrefence
template<typename T>
typename std::remove_reference<T>::type retVConvert(T p) {
	return T{};
}

// 2. Use auto
template<typename T>
auto retVAuto(T p) {
	return T{};
}

template<typename T1, typename T2>
constexpr std::pair<typename std::decay<T1>::type, typename std::decay<T2>::type>
make_pair(T1&& a, T2&& b) {
	return std::pair<typename std::decay<T1>::type, typename std::decay<T2>::type>
		(std::forward<T1>(a), std::forward<T2>(b));
}

int main() {
	// std::string hello ();
	std::string s = "Hello";
	// Small string optimization and Copy on write
	printByValue(s);						// Copy constructor
	printByValue(std::string("Hello"));		// Copy usually optimized away
	printByValue(hello());					// Copy usually optimized away
	printByValue(std::move(s));				// Move constructor

	// Passing by value decays
	printByValue("hi");
	int a[4] = { 1, 2, 3, 4 };
	printByValue(a);

	// Passing by reference
	printByReference(a);

	int b = 2;
	const int* ptr = &b;
	modifyConst(ptr);
	std::cout << b << "\n";

	outR(a);
	outR("hello");

	passR(42);

	std::string ns = "hello";
	printT(ns);				// Print s passed by value
	// Create a std::reference_wrapper<std::string const> object
	printT(std::cref(ns));	// Print s passed by reference
}