# Smart-Traffic-System-Simulator

A real-time traffic simulation system built in C++ that manages city traffic flow, handles emergency vehicles, and responds to road events.

##Features

-City traffic network visualization using graph data structure
-Dynamic vehicle routing with real-time path recalculation
-Traffic signal management with emergency override
-Emergency vehicle priority handling
-Congestion monitoring and management
-Accident simulation and road closure handling
-Real-time collision detection and reporting

##Data Structures Used

-Graph: Road network representation
-Hash Table: Vehicle tracking and road status
-Priority Queue: Traffic signal management
-Doubly Linked List: Path and vehicle management
-Queue: BFS implementation for alternative routes

##File Structure

-main.cpp: Core simulation logic and UI
-graph.h: Graph implementation for road network
-queue.h: Queue implementation
-hashtable.h: Hash table implementation
-heap.h: Priority queue implementation
-doublylinkedlist.h: Linked list implementation

##Input Files

-road_network.csv: Road connections and travel times
-vehicles.csv: Regular vehicle data
-emergency_vehicles.csv: Emergency vehicle information
-traffic_signals.csv: Signal timing data
-road_closures.csv: Road closure information

##Building and Running

###Compile the project:

g++ main.cpp -o traffic_system

###Run the executable:

./traffic_system


##Interface Controls

-Use number keys (1-8) to navigate menu options
-ESC: Return to main menu
-'O': Override traffic signals (when in signal view)

##System Requirements

-C++11 compatible compiler
-Windows OS (for console clearing)
-Threading support

##Notes

-The system updates every second
-Emergency vehicles get priority routing
Roads can become blocked due to accidents or maintenance
Traffic signals adjust based on congestion levels
