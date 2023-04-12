#include <iostream>
#include <string>
#include <mutex>
#include "ConcurrentSortedList.h"

using namespace std;

// * CONSTRUCTOR
template <typename T> 
ConcurrentSortedList<T>::ConcurrentSortedList() {

    head = new Node(-1);
    head->next = nullptr;
}


// * DESTRUCTOR
template <typename T> 
ConcurrentSortedList<T>::~ConcurrentSortedList() {
                    
    Node* t1 = head;
    Node* t2 = head->next;

    while(t2) {
        // cout << "Deleted " << t1->value << "\n"; // debug
        delete t1;
        t1 = t2;
        t2 = t2->next;
    }

    // cout << "Deleted " << t1->value << "\n"; // debug
    delete t1;
}


// * INSERT
template <typename T> 
bool ConcurrentSortedList<T>::insert(T val) {

    string str = "";

    Node* n = new Node(val);
    n->value = val;
    n->next = nullptr;

    str =  "Adding " + to_string(n->value) + "\n";
    cout << str;

    head->m.lock(); // prev lock
    Node* prev = head;
    Node* cur;

    if(prev->next == nullptr) { // empty list
        prev->next = n;
        prev->m.unlock();
        str =  to_string(val) + " successfully added as first element\n"; // debug
        cout << str;
        return true;
    }

    prev->next->m.lock();
    cur = prev->next;

    while(cur) {

        if(cur->value > n->value) { // insert between cur and prev
            // insert node
            prev->next = n;
            n->next = cur;

            // drop locks
            prev->m.unlock();
            cur->m.unlock();

            str = to_string(val) + " successfully added\n"; // debug
            cout << str;
            return true;
        }

        prev->m.unlock(); // drop prev lock
        prev = cur; // shift ptr

        if(!prev->next) break;

        prev->next->m.lock();
        cur = prev->next;
    }

    // end of list, insert regardless
    prev->next = n;
    prev->m.unlock();

    str = to_string(val) + " successfully added to end of list\n"; // debug
    cout << str;
    return true;
}


// * REMOVE
template <typename T> 
bool ConcurrentSortedList<T>::remove(T val) {
    
    string str = "";

    str = "Removing " + to_string(val) + "\n"; // debug
    cout << str;

    head->m.lock(); // prev lock
    Node* prev;
    Node* cur = head;

    if(cur->next == nullptr) { // empty list

        str = to_string(val) + " not found in list (empty)\n"; // debug
        cout << str;
        return false;
    }

    cur->next->m.lock(); // cur lock
    prev = cur; // shift ptr
    cur = cur->next;
    
    while(cur) {

        if(cur->value == val) { // insert between cur and prev

            // insert node
            prev->next = cur->next; // point prev to next
            
            // drop locks
            prev->m.unlock();
            cur->m.unlock();

            delete cur;

            str = "Removed " + to_string(val) + " from list\n";
            cout << str;
            return true;
        }

        prev->m.unlock(); // drop prev lock
        prev = cur; // shift ptr
        cur = cur->next; // grab next lock
    }

    // end of list, value not found
    prev->m.unlock();
    
    str = to_string(val) + " not found in list\n"; // debug
    cout << str;

    return false;
}


// * CONTAINS VALUE
template <typename T> 
T ConcurrentSortedList<T>::contains(T val) {
    string str = "";

    str = "Searching for " + to_string(val) + "\n";  // debug 
    cout << str;

    head->m.lock();
    Node* t = head;

    while(t) {
        
        if(t->value == val) {
            str = "Found " + to_string(val) + "\n";
            cout << str;

            t->m.unlock();
            return true;
        }

        if(t->next) t->next->m.lock();
        t->m.unlock();
        t = t->next;
    }

    str = "Could not find " + to_string(val) + " in list\n";
    cout << str;
    return false;
}


// * GET LENGTH
template <typename T> 
int ConcurrentSortedList<T>::size() { // * NOT THREAD-SAFE; USE WITH CAUTION

    Node* t = head;
    int size = 0;

    while(t->next) {
        size++;
        t = t->next;
    }
    
    return size;
}


// * PRINT FOR DEBUG
template <typename T> 
void ConcurrentSortedList<T>::print() { // * UNSAFE - DEBUGGING ONLY

    Node* t = head->next;

    if(!t) {
        cout << "(EMPTY)\n";
        return;
    }

    // cout << "List:\nHead -> ";

    while(t->next) {
        cout << t->value << " -> ";
        t = t->next;
    }
    cout << t->value << " -> End\n";
}

template class ConcurrentSortedList<int>;
template class ConcurrentSortedList<float>;
template class ConcurrentSortedList<char>;
