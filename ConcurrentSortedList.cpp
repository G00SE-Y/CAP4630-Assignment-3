#include <iostream>
#include <mutex>
#include "ConcurrentSortedList.h"

using namespace std;

// * CONSTRUCTOR
template <class T> 
ConcurrentSortedList<T>::ConcurrentSortedList() {

    head = new Node(NULL);
    head->next = nullptr;
}


// * DESTRUCTOR
template <class T> 
ConcurrentSortedList<T>::~ConcurrentSortedList() {
    
    Node* t1 = head, t2 = head->next;

    while(t2) {
        cout << "Deleted " << t1->value << "\n";
        delete t1;
        t1 = t2;
        t2 = t2->next;
    }

    delete t2;
}


// * INSERT
template <class T> 
bool ConcurrentSortedList<T>::insert(T val) {

    Node* n = new Node(val);
    n->value = val;
    n->next = nullptr;

    cout << "Adding " << n->value << "\n";

    head->m.lock(); // prev lock
    Node* prev, cur = head;

    if(cur == nullptr) { // empty list
    
        head->next = n;
        return true;
    }

    cur->next->m.lock(); // cur lock
    prev = cur; // shift ptr

    while(cur) {
        cur = cur->next; // grab next lock

        if(cur->value > n->value) { // insert between cur and prev

            // insert node
            prev->next = n;
            n->next = cur;

            // drop locks
            prev->m.unlock();
            cur->m.unlock();

            cout << val << " successfully added\n";
            return true;
        }

        prev->m.unlock(); // drop prev lock
        prev = cur; // shift ptr
    }

    // end of list, insert regardless
    prev->next = n;
    prev->m.unlock();
    cout << val << " successfully added to end of list\n";

    return true;
}


// * REMOVE
template <class T> 
bool ConcurrentSortedList<T>::remove(T val) {
    
    cout << "Removing " << val << "\n";

    head->m.lock(); // prev lock
    Node* prev, cur = head;

    if(cur == nullptr) { // empty list

        cout << "List is empty\n";
        return false;
    }

    cur->next->m.lock(); // cur lock
    prev = cur; // shift ptr

    while(cur) {
        cur = cur->next; // grab next lock

        if(cur->value == val) { // insert between cur and prev

            // insert node
            prev->next = cur->next; // point prev to next
            
            // drop locks
            prev->m.unlock();
            cur->m.unlock();

            delete cur;

            cout << val << " successfully removed\n";
            return true;
        }

        prev->m.unlock(); // drop prev lock
        prev = cur; // shift ptr
    }

    // end of list, value not found
    prev->m.unlock();
    cout << val << " not found in list\n";

    return false;
}


// * CONTAINS VALUE
template <class T> 
T ConcurrentSortedList<T>::contains(T val) {

    cout << "Searching for " << val << "\n"; 

    head->m.lock();
    Node* t = head;

    while(t) {
        
        if(t->value == val) {
            return true;
        }

        t->next->m.lock();
        t->m.unlock();
        t = t->next;
    }

    return false;
}


// * PRINT FOR DEBUG
template <class T> 
void ConcurrentSortedList<T>::print() { // * UNSAFE - DEBUGGING ONLY

    Node* t = head;

    cout << "List:\nHead -> ";

    while(t->next) {
        cout << t->value << " -> ";
    }

    cout << "End";
}