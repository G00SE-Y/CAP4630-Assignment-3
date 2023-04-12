#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <queue>
#include <string>
#include <cmath>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace std::chrono;


// * starting conditions
int n_sensors = 8;
double min_temp = -100.0;
double max_temp = 70.0;


// * global vars
bool start = false;
uniform_real_distribution<double> dist(min_temp, max_temp);


// * global mutexes



// * helper functions
void create_report(int n_threads);
void sensor_function(int id);
string format_float(double n, int precision, int len);


int main() {

    printf("Number of sensors (threads): %d\n", n_sensors);

    // run simulation
    auto start = high_resolution_clock::now(); // start time
    create_report(n_sensors);// simulation
    auto end = high_resolution_clock::now(); // end time

    auto total_time = duration_cast<milliseconds>(end - start);

    cout << "\nSimulation complete!\n\n";

    return 0;
}


void create_report(int n_threads) {
    
    queue<thread> pool;

    for(int i = 0; i < n_threads; i++) { // spawn all threads in the pool
        thread t1(sensor_function, i);
        pool.push(move(t1));
    }


    cout << "\nAll servants are ready... Begin!\n\n"; // DEBUG
    start = true; // start the process

    while(!pool.empty()) { // wait for all threads to finish, then destroy them (with laser sharks that can shoot venom up to 10 feet away and are highly trained black belts in shark jutsu)
        pool.front().join();
        pool.pop();
    }
}

void sensor_function(int id) {

    string str = "";
    double reading;

    // RNG stuff
    static thread_local mt19937* generator = new mt19937(id + clock());

    while(!start) { } // wait for signal

    reading = dist(*generator);

    str = "T" + to_string(id) + " read " + format_float(reading, 3, 8) + " F\n";
    cout << str;


    delete generator;
    return;
}


string format_float(double n, int precision, int len) {
    
    std::stringstream ss;

    ss << std::fixed << std::setprecision(precision) << n; // format float

    string s = ((n > 0)? " " : "") + ss.str(); // account for munis sign

    while(s.size() < len) s = " " + s; // pad front with spaces

    return s; 
}