#include "definitions.h"

node* createNode(char* input) //create a node
{

    node* newNode = malloc(sizeof(node)); //allocate memory for the new node
    if (newNode == NULL) //if the memory allocation fails...
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    newNode->next = NULL; //this node is currently on its own, doesn't point to any other nodes
    newNode->prev = NULL;
    
    newNode->data = malloc(strlen(input) + 1); //duplicate the string input, so that if the input is changed
                                                //it doesnt effect the node
                                   
    if(newNode->data == NULL) //if the memory allocation fails
    {
        fprintf(stderr, "Error: Memory allocation failed for data.\n");
        free(newNode);
        exit(EXIT_FAILURE);
    }
    strcpy(newNode->data, input);
    return newNode;
}


void addNode(node** head, char* input)
{
    node* newNode = createNode(input); //create a new node

    if(*head == NULL) //if the list is empty
        *head = newNode; //the new node becomes the head
    else //if the list isn't empty
    {
        node* temp = *head; //make a node that starts at head

        while(temp->next != NULL) //while the node isn't the last node
            temp = temp->next; //move over by one node

        temp->next = newNode; //set the last node to be the new node
        newNode->prev = temp; //point to the 2nd last node from the last node
    }
}


int hasItem(node* head, char* input)
{
    node* temp = head; //node at start of the list
    while(temp != NULL) //do this until we have iterated through the full list
    {
        if(strcmp(temp->data, input) == 0) //is the data of the current node the same as the input
            return 1; //match found
        temp = temp->next; //check next node
    }
    return 0; //no match
}


int countItems(node* head, char* input)
{
    int count = 0; //number of occurences of input
    node* temp = head; //start at head
    while(temp != NULL) //do this until we have iterated through the full list
    {
        if(strcmp(temp->data, input) == 0) //if the data of the current node the same as the input
            count++; //if so, add 1 to the count of occurences
        temp = temp->next; //move to next node
    }
    return count; //return # of occurences found
}


void findAndReplace(node* head, char* oldInput, char* newInput)
{
    node* temp = head; //start at the head
    while(temp != NULL) //do this until we iterate through the entire list
    {
        if(strcmp(temp->data, oldInput) == 0) //if the data of the current node is the same as the input
        {
            free(temp->data); //free the memory taken up by the old data
            temp->data = malloc(strlen(newInput) + 1); //allocate memory for new data
            if(temp->data == NULL) //if memory allocation failed
            {
                fprintf(stderr, "Error: Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }
            strcpy(temp->data, newInput); //copy the new data into the node
            return; 
        }
        temp = temp->next; //move to next node in the case that we didn't find a match for oldInput
    }
}

void deleteNode(node** head, char* input)
{
    node* temp = *head; //start at head
    while(temp != NULL) //do this until we have iterated through the entire list
    {
        if(strcmp(temp->data, input) == 0) //if the data of the current node is the same as the input
        {
            if(temp->prev != NULL) //if we are not at the head
                temp->prev->next = temp->next; //set the previous node's next pointer to the next node
            else
                *head = temp->next; //set the next node to be head
            
            if(temp->next != NULL) //if we are not at the end of the node
                temp->next->prev = temp->prev; //set the next node's previous pointer to be the previous node
            
            free(temp->data); //free the data of the node we are deleting
            free(temp); //free the node
            return;
        }
        temp = temp->next; //move to the next node
    }
}

void deleteCurrentNode(node** head, node* temp) 
{
    if(temp == *head)
        *head = temp->next;
    if(temp->prev != NULL)
        temp->prev->next = temp->next;
    if (temp->next != NULL)
        temp->next->prev = temp->prev;

    free(temp->data);
    free(temp);
}

unsigned long hash(char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c= *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

int removeDuplicates(node* head)
{
    node* temp = head;
    int count = 0;
    unsigned long hashTable[100] =  {0};
    while (temp != NULL)
    {
        unsigned long hashValue = hash(temp->data);
        if(hashTable[hashValue % 100] == 0)
        {
            hashTable[hashValue % 100] = hashValue;
            temp = temp->next;
        }
        else
        {
            node* toDelete = temp;
            temp = temp->next;
            deleteCurrentNode(&head, toDelete);
            count++;
        }   
    }
    return count;
}

void print(node* head)
{
    node* temp = head;
    while(temp != NULL)
    {
        printf("%s\n", temp->data);
        temp = temp->next;
    }
}

int stop(node** head)
{
    node* temp = *head;
    node* next;
    while(temp != NULL)
    {
        next = temp->next;
        free(temp->data);
        free(temp);
        temp = next;
    }
    return 0;
}


