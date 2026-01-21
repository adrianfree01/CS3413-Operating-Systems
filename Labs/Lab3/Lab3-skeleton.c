#include<stdio.h>
#include<sys/time.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#define MAXN 5
int** generate_square_matrix(int size){
    int** array = malloc(sizeof(int*)*size);
    for (int i = 0 ; i < size; i++){
        array[i] = malloc(sizeof(int)*size);
        memset(array[i],0,sizeof(int)*size);
    }
    return array;
}
int** generate_square_matrix_and_fill_it(int size){
    int** array = generate_square_matrix(size);
    for (int i = 0 ; i < size; i++){
        for (int j = 0 ; j < size; j++){
            array[i][j] = rand()%MAXN;
        }
    }
    return array;
}
void print_square_matrix(int** array, int size){
    for (int i = 0 ; i < size; i++){
        for (int j = 0 ; j < size; j++){
            printf("%i ", array[i][j]);
        }
        printf("\n");
    }
}
int check_if_matrices_differ(int** array, int** array2, int size){
    int result = 0;
    for (int i = 0 ; i < size; i++){
        for (int j = 0 ; j < size; j++){
            result+= array[i][j]-array2[i][j];
            if (result!=0){
                return result;
            }
        }
    }
    return result;
}
typedef struct _params
{
    int** first_array;
    int** second_array;
    int** result;
    int max_threads;
    int row_index;
    int size;
} ThreadParams;

void multiply_matrices(void* threadParams){
    ThreadParams* t = (ThreadParams*) threadParams;
    int N = t->size;
    int row = t->row_index;
    int column = 0;
    int temp_result = 0;
    while(row < N)
    {
        column = 0;
        while(column < N)
        {
        temp_result = 0;
        for(int i = 0; i < N; i++)
            {
            temp_result = temp_result + t->first_array[row][i]* t->second_array[i][column];
            }
        t->result[row][column] = temp_result;
        column = column + 1;
        }
        row = row + 1;
    }
}
void* multiply_matrices_threaded(void* threadParams){
    /**
     * write a code for matrix multiplication that will utilize the
     * threading capacity and parallelize the computation in such a
     * way that a thread computes result per one or more rows
     */
}

int main(int argc, char** argv){
    if (argc != 3){
        printf("Please provide size of the matrix and the number of threads to execute\n");
        exit(0);
    }
    int size = atoi(argv[1]);
    int max_threads = atoi(argv[2]);
    // The value you pass to srand determines the random sequence
    srand(time(NULL)); // Line to initialize the random number generator.
    int** array1 = generate_square_matrix_and_fill_it(size);
    int** array2 = generate_square_matrix_and_fill_it(size);
    int** result = generate_square_matrix(size); // generate an empty matrix
    struct timeval begin;    
    struct timeval end; 
    gettimeofday(&begin,NULL); // fills the contents with time since the beginning of epoch
    ThreadParams* thr = (ThreadParams*)malloc(sizeof(ThreadParams)); // allocate a structure for holding function parameters
    thr->first_array = array1; // first matrix to multiply
    thr->second_array = array2; // the second matrix to multiply
    thr->result = result; // where to store the results - note it needs to be generated
    thr->row_index = 0; // this variable, in combination with max_threads can be used for parallelization
    thr->size = size;
    thr->max_threads = max_threads;
    
    multiply_matrices((void*)thr);

    gettimeofday(&end,NULL); // fills the contents with time since the beginning of epoch
    //The next line is inspired by https://linuxhint.com/gettimeofday_c_language/
    long long microseconds = (end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec);
    double duration = (1.0*microseconds)/1000000;
    printf("Single threaded took %lf seconds to execute \n",duration);
    int** threaded_result = generate_square_matrix(size);
    gettimeofday(&begin,NULL);
    /**
     * Write your code to create and use max_threads here, such that the threaded_result 
     * is populated with the result of the computation.
     */
    gettimeofday(&end,NULL);
    //The next line is inspired by https://linuxhint.com/gettimeofday_c_language/
    microseconds = (end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec);
    duration = (1.0*microseconds)/1000000;
    printf("Multi-threaded took %lf seconds to execute \n",duration);
    
    if (check_if_matrices_differ(result,threaded_result,size)!=0){
        printf("Threaded result differ from single core computation, error\n");
        exit(0);
    }
    return 0;
}