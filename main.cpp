#include <iostream>
#include <fstream>
#include <sstream>
#include "stack.h"
#include "doublylinkedlist.h"
#include "avltree.h"
#include "queue.h" 
#include "hashtable.h"
#include "heap.h"
#include "graph.h"



using namespace std;


// Function to load the road network from a CSV file and add edges to the graph
void loadRoadNetwork(DirectedWeightedGraph& graph, const string& filename) {
    ifstream file(filename);
    string line;


    string intersections[26]; // 'A' to 'Z' 
    int numIntersections = 0;  // number of unique intersections

    // First pass: get intersections
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string intersection1, intersection2;
        int travelTime;

        getline(ss, intersection1, ',');  // Intersection1
        getline(ss, intersection2, ',');  // Intersection2
        ss >> travelTime;                 // TravelTime

        // Assign indices to the intersections if not already assigned
        bool found1 = false, found2 = false;
        int u, v;

        // Check and assign index for intersection1
        for (u = 0; u < numIntersections; ++u) {
            if (intersections[u] == intersection1) {
                found1 = true;
                break;
            }
        }
        if (!found1) {
            intersections[numIntersections] = intersection1;
            u = numIntersections;
            numIntersections++;
        }

        // Check and assign index for intersection2
        for (v = 0; v < numIntersections; ++v) {
            if (intersections[v] == intersection2) {
                found2 = true;
                break;
            }
        }
        if (!found2) {
            intersections[numIntersections] = intersection2;
            v = numIntersections;
            numIntersections++;
        }

        // Add the edge to the graph using the indices
        graph.addEdge(u, v, travelTime);

        //  Print road added
        cout << "Adding edge: " << intersection1 << " (" << u << ") -> " << intersection2 << " (" << v << ") with travel time " << travelTime << endl;
    }

    file.close();
}


int main() {

    DirectedWeightedGraph graph(26);
    loadRoadNetwork(graph, "road_network.csv");

    graph.displayGraph();

    // Example: placeholder right now for djkistras 
    graph.dijkstra(0);

    return 0;
}
