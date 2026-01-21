#include "definitions.h"

node* createNode(char* username, char jobID, int arrivalTime, int duration, int affinity)
{
    node* newNode = malloc(sizeof(node));
    if (newNode == NULL)
        exit(EXIT_FAILURE);
    newNode->next = NULL;
    newNode->prev = NULL;
    newNode->user = malloc(strlen(username) + 1);
    if(newNode->user == NULL)
    {
        free(newNode);
        exit(EXIT_FAILURE);
    }
    strcpy(newNode->user, username);
    newNode->job = jobID;
    newNode->arrive = arrivalTime;
    newNode->dur = duration;
    newNode->aff = affinity;
    return newNode;
}

void enqueue(node** head, char* username, char jobID, int arrivalTime, int duration, int affinity)
{
    node* newNode = createNode(username, jobID, arrivalTime, duration, affinity);
    if(*head == NULL)
        *head = newNode;
    else
    {
        node* temp = *head;
        while(temp->next != NULL)
            temp = temp->next;
        temp->next = newNode;
        newNode->prev = temp;
    }
}

void dequeue(node** head)
{
    node* temp = *head;
    if(temp == NULL)
        return;
    if(temp->next == NULL)
    {
        *head = NULL;
        free(temp->user);
        free(temp);
    }
    else
    {
        *head = temp->next;
        (*head)->prev = NULL;
        free(temp->user);
        free(temp);
    }
}

void deleteNode(node** head, char jobID)
{
    node* temp = *head;
    while(temp != NULL)
    {
        if(temp->job == jobID)
        {
            if(temp->prev != NULL)
                temp->prev->next = temp->next;
            else
                *head = temp->next;
            if(temp->next != NULL)
                temp->next->prev = temp->prev;
            free(temp->user);
            free(temp);
            return;
        }
        temp = temp->next;
    }
}

void enqueueSummary(node** head, char* username, int finalTime)
{
    node* temp = *head;
    while (temp != NULL)
    {
        if (strcmp(temp->user, username) == 0)
        {
            if (finalTime > temp->dur)
                temp->dur = finalTime;
            return;
        }
        temp = temp->next;
    }

    node* newNode = createNode(username, ' ', 0, finalTime, 0);
    if (*head == NULL)
        *head = newNode;
    else
    {
        node* current = *head;
        node* previous = NULL;
        while (current != NULL && current->dur <= finalTime)
        {
            previous = current;
            current = current->next;
        }
        if (previous == NULL)
        {
            newNode->next = *head;
            (*head)->prev = newNode;
            *head = newNode;
        }
        else
        {
            newNode->next = current;
            newNode->prev = previous;
            previous->next = newNode;
            if (current != NULL)
                current->prev = newNode;
        }
    }
}

void printSummary(node* head)
{
    node* temp = head;
    printf("\nSUMMARY\n");
    while(temp != NULL)
    {
        printf("| %s | %d |\n", temp->user, temp->dur);
        temp = temp->next;
    }
}

void stop(node** head)
{
    node* temp = *head;
    node* next;
    while(temp != NULL)
    {
        next = temp->next;
        free(temp->user);
        free(temp);
        temp = next;
    }
}

