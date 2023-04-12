#ifndef CONCURRENT_SORTED_LIST_H_
#define CONCURRENT_SORTED_LIST_H_

#include <iostream>
#include <mutex>
// #include <ConcurrentSortedList.cpp>

using namespace std;

template <typename T> 
class ConcurrentSortedList {

    private:

        class Node {

            public:

                T value;
                Node* next;
                mutex m;

                Node(T val) { // constructor

                    value = val;
                    next = nullptr;
                }

                ~Node() {

                }
        };

        Node* head;

    public:

        ConcurrentSortedList();
        ~ConcurrentSortedList();

        bool insert(T val);
        bool remove_front();
        bool remove(T val);
        T contains(T val);
        int size();
        void print();   

};

#endif
