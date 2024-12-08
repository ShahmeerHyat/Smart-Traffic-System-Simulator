#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <thread>
#include <conio.h>
#include "graph.h"
#include "queue.h"
#include "hashtable.h"
#include "heap.h"
#include "doublylinkedlist.h"
#include <cstdlib>
#include <atomic>
#include "iomanip"
#include <cfloat>

//23i0119  23i2056 22i0556    TRAFFIC MANAGMENT SYSTEM
using namespace std;

const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";

struct TrafficSignal {
    char intersection;
    bool isGreen;
    int greenDuration;
    time_t lastChange;
};
struct EmergencyVehicle {
    string id;
    char start;
    char end;
    string priority;
    LinkedList<char> path;
    int currentPosition;
    bool inTransit;
};


class RoadClosureManager {
private:
    struct RoadStatus {
        string status;
        time_t blockStartTime;                    // how long to block accidented roads
        RoadStatus() : status("Clear"), blockStartTime(0) {}
        RoadStatus(string s, time_t t) : status(s), blockStartTime(t) {}
        bool isBlocked() { 
            if(status == "Clear") return false;
            if(status == "Blocked") return true;
            if(status == "Under Repair") {
                return difftime(time(nullptr), blockStartTime) < 10;
            }
            return false;
        }
    };
    
    HashTable<string, RoadStatus> closures;
    DirectedWeightedGraph* graph;
    
    string makeRoadKey(char start, char end) {
        return string(1, start) + "-" + string(1, end);
    }

public:
    RoadClosureManager(DirectedWeightedGraph* g) : graph(g) {}
    
    void loadClosures(const string& filename) {                           //Load road closures file
        ifstream file(filename);
        string line;
        while(getline(file, line)) {
            stringstream ss(line);
            string from, to, status;
            if(getline(ss, from, ',') && getline(ss, to, ',') && getline(ss, status, ',')) {
                if(from.empty() || to.empty()) continue;
                RoadStatus rs{status, time(nullptr)};
                closures.insert(makeRoadKey(from[0], to[0]), rs);
            }
        }
    }
      void addClosure(const string& roadKey, const string& status) {
        RoadStatus rs{status, time(nullptr)};
        closures.insert(roadKey, rs);
    }
    

    bool isRoadBlocked(char start, char end) { //Check if road is blocked
        RoadStatus status;
        if(closures.get(makeRoadKey(start, end), status)) {
            return status.isBlocked();
        }
        return false;
    }

     void displayClosures() {
       cout << "\nRoad Closures Status:\n";
       cout << "===================\n";
       
       for(char start = 'A'; start <= 'Z'; start++) {
           for(char end = 'A'; end <= 'Z'; end++) {
               RoadStatus status;
               string key = makeRoadKey(start, end);
               if(closures.get(key, status)) {
                   cout << start << " -> " << end << ": ";
                   if(status.status == "Under Repair") {
                       if(status.isBlocked()) {
                           cout << RED << "Under Repair (" << 
                               10 - (int)difftime(time(nullptr), status.blockStartTime) <<                    //simple display function for road closures
                               "s remaining)" << RESET;
                       } else {
                           cout << GREEN << "REPAIRED" << RESET;
                       }
                   } else if(status.status == "Blocked") {
                       cout << RED << "BLOCKED" << RESET;
                   } else {
                       cout << GREEN << "CLEAR" << RESET;
                   }
                   cout << endl;
               }
           }
       }
   }
};



class CongestionMonitor {
private:
    HashTable<string, int> roadCongestion; // key: "start-end", value: vehicle count
    const int CONGESTION_THRESHOLD = 3;
    
    string makeRoadKey(char start, char end) {
        string key;
        key += start;
        key += "-";
        key += end;
        return key;                                           
    }

public:
    void updateCongestion(const LinkedList<char>& path, int currentPosition) {
        Node<char>* current = path.head;
        for(int i = 0; i < currentPosition && current && current->next; i++) {           //update congesion levels when a vehicle moves
            current = current->next;
        }
        
        if(current && current->next) {
            string roadKey = makeRoadKey(current->data, current->next->data);
            int count = 0;
            roadCongestion.get(roadKey, count);
            roadCongestion.insert(roadKey, count + 1);
        }
    }

    void decreaseCongestion(const LinkedList<char>& path, int position) {           //decrease congesion levels when a vehicle moves                   
        Node<char>* current = path.head;
        for(int i = 0; i < position && current && current->next; i++) {
            current = current->next;
        }
        
        if(current && current->next) {
            string roadKey = makeRoadKey(current->data, current->next->data);
            int count = 0;
            roadCongestion.get(roadKey, count);
            if(count > 0) {
                roadCongestion.insert(roadKey, count - 1);
            }
        }
    }

