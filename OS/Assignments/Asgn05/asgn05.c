#include "definitions.h"

int main()
{
    node* input = NULL;
    node* schedule = NULL;
    node* summary = NULL;

    int choice = 0;
    int roundLength;
    printf("Enter the max time a round can last: ");
    scanf("%d", &roundLength);

    while(choice != 3)
    {
        printf("\nEnter 1 to create a new job\nEnter 2 to run the created jobs\nEnter 3 to exit without running jobs\n");
        scanf("%d", &choice);
        switch(choice)
        {
            case 1:
                char* username = (char *)malloc(100 * sizeof(char));
                printf("Username: ");
                scanf(" %100s", username);

                char jobID;
                printf("Job ID: ");
                scanf(" %c", &jobID);

                int arrivalTime;
                printf("Arrival Time: ");
                scanf(" %d", &arrivalTime);

                int duration;
                printf("Duration: ");
                scanf(" %d", &duration);
                enqueue(&input, username, jobID, arrivalTime, duration);
                break;

            case 2:
                int timeQuantum = 1;
                int currentJobRoundLength = roundLength;

                while(schedule != NULL || input != NULL)
                {
                    node* temp = input;
                    while(temp != NULL)
                    {
                        if(timeQuantum == temp->arrive)
                        {
                            enqueue(&schedule, temp->user, temp->job, temp->arrive, temp->dur);
                            node* nextNode = temp->next;
                            deleteNode(&input, temp->job);
                            temp = nextNode;

                        }
                        else
                            temp = temp->next;
                    }

                    node* currentJob = schedule;

                    if(currentJob != NULL)
                    {
                        if(currentJobRoundLength > 0 && currentJob->dur > 0)
                        {
                            currentJobRoundLength--;
                            currentJob->dur--;
                            printf("| %d | %c |\n", timeQuantum, currentJob->job);
                        }
                        if(currentJob->dur == 0)
                        {
                            enqueueSummary(&summary, currentJob->user, timeQuantum);
                            dequeue(&schedule);
                            currentJobRoundLength = roundLength;
                        }
                        else if(currentJobRoundLength == 0)
                        {
                            enqueue(&schedule, currentJob->user, currentJob->job, currentJob->arrive, currentJob->dur);
                            dequeue(&schedule);
                            currentJobRoundLength = roundLength;
                        }
                        
                    }
                    else
                        printf("| %d | - |\n", timeQuantum); 

                    timeQuantum++;  
                }

                printSummary(summary);
                stop(&input);
                stop(&schedule);
                stop(&summary);
                return 0;

            case 3:
                stop(&input);
                stop(&schedule);
                stop(&summary);
                return 0;
        }
    }
}
