#include <iostream>
#include <vector>
#include <array>
#include <unordered_set>
#include <string>

template<typename T>
void print(T arg) {
	std::cout << arg << " S" << std::endl;
}

template<typename T, typename... Types>
void print(T first_arg, Types... args) {
	std::cout << first_arg << " M" << '\n';
	std::cout << sizeof...(Types) << '\n';
	std::cout << sizeof...(args) << '\n';
	print(args...);
}

template<typename T>
struct Node {
	T value;
	Node* left;
	Node* right;
	Node(T val) : value(val), left(nullptr), right(nullptr) {}
};

template<typename T>
Node<T>* Node<T>::* left = &Node<T>::left;

template<typename T>
Node<T>* Node<T>::* right = &Node<T>::right;

template < typename T, typename... TP>
Node<T>*  traverse(Node<T>* np, TP Node<T>::*... paths) {
	return (np ->* ...->*paths); // np ->* paths1 ->* paths2 ...
}

template<typename T>
class SpacePrint {
private:
	T const& ref;
public:
	SpacePrint(T const& r) : ref(r) {}

	friend std::ostream& operator << (std::ostream& os, SpacePrint<T> sp) {
		return os << sp.ref << ' ';
	}
};

template<typename... T>
void printWithSpace(T... args) {
	( std::cout << ... << SpacePrint(args) ) << "\n";
}

template<typename... T>
void printDoubled(T const&... args) {
	print(args + args...);
}

template <typename T1, typename... TN>
constexpr bool isHomogeneous(T1, TN...) {
	return (std::is_same<T1, TN>::value && ...);
}

template <typename C, typename... Index>
void printElements(C const& c, Index... index) {
	print(c[index]...);
}

/*
namespace std {

// Variadic Deduction Guides
template<typename T, typename... U> std::array(T, U...) ->
std::array < std::enable_if_t<(std::is_same_v<T, U> && ...), T>, 1 + sizeof...(U)>;

} */

class Customer {
private:
	std::string name;
public:
	Customer(std::string const& s) : name(s) {}
	std::string getName() const { return name; }
};

struct CustomerEq {
	bool operator() (Customer const& c1, Customer const& c2) {
		return (c1.getName() == c2.getName());
	}
};

struct CustomerHash {
	std::size_t operator() (Customer const& c) const {
		return std::hash<std::string>()(c.getName());
	}
};

template<typename... Bases>
struct Overloader : Bases... {
	using Bases::operator()...; // OK since C++17
};


int main() {
	std::string s("Hello");
	print(7.5, "hello", s);

	Node<int>* root = new Node<int>(0);
	root->left = new Node<int>(1);
	root->left->right = new Node<int>(2);
	Node<int>* node = traverse(root, left<int>, right<int>);
	std::cout << node->value << std::endl;

	printDoubled(31, 0.54, std::string("Hello"));
	std::vector<std::string> c = {"hello", "world", "Good", "Time"};
	printElements(c, 0, 1, 2, 3);

	using CustomerOp = Overloader<CustomerEq, CustomerHash>;
	std::unordered_set<Customer, CustomerEq, CustomerHash> customerSet;
}