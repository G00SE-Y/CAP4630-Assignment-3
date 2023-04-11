#include <iostream>
#include "ConcurrentSortedList.h"


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
    cout << "Size: " << list.size() << "\n";
    list.print();

    // remove
    list.remove(1); // remove head
    list.remove(1); // doesn't exist anymore
    cout << "Size: " << list.size() << "\n";
    list.print();

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