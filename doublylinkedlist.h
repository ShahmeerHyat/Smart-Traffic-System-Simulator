
#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iostream>
#include <string>

using namespace std;
template <typename T>

class Node {
    public:
    T data;
    Node* next;
    Node* prev;

    Node() : data(T()), next(NULL), prev(NULL) {}

    Node(T data) : data(data), next(NULL), prev(NULL) {}
};

template <typename T>
class LinkedList {
    public:
    Node<T>* head;
    Node<T>* tail;

    LinkedList() : head(NULL), tail(NULL) {}

    void insertAtHead(T v) {
         Node<T>* temp = new Node<T>(v);
        if (head == NULL) {
            head = temp;
            tail = temp;
        } else {
            temp->next = head;
            head->prev = temp;
            head = temp;
        }
    }

    void insertAtEnd(T v) {
        Node<T>* temp = new Node<T>(v);
        if (head == NULL) {
            head = temp;
            tail = temp;
        } else {
            tail->next = temp;
            temp->prev = tail;
            tail = temp;
        }
    }

    void insertAtLocation(int l, T v) {
        Node<T>* temp = new Node<T>(v);
        if (head == NULL) {
            head = temp;
            tail = temp;
        } else if (l == 0) {
            temp->next = head;
            head->prev = temp;
            head = temp;
        } else {
             Node<T>* p = head;
            for (int i = 0; i < l  && p != NULL; i++) {
                p = p->next;
            }

            if (p == NULL) {
                // Insert at the end if l is greater than length
                tail->next = temp;
                temp->prev = tail;
                tail = temp;
            } else {
                // Insert in the middle
                temp->next = p;
                temp->prev = p->prev;

                if (p->prev != NULL) {
                    p->prev->next = temp;
                } else {
                    head = temp; // Insert at head
                }
                p->prev = temp;
            }
        }
    }

    void deleteAtStart() {
        if (head == NULL) return; // Check if list is empty

         Node<T>* q = head;
        head = head->next;
        if (head != NULL) {
            head->prev = NULL;
        } else {
            tail = NULL; // List becomes empty
        }
        delete q;
    }

    void deleteAtEnd() {
        if (tail == NULL) return; // Check if list is empty

       Node<T>* q = tail;
        tail = tail->prev;
        if (tail != NULL) {
            tail->next = NULL;
        } else {
            head = NULL; // List becomes empty
        }
        delete q;
    }

    void deleteAtLocation(int l) {
        if (head == NULL) return; // Check if list is empty

       Node<T>* pre = head;
        Node<T>* cur = head;
        for (int i = 0; i < l && cur != NULL; i++) {
            pre = cur;
            cur = cur->next;
        }

        if (cur == NULL) return; // If cur is NULL, l is out of bounds

        if (pre == head) {
            // Deleting the head
            head = head->next;
            if (head != NULL) {
                head->prev = NULL;
            } else {
                tail = NULL; // List becomes empty
            }
        } else {
            pre->next = cur->next;
            if (cur->next != NULL) {
                cur->next->prev = pre;
            } else {
                tail = pre; // Update tail if last node is deleted
            }
        }
        delete cur;
    }

    void deleteValue(T v) {
        if (head == NULL) return; // Check if list is empty

        Node<T>* pre = head;
        Node<T>* curr = head;
        while (curr != NULL && curr->data != v) {
            pre = curr;
            curr = curr->next;
        }
        if (curr != NULL) {
            if (curr == head) {
                deleteAtStart();
            } else {
                pre->next = curr->next;
                if (curr->next != NULL) {
                    curr->next->prev = pre;
                } else {
                    tail = pre; // Update tail if last node is deleted
                }
            }
            delete curr;
        }
    }

    void search(T v) {
        Node<T>* p = head;
        int index = 0;
        while (p != NULL) {
            if (p->data == v) {
                cout << "Data found at index: " << index << endl;
                return;
            }
            p = p->next;
            index++;
        }
        cout << "Data not found" << endl;
    }

    void printList() {
        cout << "Forward list is: ";
        if (head == NULL) {
            cout << "List is empty\n";
        } else {
             Node<T>* p = head;
            while (p != NULL) {
                cout << p->data << " ";
                p = p->next;
            }
            cout << endl;
        }
    }

    void printReversedList() {
        cout << "Reversed list is: ";
        if (tail == NULL) {
            cout << "List is empty\n";
        } else {
            Node<T>* p = tail;
            while (p != NULL) {
                cout << p->data << " ";
                p = p->prev;
            }
            cout << endl;
        }
    }

    void countNodes() {
        Node<T>* p = head;
        int count = 0;
        while (p != NULL) {
            count++;
            p = p->next;
        }
        cout << "Linked list has: " << count << " nodes\n";
    }
};


#endif  