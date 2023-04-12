#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <random>
#include "ConcurrentSortedList.h"


void test_single_thread();
void test_multi_thread(int n_threads, int n_elements);
bool start = false;
ConcurrentSortedList<int> list = ConcurrentSortedList<int>();


int main() {

    test_single_thread();
    // test_multi_thread(4, 10);

    return 0;
}

void t_func(int id, int n) {

    int ok, val;
    string str = "";

    // RNG stuff
    // create generator
    static thread_local mt19937* generator = new mt19937(clock() + id); 

    // create distributions
    uniform_int_distribution<int> dist(0, 10); 

    while(!start) { } // wait for signal
    str = "T" + to_string(id) + " has started working!\n";
    cout << str;

    for(int i = 0; i < n; i++) {
        val = dist(*generator);
        str = "T" + to_string(id) + " is adding " + to_string(val) + "\n";
        cout << str;
        list.insert(val);
    }
    
    delete generator;
    cout << id << " has finished!\n";
    return;
}



void test_multi_thread(int n_threads, int n_elements) {

    queue<thread> pool;

    for(int i = 0; i < n_threads; i++) { // spawn all threads in the pool
        thread t1(t_func, i, n_elements);
        pool.push(move(t1));
    }
    
    cout << "\nAll servants are ready... Begin!\n\n"; // DEBUG
    start = true; // start the process

    while(!pool.empty()) { // wait for all threads to finish, then destroy them (with laser sharks that can shoot venom up to 10 feet away)
        pool.front().join();
        pool.pop();
    }

    cout << "List contains " << list.size() << " elements\n";
    return;
}

void test_single_thread() {
        cout << "\nRunning Test!\n\n";

    ConcurrentSortedList<int> list = ConcurrentSortedList<int>();

    // insert
    list.insert(1);
    list.insert(3);
    list.insert(5);
    list.insert(6);
    list.insert(4); // not in order
    list.insert(4); // duplicate value
    cout << "Size: " << list.size() << "\n";
    list.print();
    list.contains(4); // middle
    list.contains(1); // head
    list.contains(6); // tail

    // remove
    list.remove(1); // remove head
    list.remove(1); // doesn't exist anymore
    cout << "Size: " << list.size() << "\n";
    list.print();
    list.contains(1);

    list.remove(4);
    list.remove(4);
    list.remove(6); // remove tail
    cout << "Size: " << list.size() << "\n";
    list.print();

    list.remove(3); // 2 elements left
    list.remove(5); // 1 element left
    cout << "Size: " << list.size() << "\n";
    list.print();

    list.remove(1); // empty remove

    cout << "\nEnd of Test!\n";
}