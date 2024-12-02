#ifndef GRAPH_H
#define GRAPH_H

#include "queue.h"
#include "stack.h"
#include "doublylinkedlist.h"

using namespace std;

const int INF = 1000000000;

typedef unsigned int size_t;

template <typename T>
class Graph
{
private:
    DoublyLinkedList<std::pair<T, int>> *adjList;
    size_t nodeCount_;
    size_t edgeCount_;
    size_t maxNodes;

public:
    Graph(size_t maxNodes) : maxNodes(maxNodes), nodeCount_(0), edgeCount_(0)
    {
        adjList = new DoublyLinkedList<std::pair<T, int>>[maxNodes];
    }

    ~Graph()
    {
        delete[] adjList;
    }

    void addNode(const T &value)
    {
        if (nodeCount_ < maxNodes)
        {
            nodeCount_++;
        }
    }

    void addEdge(int node1, int node2, int weight, bool isDirected = false)
    {
        if (node1 < maxNodes && node2 < maxNodes)
        {
            adjList[node1].insertAtEnd({node2, weight});
            edgeCount_++;

            if (!isDirected)
            {
                adjList[node2].insertAtEnd({node1, weight});
                edgeCount_++;
            }
        }
    }

    void depthFirstSearch(int startNode)
    {
        bool *visited = new bool[maxNodes]();
        Stack<T> stack;
        stack.push(startNode);

        while (!stack.empty())
        {
            T node = stack.top();
            stack.pop();

            if (!visited[node])
            {
                cout << node << " ";
                visited[node] = true;
            }

            for (std::pair<T, int> adj : adjList[node])
            {
                if (!visited[adj.first])
                {
                    stack.push(adj.first);
                }
            }
        }
        cout << endl;
        delete[] visited;
    }

    void breadthFirstSearch(int startNode)
    {
        bool *visited = new bool[maxNodes]();
        Queue<T> queue;

        queue.push(startNode);
        visited[startNode] = true;

        while (!queue.empty())
        {
            T node = queue.front();
            queue.pop();
            cout << node << " ";

            for (std::pair<T, int> adj : adjList[node])
            {
                if (!visited[adj.first])
                {
                    queue.push(adj.first);
                    visited[adj.first] = true;
                }
            }
        }
        cout << endl;
        delete[] visited;
    }

    bool hasEdge(int node1, int node2) const
    {
        for (std::pair<T, int> adj : adjList[node1])
        {
            if (adj.first == node2)
            {
                return true;
            }
        }
        return false;
    }

    size_t nodeCount() const
    {
        return nodeCount_;
    }

    size_t edgeCount() const
    {
        return edgeCount_;
    }

    void dijkstra(int startNode)
    {
        int *distances = new int[maxNodes];
        bool *visited = new bool[maxNodes]();
        for (size_t i = 0; i < maxNodes; i++)
            distances[i] = INF;

        distances[startNode] = 0;
        Queue<int> queue;
        queue.push(startNode);

        while (!queue.empty())
        {
            int current = queue.front();
            queue.pop();

            if (visited[current])
                continue;
            visited[current] = true;

            for (std::pair<T, int> adj : adjList[current])
            {
                int neighbor = adj.first;
                int weight = adj.second;

                if (distances[current] + weight < distances[neighbor])
                {
                    distances[neighbor] = distances[current] + weight;
                    queue.push(neighbor);
                }
            }
        }

        for (size_t i = 0; i < maxNodes; i++)
        {
            if (distances[i] == INF)
                cout << "Node " << i << ": INF" << endl;
            else
                cout << "Node " << i << ": " << distances[i] << endl;
        }

        delete[] distances;
        delete[] visited;
    }
};

#endif
