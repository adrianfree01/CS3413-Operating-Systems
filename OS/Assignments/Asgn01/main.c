#include "definitions.h"

	
int main()
{
    char choice;
    int buffSize = 101;
    char* buffer = (char *)malloc(buffSize * sizeof(char));
    node* head = NULL;
    while(choice != 's')
    {
        scanf("%c", &choice);
        
        switch(choice)
        {
            case 'a':
                scanf("%100s", buffer);
                addNode(&head, buffer);
                break;

            case 'c':
                scanf("%100s", buffer);
                int itemCount = countItems(head, buffer);
                printf("%d\n", itemCount);
                break;

            case 'd':
                scanf("%100s", buffer);
                deleteNode(&head, buffer);
                break;

            case 'f':
                scanf("%100s", buffer);
                char* bufferNew = (char *)malloc(buffSize * sizeof(char));
                scanf("%100s", bufferNew);
                findAndReplace(head, buffer, bufferNew);
                free(bufferNew);
                break;

            case 'h':
                scanf("%100s", buffer);
                int listHasItem = hasItem(head, buffer);
                printf("%d\n", listHasItem);
                break;
            
            case 'r':
                int dupeAmount = removeDuplicates(head);
                printf("%d\n", dupeAmount);
                break;

            case 'p':
                print(head);
                break;

            case 's':
                stop(&head);
                break;
        }
    }

    free(buffer);
}


