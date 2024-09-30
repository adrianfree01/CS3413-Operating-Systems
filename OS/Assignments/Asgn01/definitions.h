#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
typedef struct node
{
    struct node* next;
    struct node* prev;
    char* data;
} node;

void addNode(node**, char*); //Add an element to the end of the list
                             
void deleteNode(node**, char*); //Remove the first occurence of the element given from the list
                                
int countItems(node*, char*); //Print the number of exact matches in the list of the given element
                              
int removeDuplicates(node*); //Remove all nodes that contain duplicate words from the list
                             
int hasItem(node*, char*); //returns a 0 or 1, depending on the presence of the given char* in the list
                            
void findAndReplace(node*, char*, char*); //find the first occurence of an element and replace it with a new value

void print(node*); //print the list

int stop(node**); //free all resources and stop reading inputs, terminating the program.



void deleteCurrentNode(node**, node*);

unsigned long hash(char*);

node* createNode(char*);

