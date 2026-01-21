#include<pthread.h>
#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define RANDOM_WITHIN_RANGE(a,b,seed) (rand_r(&seed)%b+a)

int gv = 1;
pthread_mutex_t mutex;
sem_t sem;

void* inc()
{
 // create a function containing a loop that increments gv
    for(int i = 0; i < 100; i++)
        gv++;
}

void* dec()
{
 // create a loop that decrements gv    
    for(int i = 0; i < 100; i++)
        gv--;
}

void* incThousand()
{
 // create a function containing a loop that increments gv
    for(int i = 0; i < 1000; i++)
        gv++;
}

void* decThousand()
{
 // create a loop that decrements gv    
    for(int i = 0; i < 1000; i++)
        gv--;
}

void* incTenThousand()
{
 // create a function containing a loop that increments gv
    for(int i = 0; i < 10000; i++)
        gv++;
}

void* decTenThousand()
{
 // create a loop that decrements gv    
    for(int i = 0; i < 10000; i++)
        gv--;
}

void* incMutexTenThousand()
{
    for(int i = 0; i < 10000; i++)
    {
        pthread_mutex_lock(&mutex);
        gv++;
        pthread_mutex_unlock(&mutex);
    }
}

void* decMutexTenThousand()
{
    for(int i = 0; i < 10000; i++)
    {
        pthread_mutex_lock(&mutex);
        gv--;
        pthread_mutex_unlock(&mutex);
    }
}
void* minus ()
{
    
    for (int i = 0 ; i < 10; i++)
    {
	// insert sem_wait here
        sem_wait(&sem);
        printf("-\n");
    }
    return NULL;
}

void* plus (void* argg)
{
    // insert sem_post somewhere in this function	
    unsigned int seed = *((unsigned int*) argg);
    int interval = RANDOM_WITHIN_RANGE(100000,500000,seed);
    
    for (int i = 0 ; i < 10; i++)
    {
        printf("+\n");
        sem_post(&sem);
        usleep(interval);
    }
    return NULL;
}

void* plusNoSleep ()
{
    // insert sem_post somewhere in this function   
    for (int i = 0 ; i < 10; i++)
    {
        printf("+\n");
        sem_post(&sem);    
    }
    return NULL;
}

void* plusThreeIterations (void* argg)
{
    // insert sem_post somewhere in this function   
    unsigned int seed = *((unsigned int*) argg);
    int interval = RANDOM_WITHIN_RANGE(100000,500000,seed);
    
    for (int i = 0; i < 3; i++)
    {
        printf("+\n");
        sem_post(&sem);
        usleep(interval);
    }
    return NULL;
}

