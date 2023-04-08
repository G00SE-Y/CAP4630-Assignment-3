#include <iostream>
#include <mutex>
// #include <ConcurrentSortedList.cpp>

using namespace std;

template <class T> class ConcurrentSortedList {

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

        ConcurrentSortedList() {

            head = new Node(-1);
            head->next = nullptr;
        }


        ~ConcurrentSortedList() {
                    
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


        bool insert(T val) {

            Node* n = new Node(val);
            n->value = val;
            n->next = nullptr;

            // cout << "Adding " << n->value << "\n";

            head->m.lock(); // prev lock
            Node* prev = head;
            Node* cur;

            if(prev->next == nullptr) { // empty list
                prev->next = n;
                prev->m.unlock();
                cout << val << " successfully added as first element\n";
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

                    cout << val << " successfully added\n";
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
            cout << val << " successfully added to end of list\n";

            return true;
        }


        bool remove(T val) {
            
            cout << "Removing " << val << "\n";

            head->m.lock(); // prev lock
            Node* prev;
            Node* cur = head;

            if(cur->next == nullptr) { // empty list

                cout << val << " not found in list (empty)\n";
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

                    return true;
                }

                prev->m.unlock(); // drop prev lock
                prev = cur; // shift ptr
                cur = cur->next; // grab next lock
            }

            // end of list, value not found
            prev->m.unlock();
            cout << val << " not found in list\n";

            return false;
        }


        T contains(T val) {

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


        void print() { // * UNSAFE - DEBUGGING ONLY

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

};
