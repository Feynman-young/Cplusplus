/*
 * Sparse table is a data structure, that allows answering range queries.
 * It can answer range minimum(maximum) queries in O(1) time.
 *
 * The sparse table precomputes answers for intervals whose lengths are powers
 * of two, i.e., the intervals of 2^0, 2^1, 2^2..., up to the largest power of
 * two smaller than or equal to the array lenth.
 *
 * For example, the array A of size N, it precomputes:
 * ST[i][0] = A[i] -- intervals of length 1.
 * ST[i][j] -- the result of the funtion on the interval [i, i + 2^j - 1].
 * Once this table is built, queries can be answered by combining two
 * precomputed intervals covering the query range.
 *
 * Only the idempotent operations ((x opt x) = x) like min, max, gcd, or and
 * queries can efficiently work with sparse table. The main reason is those
 * operations allowing overlapping intervals to be combined without affecting
 * correctness.
 *
 */

#include <cmath>
#include <functional>
#include <iostream>
#include <vector>

template <typename T> class SparseTable
{
  public:
    using F = std::function<T(const T &, const T &)>;
    SparseTable(const std::vector<T> &v, F f)
    {
        f_ = f;
        int length = v.size();
        int L = std::floor(std::log2(length)) + 1;
        ST.assign(length, std::vector<T>(L, 0));
        for (int i = 0; i < length; ++i)
        {
            ST[i][0] = v[i];
        }
        for (int j = 1; j < L; ++j)
        {
            int t = (1 << (j - 1));
            for (int i = 0; (i + t) < length; ++i)
            {
                ST[i][j] = f(ST[i][j - 1], ST[i + t][j - 1]);
            }
        }
    }

    T query(int l, int r)
    {
        int d = r - l + 1;
        int q = std::floor(std::log2(d));
        return f_(ST[l][q], ST[r - (1 << q) + 1][q]);
    }

  private:
    std::vector<std::vector<T>> ST;
    F f_;
};

int max(int a, int b)
{
    return (a > b ? a : b);
}

int main()
{
    std::vector<int> v = {2, 45, 17, 89, 90, 1};
    SparseTable<int> sparseTable(v, max);
    std::cout << sparseTable.query(0, 2) << "\n";
    std::cout << sparseTable.query(1, 3) << "\n";
    std::cout << sparseTable.query(1, 5) << "\n";
}