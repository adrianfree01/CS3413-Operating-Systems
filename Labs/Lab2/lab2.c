/*
 * Write a program that by using multiple processes converts a user message into a lower-case 
 * message. This program should use the fork () operation two times, to receive the following
 * process tree.  
 *
 *                                     (PARENT)
 *                                         |
 *                                         v
 *                                      (CHILD)
 *                                         |
 *                                         v
 *                                    (GRANDCHILD)
 *
 * Upon creation, each process should print a message with their id. The parent should report its id
 * before invoking fork() for the first time.
 *
 * After the child is created, it should set up a pipe before calling fork(). 
 * 
 * After creating the grandchild process, the child should read a one-word message from the standard input
 * (scanf()), send it to the grandchild, and close the pipe. 
 *
 * The grandchild should print the message it just received, converting all the characters to lowercase, and terminate. 
 *
 * Before termination, each process should print their id again.
 * 
 * The processes should be terminated in the order opposite to the order of their creation.
 * 
 * The parent processes should wait for the child processes to terminate by properly invoking wait().
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

#define R 0         // Read end of pipe
#define W 1         // Write end of pipe
#define BUFFERSIZE 101 // Buffer size for input

void handleChild(int pipefd[]);
void handleGrandchild(int pipefd[]);
void closePipeEnd(int pipefd[], int end);
void toLowerCase(char* str);
void getInput(char** input);

int main()
{
    int p[2];
    pid_t pid;

    printf("Parent PID: %d\n", getpid());

    // Fork to create child process
    if ((pid = fork()) < 0) 
    {
        perror("Fork Failure!");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) 
        handleChild(p);  // Child process logic
    
    // Parent waits for child to finish
    wait(NULL);
    printf("Terminated Parent PID: %d\n", getpid());
    return EXIT_SUCCESS;
}

// Child process: creates a pipe, forks a grandchild, gets input, sends it through the pipe
void handleChild(int p[])
{
    printf("Child PID: %d\n", getpid());

    // Create pipe
    if (pipe(p) < 0) 
    {
        perror("Pipe Failure!");
        exit(EXIT_FAILURE);
    }

    // Fork to create grandchild process
    pid_t grandchildPid;
    if ((grandchildPid = fork()) < 0) 
    {
        perror("Fork Failure!");
        exit(EXIT_FAILURE);
    }

    if (grandchildPid == 0) 
        handleGrandchild(p);  // Grandchild process logic
    

    // Get input from user
    char* input = NULL;
    getInput(&input);

    // Write input to the pipe
    closePipeEnd(p, R);  // Close read end
    if (write(p[W], input, strlen(input) + 1) < 0) 
    {
        perror("Write Failure!");
        free(input);
        exit(EXIT_FAILURE);
    }
    closePipeEnd(p, W);  // Close write end
    free(input);

    // Wait for grandchild to finish
    wait(NULL);
    printf("Terminated Child PID: %d\n", getpid());
    exit(EXIT_SUCCESS);
}

// Grandchild process: reads from the pipe, converts input to lowercase, and prints it
void handleGrandchild(int p[])
{
    printf("Grandchild PID: %d\n", getpid());

    // Close write end and read from pipe
    closePipeEnd(p, W);
    char buffer[BUFFERSIZE];
    if (read(p[R], buffer, BUFFERSIZE) < 0) 
    {
        perror("Read Failure!");
        exit(EXIT_FAILURE);
    }
    closePipeEnd(p, R);

    // Convert message to lowercase and print it
    printf("Message Received: %s\n", buffer);
    toLowerCase(buffer);
    printf("Lowercase Message: %s\n", buffer);

    printf("Terminated Grandchild PID: %d\n", getpid());
    exit(EXIT_SUCCESS);
}

// Convert a string to lowercase
void toLowerCase(char* str)
{
    for (int i = 0; str[i]; i++)
        str[i] = tolower(str[i]);
}

// Close a specific end of the pipe
void closePipeEnd(int p[], int end)
{
    if (close(p[end]) < 0) 
    {
        perror("Pipe Close Failure!");
        exit(EXIT_FAILURE);
    }
}

// Get user input
void getInput(char** input)
{
    *input = (char*)malloc(BUFFERSIZE * sizeof(char));
    if (*input == NULL) 
    {
        perror("Malloc Failed!");
        exit(EXIT_FAILURE);
    }
    scanf("%100s", *input);
}