    bool isRoadCongested(char start, char end) {                             
        string roadKey = makeRoadKey(start, end);
        int count = 0;
        roadCongestion.get(roadKey, count);
        return count >= CONGESTION_THRESHOLD;
    }

    void displayCongestionLevels() {
        cout << "\nRoad Congestion Levels:\n";
        cout << "=====================\n";
        
        for(char start = 'A'; start <= 'Z'; start++) {
            for(char end = 'A'; end <= 'Z'; end++) {
                string roadKey = makeRoadKey(start, end);
                int count = 0;
                if(roadCongestion.get(roadKey, count) && count > 0) {
                    cout << start << " -> " << end << ": ";
                    if(count >= CONGESTION_THRESHOLD) {
                        cout << RED << count << " vehicles (CONGESTED)" << RESET;
                    } else {
                        cout << GREEN << count << " vehicles" << RESET;
                    }
                    cout << endl;
                }
            }
        }
    }

    // BFS to find alternative routes avoiding congested roads
    LinkedList<char> findAlternativeRoute(DirectedWeightedGraph* graph, char start, char end) {
        const int MAX_VERTICES = 26;
        bool visited[MAX_VERTICES] = {false};
        char parent[MAX_VERTICES];
        CustomQueue<char> queue;
        
        queue.enqueue(start);
        visited[start - 'A'] = true;
        
        while(!queue.isEmpty()) {
            char current = queue.dequeue();
            
            if(current == end) {
                LinkedList<char> path;
                char temp = end;
                while(temp != start) {
                    path.insertAtHead(temp);
                    temp = parent[temp - 'A'];
                }
                path.insertAtHead(start);
                return path;
            }
            
            GNode* edges = graph->getEdges(current - 'A');
            while(edges) {
                char nextVertex = edges->vertex + 'A';
                if(!visited[nextVertex - 'A'] && !isRoadCongested(current, nextVertex)) {
                    visited[nextVertex - 'A'] = true;
                    parent[nextVertex - 'A'] = current;
                    queue.enqueue(nextVertex);
                }
                edges = edges->next;
            }
        }
        
        LinkedList<char> emptyPath;
        return emptyPath;
    }
};
struct Vehicle {
    string id;
    char start;
    char end;
    LinkedList<char> path;
    LinkedList<int> timings;
    int currentPosition;
    int timeInCurrentSegment;
    bool inTransit;               //checking if vehicle is moving
};

class TrafficAnalytics {
private:
    struct TravelMetrics {
        double averageTime;
        double minTime;
        double maxTime;
        int totalVehicles;
        int delayedVehicles;
        
        TravelMetrics() : averageTime(0), minTime(DBL_MAX), maxTime(0), 
                         totalVehicles(0), delayedVehicles(0) {}
    };

    HashTable<string, TravelMetrics> routeMetrics;
    HashTable<char, int> intersectionLoadCount;
    HashTable<string, double> peakHourMetrics;
    int totalEmergencyResponses;
    int totalAccidents;
    time_t startTime;
    
    string makeRouteKey(char start, char end) {
        return string(1, start) + "-" + string(1, end);
    }

public:
    TrafficAnalytics() : totalEmergencyResponses(0), totalAccidents(0) {
        startTime = time(nullptr);
    }

    void recordVehicleCompletion(const Vehicle& vehicle, bool delayed) {
        string routeKey = makeRouteKey(vehicle.start, vehicle.end);
        TravelMetrics metrics;
        routeMetrics.get(routeKey, metrics);
        
        double totalTime = 0;
        Node<int>* timing = vehicle.timings.head;
        while(timing) {
            totalTime += timing->data;
            timing = timing->next;
        }

        metrics.minTime = min(metrics.minTime, totalTime);
        metrics.maxTime = max(metrics.maxTime, totalTime);
        metrics.averageTime = (metrics.averageTime * metrics.totalVehicles + totalTime) / 
                            (metrics.totalVehicles + 1);
        metrics.totalVehicles++;
        if(delayed) metrics.delayedVehicles++;
        
        routeMetrics.insert(routeKey, metrics);
        
        // Record peak hour metrics
        time_t currentTime = time(nullptr);
        int currentHour = (currentTime / 3600) % 24;
        string hourKey = routeKey + "-" + to_string(currentHour);
        double currentHourAvg = 0;
        peakHourMetrics.get(hourKey, currentHourAvg);
        peakHourMetrics.insert(hourKey, (currentHourAvg + totalTime) / 2);
    }

    void recordIntersectionLoad(char intersection) {
        int count = 0;
        intersectionLoadCount.get(intersection, count);
        intersectionLoadCount.insert(intersection, count + 1);
    }

