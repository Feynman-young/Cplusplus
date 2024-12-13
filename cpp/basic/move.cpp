#include <iostream>

class X {};

void g(X&) {
	std::cout << "g() for variable \n";
}

void g(X const&) {
	std::cout << "g() for constant\n";
}

void g(X&&) {
	std::cout << "g() for movable object\n";
}

void f(X& val) {
	g(val);	// val is non-const lvalue;
}

void f(X const& val) {
	g(val); // val is const lvalue;
}

void f(X&& val) {
	g(std::move(val));	// val is non-const rvalue;
}

// Perfect forwarding
template<typename T>
void func(T&& val) {
	g(std::forward<T>(val));
}

class Person {
private:
	std::string name;
public:
	// Constructor for passed initial name;
	explicit Person(std::string const& s) : name(s) {
		std::cout << "Copying string-constr for " << name << "\n";
	}

	explicit Person(std::string&& s) : name(std::move(s)) {
		std::cout << "Moving  string-constr for " << name << "\n";
	}

	// Copy and move constructor
	Person(Person const& p) : name(p.name) {
		std::cout << "Copying constr Person for " << name << "\n";
	}

	Person(Person&& p) noexcept : name(std::move(p.name)) {
		std::cout << "Moving  constr Person for " << name << "\n";
	}
};

class PersonT {
private:
	std::string name;
public:
	// Generic constructor for passed initial name;
	template<typename STR>
	explicit PersonT(STR&& s) : name(std::forward<STR>(s)) {
		std::cout << "TMPL-CONSTR for " << name << "\n";
	}

	// Copy and Move Constructor
	PersonT(PersonT const& p) : name(p.name) {
		std::cout << "Copying constr PersonT for " << name << "\n";
	}

	PersonT(PersonT&& p) noexcept : name(std::move(p.name)) {
		std::cout << "Moving  constr PersonT for " << name << "\n";
	}
};

// Disable templates with enable_if<>4
template<typename T>
typename std::enable_if<(sizeof(T) > 4)>::type
foo() {
	std::cout << "Size of T: " << sizeof(T) << "\n";
}

template<typename T>
using EnableIfString = std::enable_if_t<std::is_convertible_v<T, std::string>>;

class PersonE {
private:
	std::string name;
public:
	// Generic constructor for passed initial name;
	template<typename STR, typename = EnableIfString<STR>>
	explicit PersonE(STR&& s) : name(std::forward<STR>(s)) {
		std::cout << "TMPL-CONSTR for " << name << "\n";
	}

	// Copy and Move Constructor
	PersonE(PersonE const& p) : name(p.name) {
		std::cout << "Copying constr PersonT for " << name << "\n";
	}

	PersonE(PersonE&& p) noexcept : name(std::move(p.name)) {
		std::cout << "Moving  constr PersonT for " << name << "\n";
	}
};

class C {
public:
	C() = default;
	template<typename T>
	C(T const&) {
		std::cout << "TEPL Copy Constructor" << "\n";
	}
};

class D {
public:
	D(D const volatile&) = delete;
	D() = default;
	template<typename T>
	D(T const&) {
		std::cout << "TEPL Copy Constructor" << "\n";
	}
};

template<typename T>
requires std::is_constructible_v<T, std::string>
void fr() {
	std::cout << "Requries keyword" << "\n";
}

int main() {
	// Perfect forwarding
	X v;
	X const c;
	func(v);
	func(c);
	func(X());
	func(std::move(v));

	std::string s = "name";
	Person p1(s);
	Person p2("Temp string");
	Person p3(p1);
	Person p4(std::move(p1));

	PersonT pt1(s);
	PersonT pt2("Temp String");
	// For a nonconstant lvalue PersonT,
	// TMPL Constructor is a better match 
	// ERROR: PersonT pt3(pt1);
	PersonT pt4(std::move(pt1));

	// std::enable_if<>
	foo<std::string>();

	PersonE pe1(s);
	PersonE pe2("Temp String");
	PersonE pe3(pe1);
	PersonE pe4(std::move(pe1));

	C c1;
	// Use the pre-defined copy constructor;
	C c2(c1);

	D d1;
	D d2(d1);

	fr<std::string> ();
}