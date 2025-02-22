#include <functional>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <vector>

template<typename T, typename Compare = std::less<T>>
class Heap {
public:
    void push(const T& value) {
        heap_.push_back(value);
        modifyUp(heap_.size() - 1);
    }

    void pop() {
        if (heap_.empty()) {
            throw std::runtime_error("Heap is empty");
        }
        T extreme = heap_[0];
        heap_[0] = heap_.back();
        heap_.pop_back();
        modifyDown(0);
    }

    T top() {
        if (heap_.empty()) {
            throw std::runtime_error("Heap is empty");
        }
        return heap_[0];
    }

    bool empty() {
        return heap_.empty();
    }

private:
    std::vector<T> heap_;
    Compare compare_;

    void modifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (compare_(heap_[index], heap_[parent])) {
                break;
            }
            std::swap(heap_[index], heap_[parent]);
            index = parent;
        }
    }

    void modifyDown(int index) {
        int size = heap_.size();
        while (index < size) {
            int lchild = 2 * index + 1;
            int rchild = 2 * index + 2;
            int extreme = index;
            if (lchild < size && !compare_(heap_[lchild], heap_[extreme])) {
                extreme = lchild;
            }
            if (rchild < size && !compare_(heap_[rchild], heap_[extreme])) {
                extreme = rchild;
            }
            if (extreme == index) {
                break;
            }
            std::swap(heap_[index], heap_[extreme]);
            index = extreme;
        }
    }
};

int main()
{
    std::vector<int> testData = {10, 8, 6, 64, 89, 10, 382};

    Heap<int, std::less<int>> maxHeap;
    std::priority_queue<int, std::vector<int>, std::less<int>> maxQueu;

    for (int data: testData) {
        maxHeap.push(data);
        maxQueu.push(data);
    }

    std::cout << "Max Heap:";
    while (!maxHeap.empty()) {
        int top = maxHeap.top();
        maxHeap.pop();
        std::cout << " " << top;
    }

    std::cout << "\nMax Queu:";
    while (!maxQueu.empty()) {
        int top = maxQueu.top();
        maxQueu.pop();
        std::cout << " " << top;
    }

    Heap<int, std::greater<int>> minHeap;
    std::priority_queue<int, std::vector<int>, std::greater<int>> minQueu;

    for (int data: testData) {
        minHeap.push(data);
        minQueu.push(data);
    }

    std::cout << "\nMin Heap:";
    while (!minHeap.empty()) {
        int top = minHeap.top();
        minHeap.pop();
        std::cout << " " << top;
    }

    std::cout << "\nMin Queu:";
    while (!minQueu.empty()) {
        int top = minQueu.top();
        minQueu.pop();
        std::cout << " " << top;
    }
    std::cout << "\n";
}