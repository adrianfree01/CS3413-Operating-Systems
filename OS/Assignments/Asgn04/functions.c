#include "definitions.h"

node* createNode(char* username, char jobID, int arrivalTime, int duration)
{

    node* newNode = malloc(sizeof(node));
    if (newNode == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    newNode->next = NULL;
    newNode->prev = NULL;
    
    newNode->user = malloc(strlen(username) + 1);
                                                
                                   
    if(newNode->user == NULL)
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


void enqueueSummary(node** head, char* username, int finalTime)
{
    node* temp = *head;
    
    while (temp != NULL && strcmp(temp->user, username) != 0)
        temp = temp->next;
    

    if (temp != NULL)
    {
        temp->dur = finalTime;

        if (temp->prev != NULL)
            temp->prev->next = temp->next;
        else
            *head = temp->next;
        
        if (temp->next != NULL)
            temp->next->prev = temp->prev;

        node* newPos = *head;
        node* prev = NULL;

        while (newPos != NULL && newPos->dur <= finalTime)
        {
            prev = newPos;
            newPos = newPos->next;
        }

        if (prev == NULL)
        {
            temp->next = *head;
            temp->prev = NULL;
            if (*head != NULL)
                (*head)->prev = temp;
            *head = temp;
        }
        else
        {
            temp->next = newPos;
            temp->prev = prev;
            prev->next = temp;
            if (newPos != NULL)
                newPos->prev = temp;
        }
    }
    else
    {
        node* newNode = createNode(username, ' ', 0, finalTime);
        if (*head == NULL)
        {
            *head = newNode;
        }
        else
        {
            node* current = *head;
            node* previous = NULL;

            while (current != NULL && current->dur <= finalTime)
            {
                previous = current;
                current = current->next;
            }

            if (previous == NULL)
            {
                newNode->next = *head;
                (*head)->prev = newNode;
                *head = newNode;
            }
            else
            {
                newNode->next = current;
                newNode->prev = previous;
                previous->next = newNode;
                if (current != NULL)
                    current->prev = newNode;
            }
        }
    }
}

void enqueue(node** head, char* username, char jobID, int arrivalTime, int duration)
{
    node* newNode = createNode(username, jobID, arrivalTime, duration);

    if(*head == NULL)
        *head = newNode;
    else
    {
        node* temp = *head;

        while(temp->next != NULL)
            temp = temp->next;

        temp->next = newNode;
        newNode->prev = temp;
    }
}

void dequeue(node** head)
{
    node* temp = *head;

    if(temp == NULL)
        return;

    if(temp->next == NULL)
    {
        *head = NULL;
        free(temp->user);
        free(temp);
        return;
    }
    else
    {
        *head = temp->next;
        free(temp->user);
        free(temp);
    }

}

void deleteNode(node** head, char jobID)
{
    node* temp = *head;
    while(temp != NULL)
    {
        if(temp->job == jobID)
        {
            if(temp->prev != NULL)
                temp->prev->next = temp->next; 
            else
                *head = temp->next;
            
            if(temp->next != NULL)
                temp->next->prev = temp->prev; 
            
            free(temp->user);
            free(temp);
            return;
        }
        temp = temp->next;
    }
}

int hasItem(node* head, char* username)
{
    node* temp = head;
    while(temp != NULL)
    {
        if(strcmp(temp->user, username) == 0)
            return 1;
        temp = temp->next;
    }
    return 0;
}



void print(node* head)
{
    node* temp = head;
    
    while(temp != NULL)
    {
        printf("| %s | %c | %d | %d |\n", temp->user, temp->job, temp->arrive, temp->dur);

        temp = temp->next;
    }
}

void printSummary(node* head)
{
    node* temp = head;
    printf("\nSUMMARY\n");
    while(temp != NULL)
    {
        printf("| %s | %d |\n", temp->user, temp->dur);
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
        free(temp->user);
        free(temp);
        temp = next;
    }
    return 0;
}


