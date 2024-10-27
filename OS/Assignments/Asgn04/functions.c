#include "definitions.h"

node* createNode(char* username, char jobID, int arrivalTime, int duration) //create a node
{

    node* newNode = malloc(sizeof(node)); //allocate memory for the new node
    if (newNode == NULL) //if the memory allocation fails...
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    newNode->next = NULL; //this node is currently on its own, doesn't point to any other nodes
    newNode->prev = NULL;
    
    newNode->user = malloc(strlen(username) + 1); //duplicate the string input, so that if the input is changed
                                                //it doesnt effect the node
                                   
    if(newNode->user == NULL) //if the memory allocation fails
    {
        fprintf(stderr, "Error: Memory allocation failed for data.\n");
        free(newNode);
        exit(EXIT_FAILURE);
    }
    strcpy(newNode->user, username);
    newNode->job = jobID;
    newNode->arrive = arrivalTime;
    newNode->dur = duration;
    return newNode;
}


void enqueue(node** head, char* username, char jobID, int arrivalTime, int duration)
{
    node* newNode = createNode(username, jobID, arrivalTime, duration); //create a new node

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

void dequeue(node** head)
{
    node* temp = *head;
    if(temp->next == NULL)
    {
        free(temp->user);
        free(temp->job);
        free(temp->arrive);
        free(temp->dur);
        free(temp);
        return;
    }
    else
    {
        *head = temp->next;
        free(temp->username);
        free(temp->job);
        free(temp->arrive);
        free(temp->dur);
        free(temp);
        return;
    }

}

void deleteNode(node** head, char jobID)
{
    node* temp = *head; //start at head
    while(temp != NULL) //do this until we have iterated through the entire list
    {
        if(temp->job == jobID)//if the data of the current node is the same as the input
        {
            if(temp->prev != NULL) //if we are not at the head
                temp->prev->next = temp->next; //set the previous node's next pointer to the next node
            else
                *head = temp->next; //set the next node to be head
            
            if(temp->next != NULL) //if we are not at the end of the node
                temp->next->prev = temp->prev; //set the next node's previous pointer to be the previous node
            
            free(temp->username);
            free(temp->job);
            free(temp->arrive);
            free(temp->dur);
            free(temp);
            return;
        }
        temp = temp->next; //move to the next node
    }
}

int hasItem(node* head, char* username)
{
    node* temp = head; //node at start of the list
    while(temp != NULL) //do this until we have iterated through the full list
    {
        if(strcmp(temp->user, username) == 0) //is the data of the current node the same as the input
            return 1; //match found
        temp = temp->next; //check next node
    }
    return 0; //no match
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


