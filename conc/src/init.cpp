#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class Resource
{
  public:
    Resource(int a, const std::string &s) : a_(a), s_(s)
    {
    }

    void print()
    {
        std::cout << a_ << " " << s_ << std::endl;
    }

  private:
    int a_;
    std::string s_;
};

std::shared_ptr<Resource> globalResourcePtrA;
std::shared_ptr<Resource> globalResourcePtrB;
std::shared_ptr<Resource> globalResourcePtrC;
std::shared_ptr<Resource> globalResourcePtrD;

std::mutex globalResourceMutexA;
std::mutex globalResourceMutexB;
std::mutex globalResourceMutexC;

std::once_flag resourceFlag;

void commonResourceFunc()
{
    if (!globalResourcePtrA)
    {
        globalResourcePtrA.reset(new Resource(0, __func__));
    }
    globalResourcePtrA->print();
}

void lazyResourceInitialization()
{
    std::unique_lock<std::mutex> lock(globalResourceMutexB);
    if (!globalResourcePtrB)
    {
        globalResourcePtrB.reset(new Resource(0, __func__));
    }
    lock.unlock();
    globalResourcePtrB->print();
}

void doubleCheckLocking() // Undefined Behaviour
{
    if (!globalResourcePtrC)
    {
        std::lock_guard<std::mutex> guard(globalResourceMutexC);
        if (!globalResourcePtrC)
        {
            globalResourcePtrC.reset(new Resource(0, __func__));
        }
    }
    globalResourcePtrC->print();
}

void initResource()
{
    globalResourcePtrD.reset(new Resource(0, __func__));
}

void onceFlagResource()
{
    std::call_once(resourceFlag, initResource);
    globalResourcePtrD->print();
}

void commonResourceFuncThreads()
{
    std::vector<std::thread> threads;

    for (int i = 0; i < 5; ++i)
    {
        threads.push_back(std::thread(commonResourceFunc));
    }

    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}

void lazyResourceInitializationThreads()
{
    std::vector<std::thread> threads;

    for (int i = 0; i < 5; ++i)
    {
        threads.push_back(std::thread(lazyResourceInitialization));
    }

    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}

void doubleCheckLockingThreads()
{
    std::vector<std::thread> threads;

    for (int i = 0; i < 5; ++i)
    {
        threads.push_back(std::thread(doubleCheckLocking));
    }

    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}

void onceFlagResourceThreads()
{
    std::vector<std::thread> threads;

    for (int i = 0; i < 5; ++i)
    {
        threads.push_back(std::thread(onceFlagResource));
    }

    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}

class Singleton
{
  private:
    static std::unique_ptr<Singleton> instance_;
    static std::once_flag onceFlag_;

    Singleton()
    {
        std::cout << "Singleton instance created." << std::endl;
    }

  public:
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

    static Singleton *instance()
    {
        std::call_once(onceFlag_, []() { instance_.reset(new Singleton); });
        return instance_.get();
    }

    void doImpl()
    {
        std::cout << "Singleton instance work." << std::endl;
    }
};

std::unique_ptr<Singleton> Singleton::instance_ = nullptr;
std::once_flag Singleton::onceFlag_;

int main()
{
    commonResourceFuncThreads();
    lazyResourceInitializationThreads();
    doubleCheckLockingThreads();
    onceFlagResourceThreads();

    Singleton::instance()->doImpl();
}