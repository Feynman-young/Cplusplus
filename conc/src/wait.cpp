#include <chrono>
#include <condition_variable>
#include <future>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

bool globalFlag;

std::condition_variable globalCondition;

std::mutex globalMutexA;
std::mutex globalMutexB;

class Data
{
  public:
    Data() : s_(" ")
    {
    }

    Data(const std::string &s) : s_(s)
    {
    }

    void print()
    {
        std::cout << s_ << std::endl;
    }

  private:
    std::string s_;
};

void waitForFlag()
{
    std::unique_lock<std::mutex> lock(globalMutexA);
    while (!globalFlag)
    {
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        lock.unlock();
    }
}

std::queue<Data> globalDataQueue;

Data randomData()
{
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return Data(oss.str());
}

void pushToQueue()
{
    for (int i = 0; i < 10; ++i)
    {
        Data const data = randomData();
        std::lock_guard<std::mutex> guarad(globalMutexB);
        globalDataQueue.push(data);
        globalCondition.notify_one();
    }
}

void popFromQueue()
{
    for (int i = 0; i < 10; ++i)
    {
        std::unique_lock<std::mutex> lock(globalMutexB);
        globalCondition.wait(lock, []() { return !globalDataQueue.empty(); });
        Data data = globalDataQueue.front();
        globalDataQueue.pop();
        lock.unlock();
        data.print();
    }
}

template <typename T> class ThreadSafeQueue
{
  public:
    ThreadSafeQueue()
    {
    }

    ThreadSafeQueue(ThreadSafeQueue const &other)
    {
        std::lock_guard<std::mutex> guard(other.mutex_);
        queue_ = other.queue_;
    }

    void push(T value)
    {
        std::lock_guard<std::mutex> guard(mutex_);
        queue_.push(value);
        condition_.notify_one();
    }

    void waitPop(T &value)
    {
        std::unique_ptr<std::mutex> lock(mutex_);
        condition_.wait(lock, [this]() { return !queue_.empty(); });
        value = queue_.front();
        queue_.pop();
    }

    std::shared_ptr<T> waitPop()
    {
        std::unique_ptr<std::mutex> lock(mutex_);
        condition_.wait(lock, [this]() { return !queue_.empty(); });
        std::shared_ptr<T> p(std::make_shared<T>(queue_.front()));
        queue_.pop();
        return p;
    }

    bool tryPop(T &value)
    {
        std::lock_guard<std::mutex> guard(mutex_);
        if (queue_.empty())
        {
            return false;
        }
        value = queue_.front();
        queue_.pop();
        return true;
    }

    std::shared_ptr<T> tryPop()
    {
        std::lock_guard<std::mutex> guard(mutex_);
        if (queue_.empty())
        {
            return std::make_shared<T>();
        }
        std::shared_ptr<T> p(std::make_shared<T>(queue_.front()));
        queue_.pop();
        return p;
    }

    bool const empty()
    {
        std::lock_guard<std::mutex> guard(mutex_);
        return queue_.empty();
    }

  private:
    std::condition_variable condition_;
    mutable std::mutex mutex_;
    std::queue<T> queue_;
};

int backgroundWaitTask()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return 10;
}

struct X
{
    void f(const std::string &s)
    {
        std::cout << "F: " << s << std::endl;
    }

    void g(const std::string &s)
    {
        std::cout << "G: " << s << std::endl;
    }
};

struct Y
{
    void operator()(double a)
    {
        std::cout << "Half: " << a / 2 << std::endl;
    }
};

std::string char2string(std::vector<char> *vec, int size)
{
    if (vec == nullptr || size <= 0)
    {
        return std::string();
    }

    if (vec->size() < size)
    {
        return std::string(vec->data(), vec->size());
    }

    return std::string(vec->data(), size);
}

int main()
{
    std::thread threadA(popFromQueue);
    std::thread threadB(pushToQueue);

    threadA.join();
    threadB.join();

    std::future<int> answer = std::async(backgroundWaitTask);
    std::cout << "Do other stuff" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::cout << "Answer: " << answer.get() << std::endl;

    X x;

    auto futureA = std::async(&X::f, &x, "hello");
    auto futureB = std::async(&X::g, &x, "hello");

    Y y;

    auto futureC = std::async(std::launch::async, Y(), 3.14);
    auto futureD = std::async(std::launch::deferred, std::ref(y), 2.718);

    futureC.wait();
    futureD.wait();

    std::vector<char> s = {'A', 'B', 'C', 'D', 'E'};

    std::packaged_task<std::string(std::vector<char> *, int)> task(char2string);
    std::future<std::string> futureE = task.get_future();
    std::thread threadC(std::move(task), &s, 5);
    std::string resultM = futureE.get();
    std::cout << "char2string: " << resultM << std::endl;
    threadC.join();
}