    void recordEmergencyResponse() {
        totalEmergencyResponses++;
    }

    void recordAccident() {
        totalAccidents++;
    }

    void displayMetrics() {
        time_t currentTime = time(nullptr);
        double hoursElapsed = difftime(currentTime, startTime) / 3600.0;
        
        cout << "\nTraffic System Performance Metrics\n";
        cout << "================================\n";
        cout << "System Runtime: " << fixed << setprecision(2) << hoursElapsed << " hours\n";
        cout << "Total Accidents: " << totalAccidents << "\n";
        cout << "Emergency Responses: " << totalEmergencyResponses << "\n";
        
        cout << "\nRoute Performance:\n";
        for(char start = 'A'; start <= 'Z'; start++) {
            for(char end = 'A'; end <= 'Z'; end++) {
                string routeKey = makeRouteKey(start, end);
                TravelMetrics metrics;
                if(routeMetrics.get(routeKey, metrics) && metrics.totalVehicles > 0) {
                    double delayRate = (double)metrics.delayedVehicles / metrics.totalVehicles * 100;
                    
                    // Find peak hour for this route
                    double peakTime = 0;
                    int peakHour = -1;
                    for(int hour = 0; hour < 24; hour++) {
                        string hourKey = routeKey + "-" + to_string(hour);
                        double hourAvg = 0;
                        if(peakHourMetrics.get(hourKey, hourAvg) && hourAvg > peakTime) {
                            peakTime = hourAvg;
                            peakHour = hour;
                        }
                    }
                    
                    cout << start << " -> " << end << ":\n"
                         << "  Vehicles: " << metrics.totalVehicles << "\n"
                         << "  Min Time: " << metrics.minTime << "s\n"
                         << "  Max Time: " << metrics.maxTime << "s\n"
                         << "  Avg Time: " << metrics.averageTime << "s\n"
                         << "  Delay Rate: " << delayRate << "%\n";
                    if(peakHour != -1) {
                        cout << "  Peak Hour: " << peakHour << ":00 (Avg: " << peakTime << "s)\n";
                    }
                    cout << "\n";
                }
            }
        }
        
        cout << "\nIntersection Load:\n";
        for(char intersection = 'A'; intersection <= 'Z'; intersection++) {
            int count = 0;
            if(intersectionLoadCount.get(intersection, count) && count > 0) {
                cout << "Intersection " << intersection << ": "
                     << count << " vehicles/hour\n";
            }
        }
    }
};





class SignalManagementSystem {
private:
    HashTable<char, TrafficSignal*> signals;

public:
  bool getSignalStatus(char intersection, TrafficSignal*& signal) {             //to return whether signal is red or green
    return signals.get(intersection, signal);
}
    void addSignal(char intersection, int duration) {                          //dynamically allocating sugnals 
        TrafficSignal* signal = new TrafficSignal{
            intersection,
            false,
            duration,
            time(nullptr)
        };
        signals.insert(intersection, signal);
    }



void processSignals() {
    time_t currentTime = time(nullptr);
    for(char i = 'A'; i <= 'Z'; i++) {
        TrafficSignal* signal;
        if(signals.get(i, signal)) {
            if(difftime(currentTime, signal->lastChange) >= signal->greenDuration) {
                signal->isGreen = !signal->isGreen;
                signal->lastChange = currentTime;
            }
        }
    }
}

void emergencyOverride(char intersection) {                       //  for turning signal to green no matter what the state is for emergency situations
    TrafficSignal* signal;
    if(signals.get(intersection, signal)) {
        signal->isGreen = true;
        signal->lastChange = time(nullptr);
    }
}

void displaySignalStatus() {
   cout << "\nTraffic Signal Status:\n";
   cout << "=====================\n";
   time_t currentTime = time(nullptr);                         //display function (please improve formating later) for signals
   TrafficSignal* signal;
   
   for(char i = 'A'; i <= 'Z'; i++) {
       if(signals.get(i, signal)) {
           int timeLeft = signal->greenDuration - difftime(currentTime, signal->lastChange);
           cout << "Intersection " << i << ": "
                << (signal->isGreen ? GREEN + "GREEN" : RED + "RED") << RESET 
                << " (" << timeLeft << " seconds until change)" << endl;
       }
   }
}
};
class EmergencyVehicleManager {
private:
    HashTable<string, EmergencyVehicle> vehicles;
    LinkedList<string> vehicleIds;
    DirectedWeightedGraph* graph;                                 // class for emergency vehicles
    HashTable<string, time_t> lastMoveTime;
    HashTable<string, int> currentSegmentTime;

    int getIndex(char id) { return id - 'A'; }
    char getId(int index) { return static_cast<char>('A' + index); }

public:
    EmergencyVehicleManager(DirectedWeightedGraph* g) : graph(g) {}

