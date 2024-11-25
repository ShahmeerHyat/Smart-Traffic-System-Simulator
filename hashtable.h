#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <iostream>


template<typename K, typename V>
class HashTable {
private:
    struct Node {
        K key;
        V value;
        Node* next;
        Node(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
    };

    static const size_t DEFAULT_CAPACITY = 10;
    Node** table;  // Array of Node pointers
    size_t capacity;
    size_t size;

    size_t hash(const K& key) const {
        return std::hash<K>{}(key) % capacity;
    }

    void rehash() {
        size_t oldCapacity = capacity;
        Node** oldTable = table;
        
        // Create new array with double capacity
        capacity *= 2;
        table = new Node*[capacity]();
        
        // Reset size as items will be reinserted
        size = 0;
        
        // Rehash all existing elements
        for (size_t i = 0; i < oldCapacity; i++) {
            Node* current = oldTable[i];
            while (current != nullptr) {
                insert(current->key, current->value);
                Node* temp = current;
                current = current->next;
                delete temp;
            }
        }
        
        delete[] oldTable;
    }

public:
    explicit HashTable(size_t initialCapacity = DEFAULT_CAPACITY) 
        : capacity(initialCapacity), size(0) {
        table = new Node*[capacity]();  // Initialize all to nullptr
    }

    ~HashTable() {
        clear();
        delete[] table;
    }

    void insert(const K& key, const V& value) {
        // Check load factor and rehash if necessary
        if (size >= capacity * 0.75) {
            rehash();
        }

        size_t index = hash(key);
        Node* current = table[index];

        // Check if key already exists in the chain
        while (current != nullptr) {
            if (current->key == key) {
                current->value = value;  // Update value
                return;
            }
            current = current->next;
        }

        // Create new node and add to front of chain
        Node* newNode = new Node(key, value);
        newNode->next = table[index];
        table[index] = newNode;
        size++;
    }

    bool remove(const K& key) {
        size_t index = hash(key);
        Node* current = table[index];
        Node* prev = nullptr;

        while (current != nullptr) {
            if (current->key == key) {
                if (prev == nullptr) {
                    table[index] = current->next;
                } else {
                    prev->next = current->next;
                }
                delete current;
                size--;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }

    bool get(const K& key, V& value) const {
        size_t index = hash(key);
        Node* current = table[index];

        while (current != nullptr) {
            if (current->key == key) {
                value = current->value;
                return true;
            }
            current = current->next;
        }
        return false;
    }

    V& operator[](const K& key) {
        size_t index = hash(key);
        Node* current = table[index];

        while (current != nullptr) {
            if (current->key == key) {
                return current->value;
            }
            current = current->next;
        }

        // If key doesn't exist, insert default value
        V defaultValue = V();
        insert(key, defaultValue);
        return operator[](key);  // Recursively get the newly inserted value
    }

    void clear() {
        for (size_t i = 0; i < capacity; i++) {
            Node* current = table[i];
            while (current != nullptr) {
                Node* temp = current;
                current = current->next;
                delete temp;
            }
            table[i] = nullptr;
        }
        size = 0;
    }

    size_t getSize() const { return size; }
    size_t getCapacity() const { return capacity; }
    bool isEmpty() const { return size == 0; }

    // For debugging
    void print() const {
        for (size_t i = 0; i < capacity; i++) {
            std::cout << "[" << i << "]: ";
            Node* current = table[i];
            if (!current) {
                std::cout << "null";
            }
            while (current != nullptr) {
                std::cout << "(" << current->key << ":" << current->value << ") -> ";
                current = current->next;
            }
            std::cout << std::endl;
        }
    }
};

#endif // HASH_TABLE_H