void* cpu_thread_function(void* arg)
{
    cpu_args* args = (cpu_args*)arg;
    int cpu_id = args->cpu_id;
    int time_quantum = args->time_quantum;
    node** input_queue = args->input_queue;
    pthread_mutex_t* input_mutex = args->input_mutex;
    pthread_mutex_t* time_mutex = args->time_mutex;
    node* local_schedule = NULL;
    char* shared_buffer = args->shared_buffer;
    int* current_time = args->current_time;
    pthread_mutex_t* counter_mutex = args->counter_mutex;
    pthread_cond_t* counter_cond = args->counter_cond;
    int* counter = args->counter;
    sem_t* semaphores = args->semaphores;
    node** summary = args->summary;
    pthread_mutex_t* summary_mutex = args->summary_mutex;
    int* cpu_threads_alive = args->cpu_threads_alive;
    int* no_more_jobs = args->no_more_jobs;

    int round_length = time_quantum;
    int currentJobRoundLength = 0;
    node* currentJob = NULL;

    while(1)
    {
        // Synchronize current_time
        pthread_mutex_lock(time_mutex);
        int local_current_time = *current_time;
        pthread_mutex_unlock(time_mutex);

        // Check for new arrivals
        pthread_mutex_lock(input_mutex);
        node* temp = *input_queue;
        while(temp != NULL)
        {
            node* nextNode = temp->next;
            if(temp->arrive == local_current_time && temp->aff == cpu_id)
            {
                // Remove temp from input_queue
                if(temp->prev)
                    temp->prev->next = temp->next;
                else
                    *input_queue = temp->next;
                if(temp->next)
                    temp->next->prev = temp->prev;

                // Add temp to local_schedule
                temp->next = NULL;
                temp->prev = NULL;
                if(local_schedule == NULL)
                {
                    local_schedule = temp;
                }
                else
                {
                    node* last = local_schedule;
                    while(last->next != NULL)
                        last = last->next;
                    last->next = temp;
                    temp->prev = last;
                }
            }
            temp = nextNode;
        }
        pthread_mutex_unlock(input_mutex);

        // If no current job, get the next one from the local schedule
        if(currentJob == NULL)
        {
            if(local_schedule != NULL)
            {
                currentJob = local_schedule;
                currentJobRoundLength = round_length;

                // Remove currentJob from local_schedule
                local_schedule = currentJob->next;
                if(local_schedule != NULL)
                    local_schedule->prev = NULL;
                currentJob->next = NULL;
                currentJob->prev = NULL;
            }
        }

        // Process current job
        if(currentJob != NULL)
        {
            shared_buffer[cpu_id] = currentJob->job;
            currentJob->dur--;
            currentJobRoundLength--;

            if(currentJob->dur == 0)
            {
                pthread_mutex_lock(summary_mutex);
                enqueueSummary(summary, currentJob->user, local_current_time);
                pthread_mutex_unlock(summary_mutex);
                free(currentJob->user);
                free(currentJob);
                currentJob = NULL;
                currentJobRoundLength = 0;
            }
            else if(currentJobRoundLength == 0)
            {
                // Re-add currentJob to the end of local_schedule without freeing it
                currentJob->next = NULL;
                currentJob->prev = NULL;
                if(local_schedule == NULL)
                {
                    local_schedule = currentJob;
                }
                else
                {
                    node* last = local_schedule;
                    while(last->next != NULL)
                        last = last->next;
                    last->next = currentJob;
                    currentJob->prev = last;
                }
                currentJob = NULL;
                currentJobRoundLength = 0;
            }
        }
        else
        {
            shared_buffer[cpu_id] = '-';
        }

        // Synchronization with printing thread
        pthread_mutex_lock(counter_mutex);
        (*counter)++;
        if(*counter == args->total_cpus)
            pthread_cond_signal(counter_cond);
        pthread_mutex_unlock(counter_mutex);

        sem_wait(&semaphores[cpu_id]);

        // Check for termination
        pthread_mutex_lock(input_mutex);
        int input_empty = (*input_queue == NULL);
        pthread_mutex_unlock(input_mutex);

        if(currentJob == NULL && local_schedule == NULL && input_empty && *no_more_jobs)
            break;
    }

    cpu_threads_alive[cpu_id] = 0;

    // Final synchronization
    pthread_mutex_lock(counter_mutex);
    (*counter)++;
    if(*counter == args->total_cpus)
        pthread_cond_signal(counter_cond);
    pthread_mutex_unlock(counter_mutex);

    return NULL;
}

void* printing_thread_function(void* arg)
{
    print_args* args = (print_args*)arg;
    char* shared_buffer = args->shared_buffer;
    int* current_time = args->current_time;
    pthread_mutex_t* time_mutex = args->time_mutex;
    int total_cpus = args->total_cpus;
    pthread_mutex_t* counter_mutex = args->counter_mutex;
    pthread_cond_t* counter_cond = args->counter_cond;
    int* counter = args->counter;
    sem_t* semaphores = args->semaphores;
    int* cpu_threads_alive = args->cpu_threads_alive;
    int* no_more_jobs = args->no_more_jobs;
    int max_arrival_time = args->max_arrival_time;
    node** input_queue = args->input_queue;
    pthread_mutex_t* input_mutex = args->input_mutex;

    while(1)
    {
        pthread_mutex_lock(counter_mutex);
        while(*counter < total_cpus)
            pthread_cond_wait(counter_cond, counter_mutex);
        pthread_mutex_unlock(counter_mutex);

        pthread_mutex_lock(time_mutex);
        int local_current_time = *current_time;
        pthread_mutex_unlock(time_mutex);

        // Print current state
        printf("| %d |", local_current_time);
        for(int i = 0; i < total_cpus; i++)
            printf(" %c |", shared_buffer[i]);
        printf("\n");

        pthread_mutex_lock(counter_mutex);
        *counter = 0;
        pthread_mutex_unlock(counter_mutex);

        for(int i = 0; i < total_cpus; i++)
            sem_post(&semaphores[i]);

        pthread_mutex_lock(time_mutex);
        (*current_time)++;
        pthread_mutex_unlock(time_mutex);

        // Check if all jobs have arrived
        if(local_current_time >= max_arrival_time)
        {
            pthread_mutex_lock(input_mutex);
            int input_empty = (*input_queue == NULL);
            pthread_mutex_unlock(input_mutex);
            if(input_empty)
                *no_more_jobs = 1;
        }

        // Check if all CPU threads are done
        int all_done = 1;
        for(int i = 0; i < total_cpus; i++)
        {
            if(cpu_threads_alive[i])
            {
                all_done = 0;
                break;
            }
        }
        if(all_done)
        {
            // Print final idle time
            pthread_mutex_lock(time_mutex);
            local_current_time = *current_time;
            pthread_mutex_unlock(time_mutex);

            printf("| %d |", local_current_time);
            for(int i = 0; i < total_cpus; i++)
                printf(" - |");
            printf("\n");
            break;
        }
    }
    return NULL;
}
