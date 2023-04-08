#include <iostream>
#include "ConcurrentSortedList.h"
// #include "ConcurrentSortedList.cpp"

using namespace std;


void test();

int main() {

    test();

    return 0;
}


void test() {
        cout << "\nRunning Test!\n\n";

    ConcurrentSortedList<int> list = ConcurrentSortedList<int>();

    // insert
    list.insert(1);
    list.insert(3);
    list.insert(5);
    list.insert(6);
    list.insert(4); // not in order
    list.insert(4); // duplicate value
    list.print();

    // remove
    list.remove(1); // remove head
    list.remove(1); // doesn't exist anymore
    list.print();

    list.remove(4);
    list.remove(4);
    list.remove(6); // remove tail
    list.print();

    list.remove(3); // 2 elements left
    list.remove(5); // 1 element left
    list.print();

    list.remove(1); // empty remove

    cout << "\nEnd of Test!\n";
}