#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct node
{
    struct node* next;
    struct node* prev;
    char* user;
    char job;
    int arrive;
    int dur;
    int aff;
} node;

typedef struct
{
    int cpu_id;
    int time_quantum;
    node** input_queue;
    pthread_mutex_t* input_mutex;
    char* shared_buffer;
    int* current_time;
    pthread_mutex_t* time_mutex;
    pthread_mutex_t* counter_mutex;
    pthread_cond_t* counter_cond;
    int* counter;
    sem_t* semaphores;
    node** summary;
    pthread_mutex_t* summary_mutex;
    int total_cpus;
    int* cpu_threads_alive;
    int* no_more_jobs;
    int max_arrival_time;
} cpu_args;

typedef struct
{
    char* shared_buffer;
    int* current_time;
    pthread_mutex_t* time_mutex;
    int total_cpus;
    pthread_mutex_t* counter_mutex;
    pthread_cond_t* counter_cond;
    int* counter;
    sem_t* semaphores;
    int* cpu_threads_alive;
    int* no_more_jobs;
    int max_arrival_time;
    node** input_queue;
    pthread_mutex_t* input_mutex;
} print_args;

node* createNode(char*, char, int, int, int);
void enqueue(node**, char*, char, int, int, int);
void dequeue(node**);
void deleteNode(node**, char);
void enqueueSummary(node**, char*, int);
void printSummary(node*);
void stop(node**);
void* cpu_thread_function(void*);
void* printing_thread_function(void*);
