#include<stdio.h>
#include<sys/time.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#define MAXN 5

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

typedef struct params
{
    int** firstArray;
    int** secondArray;
    int** result;
    int maxThreads;
    int rowIndex;
    int size;
} threadParams;

void multiplyMatrices(void* threadParamsA)
{
    threadParams* t = (threadParams*) threadParamsA;
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
            {
            tempResult = tempResult + t->firstArray[row][i]* t->secondArray[i][column];
            }
        t->result[row][column] = tempResult;
        column = column + 1;
        }
        row = row + 1;
    }
}

void* multiplyMatricesThreaded(void* threadParams){
    /**
     * write a code for matrix multiplication that will utilize the
     * threading capacity and parallelize the computation in such a
     * way that a thread computes result per one or more rows
     */
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
