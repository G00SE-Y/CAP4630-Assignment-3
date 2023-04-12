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
const int n_sensors = 8;

const int ms_per_minute = 200;
const int n_hours = 2;
const int n_reads_per_hour = 60;
const int n_cycles = 10; // n_hours * n_reads_per_hour;

double min_temp = -100.0;
double max_temp =   70.0;


// * global vars
bool start = false;
bool reset = false;
int cycle = 0;
double readings[n_sensors][n_cycles];
bool flags[n_sensors];


// * global mutexes



// * helper functions
int create_report(int n_threads);
void sensor_function(int id, int cycles);
string format_float(double n, int precision, int len);
bool all_ready(bool* flags, int len);
void pretty_print_readings(double arr[n_sensors][n_cycles], int nsens, int nreads);


int main() {

    printf("Number of sensors (threads): %d\n", n_sensors);

    // run simulation
    auto start = high_resolution_clock::now(); // start time
    int avg_time = create_report(n_sensors);// simulation
    auto end = high_resolution_clock::now(); // end time

    int total_time = duration_cast<milliseconds>(end - start).count();

    cout << "\nSimulation complete!\n\n";
    cout << "Total simulation time: " << total_time << "ms\n\n";
    cout << "Expected time between measurements: " << ms_per_minute << "ms\n";
    cout << "Average measured time between measurements: " << avg_time << "ms\n\n";

    return 0;
}


int create_report(int n_threads) {
    
    queue<thread> pool;

    for(int i = 0; i < n_threads; i++) { // spawn all threads in the pool
        
        flags[i] = false;
        thread t1(sensor_function, i, n_cycles);
        pool.push(move(t1));
    }


    while(!all_ready(flags, n_threads)) {} // wait for sensors to be ready
    
    cout << "\nSensors are ready... Start measurements!\n\n"; // DEBUG
    start = true; // start the process
    
    auto start = high_resolution_clock::now(); // start measuring

    while(!pool.empty()) { // wait for all threads to finish, then destroy them (with laser sharks that can shoot venom up to 10 feet away and are highly trained black belts in shark jutsu)
        pool.front().join();
        pool.pop();
    }
    auto end = high_resolution_clock::now(); // stop measuring

    pretty_print_readings(readings, 8, n_cycles);

    return duration_cast<milliseconds>(end - start).count() / n_cycles; // return avg cycle time
}



void sensor_function(int id, int cycles) {

    // RNG stuff
    auto now = high_resolution_clock::now();
    int mod = duration_cast<microseconds>(now.time_since_epoch() - duration_cast<seconds>(now.time_since_epoch())).count();
    static thread_local mt19937* generator = new mt19937(id + mod);
    uniform_real_distribution<double> dist(min_temp, max_temp);


    string str = "";
    double reading;

    flags[id] = true;
    while(!start) { } // wait for signal

    auto start = high_resolution_clock::now();

    for(int i = 0; i < cycles + 1; i++) {

        int time_in_ms = 1;
        while(time_in_ms % ms_per_minute != 0) { 
            auto stop = high_resolution_clock::now();
            time_in_ms = duration_cast<milliseconds>(stop - start).count();
        } // wait for next measurement time
        
        readings[id][i] = dist(*generator);

        // str = "T" + to_string(id) + " read " + format_float(readings[id][i], 3, 8) + " F\n";
        // cout << str;
        
        this_thread::sleep_for(chrono::milliseconds(1));
    }

    delete generator;
    return;
}


bool all_ready(bool* flags, int len) {

    for(int i = 0; i < len; i++) {
        
        if(!flags[i]) return false;
    }

    return true;
}


string format_float(double n, int precision, int len) {
    
    std::stringstream ss;

    ss << std::fixed << std::setprecision(precision) << n; // format float

    string s = ((n > 0)? " " : "") + ss.str(); // account for munis sign

    while(s.size() < len) s = " " + s; // pad front with spaces

    return s; 
}


void pretty_print_readings(double arr[n_sensors][n_cycles], int nsens, int nreads) {

    string str = "All measurements:\n\n";
    int i;
    
    for(i = 0; i < nsens; i++) {
        str += format_float(i, 0, 8);
        str += "  ";
    }

    str += "\n";

    for(i = 0; i < nreads; i++) {

        for(int j = 0; j < nsens; j++) {

            str += format_float(arr[j][i], 3, 8);
            str += "  ";
        }

        str += "\n";
    }

    cout << str;
}