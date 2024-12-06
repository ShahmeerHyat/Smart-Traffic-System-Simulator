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

using namespace std;

const string COLOR_RESET = "\033[0m";
const string COLOR_RED = "\033[31m";
const string COLOR_GREEN = "\033[32m";
const string COLOR_YELLOW = "\033[33m";

struct TrafficLight {
    char location;
    bool greenLightOn;
    int greenLightDuration;
    time_t lastSwitchTime;
};

struct Car {
    string plateNumber;
    char startPoint;
    char destination;
    LinkedList<char> route;
    LinkedList<int> travelTimes;
    int currentSegmentIndex;
    int timeSpentInSegment;
    bool inMotion;
};

class LightControlSystem {
private:
    HashTable<char, TrafficLight*> trafficLights;

public:
    bool getLightStatus(char location, TrafficLight*& light) {
        return trafficLights.get(location, light);
    }

    void addLight(char location, int duration) {
        TrafficLight* light = new TrafficLight{
            location,
            false,
            duration,
            time(nullptr)
        };
        trafficLights.insert(location, light);
    }

    void updateLights() {
        time_t currentTime = time(nullptr);
        for (char i = 'A'; i <= 'Z'; i++) {
            TrafficLight* light;
            if (trafficLights.get(i, light)) {
                if (difftime(currentTime, light->lastSwitchTime) >= light->greenLightDuration) {
                    light->greenLightOn = !light->greenLightOn;
                    light->lastSwitchTime = currentTime;
                }
            }
        }
    }

    void triggerEmergencyLight(char location) {
        TrafficLight* light;
        if (trafficLights.get(location, light)) {
            light->greenLightOn = true;
            light->lastSwitchTime = time(nullptr);
        }
    }

    void showLightStatus() {
        cout << "\nTraffic Light Status:\n";
        cout << "=====================\n";
        time_t currentTime = time(nullptr);
        TrafficLight* light;

        for (char i = 'A'; i <= 'Z'; i++) {
            if (trafficLights.get(i, light)) {
                int remainingTime = light->greenLightDuration - difftime(currentTime, light->lastSwitchTime);
                cout << "Location " << i << ": "
                     << (light->greenLightOn ? COLOR_GREEN + "GREEN" : COLOR_RED + "RED") << COLOR_RESET
                     << " (" << remainingTime << " seconds to switch)" << endl;
            }
        }
    }
};

class VehicleNavigationSystem {
private:
    DirectedWeightedGraph* cityMap;
    LightControlSystem* lights;
    HashTable<string, Car> carRecords;
    HashTable<char, int> congestionData;
    LinkedList<string> carIds;

    int mapIndex(char id) { return id - 'A'; }
    char mapLocation(int index) { return static_cast<char>('A' + index); }

public:
    VehicleNavigationSystem(DirectedWeightedGraph* map, LightControlSystem* lightSystem)
        : cityMap(map), lights(lightSystem) {}

    HashTable<char, int>& getCongestionInfo() {
        return congestionData;
    }

    void addCar(const string& plate, char start, char destination) {
        Car car{plate, start, destination, LinkedList<char>(), LinkedList<int>(), 0, 0, true};
        calculatePath(car);
        carRecords.insert(plate, car);
        carIds.insertAtEnd(plate);
    }

    void calculatePath(Car& car) {
        int path[100];
        int pathLength = cityMap->dijkstra(
            mapIndex(car.startPoint),
            mapIndex(car.destination),
            path
        );

        car.route.clear();
        car.travelTimes.clear();

        for (int i = 0; i < pathLength; i++) {
            car.route.insertAtEnd(mapLocation(path[i]));
            if (i < pathLength - 1) {
                GNode* edges = cityMap->getEdges(path[i]);
                while (edges != nullptr) {
                    if (edges->vertex == path[i + 1]) {
                        car.travelTimes.insertAtEnd(edges->weight);
                        break;
                    }
                    edges = edges->next;
                }
            }
        }
    }

    void updateAllCars() {
        Node<string>* current = carIds.head;
        while (current != nullptr) {
            updateCarPosition(current->data);
            current = current->next;
        }
    }

    void updateCarPosition(const string& plate) {
        Car car;
        if (carRecords.get(plate, car)) {
            if (!car.inMotion) return;

            TrafficLight* light;
            char currentLocation = getCurrentLocation(car);
            if (lights->getLightStatus(currentLocation, light)) {
                if (!light->greenLightOn) {
                    return; // Stay at red light
                }
            }

            car.timeSpentInSegment++;
            Node<int>* travelTimeNode = car.travelTimes.head;
            for (int i = 0; i < car.currentSegmentIndex; i++) {
                if (travelTimeNode) travelTimeNode = travelTimeNode->next;
            }

            if (travelTimeNode && car.timeSpentInSegment >= travelTimeNode->data) {
                car.timeSpentInSegment = 0;
                car.currentSegmentIndex++;
                updateTrafficData(car);
                if (car.currentSegmentIndex >= car.route.countNodes() - 1) {
                    car.inMotion = false;
                }
            }
            carRecords.insert(plate, car);
        }
    }

