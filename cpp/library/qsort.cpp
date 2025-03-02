#include <functional>
#include <iostream>
#include <string>
#include <vector>

class LessEqual {
public:
    template <typename T>
    bool operator()(const T& lhs, const T& rhs) const {
        return std::less<T>()(lhs, rhs) || std::equal_to<T>()(lhs, rhs);
    }
};

template<typename T>
void quick_sort(std::vector<T>& data, std::size_t start, std::size_t end) {

    if (start >= end) {
        return ;
    }

    LessEqual lessEqual;

    T base = data[start];

    std::size_t left = start;
    std::size_t right = end;

    while (left < right) {
        while (left < right && lessEqual(base, data[right])) {
            right--;
        }
        while (left < right && lessEqual(data[left], base)) {
            left++;
        }
        if (left < right) {
            std::swap(data[left], data[right]);
        }
    }

    std::swap(data[left], data[start]);
    quick_sort(data, start, left == 0 ? 0 : left - 1);
    quick_sort(data, left + 1, end);
}

int main() {
    std::vector<int> ints(128);
    for (int i = 0; i < ints.size(); ++i) {
        ints[i] = rand() % 1024;
    }

    quick_sort(ints, 0, ints.size() - 1);
    for (int i = 0; i < ints.size(); ++i) {
        std::cout << ints[i] << " ";
    }
    std::cout << "\n";

    std::vector<std::string> strs = {"dog", "cat", "horse", "bee", "pig", "hen"};
    quick_sort(strs, 0, strs.size() - 1);
    for (int i = 0; i < strs.size(); ++i) {
        std::cout << strs[i] << " ";
    }
    std::cout << "\n";
}