#include <chrono>
#include <condition_variable>
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
    Data() : s_(" ") {}

    Data(const std::string &s) : s_(s) {}

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
    while (true)
    {
        std::unique_lock<std::mutex> lock(globalMutexB);
        globalCondition.wait(lock, [] () { return !globalDataQueue.empty(); });
        Data data = globalDataQueue.front();
        globalDataQueue.pop();
        lock.unlock();
        data.print();
    }
}

template<typename T>
class ThreadSafeQueue
{
public:
    ThreadSafeQueue() {}

    ThreadSafeQueue(ThreadSafeQueue const& other)
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

    void waitPop(T& value)
    {
        std::unique_ptr<std::mutex> lock(mutex_);
        condition_.wait(lock, []() { return !queue_.empty(); })
        value = queue_.front();
        queue_.pop();
    }

    std::shared_ptr<T> waitPop()
    {
        std::unique_ptr<std::mutex> lock(mutex_);
        condition_.wait(lock, []() { return !queue_.empty(); })
        std::shared_ptr<T> p(std::make_shared<T>(queue_.front()));
        queue_.pop();
        return p;
    }

    bool tryPop(T& value)
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

int main()
{
    std::thread threadA(popFromQueue);
    std::thread threadB(pushToQueue);

    threadA.join();
    threadB.join();
}