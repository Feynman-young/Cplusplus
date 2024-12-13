#include <iostream>
#include <string>
#include <thread>
#include <vector>

void threadWorkA(std::string s) { std::cout << "String: " << s << std::endl; }

class BackgroundTaskA {
public:
    void operator()(std::string s) { std::cout << "String: " << s << std::endl; }
};

class BackgroundTaskB {
public:
    BackgroundTaskB(const std::string& s) : s_(s) {};
    void operator()() {
        std::cout << "String: " << s_ << std::endl;
    }
private:
    std::string s_;
};

class ThreadGuard {
public:
    explicit ThreadGuard(std::thread& thread) : thread_(thread) {}
    ~ThreadGuard() {
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    ThreadGuard(std::thread const&) = delete;
    ThreadGuard& operator=(std::thread const&) = delete;

private:
    std::thread& thread_;
};

void threadGuard()
{
    std::string s("hello");
    BackgroundTaskB background(s);
    std::thread thread(background);
    ThreadGuard guard(thread);
}

int main() {
    std::vector<std::thread> threads;

    threads.push_back(std::thread(threadWorkA, "Function"));
    threads.push_back(std::thread(BackgroundTaskA(), "Class Operator"));
    threads.push_back(std::thread(
        [](std::string s) { std::cout << "String: " << s << std::endl; },
        "Lambda"));

    for (std::thread& thread : threads) {
        thread.join();
    }
}