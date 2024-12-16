#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

void threadWorkA(std::string s) { std::cout << "String: " << s << std::endl; }

class BackgroundTaskA
{
public:
    void operator()(std::string s) { std::cout << "String: " << s << std::endl; }
};

class BackgroundTaskB
{
public:
    BackgroundTaskB(const std::string &s) : s_(s) {};
    void operator()()
    {
        std::cout << "String: " << s_ << std::endl;
    }

private:
    std::string s_;
};

class ThreadGuard
{
public:
    explicit ThreadGuard(std::thread &thread) : thread_(thread) {}
    ~ThreadGuard()
    {
        if (thread_.joinable())
        {
            thread_.join();
        }
    }

    ThreadGuard(std::thread const &) = delete;
    ThreadGuard &operator=(std::thread const &) = delete;

private:
    std::thread &thread_;
};

void threadGuard()
{
    std::string s("hello");
    BackgroundTaskB background(s);
    std::thread thread(background);
    ThreadGuard guard(thread);
}

void inc(int &a)
{
    a++;
}

class String
{
public:
    String(const std::string &s) : s_(s) {}
    void print()
    {
        std::cout << s_ << std::endl;
    }

private:
    std::string s_;
};

void print(std::unique_ptr<String> s)
{
    s->print();
}

class ScopedThread
{
    std::thread thread_;

public:
    explicit ScopedThread(std::thread thread) : thread_(std::move(thread))
    {
        if (!thread_.joinable())
        {
            throw std::logic_error("Null thread");
        }
    }

    ~ScopedThread()
    {
        thread_.join();
    }

    ScopedThread(ScopedThread const &) = delete;
    ScopedThread &operator=(ScopedThread const &) = delete;
};

template <typename Iterator, typename T>
struct AccumulateBlock
{
    void operator()(Iterator first, Iterator last, T &result)
    {
        result = std::accumulate(first, last, result);
    }
};

template <typename Iterator, typename T>
T parallelAccumulate(Iterator first, Iterator last, T init)
{
    unsigned long const length = std::distance(first, last);
    if (length == 0)
    {
        return init;
    }

    unsigned long const minBlockSize = 25;
    unsigned long const maxThreadNum = (length + minBlockSize - 1) / minBlockSize;
    unsigned long const coreNum = std::thread::hardware_concurrency();
    unsigned long const threadNum = std::min(coreNum != 0 ? coreNum : 2, maxThreadNum);
    unsigned long const blockSize = length / threadNum;

    std::vector<T> results(threadNum);
    std::vector<std::thread> threads(threadNum - 1);

    Iterator blockStart = first;
    for (unsigned long i = 0; i < (threadNum - 1); ++i)
    {
        Iterator blockEnd = blockStart;
        std::advance(blockEnd, blockSize);
        threads[i] = std::thread(AccumulateBlock<Iterator, T>(), blockStart, blockEnd, std::ref(results[i]));
        std::cout << "Thread: " << threads[i].get_id() << std::endl;
        blockStart = blockEnd;
    }
    AccumulateBlock<Iterator, T>()(blockStart, last, results[threadNum - 1]);
    std::cout << "Master thread: " << std::this_thread::get_id() << std::endl;
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
    return std::accumulate(results.begin(), results.end(), init);
}

int main()
{
    std::vector<std::thread> threads;

    threads.push_back(std::thread(threadWorkA, "Function"));
    threads.push_back(std::thread(BackgroundTaskA(), "Class Operator"));
    threads.push_back(std::thread(
        [](std::string s)
        { std::cout << "String: " << s << std::endl; },
        "Lambda"));

    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

    threadGuard();

    int a = 0;

    std::thread threadI(inc, std::ref(a));
    threadI.join();
    std::cout << a << std::endl;

    String s("String");
    std::thread threadS(&String::print, &s);
    threadS.join();

    std::unique_ptr<String> p(new String("Unique"));
    p->print();
    std::thread threadU(print, std::move(p));
    threadU.join();

    std::cout << "Core number: " << std::thread::hardware_concurrency() << "\n";

    std::vector<int> vec;
    for (int i = 0; i < 100; ++i)
    {
        vec.push_back(i);
    }
    int sum = parallelAccumulate(vec.begin(), vec.end(), 0);
    std::cout << "Sum: " << sum << std::endl;
}