#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <queue>
#include <string>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <queue>

using namespace std;
using namespace std::chrono;


// * starting conditions
const int n_sensors = 8;

const int ms_per_minute = 30;
const int n_hours = 5;
const int n_reads_per_hour = 60;
const int n_cycles = n_hours * n_reads_per_hour;

double min_temp = -100.0;
double max_temp =   70.0;


// * global vars
bool start = false;
bool reset = false;
int cycle = 0;
int current_hour;
double readings[n_sensors][n_cycles];
bool flags[n_sensors];


// * helper functions
int run_simulation(int n_threads);
void sensor_function(int id, int cycles);
string format_float(double n, int precision, int len);
bool all_ready(bool* flags, int len);
void pretty_print_readings(double arr[n_sensors][n_cycles], int nsens, int nreads);
void print_report(double arr[n_sensors][n_cycles], int nsens, int nreads, int hour);


int main() {

    printf("Number of sensors (threads): %d\n", n_sensors);

    // run simulation
    auto start = high_resolution_clock::now(); // start time
    int avg_time = run_simulation(n_sensors);// simulation
    auto end = high_resolution_clock::now(); // end time

    int total_time = duration_cast<milliseconds>(end - start).count();

    cout << "\nSimulation complete!\n\n";
    cout << "Total simulation time: " << total_time << "ms\n\n";
    cout << "Expected time between measurements: " << ms_per_minute << "ms\n";
    cout << "Average measured time between measurements: " << avg_time << "ms\n\n";

    return 0;
}


int run_simulation(int n_threads) {
    
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

    for(int i = 0; i < n_hours + 1; i++) {

        int time_in_ms = 1;
        while(time_in_ms % (ms_per_minute * n_reads_per_hour) != 0) { 
            auto cur = high_resolution_clock::now();
            time_in_ms = duration_cast<milliseconds>(cur - start).count();
        }

        current_hour = i - 1;
        // print_report(readings, n_threads, n_hours, i - 1);
        
        this_thread::sleep_for(chrono::milliseconds(1));
    }

    while(!pool.empty()) { // wait for all threads to finish, then destroy them (with laser sharks that can shoot venom up to 10 feet away and are highly trained black belts in shark jutsu)
        pool.front().join();
        pool.pop();
    }
    auto end = high_resolution_clock::now(); // stop measuring

    // pretty_print_readings(readings, 8, n_cycles);

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
        
        readings[id][i - 1] = dist(*generator);
       
        this_thread::sleep_for(chrono::milliseconds(1));

        if(id == 0 && i % n_reads_per_hour == 0) print_report(readings, n_sensors, n_hours, current_hour); // since there are 8 cores in the exmaple simulation, i believe it is intended that one of the 8 threads compiles the report as opposed to the main calling thread compiling it, which only facilitates the simulation
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


void print_report(double arr[n_sensors][n_cycles], int nsens, int nreads, int hour) {

    string report = "";

    int start_idx = hour * n_reads_per_hour;
    int stop_idx = (hour + 1) * n_reads_per_hour - 1;

    int cycles_per_10 = n_reads_per_hour / 6;

    if(start_idx < 0) return;

    report = "Report for hour " + to_string(hour) + " (cycle " + to_string(start_idx) + " to " + to_string(stop_idx) + "):\n\n";


    priority_queue<double> min_q;
    priority_queue<double, std::vector<double>, std::greater<double>> max_q;

    int i, j, k;

    // min 5 and max 5 values
    for(i = 0; i < nsens; i++) {
        for(j = start_idx; j < stop_idx; j++) {

            min_q.push(readings[i][j]);
            max_q.push(readings[i][j]);
            
            if(min_q.size() > 5) {

                min_q.pop();
                max_q.pop();
            }
        }
    }

    report += "5 lowest temperatures:  ";
    while(min_q.size() > 0) {
        report += format_float(min_q.top(), 3, 8) + " ";
        min_q.pop();
    }
    report += "\n\n";
    
    report += "5 highest temperatures: ";
    while(max_q.size() > 0) {
        report += format_float(max_q.top(), 3, 8) + " ";
        max_q.pop();
    }
    report += "\n\n";


    // finding largest temp difference in 10 minutes
    double best_low;
    double best_high;
    double best_diff = 0;

    double cur_low;
    double cur_high;
    double cur_diff;

    double val;

    deque<double> queue;
    int qsize = 0;

    for(i = start_idx; i <= stop_idx; i++) {
        qsize = queue.size();

        if( qsize >= cycles_per_10 * 8) {

            for(j = 0; j < qsize; j++) {

                val = queue.at(j);

                if(val < cur_low) {
                    cur_low = val;
                }

                if(val > cur_high) {
                    cur_high = val;
                }
            }

            for(j = 0; j < nsens; j++) {
                queue.pop_front();
            }

            cur_diff = cur_high - cur_low;

            if(cur_diff > best_diff) best_diff = cur_diff;
        }

        for(j = 0; j < nsens; j++) {

            queue.push_back(readings[j][i]);
        }
    }

    report += "Largest 10-minute difference in temperature was " + format_float(best_diff, 3, 0) + " F\n"; 

    report += "---------------------------------------\n\n";
    cout << report;
}