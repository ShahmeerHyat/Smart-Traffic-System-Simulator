#ifndef GRAPH_H
#define GRAPH_H

#include "queue.h"
#include "stack.h"
#include "linkedList.h"

namespace std
{

    template <typename T>
    const int INF = 1000000000;

    typedef unsigned int size_t;

    template <typename T>
    class Graph
    {
    private:
        LinkedList<Pair<int, int>> *adjList;
        size_t nodeCount_;
        size_t edgeCount_;
        size_t maxNodes;

    public:
        Graph(size_t maxNodes) : maxNodes(maxNodes), nodeCount_(0), edgeCount_(0)
        {
            adjList = new LinkedList<Pair<int, int>>[maxNodes];
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
                adjList[node1].insertAtEnd(Pair<int, int>(node2, weight));
                edgeCount_++;

                if (!isDirected)
                {
                    adjList[node2].insertAtEnd(Pair<int, int>(node1, weight));
                    edgeCount_++;
                }
            }
        }

        void depthFirstSearch(int startNode)
        {
            bool *visited = new bool[maxNodes]();
            Stack<int> stack;
            stack.push(startNode);

            while (!stack.empty())
            {
                int node = stack.top();
                stack.pop();

                if (!visited[node])
                {
                    cout << node << " ";
                    visited[node] = true;
                }

                for (Pair<int, int> adj : adjList[node])
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
            Queue<int> queue;

            queue.push(startNode);
            visited[startNode] = true;

            while (!queue.empty())
            {
                int node = queue.front();
                queue.pop();
                cout << node << " ";

                for (Pair<int, int> adj : adjList[node])
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

                for (Pair<int, int> adj : adjList[current])
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

}

#endif
