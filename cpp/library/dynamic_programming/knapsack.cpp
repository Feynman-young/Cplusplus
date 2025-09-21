/* Given a set of items, each with a weight and a value, determine which items
 * to include in the collection so that the total weight is less than or equal
 * to a given limit and the total value is as large as possible.
 */

#include <algorithm>
#include <iostream>
#include <vector>

int depthFirstSearch(int i, int cap, int n, std::vector<int> &w, std::vector<int> &v)
{
    if (i == n)
    {
        return 0;
    }

    int res = depthFirstSearch(i + 1, cap, n, w, v);
    if (cap >= w[i])
    {
        res = std::max(res, depthFirstSearch(i + 1, cap - w[i], n, w, v) + v[i]);
    }
    return res;
}

int depthFirstSearchWithMemory(int n, int W, std::vector<int> &w, std::vector<int> &v)
{
    std::vector<std::vector<int>> f(n + 1, std::vector<int>(W + 1, 0));
    for (int i = 1; i < n + 1; ++i)
    {
        for (int j = 0; j < W + 1; ++j)
        {
            f[i][j] = f[i - 1][j];
            if (j >= w[i - 1])
            {
                f[i][j] = std::max(f[i][j], f[i - 1][j - w[i - 1]] + v[i - 1]);
            }
        }
    }
    return f[n][W];
}

/* Space-optimized approach. The core state transition equation is
 * f_{i, j} = max(f_{i - 1, j}, f_(i - 1, j - w_i) + vi)
 * For calculating the current row of the dp[] array we require only previous
 * row, but if we start traversing the rows from right to left then it can be
 * done with a single row only.
 * Iterate the k from right to left to avoid the overlap which means that an
 * item can be put into the backpack multiple times.
 */
int dynamicProgramming01Knapsack(int n, int W, std::vector<int> &w, std::vector<int> &v)
{
    std::vector<int> dp(W + 1, 0);
    for (int i = 1; i <= n; ++i)
    {
        for (int k = W; k >= w[i - 1]; --k)
        {
            if ((dp[k - w[i - 1]] + v[i - 1]) > dp[k])
            {
                dp[k] = (dp[k - w[i - 1]] + v[i - 1]);
            }
        }
    }
    return dp[W];
}

/* Unbounded Knapsack problem
 * The difference is each item can be included an unlimited number of times.
 * Iterate k from left to right.
 */
int dynamicProgrammingUnboundedKnapsack(int n, int W, std::vector<int> &w, std::vector<int> &v)
{
    std::vector<int> dp(W + 1, 0);
    for (int i = 1; i <= n; ++i)
    {
        for (int k = w[i - 1]; k <= W; ++k)
        {
            if ((dp[k - w[i - 1]] + v[i - 1]) > dp[k])
            {
                dp[k] = (dp[k - w[i - 1]] + v[i - 1]);
            }
        }
    }
    return dp[W];
}

int main()
{
    int n = 6;
    int W = 15;
    std::vector<int> w = {2, 2, 6, 5, 4, 3};
    std::vector<int> v = {6, 3, 5, 4, 6, 6};
    std::vector<int> c = {2, 2, 1, 1, 2, 2};
    std::cout << "0-1 Knapsack DFS: " << depthFirstSearch(0, W, n, w, v) << "\n";
    std::cout << "0-1 Knapsack DFS With Memory: " << depthFirstSearchWithMemory(n, W, w, v) << "\n";
    std::cout << "0-1 Knapsack DP: " << dynamicProgramming01Knapsack(n, W, w, v) << "\n";
    std::cout << "Unbouned Knapsack DP: " << dynamicProgrammingUnboundedKnapsack(n, W, w, v) << "\n";
}