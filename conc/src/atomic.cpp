#include <atomic>
#include <iostream>
#include <thread>

/*
** There are six memory ordering operations that can be applied on atomic
** types: memory_order_relaxed, memory_order_consume, memory_order_acquire,
** memory_order_release, memory_order_acq_rel, and memory_order_seq_cst.
** The default memory ordering is memory_order_seq_cst.
**/

std::atomic<bool> x;
std::atomic<bool> y;
std::atomic<int> z;

void writeXAndY()
{
    x.store(true, std::memory_order_relaxed);
    y.store(true, std::memory_order_relaxed);
}

void readYThenX()
{
    while (!y.load(std::memory_order_relaxed))
        ;
    if (x.load(std::memory_order_relaxed))
    {
        ++z;
    }
}

void atomicMemoryOrderRelax()
{
    /*
    ** Operations on atomic types performed with relaxed ordering don't participate
    ** in sychronizes-with relationships;
    **/
    x = false;
    y = false;
    z = 0;
    std::thread a(writeXAndY);
    std::thread b(readYThenX);
    a.join();
    b.join();
    std::cout << "z: " << z.load() << "\n";
}

int main()
{
    while (true)
    {
        atomicMemoryOrderRelax();
    }
}