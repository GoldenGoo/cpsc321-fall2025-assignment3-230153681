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

//Global variables:
bool thread_free[NUM_THREADS];


typedef struct Node{
    int process_id;
    int arrival_time;
    int burst_time;
    int waiting_time;
    bool is_completed;
    struct Node* next;
} Node;

typedef struct ReadyQueue{
    struct Node* front;
    struct Node* rear;
    pthread_mutex_t mutex;
} ReadyQueue;

// Method to add a node to the ready queue, thread-safe
void enqueue(ReadyQueue* queue, Node* new_node){
    pthread_mutex_lock(&queue->mutex); // The mutex lock allows only one thread to access the queue at a time.
    if (queue->rear == NULL){  // If the queue is empty
        queue->front = new_node;
        queue->rear = new_node;
    } 
    else {                    // Else, add to the end of the queue
        queue->rear->next = new_node;
        queue->rear = new_node;
    }
    pthread_mutex_unlock(&queue->mutex); 
}

// Method to remove a node from the ready queue, thread-safe
Node* dequeue(ReadyQueue* queue){
    pthread_mutex_lock(&queue->mutex);
    if (queue->front == NULL){  // If the queue is empty
        pthread_mutex_unlock(&queue->mutex); 
        return NULL;
    }
    Node* temp = queue->front;
    queue->front = queue->front->next;
    if (queue->front == NULL){  // If the queue is now empty
        queue->rear = NULL;
    }
    pthread_mutex_unlock(&queue->mutex); 
    return temp;
}

Node* getNextJob(ReadyQueue* queue){
    pthread_mutex_lock(&queue->mutex);
    if (queue->front == NULL){  // If the queue is empty
        pthread_mutex_unlock(&queue->mutex); 
        return NULL;
    }
    Node *current = queue->front;
    Node *shortest = current;
    while (current != NULL){ // Last node in the queue will point to null
        if (shortest->burst_time > current->burst_time){
            shortest = current;
        }
        current = current->next;
    }
    // Remove shortest from queue
    if (shortest == queue->front){ // Base case before the loop, also handles size 1 queue
        queue->front = queue->front->next;
        if (queue->front == NULL){
            queue->rear = NULL;
        }
    }
    else {
        current = queue->front;
        while (current->next != shortest){
            current = current->next;
        }
        current->next = shortest->next; // Effectively removing shortest
        if (shortest == queue->rear){ // Edge case handling
            queue->rear = current;
        }
    }
    pthread_mutex_unlock(&queue->mutex); 
    return shortest;
}

typedef struct CPUArgs{
    int thread_id;
    int current_time;
    ReadyQueue* queue;
} CPUArgs;

// Worker thread function pretending to do CPU work
// Method should only be called when there is at least one job in the queue
void* doCPUWork(CPUArgs* cpu_args){
    ReadyQueue* queue = cpu_args->queue;
    int current_time = cpu_args->current_time;
    Node *job = getNextJob(queue);

    if (job == NULL){ // Error handling just in case, but this should never happen
        return NULL; 
    }
    sleep(job->burst_time); // Simulate CPU work by sleeping
    
    job->is_completed = true;
    // As long as current_time is correct, this will be correct
    job->waiting_time = current_time - job->arrival_time;

    thread_free[cpu_args->thread_id] = true; // Mark thread as free
    return NULL;
}

bool jobsRemaining(Node* nodes[], int N){
    for (int i = 0; i < N; i++){
        if (!nodes[i]->is_completed){
            return true;
        }
    }
    return false;
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

    // Initialize thread_free array
    for (int i = 0; i < NUM_THREADS; i++){
        thread_free[i] = true;
    }

    // Initialize the nodes, I'll add them to the queue later based on their arrival times.
    Node nodes[N];
    for (int i = 0; i < N; i++){
        nodes[i].process_id = i;
        nodes[i].arrival_time = arrival[i];
        nodes[i].burst_time = burst[i];
        nodes[i].waiting_time = 0; // Initialized to 0, but will be calculated later
        nodes[i].is_completed = false;
        nodes[i].next = NULL;
    }
    for(int i = 0; i < N-1; i++){
        nodes[i].next = &nodes[i+1];
    }

    // Initialize the Ready Queue and Mutex
    ReadyQueue ready_queue;
    ready_queue.front = NULL;
    ready_queue.rear = NULL;
    pthread_mutex_init(&ready_queue.mutex, NULL);

    pthread_t threads[NUM_THREADS];

    /* Im commenting this out, because I am assuming nodes are pre-sorted by arrival time, if they are not
    this would be a simple insertion sort to sort them by arrival time. I also think this is now unneccessary 
    because instead of doing event-driven simulation, I am doing a time driven simulation and checking all 
    jobs at each time unit.

    for (int i = 1; i < N; i++){
        Node key = nodes[i];
        int j = i - 1;
        while (j >= 0 && nodes[j].arrival_time > key.arrival_time) {
            nodes[j + 1] = nodes[j];
            j = j - 1;
        }
        nodes[j + 1] = key;
    }
    */
    
    // Simulation loop
    int jobs_queued = 0;
    int time = 0;
    while (jobs_queued < N &&!jobsRemaining(&nodes, N)){ // jobs_queued is just for short circuiting the check
        for (int i = 0; i < N; i++){
            if(time == nodes[i].arrival_time){
                enqueue(&ready_queue, &nodes[i]);
                jobs_queued++;
            }
        }
        if(ready_queue.front != NULL){
            for (int i = 0; i < NUM_THREADS; i++){
                if (thread_free[i]){
                    // Assign job to free thread
                    thread_free[i] = false; // Mark thread as busy

                    CPUArgs cpu_args = {i, time, &ready_queue};
                    pthread_create(&threads[i], NULL, doCPUWork, &cpu_args);
                    break; // Break as soon as we assign a thread
                }
            }
        }
        time++;
    }

    // Simulation finished, wait for all threads to complete, then do thread cleanup, calculations, and output
    for (int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    float total_waiting_time = 0;
    float total_turnaround_time = 0;
    for (int i = 0; i < N; i++) {
        total_waiting_time += nodes[i].waiting_time;
        total_turnaround_time += nodes[i].waiting_time + nodes[i].burst_time;
    }
    for (int i = 0; i < N; i++) {
        printf("Process: %s, Arrival: %d, Burst: %d, Waiting Time: %d, Turnaround Time: %d\n",
               names[nodes[i].process_id], // Grabs the actual name instead of just the ID
               nodes[i].arrival_time,
               nodes[i].burst_time,
               nodes[i].waiting_time,
               nodes[i].waiting_time + nodes[i].burst_time);
    }

    printf("Average waiting time = %.2f\n", total_waiting_time / N);
    printf("Average turnaround time = %.2f\n", total_turnaround_time / N);

    return 0;
}