#include<stdio.h>
#include<sys/time.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>

#define MAXN 5

typedef struct params
{
    int** firstArray;
    int** secondArray;
    int** result;
    int maxThreads;
    int rowIndex;
    int size;
} threadParams;

int** generateSquareMatrix(int size){
    int** array = malloc(sizeof(int*)*size);
        for (int i = 0 ; i < size; i++)
        {
            array[i] = malloc(sizeof(int)*size);
            memset(array[i],0,sizeof(int)*size);
        }
    return array;
}

int** generateSquareMatrixFilled(int size){
    int** array = generateSquareMatrix(size);
    for (int i = 0 ; i < size; i++)
    {
        for (int j = 0 ; j < size; j++)
            array[i][j] = rand()%MAXN;
    }
    return array;
}

void printSquareMatrix(int** array, int size)
{
    for (int i = 0 ; i < size; i++)
    {
        for (int j = 0 ; j < size; j++)
            printf("%i ", array[i][j]);
        printf("\n");
    }
}

int doMatricesDiffer(int** array, int** array2, int size){
    int result = 0;
    for (int i = 0 ; i < size; i++)
    {
        for (int j = 0 ; j < size; j++)
        {
            result+= array[i][j]-array2[i][j];
            if (result!=0)
                return result;
        }
    }
    return result;
}

void multiplyMatrices(void* newThreadParams)
{
    threadParams* t = (threadParams*) newThreadParams;
    int N = t->size;
    int row = t->rowIndex;
    int column = 0;
    int tempResult = 0;
    while(row < N)
    {
        column = 0;
        while(column < N)
        {
        tempResult = 0;
        for(int i = 0; i < N; i++)
            tempResult = tempResult + t->firstArray[row][i]* t->secondArray[i][column];
        t->result[row][column] = tempResult;
        column = column + 1;
        }
        row = row + 1;
    }
}

void* multiplyMatricesThreaded(void* newThreadParams)
{
    /**
     * write a code for matrix multiplication that will utilize the
     * threading capacity and parallelize the computation in such a
     * way that a thread computes result per one or more rows
     */

    threadParams* t = (threadParams*) newThreadParams;
    int N = t->size;
    int startRow = t->rowIndex;
    int rowsPerThread = N/t->maxThreads;
    int endRow = startRow + rowsPerThread;

    if(startRow + rowsPerThread * t->maxThreads < N)
        endRow = N;

    int column = 0;
    int tempResult = 0;
    while(startRow < endRow)
    {
        column = 0;
        while(column < N)
        {
            tempResult = 0;
            for(int i = 0; i < N; i++)
                tempResult = tempResult + t->firstArray[startRow][i]* t->secondArray[i][column];
            t->result[startRow][column] = tempResult;
            column = column + 1;
        }
        startRow = startRow + 1;
    }
    pthread_exit(NULL);
}

int main(int argc, char** argv){
    if (argc != 3)
    {
        printf("Please provide size of the matrix and the number of threads to execute\n");
        exit(0);
    }
    
    int size = atoi(argv[1]);
    int maxThreads = atoi(argv[2]);

    // The value you pass to srand determines the random sequence
    srand(time(NULL)); // Line to initialize the random number generator.

    int** array1 = generateSquareMatrixFilled(size);
    int** array2 = generateSquareMatrixFilled(size);
    int** result = generateSquareMatrix(size); // generate an empty matrix

    struct timeval begin;    
    struct timeval end; 

    gettimeofday(&begin,NULL); // fills the contents with time since the beginning of epoch

    threadParams* thr = (threadParams*)malloc(sizeof(threadParams)); // allocate a structure for holding function parameters
    thr->firstArray = array1; // first matrix to multiply
    thr->secondArray = array2; // the second matrix to multiply
    thr->result = result; // where to store the results - note it needs to be generated
    thr->rowIndex = 0; // this variable, in combination with max_threads can be used for parallelization
    thr->size = size;
    thr->maxThreads = maxThreads;
    multiplyMatrices((void*)thr);

    gettimeofday(&end,NULL); // fills the contents with time since the beginning of epoch
    //The next line is inspired by https://linuxhint.com/gettimeofday_c_language/

    long long microseconds = (end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec);
    double duration = (1.0*microseconds)/1000000;

    printf("Single threaded took %lf seconds to execute \n",duration);
    int** threadedResult = generateSquareMatrix(size);


    gettimeofday(&begin,NULL);
    /**
     * Write your code to create and use maxThreads here, such that the threaded_result 
     * is populated with the result of the computation.
     */
    pthread_t* threads = malloc(maxThreads * sizeof(pthread_t));
    threadParams* threadParamsArray = malloc(maxThreads * sizeof(threadParams));
    for(int i = 0; i < maxThreads; i++)
    {
        threadParamsArray[i].firstArray = array1;
        threadParamsArray[i].secondArray = array2;
        threadParamsArray[i].result = threadedResult;
        threadParamsArray[i].maxThreads = maxThreads;
        threadParamsArray[i].rowIndex = i * (size / maxThreads);
        threadParamsArray[i].size = size;

        pthread_create(&threads[i], NULL, multiplyMatricesThreaded, (void*)&threadParamsArray[i]);
    }

    for(int i = 0; i < maxThreads; i++)
        pthread_join(threads[i], NULL);

    gettimeofday(&end,NULL);


    //The next line is inspired by https://linuxhint.com/gettimeofday_c_language/
    microseconds = (end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec);
    duration = (1.0*microseconds)/1000000;

    printf("Multi-threaded took %lf seconds to execute \n",duration);
    
    if (doMatricesDiffer(result,threadedResult,size)!=0)
    {
        printf("Threaded result differ from single core computation, error\n");
        exit(0);
    }
    return 0;
}


/*
Questions:
1. Run the program with the problem size of 1000 and 10 threads, what is the approximate
    speedup you are achieving?
    $ ./lab3 1000 10
        Single threaded took 2.483015 seconds to execute 
        Multi-threaded took 0.476932 seconds to execute
    It ran in approximately 1/5th of the time, so 5x faster.

2. Is there a problem size / number of threads combination that slows down the computation
    process? Why do you think it is happening?
        I couldn't find an exact combination that slows it down, but I noticed that making too many threads for a problem can
        reduce the speed, as the time it takes to actually make said thread outweighs the benefits of parallelism.  As well, if
        we make more threads than what we have cpu cores, we come upon the issue that is known as context switching.

3. What is the minimum size of the problem that benefits from creating an extra thread?
    I noticed around problem size 50 that 2 threads started seeing a smaller time than 1 thread, but it wasn't until a problem size of 60
    that the 2 threads consistently beat the 1 thread.
4. Does using the threads always improve execution duration?
    No, if the problem size is small, it may be more beneficial to use a single thread.

5. Guesstimate and comment on the nature of growth of the speedup with the number of threads
    – is it linear, exponential, are there any limits?
    As I mentioned before, it starts becoming worse if you exceed the number of cores in your
    CPU, but besides that, it seems to be linear at the start, assuming we are using a large problem size
    then, it starts to become inconsistent, and then it stops increasing, before decreasing after our core limit.
    I believe this is where Amdahl's Law comes into play.



    I haven't gotten to reading about Amdahl's Law in the textbook yet, and this is due
    in one hour, so I'm gonna stop there so I can go make supper and watch the hockey game.
*/