    void addVehicle(const string& id, char start, char end, const string& priority) {
        EmergencyVehicle v{id, start, end, priority, LinkedList<char>(), 0, true};
        calculateRoute(v);
        vehicles.insert(id, v);
        vehicleIds.insertAtEnd(id);                                   // adding emergency vehicles
    }

    void calculateRoute(EmergencyVehicle& vehicle) {
        int path[100];
        int pathLength = graph->dijkstra(getIndex(vehicle.start), getIndex(vehicle.end), path);
        
        for(int i = 0; i < pathLength; i++) {                     //calculating optimal route for emergency vehicles with 
            vehicle.path.insertAtEnd(getId(path[i]));               // congesion leveles and weights
        }
    }

       void updatePositions() {
        Node<string>* current = vehicleIds.head;
        while(current != nullptr) {
            EmergencyVehicle v;
            if(vehicles.get(current->data, v) && v.inTransit) {                         //emergency movement through graph
                time_t currentTime = time(nullptr);
                                    
                // Get current segment weight
                Node<char>* pathNode = v.path.head;
                for(int i = 0; i < v.currentPosition && pathNode && pathNode->next; i++) {
                    pathNode = pathNode->next;
                }
                
                if(pathNode && pathNode->next) {
                    int currentWeight = 0;
                    GNode* edges = graph->getEdges(pathNode->data - 'A');
                    while(edges) {
                        if(edges->vertex == (pathNode->next->data - 'A')) {
                            currentWeight = edges->weight;
                            break;
                        }
                        edges = edges->next;
                    }
                    
                    // Reduce time for emergency vehicles (move faster than regular vehicles)
                    currentWeight = max(1, currentWeight / 2);
                    
                    int elapsedTime;
                    if(!currentSegmentTime.get(v.id, elapsedTime)) {
                        elapsedTime = 0;
                        currentSegmentTime.insert(v.id, 0);
                    }
                    
                    if(difftime(currentTime, lastMoveTime[v.id]) >= 1) {
                        elapsedTime++;
                        currentSegmentTime.insert(v.id, elapsedTime);
                        lastMoveTime.insert(v.id, currentTime);
                        
                        if(elapsedTime >= currentWeight) {
                            v.currentPosition++;
                            currentSegmentTime.insert(v.id, 0);
                            
                            if(v.currentPosition >= v.path.countNodes() - 1) {
                                v.inTransit = false;
                            }
                        }
                        vehicles.insert(current->data, v);
                    }
                }
            }
            current = current->next;
        }
    }


    void displayVehicles() {
        Node<string>* current = vehicleIds.head;                         //display funciton for all vehicles from vehicle file
        while(current != nullptr) {
            EmergencyVehicle v;
            if(vehicles.get(current->data, v)) {
                cout << "\n" << RED << v.id << " (Priority: " << v.priority << "):" << RESET;
                if(!v.inTransit) {
                    cout << " ARRIVED at " << v.end;
                    current = current->next;
                    continue;
                }

                Node<char>* pathNode = v.path.head;
                for(int i = 0; i <= v.currentPosition && pathNode; i++) {
                    if(i < v.currentPosition) {
                        cout << pathNode->data << " -> ";
                        pathNode = pathNode->next;
                    }
                }
                if(pathNode && pathNode->next) {
                    cout << RED << pathNode->data << " -> " << pathNode->next->data << RESET;
                }
            }
            current = current->next;
        }
        cout << endl;
    }

    char getCurrentLocation(const EmergencyVehicle& v) {                          //to return current locaiton of vehicle in the graph
        Node<char>* current = v.path.head;
        for(int i = 0; i < v.currentPosition && current; i++) {
            current = current->next;
        }
        return current ? current->data : v.start;
    }

    void forceSignalOverride(SignalManagementSystem* signals) {
    Node<string>* current = vehicleIds.head;
    while(current != nullptr) {
        EmergencyVehicle v;
        if(vehicles.get(current->data, v) && v.inTransit) {
            char location = getCurrentLocation(v);
            signals->emergencyOverride(location);
            
            // For high priority vehicles, turn signal behind them red
            if(v.priority == "High") {
                Node<char>* pathNode = v.path.head;
                for(int i = 0; i < v.currentPosition - 1 && pathNode; i++) {
                    pathNode = pathNode->next;
                }
                
                if(pathNode && v.currentPosition > 0) {
                    TrafficSignal* signal;
                    if(signals->getSignalStatus(pathNode->data, signal)) {
                        signal->isGreen = false;
                        signal->lastChange = time(nullptr);
                    }
                }
            }
        }
        current = current->next;
    }
}
};
class VehicleRoutingSystem {
private:
    DirectedWeightedGraph* graph;
    SignalManagementSystem* signals;
    HashTable<string, Vehicle> vehicles;
    HashTable<char, int> congestionLevels;
    CongestionMonitor congestionMonitor;               //darastructures for vehicle routing
    RoadClosureManager* closureManager;
    TrafficAnalytics* analytics;

