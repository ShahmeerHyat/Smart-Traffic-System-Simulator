#include <iostream>
using namespace std;

template <typename T>
class Queue {
private:
    struct Node {
        T data;
        Node* next;
        Node(T value) : data(value), next(nullptr) {}
    };

    Node* front; 
    Node* rear;  
    int size;    

public:
    Queue() : front(nullptr), rear(nullptr), size(0) {}

    ~Queue() {
        while (!isEmpty()) {
            dequeue();
        }
    }

    void enqueue(T value) {
        Node* newNode = new Node(value);
        if (rear == nullptr) {
          
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
        size++;
    }


    T dequeue() {
        if (isEmpty()) {
            throw underflow_error("Queue is empty. Cannot dequeue.");
        }
        Node* temp = front;
        T data = temp->data;
        front = front->next;
        if (front == nullptr) {
            rear = nullptr; 
        }
        delete temp;
        size--;
        return data;
    }

  
    T peek() const {
        if (isEmpty()) {
            throw underflow_error("Queue is empty. Cannot peek.");
        }
        return front->data;
    }


    bool isEmpty() const {
        return size == 0;
    }

  
    int getSize() const {
        return size;
    }
};


