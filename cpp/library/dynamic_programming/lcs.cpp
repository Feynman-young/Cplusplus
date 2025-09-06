/* Longest common subsequence
 */

#include <iostream>
#include <vector>

template <typename T> size_t lcs(std::vector<T> &a, std::vector<T> &b)
{
    size_t N = a.size();
    size_t M = b.size();
    std::vector<std::vector<size_t>> f(N + 1, std::vector<size_t>(M + 1, 0));
    for (size_t i = 1; i <= N; ++i)
    {
        for (size_t j = 1; j <= M; ++j)
        {
            if (a[i - 1] == b[j - 1])
            {
                f[i][j] = (f[i - 1][j - 1] + 1);
            }
            else
            {
                f[i][j] = std::max(f[i - 1][j], f[i][j - 1]);
            }
        }
    }
    return f[N][M];
}

int main()
{
    std::vector<int> a = {25, 37, 39, 87, 9};
    std::vector<int> b = {89, 57, 39, 27, 9, 5};
    std::cout << lcs(a, b) << std::endl;
}