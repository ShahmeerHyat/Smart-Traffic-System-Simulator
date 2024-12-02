#ifndef LINKEDLIST_H
#define LINKEDLIST_H

template <typename T>
class LinkedList
{
private:
    struct Node
    {
        T data;
        Node *next;

        Node(const T &data, Node *next = nullptr) : data(data), next(next) {}
    };

    Node *head;
    Node *tail;
    size_t size_;

public:
    LinkedList() : head(nullptr), tail(nullptr), size_(0) {}

    ~LinkedList()
    {
        clear();
    }

    void insertAtEnd(const T &value)
    {
        Node *newNode = new Node(value);
        if (!head)
        {
            head = tail = newNode;
        }
        else
        {
            tail->next = newNode;
            tail = newNode;
        }
        size_++;
    }

    void insert(const T &value, size_t position)
    {
        if (position > size_)
            return;

        Node *newNode = new Node(value);

        if (position == 0)
        {
            newNode->next = head;
            head = newNode;
            if (size_ == 0)
                tail = newNode;
        }
        else
        {
            Node *current = head;
            for (size_t i = 0; i < position - 1; i++)
                current = current->next;

            newNode->next = current->next;
            current->next = newNode;

            if (newNode->next == nullptr)
                tail = newNode;
        }

        size_++;
    }

    void deleteAtEnd()
    {
        if (isEmpty())
            return;

        if (head == tail)
        {
            delete head;
            head = tail = nullptr;
        }
        else
        {
            Node *current = head;
            while (current->next != tail)
                current = current->next;

            delete tail;
            tail = current;
            tail->next = nullptr;
        }
        size_--;
    }

    void deleteAtPosition(size_t position)
    {
        if (position >= size_ || isEmpty())
            return;

        if (position == 0)
        {
            Node *temp = head;
            head = head->next;
            delete temp;
            if (head == nullptr)
                tail = nullptr;
        }
        else
        {
            Node *current = head;
            for (size_t i = 0; i < position - 1; i++)
                current = current->next;

            Node *temp = current->next;
            current->next = current->next->next;
            if (current->next == nullptr)
                tail = current;
            delete temp;
        }
        size_--;
    }

    void deleteValue(const T &value)
    {
        if (isEmpty())
            return;

        if (head->data == value)
        {
            Node *temp = head;
            head = head->next;
            delete temp;
            if (head == nullptr)
                tail = nullptr;
            size_--;
            return;
        }

        Node *current = head;
        while (current->next && current->next->data != value)
        {
            current = current->next;
        }

        if (current->next)
        {
            Node *temp = current->next;
            current->next = current->next->next;
            if (current->next == nullptr)
                tail = current;
            delete temp;
            size_--;
        }
    }

    void clear()
    {
        Node *current = head;
        while (current)
        {
            Node *temp = current;
            current = current->next;
            delete temp;
        }
        head = tail = nullptr;
        size_ = 0;
    }

    size_t size() const
    {
        return size_;
    }

    Node *getHead() const
    {
        return head;
    }

    bool isEmpty() const
    {
        return size_ == 0;
    }

    void printList() const
    {
        Node *current = head;
        while (current)
        {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }
};

#endif
