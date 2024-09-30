#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define R 0
#define W 1
#define MAX_ARGS 6

int setInputNum(char* input);
void spawnProcesses(int depth, int number);
int highestSetBit(int n); 
void createProcess(int number, int maxBit);

int main()
{
    char* input = (char*)malloc(100 * sizeof(char)); //input allocation
    if(input == NULL)
    {
        perror("Memory Allocation Failed!");
        free(input);
        exit(EXIT_FAILURE);
    }
    int inputNum = 0;
    while(inputNum != 5)
    {
        printf("(%d) Enter a command: ", getpid()); //Enter a command
        scanf("%s", input); //scan input and assign
        inputNum = setInputNum(input); //function to clean code up a bit

        switch(inputNum) 
        {
            case 0:
                printf("(%d) Invalid Command, Try Again\n", getpid()); //womp womp
                break;
            case 1:
                printf("(%d) Parent PID: %d\n", getpid(), getpid()); //make a child, how cute
                if (fork() == 0)
                {
                    printf("(%d) Child PID: %d\n", getpid(), getpid());
                    exit(EXIT_SUCCESS);
                }
                wait(NULL);
                break;
            case 2:
                int p[2], pT[2]; //pipe for number and total
                if(pipe(p) < 0 || pipe(pT) < 0)
                {
                    perror("Pipe Failure!");
                    free(input);
                    exit(EXIT_FAILURE);
                    
                }
                int total = 0;
                int num = 0;
                int count = 0;
                if (fork() == 0)
                {
                    close(p[W]);
                    close(pT[R]);
                    int numFork = -1;
                    while(numFork != 0)
                    {
                        read(p[R], &numFork, sizeof(int));
                        total += numFork;
                        write(pT[W], &total, sizeof(int));
                    }
                    close/
                    close(p[R]);
                    close(pT[W]);
                    exit(EXIT_SUCCESS);
                }
                else
                {
                    
                    while(num != 0 || count == 0) //I misread the instructions, so I print the total every time.
                    {
                        count++; //count is literally just used to make sure this runs once, because I'm silly
                        if(count > 1) //if I didn't do this, we'd be stuck
                            read(pT[R], &total, sizeof(int));
                        printf("(%d) (TOTAL: %d) Enter a number to add to the total: ", getpid(), total); 
                        scanf("%d", &num); 
                        write(p[W], &num, sizeof(int));
                    }
                    close(pT[W]);
                    close(p[W]);
                    close(p[R]);
                    wait(NULL);
                    read(pT[R], &total, sizeof(int));
                    close(pT[R]);
                    printf("(%d) The total is: %d\n", getpid(), total);
                }
                break;
            case 3:
                char** args = (char **)malloc(MAX_ARGS * sizeof(char *)); //honestly hated doing this one
                if (args == NULL)
                {
                    perror("Memory Allocation Failed!");
                    free(input);
                    free(args); 
                    exit(EXIT_FAILURE);
                    
                }
                for(int i = 0; i < MAX_ARGS; i++) //assign all args to be NULL
                    args[i] = NULL;

                char* buff = (char *)(malloc(100 * sizeof(char))); //more allocation :(
                if(buff == NULL)
                {
                    perror("Memory Allocation Failed!");
                    free(input);
                    free(buff);
                    free(args); 
                    exit(EXIT_FAILURE);
                    
                }
                int argCount = 0;
                printf("(%d) Enter a command, followed by arguments (End with ';')\nCommand: ", getpid()); //one at a time
                while(argCount < MAX_ARGS - 1)
                {
                    scanf("%s", buff);
                    if(strcmp(buff, ";") == 0)
                        break;

                    args[argCount] = (char *)malloc(strlen(buff) + 1); //MORE ALLOCATION WEEEEE
                    if(args[argCount] == NULL) 
                    {
                        perror("Memory Allocation Failed");
                        free(input);
                        for(int i = 0; i < MAX_ARGS; i++)
                            free(args[i]);
                        free(args); 
                        exit(EXIT_FAILURE);
                    }
                    strcpy(args[argCount], buff);
                    argCount++;
                    if(argCount != 5)
                        printf("(%d) Argument: ", getpid());
                }
                if(fork() == 0)
                {
                    execvp(args[0], args);
                    perror("EXECVP Failed!");
                    free(input);
                    for(int i = 0; i < MAX_ARGS; i++)
                        free(args[i]);
                    free(args); 
                    exit(EXIT_FAILURE);
                }
                wait(NULL);
                printf("(%d) Child Process Finished.\n", getpid());

                for(int i = 0; i < argCount; i++)
                    free(args[i]);
                free(args);
                free(buff);
                break;
            case 4:
                int procNum;
                scanf("%d", &procNum);
                int totalProcNum = (1 << procNum) - 1;
                int hsb = highestSetBit(totalProcNum);
                createProcess(totalProcNum, hsb);
                break;
            case 5:
                free(input);
                break;
        }
    }
}

int highestSetBit(int n) {
    for(int i = sizeof(int)*8 - 1; i >= 0; i--) 
    {
        if(n & (1 << i)) 
            return i;
    }
    return -1;
}


void createProcess(int number, int maxBit) 
{
    printf("(%d) I am a child of %d, number is %d\n", getpid(), getppid(), number);

    if(number == 0 || maxBit < 0) return; // Base case

    for(int bit = maxBit; bit >= 0; bit--) 
    {
        if(number & (1 << bit)) 
        {
            int childNumber = number - (1 << bit);
            pid_t childPid = fork();
            if(childPid == 0) 
            {
                // Child process
                createProcess(childNumber, bit - 1);
                exit(EXIT_SUCCESS);
            } 
            else if (childPid > 0) 
            {
                // Parent continues to create other children
            } 
            else 
            {
                perror("Fork Failure!");
                free(input);
                exit(EXIT_FAILURE);
            }
        }
    }

    // Parent waits for all child processes to finish
    while(wait(NULL) > 0);
}

int setInputNum(char* input) //set the input number for the switch case
{
    int inputNum = 0;
    if(strcmp(input, "onechild") == 0)
        inputNum = 1;
    else if(strcmp(input, "addnumbers") == 0)
        inputNum = 2;
    else if(strcmp(input, "exec") == 0)
        inputNum = 3;
    else if(strcmp(input, "processtree") == 0)
        inputNum = 4;
    else if(strcmp(input, "stop") == 0)
        inputNum = 5;
    else
        inputNum = 0;

    return inputNum;
}
