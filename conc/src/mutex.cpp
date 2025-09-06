#include <algorithm>
#include <climits>
#include <exception>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <stack>
#include <string>
#include <thread>
#include <vector>

std::list<int> globalList;
std::mutex globalMutex;

void pushBack(int val)
{
    std::lock_guard<std::mutex> guard(globalMutex);
    globalList.push_back(val);
}

bool isContained(int val)
{
    std::lock_guard<std::mutex> guard(globalMutex);
    return std::find(globalList.begin(), globalList.end(), val) != globalList.end();
}

typedef struct
{
    int a;
    std::string s;

    void print()
    {
        std::cout << a << " " << s << std::endl;
    }
} Data;

class DataWrapper
{
  public:
    template <typename F> void process(F f)
    {
        std::lock_guard<std::mutex> guard(mutex_);
        f(data_);
    }

  private:
    Data data_;
    std::mutex mutex_;
};

Data *globalHole;

void Func(Data &data)
{
    globalHole = &data;
}

DataWrapper globalDataWrapper;

void foo()
{
    globalDataWrapper.process(Func);
    globalHole->print();
}

struct EmptyStackException : std::exception
{
    const char *what() const throw()
    {
        return "Empty Stack";
    }
};

template <typename T> class ThreadSafeStack
{
  public:
    ThreadSafeStack()
    {
    }
    ThreadSafeStack(const ThreadSafeStack &other)
    {
        std::lock_guard<std::mutex> guard(other.mutex_);
        stack_ = other.stack_;
    }
    ThreadSafeStack &operator=(const ThreadSafeStack &) = delete;

    void push(T val)
    {
        std::lock_guard<std::mutex> guard(mutex_);
        stack_.push(val);
    }

    std::shared_ptr<T> pop()
    {
        std::lock_guard<std::mutex> guard(mutex_);
        if (stack_.empty())
        {
            throw EmptyStackException();
        }
        std::shared_ptr<T> const p(std::make_shared<T>(stack_.top()));
        stack_.pop();
        return p;
    }

    void pop(T &val)
    {
        std::lock_guard<std::mutex> guard(mutex_);
        if (stack_.empty())
        {
            throw EmptyStackException();
        }
        val = stack_.top();
        stack_.pop();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> guard(mutex_);
        return stack_.empty();
    }

  private:
    std::stack<T> stack_;
    mutable std::mutex mutex_;
};

class Account
{
  public:
    Account(int balance) : balance_(balance)
    {
    }

    int balance_;
    std::mutex mutex_;

    void print()
    {
        std::cout << "Balance: " << balance_ << std::endl;
    }
};

void transferDeadLock(Account &a, Account &b, int amount)
{
    std::unique_lock<std::mutex> lockA(a.mutex_);
    std::unique_lock<std::mutex> lockB(b.mutex_);
    a.balance_ -= amount;
    b.balance_ += amount;
}

void transferLock(Account &a, Account &b, int amount)
{
    std::lock(a.mutex_, b.mutex_);
    std::unique_lock<std::mutex> lockA(a.mutex_, std::adopt_lock);
    std::unique_lock<std::mutex> lockB(b.mutex_, std::adopt_lock);
    a.balance_ -= amount;
    b.balance_ += amount;
}

void deadLock()
{
    Account accountA(1000);
    Account accountB(2000);

    std::thread tA(transferDeadLock, std::ref(accountA), std::ref(accountB), 100);
    std::thread tB(transferDeadLock, std::ref(accountB), std::ref(accountA), 50);

    tA.join();
    tB.join();

    accountA.print();
    accountB.print();
}

void lock()
{
    Account accountA(1000);
    Account accountB(2000);

    std::thread tA(transferLock, std::ref(accountA), std::ref(accountB), 100);
    std::thread tB(transferLock, std::ref(accountB), std::ref(accountA), 50);

    tA.join();
    tB.join();

    accountA.print();
    accountB.print();
}

class HierarchicalMutex
{
    std::mutex mutex_;
    unsigned long const hierarchicalLevel_;
    unsigned long previousHierarchicalLevel_;
    static thread_local unsigned long thisHierarchicalLevel_;

    void checkForHierarchicalViolation()
    {
        if (thisHierarchicalLevel_ <= hierarchicalLevel_)
        {
            throw std::logic_error("Mutex hierarchical violation.");
        }
    }

    void updateHierarchicalLevel()
    {
        previousHierarchicalLevel_ = thisHierarchicalLevel_;
        thisHierarchicalLevel_ = hierarchicalLevel_;
    }

  public:
    explicit HierarchicalMutex(unsigned long level) : hierarchicalLevel_(level), previousHierarchicalLevel_(0)
    {
    }

    void lock()
    {
        checkForHierarchicalViolation();
        mutex_.lock();
        updateHierarchicalLevel();
    }

    void unlock()
    {
        thisHierarchicalLevel_ = previousHierarchicalLevel_;
        mutex_.unlock();
    }

    bool tryLock()
    {
        checkForHierarchicalViolation();
        if (!mutex_.try_lock())
        {
            return false;
        }
        updateHierarchicalLevel();
        return true;
    }
};

thread_local unsigned long HierarchicalMutex::thisHierarchicalLevel_(ULONG_MAX);

class X
{
  public:
    X(int val) : val_(val) {};

    friend void swap(X &lx, X &rx)
    {
        if (&lx == &rx)
        {
            return;
        }
        std::unique_lock<std::mutex> lockA(lx.mutex_, std::defer_lock);
        std::unique_lock<std::mutex> lockB(rx.mutex_, std::defer_lock);
        std::lock(lockA, lockB);
        std::swap(lx.val_, rx.val_);
    }

  private:
    int val_;
    std::mutex mutex_;
};

class Y
{
  public:
    Y(int val) : val_(val) {};

    friend bool operator==(Y const &lhs, Y const &rhs)
    {
        if (&lhs == &rhs)
        {
            return true;
        }
        int lval = lhs.Val();
        int rval = rhs.Val();
        return lval == rval;
    }

  private:
    int Val() const
    {
        std::lock_guard<std::mutex> guard(mutex_);
        return val_;
    }

    int val_;
    mutable std::mutex mutex_;
};

class DNSEntry
{
};

class DNSCache
{
    std::map<std::string, DNSEntry> entries_;
    mutable std::shared_mutex mutex_;

  public:
    DNSEntry findEntry(std::string const &domain) const
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        std::map<std::string, DNSEntry>::const_iterator const it = entries_.find(domain);
        return (it == entries_.end()) ? DNSEntry() : it->second;
    }

    void updateOrAddEntry(std::string const &domain, DNSEntry const &entry)
    {
        std::lock_guard<std::shared_mutex> guard(mutex_);
        entries_[domain] = entry;
    }
};

class Recursive
{
    std::recursive_mutex mutex_;
    std::string data_;

  public:
    void fA()
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        data_ = "FA";
        std::cout << "fA(), shared variable: " << data_ << '\n';
    }

    void fB()
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        data_ = "FB";
        std::cout << "fB(), shared variable: " << data_ << '\n';
        fA();
    }
};

int main()
{
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i)
    {
        threads.push_back(std::thread(pushBack, i));
    }
    std::cout << isContained(3) << std::endl;
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
    std::cout << isContained(3) << std::endl;

    ThreadSafeStack<int> threadSafeStack;
    threadSafeStack.push(5);
    std::cout << *threadSafeStack.pop() << std::endl;

    deadLock();
    lock();

    Recursive r;
    std::thread tA(&Recursive::fA, &r);
    std::thread tB(&Recursive::fB, &r);
    tA.join();
    tB.join();
}