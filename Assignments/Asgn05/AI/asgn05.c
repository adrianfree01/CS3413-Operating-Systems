#include "definitions.h"

int main()
{
    node* input = NULL;
    node* summary = NULL;
    int choice = 0;
    int CPUs;
    printf("Enter the number of CPUs to utilize: ");
    scanf("%d", &CPUs);
    int quantums[CPUs];
    for(int i = 0; i < CPUs; i++)
    {
        printf("Enter the quantum for CPU %d: ", i);
        scanf("%d", &quantums[i]);
    }
    while(choice != 3)
    {
        printf("\nEnter 1 to create a new job\nEnter 2 to run the created jobs\nEnter 3 to exit without running jobs\n");
        scanf("%d", &choice);
        switch(choice)
        {
            case 1:
                {
                    char* username = (char *)malloc(100 * sizeof(char));
                    printf("Username: ");
                    scanf(" %99s", username);
                    char jobID;
                    printf("Job ID: ");
                    scanf(" %c", &jobID);
                    int arrivalTime;
                    printf("Arrival Time: ");
                    scanf("%d", &arrivalTime);
                    int duration;
                    printf("Duration: ");
                    scanf("%d", &duration);
                    int affinity;
                    printf("Affinity: ");
                    scanf("%d", &affinity);
                    enqueue(&input, username, jobID, arrivalTime, duration, affinity);
                    break;
                }
            case 2:
                {
                    // Calculate the maximum arrival time
                    int max_arrival_time = 0;
                    node* temp = input;
                    while (temp != NULL)
                    {
                        if (temp->arrive > max_arrival_time)
                            max_arrival_time = temp->arrive;
                        temp = temp->next;
                    }

                    pthread_t cpu_threads[CPUs];
                    pthread_t print_thread;
                    cpu_args args[CPUs];
                    print_args p_args;
                    pthread_mutex_t input_mutex = PTHREAD_MUTEX_INITIALIZER;
                    pthread_mutex_t summary_mutex = PTHREAD_MUTEX_INITIALIZER;
                    pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
                    pthread_mutex_t time_mutex = PTHREAD_MUTEX_INITIALIZER;
                    pthread_cond_t counter_cond = PTHREAD_COND_INITIALIZER;
                    int counter = 0;
                    int current_time = 1;
                    char* shared_buffer = malloc(sizeof(char) * CPUs);
                    sem_t semaphores[CPUs];
                    for(int i = 0; i < CPUs; i++)
                        sem_init(&semaphores[i], 0, 0);
                    int cpu_threads_alive[CPUs];
                    for(int i = 0; i < CPUs; i++)
                        cpu_threads_alive[i] = 1;
                    int no_more_jobs = 0;

                    p_args.shared_buffer = shared_buffer;
                    p_args.current_time = &current_time;
                    p_args.time_mutex = &time_mutex;
                    p_args.total_cpus = CPUs;
                    p_args.counter_mutex = &counter_mutex;
                    p_args.counter_cond = &counter_cond;
                    p_args.counter = &counter;
                    p_args.semaphores = semaphores;
                    p_args.cpu_threads_alive = cpu_threads_alive;
                    p_args.no_more_jobs = &no_more_jobs;
                    p_args.max_arrival_time = max_arrival_time;
                    p_args.input_queue = &input;
                    p_args.input_mutex = &input_mutex;

                    pthread_create(&print_thread, NULL, printing_thread_function, (void*)&p_args);

                    for(int i = 0; i < CPUs; i++)
                    {
                        args[i].cpu_id = i;
                        args[i].time_quantum = quantums[i];
                        args[i].input_queue = &input;
                        args[i].input_mutex = &input_mutex;
                        args[i].shared_buffer = shared_buffer;
                        args[i].current_time = &current_time;
                        args[i].time_mutex = &time_mutex;
                        args[i].counter_mutex = &counter_mutex;
                        args[i].counter_cond = &counter_cond;
                        args[i].counter = &counter;
                        args[i].semaphores = semaphores;
                        args[i].summary = &summary;
                        args[i].summary_mutex = &summary_mutex;
                        args[i].total_cpus = CPUs;
                        args[i].cpu_threads_alive = cpu_threads_alive;
                        args[i].no_more_jobs = &no_more_jobs;
                        args[i].max_arrival_time = max_arrival_time;
                        pthread_create(&cpu_threads[i], NULL, cpu_thread_function, (void*)&args[i]);
                    }

                    for(int i = 0; i < CPUs; i++)
                        pthread_join(cpu_threads[i], NULL);
                    pthread_join(print_thread, NULL);

                    printSummary(summary);
                    stop(&input);
                    stop(&summary);
                    free(shared_buffer);
                    return 0;
                }
            case 3:
                {
                    stop(&input);
                    stop(&summary);
                    return 0;
                }
        }
    }
}
