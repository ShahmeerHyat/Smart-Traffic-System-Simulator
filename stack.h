#ifndef STACK_H
#define STACK_H

#include <iostream>
using namespace std;

template <typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;
        Node(T value) : data(value), next(nullptr) {}
    };

    Node* top_;
    size_t size_;

public:
    // Constructor
    Stack() : top_(nullptr), size_(0) {}

    // Destructor
    ~Stack() {
        while (!isEmpty()) {
            pop();
        }
    }

    // Copy Constructor
    Stack(const Stack& other) : top_(nullptr), size_(0) {
        if (other.isEmpty()) {
            return;
        }
        Node* otherCurrent = other.top_;
        Node* newNode = new Node(otherCurrent->data);
        top_ = newNode;

        otherCurrent = otherCurrent->next;
        while (otherCurrent != nullptr) {
            newNode->next = new Node(otherCurrent->data);
            newNode = newNode->next;
            otherCurrent = otherCurrent->next;
        }

        size_ = other.size_;
    }

    // Assignment Operator
    Stack& operator=(const Stack& other) {
        if (this != &other) {
            while (!isEmpty()) {
                pop();
            }
            if (!other.isEmpty()) {
                Node* otherCurrent = other.top_;
                Node* newNode = new Node(otherCurrent->data);
                top_ = newNode;

                otherCurrent = otherCurrent->next;
                while (otherCurrent != nullptr) {
                    newNode->next = new Node(otherCurrent->data);
                    newNode = newNode->next;
                    otherCurrent = otherCurrent->next;
                }

                size_ = other.size_;
            }
        }
        return *this;
    }

    // Push a value onto the stack
    void push(T value) {
        Node* newNode = new Node(value);
        newNode->next = top_;
        top_ = newNode;
        size_++;
    }

    // Pop a value from the stack
    void pop() {
        if (isEmpty()) {
            cout << "Stack is empty\n";
            return;
        }
        Node* temp = top_;
        top_ = top_->next;
        delete temp;
        size_--;
    }

    // Peek at the top value of the stack
    T peek() {
        if (isEmpty()) {
            cout << "Stack is empty\n";
            return T();  // Return a default-constructed object if the stack is empty
        }
        return top_->data;
    }

    // Check if the stack is empty
    bool isEmpty() const {
        return top_ == nullptr;
    }

    // Return the size of the stack
    size_t size() const {
        return size_;
    }
};

#endif // STACK_H
