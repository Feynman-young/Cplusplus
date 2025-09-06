#include <cstddef>
#include <functional>
#include <iostream>
#include <vector>

template <typename T> class SegmentTree
{
  public:
    SegmentTree(const std::vector<T> &data, std::function<T(T, T)> f, T identity)
        : n_(data.size()), f_(f), identity_(identity)
    {
        tree_.assign(4 * n_, identity_);
        if (n_ > 0)
        {
            build(data, 1, 0, n_ - 1);
        }
    }

    void update(size_t pos, T val)
    {
        update(1, 0, n_ - 1, pos, val);
    }

    T query(size_t left, size_t right)
    {
        return query(1, 0, n_ - 1, left, right);
    }

  private:
    std::vector<T> tree_;
    std::function<T(T, T)> f_;
    T identity_;
    size_t n_;

    void build(const std::vector<T> &data, size_t node, size_t start, size_t end)
    {
        if (start == end)
        {
            tree_[node] = data[start];
            return;
        }
        size_t mid = (start + end) / 2;
        build(data, node * 2, start, mid);
        build(data, node * 2 + 1, mid + 1, end);
        tree_[node] = f_(tree_[2 * node], tree_[2 * node + 1]);
    }

    void update(size_t node, size_t start, size_t end, size_t pos, size_t val)
    {
        if (start == end)
        {
            tree_[node] = val;
            return;
        }
        size_t mid = (start + end) / 2;
        if (pos <= mid)
        {
            update(node * 2, start, mid, pos, val);
        }
        else
        {
            update(node * 2 + 1, mid + 1, end, pos, val);
        }
        tree_[node] = f_(tree_[node * 2], tree_[node * 2 + 1]);
    }

    T query(size_t node, size_t start, size_t end, size_t left, size_t right)
    {
        if (right < start || left > end)
        {
            return identity_;
        }

        if (left <= start && right >= end)
        {
            return tree_[node];
        }

        size_t mid = (start + end) / 2;
        T lv = query(node * 2, start, mid, left, right);
        T rv = query(node * 2 + 1, mid + 1, end, left, right);
        return f_(lv, rv);
    }
};

int main()
{
    std::vector<int> a = {1, 3, 4, 8, 9};
    SegmentTree<int> addTree(a, [](int a, int b) { return a + b; }, 0);
    std::cout << addTree.query(1, 3) << "\n";
    addTree.update(2, 10);
    std::cout << addTree.query(1, 3) << "\n";

    SegmentTree<int> mulTree(a, [](int a, int b) { return a * b; }, 1);
    std::cout << mulTree.query(1, 3) << "\n";
    mulTree.update(2, 10);
    std::cout << mulTree.query(1, 3) << "\n";
}