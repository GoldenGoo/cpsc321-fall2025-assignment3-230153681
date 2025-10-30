/*
Name: Josh Holuboch
Student Number: 230153681
GitHub Repository: https://github.com/GoldenGoo/cpsc321-fall2025-assignment3-230153681

Objective:
• Extend CPU scheduling algorithms with synchronization techniques.
• Gain hands-on experience with mutexes/semaphore.
• Simulate how multi-core scheduling and synchronization interact in real operating systems.

Instructions:
Simulate a multi-CPU scheduling system using the Shortest Job First (SJF) algorithm. Each
CPU core will be represented by a worker thread. Processes will “arrive” at given times, wait in a 
shared ready queue, and be scheduled according to SJF (non-preemptive). Use synchronization 
primitives (mutex/semaphore) to ensure correct behavior and prevent race conditions.
*/

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>





/*
Example Output

Process: P1 Arrival: 0 Burst: 10 CPU: 0 Waiting Time: 0 Turnaround Time: 10
Process: P2 Arrival: 1 Burst: 5 CPU: 1 Waiting Time: 0 Turnaround Time: 5
Process: P3 Arrival: 2 Burst: 8 CPU: 0 Waiting Time: 8 Turnaround Time: 16
Process: P4 Arrival: 3 Burst: 6 CPU: 1 Waiting Time: 6 Turnaround Time: 12
Process: P5 Arrival: 4 Burst: 3 CPU: 1 Waiting Time: 2 Turnaround Time: 5
Average waiting time = 3.20
Average turnaround time = 9.60
*/
int main() {
    static const int N = 5;
    static const char* names[] = {"P1","P2","P3","P4","P5"}; // the process identifiers
    static const int arrival[] = { 0, 1, 2, 3, 4 }; // the arrival time of each process (in time units).
    static const int burst[] = {10, 5, 8, 6, 3 }; // the burst time (execution time in time units) of each process.

    pthread_t CPU1, CPU2;
}