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
#define MAX_PATH_LENGTH 26

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


class Vehicle {
public:
    string vehicleID;      // Vehicle ID (e.g., "V1", "V2")
    int currentNode;       // Current intersection/node (represented as an integer)
    int destinationNode;   // Destination intersection/node (represented as an integer)
    bool isMoving;         // Flag to check if the vehicle is still moving

   
    Vehicle(string id, int start, int end)
        : vehicleID(id), currentNode(start), destinationNode(end), isMoving(true) {}

    
    void move(DirectedWeightedGraph& graph) {
        int path[MAX_PATH_LENGTH];  // Array to store the path
        int pathLength = graph.dijkstra(currentNode, destinationNode, path);  // Get the shortest path

        if (pathLength > 0 && isMoving) {
            // Move the vehicle along the path (just move one step for simplicity)
            currentNode = path[1];  // Move to the next intersection in the path

            // Print the vehicle's movement
            cout << "Vehicle " << vehicleID << " moved to intersection " << (char)(currentNode + 'A') << endl;

            // Check if the vehicle has reached its destination
            if (currentNode == destinationNode) {
                isMoving = false;
                cout << "Vehicle " << vehicleID << " has reached its destination!" << endl;
            }
        } else {
            isMoving = false; // If no valid path, stop the vehicle
            cout << "Vehicle " << vehicleID << " cannot move. No valid path!" << endl;
        }
    }

    // Get the current position of the vehicle as a character (A-Z)
    char getCurrentPosition() const {
        return (char)(currentNode + 'A');
    }

    // Check if the vehicle has reached its destination
    bool hasReachedDestination() const {
        return currentNode == destinationNode;
    }

    // Display vehicle details
    void displayVehicleInfo() const {
        cout << "Vehicle " << vehicleID << " is currently at intersection " << getCurrentPosition()
             << " and heading to intersection " << (char)(destinationNode + 'A') << endl;
    }
};

void loadVehicles(DirectedWeightedGraph& graph, const string& filename, CustomQueue<Vehicle>& vehicleQueue) {
    ifstream file(filename);
    string line;

    
    if (!file.is_open()) {
        cout << "Error opening file: " << filename << endl;
        return;
    }

    
    while (getline(file, line)) {
        if (line.empty()) continue;  

        stringstream ss(line);  
        string vehicleID, startIntersection, endIntersection;

    
        getline(ss, vehicleID, ',');  
        getline(ss, startIntersection, ',');  
        getline(ss, endIntersection, ',');  

       
        int start = startIntersection[0] - 'A';
        int end = endIntersection[0] - 'A';

        // Create a new Vehicle object and enqueue it into the custom queue
        Vehicle newVehicle(vehicleID, start, end);
        vehicleQueue.enqueue(newVehicle);  

      
        cout << "Loaded vehicle " << vehicleID << " from " << startIntersection
             << " to " << endIntersection << endl;
    }

    file.close();  
}


int main() {
    DirectedWeightedGraph graph(26);  // 26 intersections (A-Z)
    loadRoadNetwork(graph, "road_network.csv");

    graph.displayGraph();

  
    CustomQueue<Vehicle> vehicleQueue;  // Custom Queue for vehicles
    loadVehicles(graph, "vehicles.csv", vehicleQueue);

    // Move vehicles one step at a time
    while (!vehicleQueue.isEmpty()) {
        Vehicle vehicle = vehicleQueue.dequeue(); 
        vehicle.move(graph);  // Move the vehicle to the next intersection based on Dijkstra
    }

    return 0;
}
