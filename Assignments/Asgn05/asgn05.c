#include "definitions.h"

int CPUs;
int finishedCPUs = 0;
int currentTime = 1;  
pthread_mutex_t mutex;
sem_t *sems;
sem_t printSem;
node *inputQueue = NULL;
node **cpuQueues;
char *sharedBuffer;
int *quantums;  
node *summary = NULL;

void addToSchedule(node** inputHead, node** scheduleHead, int timeQuantum) {
    node* temp = *inputHead;
    while(temp != NULL) {
        if(timeQuantum == temp->arrive) {
            enqueue(scheduleHead, temp->user, temp->job, temp->arrive, temp->dur, temp->aff);
            node* nextNode = temp->next;
            deleteNode(inputHead, temp->job);
            temp = nextNode;
        } else {
            temp = temp->next;
        }
    }
}

node* getJob(node** scheduleHead, int affinity) {
    node* temp = *scheduleHead;
    while(temp != NULL) {
        if(temp->aff == affinity) return temp;
        temp = temp->next;
    }
    return NULL;
}

void getInputs(node** inputHead) {
    char* username = (char *)malloc(100 * sizeof(char));
    printf("Username: ");
    scanf(" %100s", username);

    char jobID;
    printf("Job ID: ");
    scanf(" %c", &jobID);

    int arrivalTime;
    printf("Arrival Time: ");
    scanf(" %d", &arrivalTime);

    int duration;
    printf("Duration: ");
    scanf(" %d", &duration);

    int affinity;
    printf("Affinity: ");
    scanf(" %d", &affinity);

    enqueue(inputHead, username, jobID, arrivalTime, duration, affinity);
}

void* simulateCPU(void* arg) {
    int cpuID = *(int*)arg;
    int timeQuantum = quantums[cpuID];
    int currentJobTimeRemaining = timeQuantum;
    int hasFinished = 0;

    while (1) {
        sem_wait(&sems[cpuID]);
        printf("CPU %d acquired semaphore at time %d.\n", cpuID, currentTime);

        pthread_mutex_lock(&mutex);
        printf("CPU %d checking schedule at time %d.\n", cpuID, currentTime);
        addToSchedule(&inputQueue, &cpuQueues[cpuID], currentTime);
        pthread_mutex_unlock(&mutex);

        node* currentJob = getJob(&cpuQueues[cpuID], cpuID);

        if (currentJob != NULL) {
            currentJobTimeRemaining--;
            currentJob->dur--;

            pthread_mutex_lock(&mutex);
            sharedBuffer[cpuID] = currentJob->job;
            printf("CPU %d is processing job %c at time %d with %d remaining.\n", cpuID, currentJob->job, currentTime, currentJob->dur);
            pthread_mutex_unlock(&mutex);

            if (currentJob->dur == 0) {
                enqueueSummary(&summary, currentJob->user, currentTime);
                dequeue(&cpuQueues[cpuID]);
                currentJobTimeRemaining = timeQuantum;
            } else if (currentJobTimeRemaining == 0) {
                enqueue(&cpuQueues[cpuID], currentJob->user, currentJob->job, currentJob->arrive, currentJob->dur, currentJob->aff);
                dequeue(&cpuQueues[cpuID]);
                currentJobTimeRemaining = timeQuantum;
            }
        } else {
            pthread_mutex_lock(&mutex);
            sharedBuffer[cpuID] = '-';
            pthread_mutex_unlock(&mutex);
        }

        sem_post(&printSem);
        printf("CPU %d posted to printSem at time %d.\n", cpuID, currentTime);

        pthread_mutex_lock(&mutex);
        if (cpuQueues[cpuID] == NULL && inputQueue == NULL && !hasFinished) {
            printf("CPU %d has finished all jobs.\n", cpuID);
            finishedCPUs++;
            hasFinished = 1;
        }
        pthread_mutex_unlock(&mutex);

        if (hasFinished) break;
    }
    return NULL;
}

void* printStatus(void* arg) {
    while (1) {
        for (int i = 0; i < CPUs; i++) {
            sem_wait(&printSem);
        }

        pthread_mutex_lock(&mutex);
        printf("Time %d |", currentTime);
        for (int i = 0; i < CPUs; i++) {
            printf(" CPU%d: %c |", i, sharedBuffer[i]);
        }
        printf("\n");
        pthread_mutex_unlock(&mutex);

        currentTime++;
        printf("Advancing to time %d.\n", currentTime);

        for (int i = 0; i < CPUs; i++) {
            sem_post(&sems[i]);
        }

        pthread_mutex_lock(&mutex);
        if (finishedCPUs == CPUs) {
            printf("All CPUs have finished. Exiting printStatus.\n");
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    printf("Enter the number of CPUs to utilize: ");
    scanf("%d", &CPUs);

    sems = (sem_t*) malloc(CPUs * sizeof(sem_t));
    for(int i = 0; i < CPUs; i++)
        sem_init(&sems[i], 0, 0);
    sem_init(&printSem, 0, 0);

    pthread_mutex_init(&mutex, NULL);

    cpuQueues = (node**) malloc(CPUs * sizeof(node*));
    for(int i = 0; i < CPUs; i++)
        cpuQueues[i] = NULL;

    sharedBuffer = (char*) malloc(CPUs * sizeof(char));
    for(int i = 0; i < CPUs; i++)
        sharedBuffer[i] = '-';

    quantums = (int*) malloc(CPUs * sizeof(int));
    for(int i = 0; i < CPUs; i++) {
        printf("Enter the quantum for CPU %d: ", i);
        scanf(" %d", &quantums[i]);
    }

    int choice = 0;
    while(choice != 3) {
        printf("\nEnter 1 to create a new job\nEnter 2 to run the created jobs\nEnter 3 to exit without running jobs\n");
        scanf("%d", &choice);
        switch(choice) {
            case 1:
                getInputs(&inputQueue);
                break;

            case 2: {
                pthread_t cpuThreads[CPUs];
                pthread_t printThread;

                pthread_create(&printThread, NULL, printStatus, NULL);

                int cpuIDs[CPUs];
                for (int i = 0; i < CPUs; i++) {
                    cpuIDs[i] = i;
                    pthread_create(&cpuThreads[i], NULL, simulateCPU, &cpuIDs[i]);
                }

                pthread_join(printThread, NULL);
                for (int i = 0; i < CPUs; i++) 
                    pthread_join(cpuThreads[i], NULL);
                
                printSummary(summary);

                stop(&inputQueue);
                stop(&summary);
                for (int i = 0; i < CPUs; i++) 
                    sem_destroy(&sems[i]);
                sem_destroy(&printSem);
                free(sems);
                free(cpuQueues);
                free(sharedBuffer);
                free(quantums);
                pthread_mutex_destroy(&mutex);
                return 0;
            }

            case 3:
                stop(&inputQueue);
                stop(&summary);
                for (int i = 0; i < CPUs; i++)
                    sem_destroy(&sems[i]);
                sem_destroy(&printSem);
                free(sems);
                free(cpuQueues);
                free(sharedBuffer);
                free(quantums);
                pthread_mutex_destroy(&mutex);
                return 0;
        }
    }
}