#include <iostream>
#include <string>

using namespace std;

class Stack {
   
    private:
    struct Node {
        char data;
        Node* next;
        Node(char value) : data(value), next(nullptr) {}
    };

    Node* top_;      
    size_t size_;    

public:
    Stack() : top_(nullptr), size_(0) {}
    
    ~Stack() {
        while (!isEmpty()) {
            pop();
        }
    }
    
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
    
    void push(char value) {
        Node* newNode = new Node(value);
        newNode->next = top_;
        top_ = newNode;
        size_++;
    }
    
    void pop() {
        if (isEmpty()) {
            cout << "Stack is empty\n";
        }
        
        Node* temp = top_;
        top_ = top_->next;
        delete temp;
        size_--;
    }
    
    char peek() {
        if (isEmpty()) {
            cout <<"Stack is empty\n";
        }
        return top_->data;
    }
    
    bool isEmpty() const {
        return top_ == nullptr;
    }
    
    size_t size() const {
        return size_;
    }
   

   
};