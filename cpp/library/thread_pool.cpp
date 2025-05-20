#include <condition_variable>
#include <coroutine>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
    ThreadPool(std::size_t numsOfThreads) {
        for (std::size_t i = 0; i < numsOfThreads; ++i) {
            workers_.emplace_back(
                [this] {
                    while (true) {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(this->mutex_);
                            conditionalVariable_.wait(
                                lock, [this] {
                                    return this->stop_ || !this->tasks_.empty();
                                }
                            );
                            if (this->stop_ && this->tasks_.empty()) {
                                return ;
                            }
                            task = std::move(this->tasks_.front());
                            this->tasks_.pop();
                        }
                        task();
                    }
                }
            );
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stop_ = true;
        }
        conditionalVariable_.notify_all();
        for (std::thread& thread : workers_) {
            thread.join();
        }
    }

    void enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            tasks_.push(task);
        }
        conditionalVariable_.notify_one();
    }

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable conditionalVariable_;
    bool stop_ = false;
};

void counterTask(std::atomic<int>& sum) {
    for (int i = 0; i < 10000; ++i) {
        sum++;
    }
}

void threadPoolCounterTask() {
    ThreadPool pool(4);
    std::atomic<int> sum = 0;
    for (int i = 0; i < 10; ++i) {
        pool.enqueue([&sum] { counterTask(sum); });
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "sum: " << sum << std::endl;
}

int main() {
    threadPoolCounterTask();
    return 0;
}