int main(int argc, char** argv)
{
    setvbuf(stdout, NULL, _IONBF,0);
    int loopvar = 0;
    pthread_t thread[4];

// QUESTION 1
    /*  
    *   Write a C program to create two child processes using pthread_create(). Create a global
    *       integer variable (gv initialized to 1) that should be incremented by one 100 times by one
    *       pthread (running the “inc” function) while the second pthread does the same but
    *       decrements the same variable by one (in the “dec”) function. Have the main function
    *       wait for the two pthreads to complete and then print the value of the variable. Do not
    *       forget to join the pthreads.
    */

    for(loopvar = 0; loopvar < 5; loopvar++)
    {
        pthread_create(&thread[0], NULL, inc, NULL);  
        pthread_create(&thread[1], NULL, dec, NULL);

        pthread_join(thread[0], NULL);
        pthread_join(thread[1], NULL);

        printf("gv100 #%d = %d\n", loopvar, gv);
        gv = 1;
    }

// QUESTION 2
    /*
    *   Run your program several times. Do you always see 1 as the result? Explain in the
    *       comments in the file
    *   
    *   Yes, The program does see 1 as the result, I ran the program 5 times by using a for loop.  gv always returns as 1.
    */

// QUESTION 3
    /*
    *   Alter your program for the threads to repeat addition and decrementing 1000 times
    */
    for(loopvar = 0; loopvar < 5; loopvar++)
    {
        pthread_create(&thread[0], NULL, incThousand, NULL);  
        pthread_create(&thread[1], NULL, decThousand, NULL);

        pthread_join(thread[0], NULL);
        pthread_join(thread[1], NULL);

        printf("gv1000 #%d = %d\n", loopvar, gv);
        gv = 1;
    }

// QUESTION 4.  
    /*
    *   Run your program several times. Do you always see 1 as the result? Explain in the
    *       comments in the file.
    *
    *   Same as before, always returns 1.
    */

// QUESTION 5
    /*
    *   Alter your program for the threads to repeat addition and decrementing 10000 times
    */
    for(loopvar = 0; loopvar < 5; loopvar++)
    {
        pthread_create(&thread[0], NULL, incTenThousand, NULL);  
        pthread_create(&thread[1], NULL, decTenThousand, NULL);

        pthread_join(thread[0], NULL);
        pthread_join(thread[1], NULL);

        printf("gv10000 #%d = %d\n", loopvar, gv);
        gv = 1;
    }

// QUESTION 6
    /*
    *   Run your program several times. Do you always see 1 as the result? Explain in the
    *       comments in the file.
    *
    * No, this does not always return 1, as it creates a race condition.
    * A race condition is where both threads access gv at the same time and are not synchronized.
    */

// QUESTION 7
    /*
    *   Modify your program so that your child processes protect the global variable using
    *       pthread_mutex_lock() and pthread_mutex_unlock().
    *           a. You can declare your mutex as a global variable
    *           b. Do not forget to initialize your mutex in the main before using it and destroy it
    *               after you no longer need it. From the manual:
    *                   i. pthread_mutex_init(&mutex, NULL);
    *                   ii. pthread_mutex_destroy(&mutex, NULL);
    */
    for(loopvar = 0; loopvar < 5; loopvar++)
    {
        pthread_mutex_init(&mutex, NULL);
        
        pthread_create(&thread[0], NULL, incMutexTenThousand, NULL);
        pthread_create(&thread[1], NULL, decMutexTenThousand, NULL);
        
        pthread_join(thread[0], NULL);
        pthread_join(thread[1], NULL);
        
        pthread_mutex_destroy(&mutex);

        printf("Mutex gv10000 #%d = %d\n", loopvar, gv);
        gv = 1;
    }

// QUESTION 8
    /*
    *   Run your program several times. Do you always see 1 as the result? Explain in the
    *       comments in the file.
    *
    *   Yes, I am always seeing 1 as the result.  As there is now no longer a race condition
    *   due to the mutex lock and unlocks
    */

// QUESTION 9
    /*
    *   The second part of your task for the day is to create threads for the “plus” and “minus”
    *       functions. This should be done in a way that makes sure that + and – are always
    *       alternating. You need to use the semaphores for that. Recall that sem_wait() and
    *       sem_post() functions can be used for ensuring the execution order between statements.
    *       Hint: Initialize the value of semaphore to 0; and do not forget to join the two threads
    */
    for(loopvar = 0; loopvar < 5; loopvar++)
    {
        sem_init(&sem, 0, 0);

        unsigned int* t = (unsigned int *) malloc(sizeof(unsigned int));
        *t = rand();

        printf("usleep Attempt # %d\n", loopvar);
        pthread_create(&thread[2], NULL, plus, (void*)t);
        pthread_create(&thread[3], NULL, minus, NULL);

        pthread_join(thread[2], NULL);
        pthread_join(thread[3], NULL);

        sem_destroy(&sem);
    }

// QUESTION 10
    /*
    *   Run the program multiple times to make sure it always generates the same output.
    *
    *   It seems to always print the same + - + - + - + -, but at different speeds.
    */

// QUESTION 11
    /*
    *   Comment out the usleep() statement in the plus function and try running the program
    *       multiple times. Observe the output, and recall that sleep “helps” synchronization, as
    *       nothing prevents the plus function from signaling the semaphore repeatedly. Note that 
    *       relying on sleep for synchronization is incorrect, as it introduces delays that potentially
    *       can be avoided.
    */
    for(loopvar = 0; loopvar < 5; loopvar++)
    {
        sem_init(&sem, 0, 0);

        printf("No usleep Attempt # %d\n", loopvar);
        pthread_create(&thread[2], NULL, plusNoSleep, NULL);
        pthread_create(&thread[3], NULL, minus, NULL);

        pthread_join(thread[2], NULL);
        pthread_join(thread[3], NULL);

        sem_destroy(&sem);
    }
    /*
    *   without usleep, this returns ++++++++++----------
    */

// QUESTION 12
    /*
    *   Uncomment the usleep statement. Reduce the maximum number of iterations of a loop in
    *       the plus function to three
    */
    for(loopvar = 0; loopvar < 5; loopvar++)
    {
        sem_init(&sem, 0, 0);

        unsigned int* t = (unsigned int *) malloc(sizeof(unsigned int));
        *t = rand();

        printf("3 Iterations of Plus Attempt # %d\n", loopvar);
        pthread_create(&thread[2], NULL, plusThreeIterations, (void*)t);
        pthread_create(&thread[3], NULL, minus, NULL);

        pthread_join(thread[2], NULL);
        pthread_join(thread[3], NULL);

        sem_destroy(&sem);
    }
    return 0;
}

// QUESTION 13
    /*
    *   Compile and run your program, explain in the comments in the file what happens.
    *
    *  Compiling with only 3 iterations of plus stops the minus function from executing 
    *   as it is stuck waiting for plus to give it another frame to run.
    *   The function is stuck until you exit manually.
    */