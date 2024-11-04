#include "definitions.h"

int main()
{
    node* input = NULL;
    node* schedule = NULL;
    node* summary = NULL;

    int choice = 0;
    int roundLength;
    printf("Enter the max time a round can last: ");
    scanf("%d", &roundLength);
    int maxCPUs;
    printf("Enter the maximum amount of CPUs you would like to use: ");
    scanf("%d", &maxCPUs);
    int shared_buffer[maxCPUs];

    pthread_mutex_t queueMutex;
    sem_t bufferReady;
    sem_t printReady;



    while(choice != 3)
    {
        printf("\nEnter 1 to create a new job\nEnter 2 to run the created jobs\nEnter 3 to exit without running jobs\n");
        scanf("%d", &choice);
        switch(choice)
        {
            case 1:
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
                enqueue(&input, username, jobID, arrivalTime, duration);
                break;

            case 2:
                int timeQuantum = 1;
                int currentJobRoundLength = roundLength;

                while(schedule != NULL || input != NULL)
                {
                    node* temp = input;
                    while(temp != NULL)
                    {
                        if(timeQuantum == temp->arrive)
                        {
                            enqueue(&schedule, temp->user, temp->job, temp->arrive, temp->dur);
                            node* nextNode = temp->next;
                            deleteNode(&input, temp->job);
                            temp = nextNode;

                        }
                        else
                            temp = temp->next;
                    }

                    node* currentJob = schedule;

                    if(currentJob != NULL)
                    {
                        if(currentJobRoundLength > 0 && currentJob->dur > 0)
                        {
                            currentJobRoundLength--;
                            currentJob->dur--;
                            printf("| %d | %c |\n", timeQuantum, currentJob->job);
                        }
                        if(currentJob->dur == 0)
                        {
                            enqueueSummary(&summary, currentJob->user, timeQuantum);
                            dequeue(&schedule);
                            currentJobRoundLength = roundLength;
                        }
                        else if(currentJobRoundLength == 0)
                        {
                            enqueue(&schedule, currentJob->user, currentJob->job, currentJob->arrive, currentJob->dur);
                            dequeue(&schedule);
                            currentJobRoundLength = roundLength;
                        }
                        
                    }
                    else
                        printf("| %d | - |\n", timeQuantum); 

                    timeQuantum++;  
                }

                printSummary(summary);
                stop(&input);
                stop(&schedule);
                stop(&summary);
                return 0;

            case 3:
                stop(&input);
                stop(&schedule);
                stop(&summary);
                return 0;
        }
    }
}

/*
*   Task
*       In this assignment you’re to extend your simulation of the Round Robin
*       scheduling algorithm with multiple threads. The threads are to simulate 
*       multiple CPUs having their own schedules. To assign a job to a CPU, you need to
*       use the processor affinity - a new field in a job’s description.
*
*   Requirements
*       First, your program should read the number of CPUs. After that, your program
*       should read the inputs, very similar to the previous assignment, except there’s a
*       new field: Affinity. This field is represented by an integer value and corresponds
*       to the ID of the (simulated) CPU, which should be executing the job. The jobs
*       with affinity 0 can only be executed by the CPU0, the Jobs with affinity 1 can
*       only be executed by the CPU1, etc.
*
*       Your program should have three kinds of threads:
*           The main thread, which has several responsibilities:
*               Reading the input
*               Creating the CPU threads and a printing thread
*               Printing the summary
*
*           CPU thread(s) - threads that simulate one core of CPU each. You will
*       need to make your entire simulation from the previous assignment solution
*       to run as a separate thread, using pthread_create(). The number of the
*       threads should correspond to the number of the CPU cores, which is a
*       user input. Each of the CPUs can have a local running queue, but should
*       compete for access to the “input queue” with another CPUs. Every unit
*       of time within the simulation, the CPUs should send the jobs they were
*       executing to a buffer shared with the printing thread.
*
*           Finally, your program should have a printing thread, also created using
*       pthread_create. The thread will be executing a function that is responsible
*       for printing the time and jobs currently performed by each CPU. This
*       function should wait for all CPU threads to finish simulating one unit of
*       execution, and print the contents of the shared buffer, populated by the
*       CPUs. After that, the printing thread should signal the CPU threads
*       that they can resume their simulations. Importantly, this thread should
*       be created before the CPU threads.
*       Finally, a summary should be printed following the same rules as in the
*       previous assignment.
*
*   Limitations
*       For this exercise, you are prohibited from allocating a 2d array that would store
*       history of CPU states. The shared buffer (can be a global variable) should be
*       only big enough to hold one character per CPU. You are to use semaphores and
*       mutexes, not pipes. Solutions using busy wait loops will be lightly penalized
*       for each busy wait loop. Solutions using sleep for synchronization will lightly
*       be penalized for each sleep statement in the code.
*
*   Working solution for the previous assignment
*       If you did not manage to finish the previous assignment or your design choices
*       make the change too difficult, instructor’s solution to the previous assignment
*       will be posted on Thursday (29th of October) evening, 
*       under Course Content/Misc. useful stuff.
*
*   Input/Output format
*       The assignment will contain two lines before the processes table: the number of
*       CPUs, and their corresponding quantums. See the attached sample input1.txt
*       and sample output1.txt for clarifications.
*/