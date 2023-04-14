# Assignment 3 - COP4520 Parallel and Distributed Programming with Juan Parra
### By Ethan Woollet

This repository contains the solutions to the 2 problems proposed in the included PDF. 


# Problem 1: The Birthday Presents Party

This problem essentially just asks to provide a program that performs insertion, deletion, and searching on a concurrent linked list.

The problem asks that 4 threads are used as independent actors on the list, and that each thread perform one of those three actions "in no particular order". Thus, I interpreted that as them being able to be selected as an action randomly, so long as all generated items are put through the list, and in my implementation, I had each thread randomly select an action to perform. If a thread could not complete that action, it would just cycle again and generate a new action to attempt to perform.

As far as the list goes, I implemented a hand-over-hand locking approach to ensure that no thread could attempt to change a part of the list that was being worked on by another thread. All of the actual locking and data management is entirely abstracted to the `ConcurrentSortedList.cpp` file and from the perspective of the runner file (`problem1.cpp`), it could be any implementation.

After doing some empirical testing, my code regularly completes in ~6.5 seconds and leaves no values to be added to the list and no nodes remaining in the list afterwards.

## Run Instructions

To run the solution to problem 1, do

`g++ -o [executable name] problem1.cpp ConcurrentSortedList.cpp`
`./[executable name]`


# Problem 2: Atmospheric Temperature Reading Module

For this problem, I had several instances where a lack of explicit description might have led me to interpreting this question differently from others.

From my understanding, the problem itself specifies that on some Mars rover, there is a temperature measurement unit with 8 sensors and a CPU that supports 8 threads, and that those sensors take measurements every minute and on every hour, the module compiles a summary of the measurements taken in that hour.

Overall, my solution takes slightly longer than the number of 'hours' being simulated times the number of ms per minute used in the simulation. This 'slightly longer' is due to the overhead of threading and the iterations done, which does add up over time, but the iterations themselves seem to always average the correct amount down to the millisecond.

The issues that I have is that there are 2, potentially contentious, decisions that I feel I made when interpreting this problem.

### 1. Synchronicity with a clock cycle.

The description of the problem seems to heavily imply that time is a very integral component, however, it never explicitly states that we must use any clock-based synchronization and the problem of concurrently writing and reading data is not inherently linked to the current system time, but it seems that the scenario that is described is. I took it to mean that in this scenario, we are attempting to implement a time-scaled simulation of the program run by this module, and, following that, I chose to make my program run based on system time and thus actions are taken relative to that time.

There is a variable in `problem2.cpp` called `ms_per_minute` that changes how quickly the simulation is run. If this is changed to be below 30ms, there is a very good chance that the program will act with undefined behavior, but anything above that just makes it so that the execution time takes longer. I also include an empirical measurement of the average time taken for every cycle as measured by the running threads to ensure that they do not have any singificant effects on how frequently the threads are able to take measurements, as that seemed to be a critical aspect of the problem

That said, it does work correctly for any value above 30, and considering the 'real' implementation being simulated takes measurements every minute, this far exceeds the requirement of not preventing proper measurement taking.

### 2. Simulation of an 8-thread CPU

This decision is simple, but I do believe it affects my program's performance. The idea of simulating an 8-thread CPU is simple, but the question is about how it would compile the report at the end of every hour. Since this program is actually written using 9 threads (1 main thread, and 8 spawned ones), my question was whether or not one or all of those 8 threads were intended to compile the report, or if it was fine for the main thread to compile the report. 

In terms of the simulation, I could see it being that there were 8 threads solely responsible for handling the measurement taking and another component of the module performing the summary. However, that is neither stated nor implied, so I chose to instead interpret that one or all of the 8 threads were required to perform the compilation as opposed to assuming something else in the module would do it.

In my implementation, the only difference in the code is that I have the thread with id `0` perform the compilation every hour as opposed to the main thread doing it. But, this means that one of the threads is limited by how fast it can create the report, which is why my program's lower functional bound is 30ms. Any lower, and that thread started to miss readings.

That said, at the scale referenced in the problem, 30ms is still insanely faster than a real world minute, so, again, this would not be an issue.

## Run Instructions

To run the solution to problem 2, do

`g++ -o [executable name] problem2.cpp`
`./[executable name]`
