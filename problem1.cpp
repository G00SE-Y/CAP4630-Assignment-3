#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>
#include <mutex>
#include <queue>
#include <thread>
#include <string>
#include <ctime>
#include <random>

#include "ConcurrentSortedList.h"

using namespace std;
using namespace std::chrono;


// * starting conditions
int n_servants = 4;
int n_presents = 5000; // 500,000


// * gloabl vars
ConcurrentSortedList<int> list = ConcurrentSortedList<int>(); // the list of presents
bool start = false;

bool bag_empty = false; // if all gifts removed from bag 
bool chain_empty = false; // if all gifts removed from chain

int gifts_in_bag = n_presents; // number of gifts left to write cards for
int gifts_in_chain = 0;


// * global mutexes
mutex mut_gifts_in_bag;
mutex mut_gifts_in_chain;


// * helper functions
void servant_function(int id);
int write_cards(int n_threads, int n_elements);


int main() {

    printf("Number of presents: %d\n", n_presents);
    printf("Number of servants (threads): %d\n", n_servants);

    // run simulation
    auto start = high_resolution_clock::now(); // start time
    int leftover = write_cards(n_servants, n_presents);
    auto end = high_resolution_clock::now(); // end time

    auto total_time = duration_cast<milliseconds>(end - start);

    cout << "\nIt took " << total_time.count() << "ms to write all of the cards and there are " << leftover << " gifts without cards.\nThanks for all the gifts!\n\n~ The Minotaur\n\n";

    return 0;
}


void servant_function(int id) {

    int gift_id, action, ok;
    string str = "";

    // RNG stuff
    int a_max = 3 - 1;
    int g_max = n_presents - 1;

    // create generator
    static thread_local mt19937* generator = new mt19937(clock() + id); 

    // create distributions
    uniform_int_distribution<int> action_dist(0, a_max); 
    uniform_int_distribution<int> gift_dist(0, g_max);

    while(!start) { } // wait for signal
    str = "T" + to_string(id) + " has started working!\n";
    cout << str;

    while(!(chain_empty && bag_empty)) { // run loop

        action = action_dist(*generator); // generate an action
        gift_id = gift_dist(*generator); // generate a random gift id

        // str = "T" + to_string(id) + " wants to do action " + to_string(action) + " on " + to_string(gift_id) + "\n";
        // cout << str;

        switch(action) { // random actions

            case 0: // insert
                if(bag_empty) break; // no presents left to insert, skip

                // atomic compare and set
                mut_gifts_in_bag.lock();
                
                if(gifts_in_bag <= 0 ) { // no more gifts to add, notify all and skip
                    bag_empty = true;
                    mut_gifts_in_bag.unlock();
                    break;
                }

                ok = list.insert(gift_id);

                if(ok) { // insert random gift
                    gifts_in_bag--; // update count
                    // str = "T" + to_string(id) + " inserted " + to_string(gift_id) + ".\n"; // debug
                    // cout << str; // debug
                }
                mut_gifts_in_bag.unlock();

                if(ok) {
                    mut_gifts_in_chain.lock();
                    gifts_in_chain++; // update count
                    chain_empty = false; // notify all that gifts are in the chain
                    mut_gifts_in_chain.unlock();
                }

                break;


            case 1: // remove

                if(chain_empty) break; // nothing to remove, skip

                mut_gifts_in_chain.lock();
                
                if(gifts_in_chain <= 0) { // no more gifts to add, notify all, and skip
                    chain_empty = true;
                    mut_gifts_in_chain.unlock();
                    break;
                }

                ok = list.remove(gift_id);

                if(ok) { // remove gift with random id
                    gifts_in_chain--; // update count
                    // str = "T" + to_string(id) + " removed " + to_string(gift_id) + ".\n"; // debug
                    // cout << str;// debug
                }
                mut_gifts_in_chain.unlock();

                break;

            
            case 2:

                if(list.contains(gift_id)) { // remove gift with random id
                    // str = "T" + to_string(id) + " found " + to_string(gift_id) + " in chain.\n"; // debug
                }
                else {
                    // str = "T" + to_string(id) + " did not find " + to_string(gift_id) + " in chain.\n"; // debug
                }

                // cout << str; // debug

                break;
        }
    }
    
    delete generator;
    cout << id << " has finished!\n";
    return;
}


int write_cards(int n_threads, int n_elements) {

    queue<thread> pool;

    for(int i = 0; i < n_threads; i++) { // spawn all threads in the pool
        thread t1(servant_function, i);
        pool.push(move(t1));
    }
    
    cout << "\nAll servants are ready... Begin!\n\n"; // DEBUG
    start = true; // start the process

    // while(!chain_empty) {

    //     if(bag_empty) {

    //     }
    // }

    while(!pool.empty()) { // wait for all threads to finish, then destroy them (with laser sharks that can shoot venom up to 10 feet away)
        pool.front().join();
        pool.pop();
    }

    cout << "Remaining gifts in bag: " << gifts_in_bag << "\n";
    cout << "Remaining gifts in bag: " << gifts_in_chain << "\n";

    return list.size();
}


/* 
 * I took this function from stack overflow for generating random numbers per thread
 * https://stackoverflow.com/questions/21237905/how-do-i-generate-thread-safe-uniform-random-numbers
 * 
 * I took it apart and adapted it in my code, but this function is not actually used. (it also leaks memory)
 */
int intRand(const int & min, const int & max, int id) {

    static thread_local mt19937* generator = nullptr; // init generator
    if (!generator) generator = new mt19937(clock() + id);
    uniform_int_distribution<int> distribution(min, max);
    return distribution(*generator);
}