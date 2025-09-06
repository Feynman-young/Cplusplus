#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

std::vector<std::vector<int>> permutation(std::vector<int> a)
{
    std::sort(a.begin(), a.end());
    std::vector<std::vector<int>> perm;

    do
    {
        perm.push_back(a);
    } while (std::next_permutation(a.begin(), a.end()));
    return perm;
}

std::vector<std::vector<int>> combination(int n, int k)
{
    std::vector<int> a(n);
    std::iota(a.begin(), a.end(), 0);
    std::vector<int> mask(n, 0);
    std::fill(mask.end() - k, mask.end(), 1);
    std::vector<std::vector<int>> combination;

    do
    {
        std::vector<int> p;
        for (int i = 0; i < mask.size(); ++i)
        {
            if (mask[i] == 1)
            {
                p.push_back(a[i]);
            }
        }
        combination.push_back(p);
    } while (std::next_permutation(mask.begin(), mask.end()));
    return combination;
}

int main()
{
    std::vector<int> a = {23, 19, 47, 53, 7};
    std::vector<std::vector<int>> perm = permutation(a);
    for (std::vector<int> &p : perm)
    {
        for (int i : p)
        {
            std::cout << i << ", ";
        }
        std::cout << "\n";
    }

    std::vector<std::vector<int>> comb = combination(5, 2);
    for (std::vector<int> &c : comb)
    {
        for (int i : c)
        {
            std::cout << i << ", ";
        }
        std::cout << "\n";
    }
}