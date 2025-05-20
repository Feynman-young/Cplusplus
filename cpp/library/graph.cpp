#include <limits>
#include <queue>
#include <unordered_map>
#include <vector>


const int INF = std::numeric_limits<int>::max();

struct Node;

struct Edge {
    Node *to_;
    int weight_;
};

struct Node {
    int id_;
    int val_;
    std::vector<Edge*> edges_;
};

struct Compare {
    bool operator()(std::pair<int, Node*> lp, std::pair<int, Node*> rp) {
        return lp.first > rp.first;
    }
};

std::pair<std::unordered_map<Node*, int>, std::unordered_map<Node*, Node*>>
shortestPath(Node* start)
{
    std::unordered_map<Node*, int> distance;
    std::unordered_map<Node*, Node*> previous;

    std::priority_queue<
        std::pair<int, Node*>,
        std::vector<std::pair<int, Node*>>,
        Compare
    > pq;

    distance[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [currDst, node] = pq.top();
        pq.pop();

        if (currDst > distance[node]) {
            continue;
        }

        for (Edge* edge : node->edges_) {
            Node* neighbor = edge->to_;
            int weight = edge->weight_;
            int newDst = distance[node] + weight;

            if (!distance.count(neighbor) || newDst < distance[neighbor]) {
                distance[neighbor] = newDst;
                previous[neighbor] = node;
                pq.push({newDst, neighbor});
            }
        }
    }
    return {distance, previous};
}