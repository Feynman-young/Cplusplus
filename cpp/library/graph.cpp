#include <iostream>
#include <limits>
#include <queue>
#include <stack>
#include <unordered_map>
#include <vector>

const int INF = std::numeric_limits<int>::max();

struct Node;

struct Edge
{
    Node *to_;
    int weight_;
};

struct Node
{
    int id_;
    int val_;
    std::vector<Edge *> edges_;
};

struct Compare
{
    bool operator()(std::pair<int, Node *> lp, std::pair<int, Node *> rp)
    {
        return lp.first > rp.first;
    }
};

std::pair<std::unordered_map<Node *, int>, std::unordered_map<Node *, Node *>> shortestPath(Node *start)
{
    std::unordered_map<Node *, int> distance;
    std::unordered_map<Node *, Node *> previous;

    std::priority_queue<std::pair<int, Node *>, std::vector<std::pair<int, Node *>>, Compare> pq;

    distance[start] = 0;
    pq.push({0, start});

    while (!pq.empty())
    {
        auto [currDst, node] = pq.top();
        pq.pop();

        if (currDst > distance[node])
        {
            continue;
        }

        for (Edge *edge : node->edges_)
        {
            Node *neighbor = edge->to_;
            int weight = edge->weight_;
            int newDst = distance[node] + weight;

            if (!distance.count(neighbor) || newDst < distance[neighbor])
            {
                distance[neighbor] = newDst;
                previous[neighbor] = node;
                pq.push({newDst, neighbor});
            }
        }
    }
    return {distance, previous};
}

int main()
{
    Node n0{0, 0, {}};
    Node n1{1, 0, {}};
    Node n2{2, 0, {}};
    Node n3{3, 0, {}};
    Node n4{4, 0, {}};

    auto e0_1 = new Edge{&n1, 10};
    auto e0_2 = new Edge{&n2, 3};
    auto e1_2 = new Edge{&n2, 1};
    auto e1_3 = new Edge{&n3, 2};
    auto e2_1 = new Edge{&n1, 4};
    auto e2_3 = new Edge{&n3, 8};
    auto e2_4 = new Edge{&n4, 2};
    auto e3_4 = new Edge{&n4, 7};
    auto e4_3 = new Edge{&n3, 9};

    n0.edges_ = {e0_1, e0_2};
    n1.edges_ = {e1_2, e1_3};
    n2.edges_ = {e2_1, e2_3, e2_4};
    n3.edges_ = {e3_4};
    n4.edges_ = {e4_3};

    auto [dist, prev] = shortestPath(&n0);

    for (auto &[node, d] : dist)
    {
        std::cout << "Distance from node 0 to node " << node->id_ << " = " << d << "\n";

        std::stack<int> path;
        Node *cur = node;
        while (cur)
        {
            path.push(cur->id_);
            if (prev.count(cur))
                cur = prev[cur];
            else
                break;
        }

        std::cout << "Path: ";
        while (!path.empty())
        {
            std::cout << path.top();
            path.pop();
            if (!path.empty())
                std::cout << " -> ";
        }
        std::cout << "\n";
    }

    delete e0_1;
    delete e0_2;
    delete e1_2;
    delete e1_3;
    delete e2_1;
    delete e2_3;
    delete e2_4;
    delete e3_4;
    delete e4_3;

    return 0;
}