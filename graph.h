#ifndef GRAPH_H 
#define GRAPH_H
#include <iostream>
#include <climits>

using namespace std;

// Node class to represent an edge in the adjacency list
class GNode {
public:
    int vertex;
    int weight;
    GNode* next;

    GNode(int v, int w) : vertex(v), weight(w), next(nullptr) {}
};

// Directed, Weighted Graph Class
class DirectedWeightedGraph {
private:
    int numVertices;
    GNode** adjacencyList;  // Array of adjacency lists (one for each vertex)

public:
    DirectedWeightedGraph(int n) {
        numVertices = n;
        adjacencyList = new GNode*[numVertices]; // Allocate memory for each vertex's adjacency list

        for (int i = 0; i < numVertices; ++i) {
            adjacencyList[i] = nullptr;  // Initialize adjacency list for each vertex
        }
    }

    ~DirectedWeightedGraph() {
        for (int i = 0; i < numVertices; ++i) {
            GNode* current = adjacencyList[i];
            while (current != nullptr) {
                GNode* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] adjacencyList;
    }

    // Function to add a directed, weighted edge from u to v with weight w
    void addEdge(int u, int v, int weight) {
        GNode* newNode = new GNode(v, weight);
        newNode->next = adjacencyList[u];
        adjacencyList[u] = newNode;
    }

    // Function to display the graph's adjacency list
    void displayGraph() {
        cout << "Adjacency List Representation (Directed, Weighted):\n";
        for (int i = 0; i < numVertices; ++i) {
            cout << "Vertex " << i << ": ";
            GNode* current = adjacencyList[i];
            while (current != nullptr) {
                cout << "(" << current->vertex << ", " << current->weight << ") ";
                current = current->next;
            }
            cout << endl;
        }
    }

    // Dijkstra's algorithm to find shortest paths from a source vertex
    void dijkstra(int source) {
        int* dist = new int[numVertices];  // Array to store the shortest distance from source to each vertex
        bool* visited = new bool[numVertices]; // Array to track visited vertices

        // Initialize distances and visited array
        for (int i = 0; i < numVertices; ++i) {
            dist[i] = INT_MAX;  // Set all distances to infinity
            visited[i] = false; // Mark all vertices as unvisited
        }
        dist[source] = 0;  // Distance to source is 0

        // Dijkstra's algorithm loop
        for (int i = 0; i < numVertices - 1; ++i) {
            // Find the vertex with the smallest distance that has not been visited
            int minDist = INT_MAX;
            int u = -1;
            for (int j = 0; j < numVertices; ++j) {
                if (!visited[j] && dist[j] < minDist) {
                    minDist = dist[j];
                    u = j;
                }
            }

            // If there are no more reachable vertices, break
            if (u == -1) break;

            visited[u] = true;  // Mark the vertex as visited

            // Update the distances of adjacent vertices
            GNode* current = adjacencyList[u];
            while (current != nullptr) {
                int v = current->vertex;
                int weight = current->weight;

                // Relax the edge (if a shorter path is found)
                if (dist[u] != INT_MAX && dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                }

                current = current->next;
            }
        }

        // Output the results
        cout << "Shortest distances from vertex " << source << ":\n";
        for (int i = 0; i < numVertices; ++i) {
            if (dist[i] == INT_MAX)
                cout << "Vertex " << i << ": INF" << endl;
            else
                cout << "Vertex " << i << ": " << dist[i] << endl;
        }

        // Cleanup
        delete[] dist;
        delete[] visited;
    }
};

#endif 