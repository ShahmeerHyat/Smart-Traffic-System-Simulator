#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
#include <stdexcept>

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
    // Constructor
    Queue() : front(nullptr), rear(nullptr), size(0) {}

    // Destructor
    ~Queue() {
        while (!isEmpty()) {
            dequeue();
        }
    }

    // Enqueue operation
    void enqueue(T value) {
        Node* newNode = new Node(value);
        if (rear == nullptr) {
            // If the queue is empty, both front and rear are the new node
            front = rear = newNode;
        } else {
            // Add the new node at the end of the queue
            rear->next = newNode;
            rear = newNode;
        }
        size++;
    }

    // Dequeue operation
    T dequeue() {
        if (isEmpty()) {
            throw std::underflow_error("Queue is empty. Cannot dequeue.");
        }
        Node* temp = front;
        T data = temp->data;
        front = front->next;
        if (front == nullptr) {
            // If the queue becomes empty, set rear to nullptr
            rear = nullptr;
        }
        delete temp;
        size--;
        return data;
    }

    // Peek operation (view the front element)
    T peek() const {
        if (isEmpty()) {
            throw std::underflow_error("Queue is empty. Cannot peek.");
        }
        return front->data;
    }

    // Check if the queue is empty
    bool isEmpty() const {
        return size == 0;
    }

    // Get the current size of the queue
    int getSize() const {
        return size;
    }
};

#endif // QUEUE_H
