#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

typedef struct node
{
    struct node* next;
    struct node* prev;
    char* user;
    char job;
    int arrive;
    int dur;
} node;

node* createNode(char*);

void enqueue(node**, char*); //Add an element to the end of the list

void dequeue(node**, char*); //Removes the first element in the list
                             
void deleteNode(node**, char*); //Remove the first occurence of the element given from the list
                                                     
int hasItem(node*, char*); //returns a 0 or 1, depending on the presence of the given char* in the list
                            
void print(node*); //print the list

int stop(node**); //free all resources and stop reading inputs, terminating the program.




