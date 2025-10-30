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

#define NUM_THREADS 2

typedef struct Node {
    int process_id;
    int arrival_time;
    int burst_time;
    struct Node* next;
} Node;

typedef struct ReadyQueue {
    struct Node* front;
    struct Node* rear;
    pthread_mutex_t mutex;
} ReadyQueue;

// Method to add a node to the ready queue, thread-safe
void enqueue(ReadyQueue* queue, Node* new_node) {
    pthread_mutex_lock(&queue->mutex); // The mutex lock allows only one thread to access the queue at a time.
    if (queue->rear == NULL) {  // If the queue is empty
        queue->front = new_node;
        queue->rear = new_node;
    } else {                    // Else, add to the end of the queue
        queue->rear->next = new_node;
        queue->rear = new_node;
    }
    pthread_mutex_unlock(&queue->mutex); 
}

// Method to remove a node from the ready queue, thread-safe
void dequeue(ReadyQueue* queue) {
    pthread_mutex_lock(&queue->mutex);
    if (queue->front == NULL) {  // If the queue is empty
        pthread_mutex_unlock(&queue->mutex); 
        return NULL;
    }
    Node* temp = queue->front;
    queue->front = queue->front->next;
    if (queue->front == NULL) {  // If the queue is now empty
        queue->rear = NULL;
    }
    pthread_mutex_unlock(&queue->mutex); 
    return temp;
}



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

    // Initialize the nodes, I'll add them to the queue later based on their arrival times.
    Node nodes[N];
    for (int i = 0; i < N; i++) {
        nodes[i].process_id = i;
        nodes[i].arrival_time = arrival[i];
        nodes[i].burst_time = burst[i];
        nodes[i].next = NULL;
    }
    for(int i = 0; i < N-1; i++) {
        nodes[i].next = &nodes[i+1];
    }

    // Initialize the Ready Queue and Mutex
    ReadyQueue ready_queue;
    ready_queue.front = NULL;
    ready_queue.rear = NULL;
    pthread_mutex_init(&ready_queue.mutex, NULL);

    pthread_t threads[NUM_THREADS];
}