    struct CollisionEvent {
        string vehicle1;
        string vehicle2;
        char location;
        time_t timestamp;
    };
    

    int getIndex(char id) { return id - 'A'; }
    char getId(int index) { return static_cast<char>('A' + index); }

public:

LinkedList<string> vehicleIds;
LinkedList<CollisionEvent> collisions;             // linked lists to track vehicle ids and collision events
   VehicleRoutingSystem(DirectedWeightedGraph* g, SignalManagementSystem* s, TrafficAnalytics* a) 
        : graph(g), signals(s), analytics(a) {}

    HashTable<char, int>& getCongestionLevels() {
        return congestionLevels;
    }

    void addVehicle(const string& id, char start, char end) {
        Vehicle v{id, start, end, LinkedList<char>(), LinkedList<int>(), 0, 0, true};
        calculateRoute(v);
        vehicles.insert(id, v);
        vehicleIds.insertAtEnd(id);
    }

    void calculateRoute(Vehicle& vehicle) {
        int path[100];
        int pathLength = graph->dijkstra(
            getIndex(vehicle.start),                         //djistra algorithm implementation
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
    bool checkCollision(const Vehicle& v1, const Vehicle& v2) {
        if (!v1.inTransit || !v2.inTransit) return false;
                                                                          
        char loc1 = getCurrentLocation(v1);
        char loc2 = getCurrentLocation(v2);
        
        // Check if vehicles are at same intersection
        if (loc1 == loc2) {
            Node<char>* path1 = v1.path.head;
            Node<char>* path2 = v2.path.head;
            
            // Move to current positions
            for (int i = 0; i < v1.currentPosition && path1; i++) path1 = path1->next;
            for (int i = 0; i < v2.currentPosition && path2; i++) path2 = path2->next;
            
            // Check if vehicles are moving to same next intersection
            if (path1 && path1->next && path2 && path2->next) {
                if (path1->next->data == path2->next->data) {
                    CollisionEvent collision{v1.id, v2.id, loc1, time(nullptr)};
                    collisions.insertAtEnd(collision);
                    return true;
                }
            }
        }
        return false;
    }

    void handleCollisions() {
       //                                                                          Check all vehicle pairs for collisions
        Node<string>* current1 = vehicleIds.head;
        while (current1 != nullptr) {
            Node<string>* current2 = current1->next;
            while (current2 != nullptr) {
                Vehicle v1, v2;
                if (vehicles.get(current1->data, v1) && vehicles.get(current2->data, v2)) {
                    if (checkCollision(v1, v2)) {
//                                                                    Stop both vehicles if leaving intersection and joining same new road
                        v1.inTransit = false;
                        v2.inTransit = false;
                        vehicles.insert(v1.id, v1);
                        vehicles.insert(v2.id, v2);
                    }
                }
                current2 = current2->next;
            }
            current1 = current1->next;
        }
    }
    void handleCollision(const Vehicle& v1, const Vehicle& v2, char location) {
        Node<char>* path1 = v1.path.head;
        Node<char>* path2 = v2.path.head;
        analytics->recordAccident();
        
        for(int i = 0; i < v1.currentPosition && path1; i++) path1 = path1->next;
        for(int i = 0; i < v2.currentPosition && path2; i++) path2 = path2->next;          //update road path to blocked for 10 second upon collision
                   
        if(path1 && path1->next && path2 && path2->next) {
            string roadKey1 = string(1, location) + "-" + string(1, path1->next->data);
            string roadKey2 = string(1, location) + "-" + string(1, path2->next->data);
            
            closureManager->addClosure(roadKey1, "Blocked");
            closureManager->addClosure(roadKey2, "Blocked");
            
            Vehicle v1Copy = v1;
            Vehicle v2Copy = v2;
            v1Copy.inTransit = false;
            v2Copy.inTransit = false;
            vehicles.insert(v1.id, v1Copy);
            vehicles.insert(v2.id, v2Copy);
            
            CollisionEvent collision{v1.id, v2.id, location, time(nullptr)};
            collisions.insertAtEnd(collision);
        }
    }

    void displayCollisions() {
        if (collisions.head == nullptr) {                                //fucntion to print collisions along with their time
            cout << "\nNo collisions reported.\n";
            return;
        }
        
        cout << "\nCollision Reports:\n";
        cout << "=================\n";
        Node<CollisionEvent>* current = collisions.head;
        while (current != nullptr) {
            cout << RED << "Collision between " << current->data.vehicle1 
                 << " and " << current->data.vehicle2
                 << " at intersection " << current->data.location 
                 << " (Time: " << ctime(&current->data.timestamp) << ")" << RESET;
            current = current->next;
        }
    }

    void checkAndUpdateRoute(const string& id) {
    Vehicle v;
    if(vehicles.get(id, v) && v.inTransit) {
        // Get current location and check surrounding roads
        char currentLoc = getCurrentLocation(v);
        Node<char>* pathNode = v.path.head;
        
        // Move to current position
        for(int i = 0; i < v.currentPosition && pathNode && pathNode->next; i++) {
            pathNode = pathNode->next;
        }
        
        // Check if next road segment is blocked or congested
        if(pathNode && pathNode->next) {
            if(closureManager->isRoadBlocked(pathNode->data, pathNode->next->data) || 
               congestionMonitor.isRoadCongested(pathNode->data, pathNode->next->data)) {
                
                // Calculate new path from current location
                int newPath[100];
                int newPathLength = graph->dijkstra(
                    currentLoc - 'A',
                    v.end - 'A',
                    newPath
                );
                
                // Update vehicle's path
                v.path.clear();
                v.timings.clear();
                for(int i = 0; i < newPathLength; i++) {
                    v.path.insertAtEnd(getId(newPath[i]));
                }
                v.currentPosition = 0;
                calculateRoute(v);
                vehicles.insert(id, v);
            }
        }
    }
}

    string makeRoadKey(char start, char end) {                               //to beautify display making keys for all neccesary things
    return string(1, start) + "-" + string(1, end);
}

void displayVehiclesPerRoad() {
    HashTable<string, int> roadCount;
    
    // Count vehicles on each road segment
    Node<string>* current = vehicleIds.head;
    while(current != nullptr) {
        Vehicle v;
        if(vehicles.get(current->data, v) && v.inTransit) {
            Node<char>* pathNode = v.path.head;
            // Move to current position in path
            for(int i = 0; i < v.currentPosition && pathNode && pathNode->next; i++) {
                pathNode = pathNode->next;
            }
            
            // If vehicle is on a road segment
            if(pathNode && pathNode->next) {
                string roadKey = makeRoadKey(pathNode->data, pathNode->next->data);
                int count = 0;
                roadCount.get(roadKey, count);
                roadCount.insert(roadKey, count + 1);
            }
        }
        current = current->next;
    }
    
    // Display counts
    cout << "\nCurrent Vehicle Distribution on Roads:\n";
    cout << "===================================\n";
    for(char start = 'A'; start <= 'Z'; start++) {
        for(char end = 'A'; end <= 'Z'; end++) {
            string roadKey = makeRoadKey(start, end);
            int count = 0;
            if(roadCount.get(roadKey, count) && count > 0) {
                cout << start << " -> " << end << ": ";
                if(count >= 3) {
                    cout << RED << count << " vehicles (CONGESTED)" << RESET;
                } else {
                    cout << GREEN << count << " vehicles" << RESET;
                }
                cout << endl;
            }
        }
    }
}
    

    void updateAllVehicles() {
         handleCollisions(); 
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

            // Get current signal status
            TrafficSignal* signal;
            char currentLocation = getCurrentLocation(v);
            
            // Check for congestion and reroute if needed
            Node<char>* pathNode = v.path.head;
            for(int i = 0; i < v.currentPosition && pathNode && pathNode->next; i++) {
                pathNode = pathNode->next;
            }
            
            if(pathNode && pathNode->next) {
                if(congestionMonitor.isRoadCongested(pathNode->data, pathNode->next->data)) {
                    LinkedList<char> newPath = congestionMonitor.findAlternativeRoute(
                        graph, 
                        currentLocation, 
                        v.end
                    );
                    if(newPath.head != nullptr) {
                        congestionMonitor.decreaseCongestion(v.path, v.currentPosition);
                        v.path = newPath;
                        v.currentPosition = 0;
                        calculateRoute(v);
                    }
                }
            }

            if(signals->getSignalStatus(currentLocation, signal)) {
                if(!signal->isGreen) {
                    return;
                }
            }

            v.timeInCurrentSegment++;
            Node<int>* timingNode = v.timings.head;
            for(int i = 0; i < v.currentPosition; i++) {
                if(timingNode) timingNode = timingNode->next;
            }

            if(timingNode && v.timeInCurrentSegment >= timingNode->data) {
                congestionMonitor.decreaseCongestion(v.path, v.currentPosition);
                v.timeInCurrentSegment = 0;
                v.currentPosition++;
                if(v.currentPosition < v.path.countNodes() - 1) {
                    congestionMonitor.updateCongestion(v.path, v.currentPosition);
                }
                if(v.currentPosition >= v.path.countNodes() - 1) {
                    v.inTransit = false;
                     analytics->recordVehicleCompletion(v, v.timeInCurrentSegment > timingNode->data);
                }
                analytics->recordIntersectionLoad(currentLocation);
            }
            vehicles.insert(id, v);
        }
    }

    void updateCongestionLevels(const Vehicle& v) {
        char currentLocation = getCurrentLocation(v);
        int count;
        if(congestionLevels.get(currentLocation, count)) {                //after vehicles have moved reclassify the congesion levels
            congestionLevels.insert(currentLocation, count + 1);
        } else {
            congestionLevels.insert(currentLocation, 1);
        }
    }

    char getCurrentLocation(const Vehicle& v) {
        Node<char>* current = v.path.head;
        for(int i = 0; i < v.currentPosition && current; i++) {                              //current locatio of vehicle in the graph
            current = current->next;
        }
        return current ? current->data : v.start;
    }

    void displayVehicles() {
        Node<string>* current = vehicleIds.head;
        while(current != nullptr) {                                            //funciton to display behicles in the main 
            displayVehicleStatus(current->data);
            current = current->next;
        }
        cout << "\n";
        congestionMonitor.displayCongestionLevels();
    }


    void displayVehicleStatus(const string& id) {
        Vehicle v;                                                         //check if vehicle has arrived at destination or is curreently moving
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
public:
    DirectedWeightedGraph* graph;
    int numIntersections;                        //datastructures of city traphic system
    VehicleRoutingSystem* router;
    SignalManagementSystem* signalManager;
    EmergencyVehicleManager* emergencyManager;
    RoadClosureManager* closureManager;
    TrafficAnalytics* analytics;

    int getIndex(char id) { return id - 'A'; }
    char getId(int index) { return static_cast<char>('A' + index); }

    void calculateNumIntersections(const string& filename) {
        ifstream file(filename);                                          //calculating number of intersections in the file
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
            cerr << "Failed to open file: " << filename << endl;                   //input graph from road network csv
            return;
        }
        
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            
            stringstream ss(line);
            string from, to, weight;
            
            if (getline(ss, from, ',') && getline(ss, to, ',') && getline(ss, weight, ',')) {
                try {
                    int w = stoi(weight);
                    graph->addEdge(getIndex(from[0]), getIndex(to[0]), w);
                } catch(const exception& e) {
                    cerr << "Error parsing line: " << line << endl;
                }
            }
        }
        file.close();
    }

