#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <thread>
#include "graph.h"
#include "queue.h"
#include "hashtable.h"
#include "heap.h"
#include "doublylinkedlist.h"

using namespace std;

const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";

struct Vehicle {
    string id;
    char start;
    char end;
    LinkedList<char> path;
    LinkedList<int> timings;
    int currentPosition;
    int timeInCurrentSegment;
    bool inTransit;
};

class VehicleRoutingSystem {
private:
    DirectedWeightedGraph* graph;
    HashTable<string, Vehicle> vehicles;
    HashTable<char, int> congestionLevels;
    LinkedList<string> vehicleIds;

    int getIndex(char id) { return id - 'A'; }
    char getId(int index) { return static_cast<char>('A' + index); }

public:
    VehicleRoutingSystem(DirectedWeightedGraph* g) : graph(g) {}

    void addVehicle(const string& id, char start, char end) {
        Vehicle v{id, start, end, LinkedList<char>(), LinkedList<int>(), 0, 0, true};
        calculateRoute(v);
        vehicles.insert(id, v);
        vehicleIds.insertAtEnd(id);
    }

    void calculateRoute(Vehicle& vehicle) {
        int path[100];
        int pathLength = graph->dijkstra(
            getIndex(vehicle.start),
            getIndex(vehicle.end),
            path
        );

        vehicle.path.clear();
        vehicle.timings.clear();

        for(int i = 0; i < pathLength; i++) {
            vehicle.path.insertAtEnd(getId(path[i]));
            if(i < pathLength - 1) {
                GNode* edges = graph->getEdges(path[i]);
                while(edges != nullptr) {
                    if(edges->vertex == path[i+1]) {
                        vehicle.timings.insertAtEnd(edges->weight);
                        break;
                    }
                    edges = edges->next;
                }
            }
        }
    }

    void updateAllVehicles() {
        Node<string>* current = vehicleIds.head;
        while(current != nullptr) {
            updateVehiclePosition(current->data);
            current = current->next;
        }
    }

    void updateVehiclePosition(const string& id) {
        Vehicle v;
        if(vehicles.get(id, v)) {
            if(!v.inTransit) return;

            v.timeInCurrentSegment++;
            Node<int>* timingNode = v.timings.head;
            for(int i = 0; i < v.currentPosition; i++) {
                if(timingNode) timingNode = timingNode->next;
            }

            if(timingNode && v.timeInCurrentSegment >= timingNode->data) {
                v.timeInCurrentSegment = 0;
                v.currentPosition++;
                if(v.currentPosition >= v.path.countNodes() - 1) {
                    v.inTransit = false;
                }
            }
            vehicles.insert(id, v);
        }
    }

    void displayVehicles() {
        Node<string>* current = vehicleIds.head;
        while(current != nullptr) {
            displayVehicleStatus(current->data);
            current = current->next;
        }
    }

    void displayVehicleStatus(const string& id) {
        Vehicle v;
        if(vehicles.get(id, v)) {
            cout << "\nVehicle " << id << ":\n";
            
            if(!v.inTransit && v.currentPosition >= v.path.countNodes() - 1) {
                cout << "Status: ARRIVED at destination " << v.end << "\n";
                return;
            }

            Node<char>* pathNode = v.path.head;
            Node<int>* timingNode = v.timings.head;
            
            for(int i = 0; i <= v.currentPosition && pathNode; i++) {
                if(i < v.currentPosition) {
                    cout << pathNode->data << " -> ";
                    pathNode = pathNode->next;
                    if(timingNode) timingNode = timingNode->next;
                }
            }
            
            if(pathNode && pathNode->next) {
                cout << RED << pathNode->data << " -> " << pathNode->next->data << RESET;
                if(timingNode) {
                    cout << " (" << v.timeInCurrentSegment << "/" << timingNode->data << " seconds)";
                }
            }
        }
    }
};

class CityTrafficSystem {
private:
    DirectedWeightedGraph* graph;
    int numIntersections;
    VehicleRoutingSystem* router;

    void calculateNumIntersections(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Failed to open file: " << filename << endl;
            return;
        }

        string line;
        char maxIntersection = 'A';
        
        while (getline(file, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string from, to;
            if (getline(ss, from, ',') && getline(ss, to, ',')) {
                maxIntersection = max(maxIntersection, from[0]);
                maxIntersection = max(maxIntersection, to[0]);
            }
        }
        
        numIntersections = getIndex(maxIntersection) + 1;
        file.close();
    }
    
    void loadRoadNetwork(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Failed to open file: " << filename << endl;
            return;
        }
        
        string line;
        int lineNum = 0;
        
        while (getline(file, line)) {
            lineNum++;
            if (line.empty()) continue;
            
            stringstream ss(line);
            string from, to, weight;
            
            if (getline(ss, from, ',') && getline(ss, to, ',') && getline(ss, weight, ',')) {
                try {
                    int w = stoi(weight);
                    graph->addEdge(getIndex(from[0]), getIndex(to[0]), w);
                } catch(const exception& e) {
                    cerr << "Error at line " << lineNum << ": " << e.what() << endl;
                    cerr << "Line content: " << line << endl;
                }
            } else {
                cerr << "Invalid format at line " << lineNum << ": " << line << endl;
            }
        }
        file.close();
    }

    void loadVehicles(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Failed to open file: " << filename << endl;
            return;
        }
        
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            
            stringstream ss(line);
            string id, start, end;
            
            if (getline(ss, id, ',') && getline(ss, start, ',') && getline(ss, end, ',')) {
                try {
                    router->addVehicle(id, start[0], end[0]);
                } catch(const exception& e) {
                    cerr << "Error adding vehicle: " << line << endl;
                }
            }
        }
        file.close();
    }

    int getIndex(char id) { return id - 'A'; }
    char getId(int index) { return static_cast<char>('A' + index); }

public:
    CityTrafficSystem() : graph(nullptr), numIntersections(0), router(nullptr) {}
    
    ~CityTrafficSystem() {
        delete graph;
        delete router;
    }

    void initializeFromFile(const string& filename) {
        calculateNumIntersections(filename);
        cout << "Number of intersections: " << numIntersections << endl;
        graph = new DirectedWeightedGraph(numIntersections);
        loadRoadNetwork(filename);
        router = new VehicleRoutingSystem(graph);
    }

    void displayNetwork() {
        if (!graph) {
            cout << "Network not initialized!" << endl;
            return;
        }

        cout << "\nCity Traffic Network Structure:" << endl;
        cout << "================================" << endl;

        for (int i = 0; i < numIntersections; i++) {
            char intersection = getId(i);
            cout << "\nIntersection " << intersection << " connects to:" << endl;

            GNode* current = graph->getEdges(i);
            while (current != nullptr) {
                char toIntersection = getId(current->vertex);
                cout << "  -> " << toIntersection 
                     << " (Travel time: " << current->weight << " seconds)" << endl;
                current = current->next;
            }
        }
    }

    void startSimulation() {
        loadVehicles("vehicles.csv");

        while(true) {
            system("cls");
            displayNetwork();
            cout << "\nVehicle Positions:\n";
            cout << "=================\n";
            router->displayVehicles();
            router->updateAllVehicles();
            this_thread::sleep_for(chrono::seconds(1));
        }
    }
};

int main() {
    CityTrafficSystem system;
    system.initializeFromFile("road_network.csv");
    system.startSimulation();
    return 0;
}