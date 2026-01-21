#include<pthread.h>
#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define RANDOM_WITHIN_RANGE(a,b,seed) (rand_r(&seed)%b+a)
/* This section is to be uncommented and the statements to be moved to the 
appropriate place where they belong
#include <semaphore.h>
pthread_mutex_init(&mutex, NULL);
sem_t sem;
pthread_mutex_t mutex;
int gv = 1;
pthread_mutex_lock(&mutex);
pthread_mutex_unlock(&mutex);
pthread_mutex_destroy(&mutex);
sem_wait(&sem);		
sem_post(&sem);		
sem_init(&sem,0,0);
*/


void* inc(){
 // create a function containing a loop that increments gv
}

void* dec(){
 // create a loop that decrements gv    
}

void* minus (){
    int i =0 ;
    for (i = 0 ; i < 10; i++){
	// insert sem_wait here
        printf("-");
    }
    return NULL;
}

void* plus (void* argg){
    // insert sem_post somewhere in this function	
    unsigned int seed = *((unsigned int*) argg);
    int interval = RANDOM_WITHIN_RANGE(100000,500000,seed);
    int i =0 ;

    for (i = 0 ; i < 10; i++){


        printf("+");
        usleep(interval);
    }
    return NULL;
}

int main(int argc, char** argv){
    setvbuf(stdout, NULL, _IONBF,0);
    int i = 0;
    pthread_t thread[4];
//  parts 1-8
//   for (i = 0 ; i < 1; i++){
//         unsigned int* t = (unsigned int *) malloc(sizeof(unsigned int));
//         *t = rand();
        
//         pthread_create(&(thread[i]), NULL, &inc,(void*)t);
//         pthread_create(&(thread[i+1]), NULL, &dec,NULL);
//     }    

//   pthread_join(thread[0]);
//   pthread_join(thread[1]);

// part 9-13
//   for (i = 2 ; i < 3; i++){
//         unsigned int* t = (unsigned int *) malloc(sizeof(unsigned int));
//         *t = rand();
        
//         pthread_create(&(thread[i]), NULL, &plus,(void*)t);
//         pthread_create(&(thread[i+1]), NULL, &minus,NULL);
//     }

//   pthread_join(thread[2]);
//   pthread_join(thread[3]);
    return 0;
}