    void loadVehicles(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {                                               //input vehicles from vehicles csv
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

    void loadEmergencyVehicles(const string& filename) {                                 //input emergency vehicles from emergency csv
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return;
    }
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        string id, start, end, priority;
        
        if (getline(ss, id, ',') && getline(ss, start, ',') && 
            getline(ss, end, ',') && getline(ss, priority, ',')) {
            emergencyManager->addVehicle(id, start[0], end[0], priority);
        }
    }
    file.close();
}


    void loadTrafficSignals(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Failed to open file: " << filename << endl;                        //input traffic signals from traffic csv
            return;
        }
        
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            
            stringstream ss(line);
            string intersection, greenTime;
            
            if (getline(ss, intersection, ',') && getline(ss, greenTime, ',')) {
                try {
                    signalManager->addSignal(intersection[0], stoi(greenTime));
                } catch(const exception& e) {
                    cerr << "Error adding signal: " << line << endl;
                }
            }
        }
        file.close();
    }

public:                      //consturctor for city class
    CityTrafficSystem() : graph(nullptr), numIntersections(0), router(nullptr), signalManager(nullptr), emergencyManager(nullptr), analytics(nullptr) {
        analytics = new TrafficAnalytics();
    }
    
    ~CityTrafficSystem() {
        delete graph;
        delete router;                                                 //destory objects after they have been used
        delete signalManager;
        delete emergencyManager;
        delete analytics;
    }

    
       void initializeFromFile(const string& filename) {
    calculateNumIntersections(filename);
    cout << "Number of intersections: " << numIntersections << endl;
    graph = new DirectedWeightedGraph(numIntersections);
    loadRoadNetwork(filename);
    signalManager = new SignalManagementSystem();
    loadTrafficSignals("traffic_signals.csv");
   router = new VehicleRoutingSystem(graph, signalManager, analytics);
    emergencyManager = new EmergencyVehicleManager(graph); // initializeing all files by calling there functions
    loadVehicles("vehicles.csv");
    loadEmergencyVehicles("emergency_vehicles.csv");
    closureManager = new RoadClosureManager(graph);
    closureManager->loadClosures("road_closures.csv");
    
}

    void displayNetwork() {
        if (!graph) {
            cout << "Network not initialized!" << endl;
            return;
        }                                                                //display for graph network

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
    while(true) {
        system("cls");
        displayNetwork();
        signalManager->processSignals();
        emergencyManager->forceSignalOverride(signalManager);
        signalManager->displaySignalStatus();
        closureManager->displayClosures();                                //starting our simulation from clocks and timings 

        // Check and update routes for all vehicles
        Node<string>* current = router->vehicleIds.head;
        while(current != nullptr) {
            router->checkAndUpdateRoute(current->data);
            current = current->next;
        }

 
        cout << "\nEmergency Vehicles:\n";
        cout << "==================\n";
        emergencyManager->displayVehicles();
        
        cout << "\nRegular Vehicles:\n";
        cout << "================\n";
        router->displayVehicles();
        
        router->updateAllVehicles();
        emergencyManager->updatePositions();
        this_thread::sleep_for(chrono::seconds(1));
    }
}
};



