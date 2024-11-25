#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

template<typename T, size_t MAX_SIZE>
class PriorityQueue {
private:
    T heap[MAX_SIZE];
    size_t currentSize;
    bool isMinHeap;  // true for min heap, false for max heap
    
    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if ((isMinHeap && heap[index] < heap[parent]) || 
                (!isMinHeap && heap[index] > heap[parent])) {
                T temp = heap[index];
                heap[index] = heap[parent];
                heap[parent] = temp;
                index = parent;
            } else {
                break;
            }
        }
    }
    
    void heapifyDown(int index) {
        while (true) {
            int leftChild = 2 * index + 1;
            int rightChild = 2 * index + 2;
            int selected = index;
            
            if (leftChild < currentSize && 
                ((isMinHeap && heap[leftChild] < heap[selected]) ||
                 (!isMinHeap && heap[leftChild] > heap[selected]))) {
                selected = leftChild;
            }
            
            if (rightChild < currentSize && 
                ((isMinHeap && heap[rightChild] < heap[selected]) ||
                 (!isMinHeap && heap[rightChild] > heap[selected]))) {
                selected = rightChild;
            }
            
            if (selected == index) {
                break;
            }
            
            T temp = heap[index];
            heap[index] = heap[selected];
            heap[selected] = temp;
            index = selected;
        }
    }

public:
    PriorityQueue(bool isMin = true) : currentSize(0), isMinHeap(isMin) {}
    
    void push(const T& value) {
        if (currentSize >= MAX_SIZE) {
            throw "Heap is full";
        }
        heap[currentSize] = value;
        heapifyUp(currentSize);
        currentSize++;
    }
    
    T pop() {
        if (currentSize == 0) {
            throw "Heap is empty";
        }
        
        T top = heap[0];
        currentSize--;
        heap[0] = heap[currentSize];
        
        if (currentSize > 0) {
            heapifyDown(0);
        }
        
        return top;
    }
    
    T& top() {
        if (currentSize == 0) {
            throw "Heap is empty";
        }
        return heap[0];
    }
    
    bool empty() const {
        return currentSize == 0;
    }
    
    bool full() const {
        return currentSize == MAX_SIZE;
    }
    
    size_t size() const {
        return currentSize;
    }
    
    void clear() {
        currentSize = 0;
    }
};

#endif // PRIORITY_QUEUE_H