    void updateTrafficData(const Car& car) {
        char currentLocation = getCurrentLocation(car);
        int count;
        if (congestionData.get(currentLocation, count)) {
            congestionData.insert(currentLocation, count + 1);
        } else {
            congestionData.insert(currentLocation, 1);
        }
    }

    char getCurrentLocation(const Car& car) {
        Node<char>* current = car.route.head;
        for (int i = 0; i < car.currentSegmentIndex && current; i++) {
            current = current->next;
        }
        return current ? current->data : car.startPoint;
    }

    void showAllCars() {
        Node<string>* current = carIds.head;
        while (current != nullptr) {
            showCarStatus(current->data);
            current = current->next;
        }
    }

    void showCarStatus(const string& plate) {
        Car car;
        if (carRecords.get(plate, car)) {
            cout << "\nCar " << plate << ":\n";

            if (!car.inMotion && car.currentSegmentIndex >= car.route.countNodes() - 1) {
                cout << "Status: ARRIVED at destination " << car.destination << "\n";
                return;
            }

            Node<char>* routeNode = car.route.head;
            Node<int>* timingNode = car.travelTimes.head;

            for (int i = 0; i <= car.currentSegmentIndex && routeNode; i++) {
                if (i < car.currentSegmentIndex) {
                    cout << routeNode->data << " -> ";
                    routeNode = routeNode->next;
                    if (timingNode) timingNode = timingNode->next;
                }
            }

            if (routeNode && routeNode->next) {
                cout << COLOR_RED << routeNode->data << " -> " << routeNode->next->data << COLOR_RESET;
                if (timingNode) {
                    cout << " (" << car.timeSpentInSegment << "/" << timingNode->data << " seconds)";
                }
            }
        }
    }
};

class UrbanTrafficControl {
private:
    DirectedWeightedGraph* mapGraph;
    int numberOfLocations;
    VehicleNavigationSystem* vehicleRouter;
    LightControlSystem* lightManager;

    int getLocationIndex(char id) { return id - 'A'; }
    char getLocationId(int index) { return static_cast<char>('A' + index); }

    void countLocations(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Unable to open file: " << filename << endl;
            return;
        }

        string line;
        char maxLocation = 'A';

        while (getline(file, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string from, to;
            if (getline(ss, from, ',') && getline(ss, to, ',')) {
                maxLocation = max(maxLocation, from[0]);
                maxLocation = max(maxLocation, to[0]);
            }
        }

        numberOfLocations = getLocationIndex(maxLocation) + 1;
        file.close();
    }

    void loadMapData(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Unable to open file: " << filename << endl;
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
                    mapGraph->addEdge(getLocationIndex(from[0]), getLocationIndex(to[0]), w);
                } catch (const exception& e) {
                    cerr

 << "Error parsing weight: " << weight << endl;
                }
            }
        }

        file.close();
    }

public:
    UrbanTrafficControl() : mapGraph(nullptr), numberOfLocations(0), vehicleRouter(nullptr), lightManager(nullptr) {}

    void initializeGraph(const string& filename) {
        countLocations(filename);
        mapGraph = new DirectedWeightedGraph(numberOfLocations);
        loadMapData(filename);
    }

    void start() {
        if (!mapGraph) {
            cerr << "Graph not initialized. Cannot start traffic control." << endl;
            return;
        }

        lightManager = new LightControlSystem();
        vehicleRouter = new VehicleNavigationSystem(mapGraph, lightManager);

        while (true) {
            cout << "\nUrban Traffic Management System\n";
            cout << "1. Add Traffic Light\n";
            cout << "2. Add Car\n";
            cout << "3. Show Traffic Lights\n";
            cout << "4. Show All Cars\n";
            cout << "5. Update\n";
            cout << "6. Exit\n";

            int choice;
            cin >> choice;

            if (choice == 6) break;

            char location, start, destination;
            string plate;
            int duration;

            switch (choice) {
                case 1:
                    cout << "Enter Traffic Light Location: ";
                    cin >> location;
                    cout << "Enter Green Light Duration (in seconds): ";
                    cin >> duration;
                    lightManager->addLight(location, duration);
                    break;
                case 2:
                    cout << "Enter Car Plate Number: ";
                    cin >> plate;
                    cout << "Enter Start Location: ";
                    cin >> start;
                    cout << "Enter Destination Location: ";
                    cin >> destination;
                    vehicleRouter->addCar(plate, start, destination);
                    break;
                case 3:
                    lightManager->showLightStatus();
                    break;
                case 4:
                    vehicleRouter->showAllCars();
                    break;
                case 5:
                    lightManager->updateLights();
                    vehicleRouter->updateAllCars();
                    break;
                default:
                    cout << "Invalid choice. Try again." << endl;
            }

            this_thread::sleep_for(chrono::seconds(1));
        }
    }
};

int main() {
    UrbanTrafficControl system;
    system.initializeGraph("roadmap.txt");
    system.start();
    return 0;
}
