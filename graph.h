#ifndef GRAPH_H
#define GRAPH_H

#include "queue.h"
#include "stack.h"
#include "doublylinkedlist.h"

namespace std
{

    template <typename T>
    const int INF = 1000000000;

    typedef unsigned int size_t;

    template <typename T>
    class Graph
    {
    private:
        DoublyLinkedList<T> *adjList;
        size_t nodeCount_;
        size_t edgeCount_;
        size_t maxNodes;

    public:
        Graph(size_t maxNodes) : maxNodes(maxNodes), nodeCount_(0), edgeCount_(0)
        {
            adjList = new DoublyLinkedList<T>[maxNodes];
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

        void addEdge(int node1, int node2, bool isDirected = false)
        {
            if (node1 < maxNodes && node2 < maxNodes)
            {
                adjList[node1].insertAtEnd(node2);
                edgeCount_++;

                if (!isDirected)
                {
                    adjList[node2].insertAtEnd(node1);
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

                for (T adj : adjList[node])
                {
                    if (!visited[adj])
                    {
                        stack.push(adj);
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

                for (T adj : adjList[node])
                {
                    if (!visited[adj])
                    {
                        queue.push(adj);
                        visited[adj] = true;
                    }
                }
            }
            cout << endl;
            delete[] visited;
        }

        bool hasEdge(int node1, int node2) const
        {
            for (T adj : adjList[node1])
            {
                if (adj == node2)
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
    };

}

#endif
