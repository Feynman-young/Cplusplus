#include <iostream>

class A {
public:
    virtual void vfunc1() {
        std::cout << "Class A vfunc1()" << std::endl;
    }

    virtual void vfunc2() {
        std::cout << "Class A vfunc2()" << std::endl;
    }

    void func1() {
        std::cout << "Class A func1()" << std::endl;
    }

    void func2() {
        std::cout << "Class A func2()" << std::endl;
    }
private:
    int m_data1, m_data2;
};

class B : public A {
public:
    virtual void vfunc1() {
        std::cout << "Class B vfunc1()" << std::endl;
    }

    void func2() {
        std::cout << "Class B func2()" << std::endl;
    }
private:
    int m_data3;
};

class C : public B {
public:
    virtual void vfunc1() {
        std::cout << "Class C vfunc1()" << std::endl;
    }

    void func2() {
        std::cout << "Class C func2()" << std::endl;
    }
private:
    int m_data1, m_data4;
};

class Quote {
public:
    Quote() = default;
    Quote(const std::string& book_no, double price) : book_no(book_no), price(price) {}
    std::string isbn() const { return book_no; }
    virtual double net_price(std::size_t n) const { return n * price; }
    virtual ~Quote() = default; // virtual destructors are needed;
private:
    std::string book_no;
protected:
    double price = 0;
};

class BulkQuote : public Quote {
public:
    BulkQuote() = default;
    BulkQuote(const std::string& book_no, double price, std::size_t num, double discount) :
        Quote(book_no, price), minimum_purchase_num(num), discount(discount) {}
    double net_price(std::size_t) const override;
private:
    std::size_t minimum_purchase_num;
    double discount = 0;
};

double BulkQuote::net_price(std::size_t n) const {
    if (n > minimum_purchase_num) {
        return n * (1 - discount) * price;
    } else {
        return n * price;
    }
}

class Base {
public:
    static void print() {
        std::cout << "Static print()" << '\n';
    }
};

class Derived : public Base {
public:
    void f(const Derived& obj) {
        Base::print();
        Derived::print();
        obj.print();
        print();
    }
};

int main() {
    C c;
    A* a = &c;

    a->func1();
    a->vfunc1();

    BulkQuote bulkQuote("Book", 10, 2, 0.1);
    Quote* quoteP = &bulkQuote;
    std::cout << quoteP->net_price(5) << '\n';
    Quote& quoteR = bulkQuote;
    std::cout << quoteR.isbn() << '\n';

    Derived derived;
    derived.f(derived);
}