void clearScreen() {
    #ifdef _WIN32
        std::system("cls");
    #else
        std::system("clear");
    #endif
}

void displayMenu() {
    cout << "\nSmart Traffic Management System\n";
    cout << "==============================\n";
    cout << "1. Display Graph Connectivity\n";
    cout << "2. Traffic Signal Status\n";
    cout << "3. Display Congestion Status\n";
    cout << "4. Display Road Blockage Status\n";
    cout << "5. Emergency Vehicle Routing\n";
    cout << "6. Vehicle Routing Status\n";
    cout << "7. Collision Reports\n";      
    cout << "8. Display performance metrics\n";      
    cout << RED <<"9. Exit\n\n";              
    cout <<  YELLOW << "Enter your choice: ";

}

int main() {
    CityTrafficSystem system;
    system.initializeFromFile("road_network.csv");
    
    std::atomic<bool> running{true};
    
    std::thread updateThread([&]() {
        while(running) {
            system.signalManager->processSignals();
            system.emergencyManager->forceSignalOverride(system.signalManager);
            system.router->updateAllVehicles();
            system.emergencyManager->updatePositions();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
    
    while(running) {
        clearScreen();
        displayMenu();
        char choice = _getch();
        
        if(choice == '9') {
            running = false;
            break;
        }
        
        clearScreen();
        bool viewingStatus = true;
        time_t lastUpdate = time(nullptr);
        
        while(viewingStatus && running) {
            if(difftime(time(nullptr), lastUpdate) >= 1) {
                clearScreen();
                switch(choice) {
                    case '1':
                        cout << "Graph Connectivity (Press ESC to return)\n";
                        cout << "=======================================\n";
                        system.displayNetwork();
                        break;
                        
                    case '2':
                        cout << "Traffic Signal Status (Press 'O' to override, ESC to return)\n";
                        cout << "====================================================\n";
                        system.signalManager->displaySignalStatus();
                        break;
                        
                    case '3':
                        cout << "Vehicles per road (Press ESC to return)\n";
                        cout << "====================================\n";
                        system.router->displayVehiclesPerRoad();
                        break;
                        
                    case '4':
                        cout << "Road Blockage Status (Press ESC to return)\n";
                        cout << "======================================\n";
                        system.closureManager->displayClosures();
                        break;
                        
                    case '5':
                        cout << "Emergency Vehicle Status (Press ESC to return)\n";
                        cout << "========================================\n";
                        system.emergencyManager->displayVehicles();
                        break;
                        
                    case '6':
                        cout << "Vehicle Routing Status (Press ESC to return)\n";
                        cout << "======================================\n";
                        system.router->displayVehicles();
                        break;
                    case '7':
                        cout << "Collision Reports (Press ESC to return)\n";
                        cout << "================================\n";
                        system.router->displayCollisions();
                     break;
                     case '8':
                         cout << "Performance Metrics (Press ESC to return)\n";
                         cout << "===================================\n";
                         system.analytics->displayMetrics();
                     break;
                }
                lastUpdate = time(nullptr);
            }
            
            if(_kbhit()) {
                char key = _getch();
                if(key == 27) { // ESC
                    viewingStatus = false;
                }
                else if(choice == '2' && (key == 'o' || key == 'O')) {
                    cout << "\nEnter intersection to override (A-Z): ";
                    char intersection = _getch();
                    cout << intersection << endl;
                    if(intersection >= 'A' && intersection <= 'Z') {
                        system.signalManager->emergencyOverride(intersection);
                        cout << "Signal overridden at intersection " << intersection << endl;
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                    }
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    
    updateThread.join();
    cout << "\nExiting system...\n";
    return 0;
}
