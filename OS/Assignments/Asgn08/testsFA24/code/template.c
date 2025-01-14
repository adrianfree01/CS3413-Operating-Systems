#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#define MAX_REQUESTS 10000
#define DISK_SIZE 10000
#define SPEED 5.0
#define PENALTY 10.0

typedef struct 
{
    int position;
    int arrivalTime;
} Request;

Request requests[MAX_REQUESTS];
int requestCount = 0;

void fcfs(int start) 
{
    int movement = 0;
    double time = 0.0;
    int currentPosition = start;
    int prevDirection = 1;

    for (int i = 0; i < requestCount; i++) 
    {
        double idleTime = 0.0;

        if (time < requests[i].arrivalTime) 
        {
            idleTime = requests[i].arrivalTime - time;
            time = requests[i].arrivalTime;
        }

        int distance = abs(currentPosition - requests[i].position);
        movement += distance;
        double travelTime = distance / SPEED;

        int newDirection = (requests[i].position - currentPosition) >= 0 ? 1 : -1;

        if (i > 0 && newDirection != prevDirection) 
        {
            time += PENALTY; // Add 10 milliseconds penalty
        }

        time += travelTime;
        currentPosition = requests[i].position;
        prevDirection = newDirection;
    }
    printf("Movement:%d Time:%g\n", movement, time);
}

void cscan(int start) 
{
    int movement = 0;
    double time = 0.0;
    int currentPosition = start;
    int requestsRemaining = requestCount;
    int serviced[MAX_REQUESTS] = {0};
    while (requestsRemaining > 0) 
    {
        int nextRequestIndex = -1;
        int minPosition = DISK_SIZE + 1;
        for (int i = 0; i < requestCount; i++) 
        {
            if (!serviced[i] && requests[i].arrivalTime <= time) 
            {
                if (requests[i].position >= currentPosition && requests[i].position < minPosition) 
                {
                    minPosition = requests[i].position;
                    nextRequestIndex = i;
                }
            }
        }

        if (nextRequestIndex != -1) 
        {
            int distance = requests[nextRequestIndex].position - currentPosition;
            movement += distance;
            double travelTime = distance / SPEED;
            time += travelTime;

            currentPosition = requests[nextRequestIndex].position;
            serviced[nextRequestIndex] = 1;
            requestsRemaining--;
        } 
        else 
        {
            int anyArrivedRequests = 0;
            for (int i = 0; i < requestCount; i++) 
            {
                if (!serviced[i] && requests[i].arrivalTime <= time) 
                {
                    anyArrivedRequests = 1;
                    break;
                }
            }

            if (!anyArrivedRequests) 
            {
                int nextArrivalTime = INT_MAX;
                for (int i = 0; i < requestCount; i++) {
                    if (!serviced[i] && requests[i].arrivalTime > time) 
                    {
                        if (requests[i].arrivalTime < nextArrivalTime)
                            nextArrivalTime = requests[i].arrivalTime;
                    }
                }
                if (nextArrivalTime != INT_MAX)
                    time = nextArrivalTime;
            } 
            else 
            {
                if (currentPosition < DISK_SIZE - 1) 
                {
                    int distance = (DISK_SIZE - 1) - currentPosition;
                    movement += distance;
                    double travelTime = distance / SPEED;
                    time += travelTime;
                    currentPosition = DISK_SIZE - 1;
                }
                movement += 1;
                double wrapAroundTime = 1 / SPEED;
                time += wrapAroundTime;
                currentPosition = 0;
            }
        }
    }
    printf("Movement: %d Time:%g\n", movement, time);
}

int main(int argc, char **argv) {
    int position, time;
    char algorithm = argv[1][0];
    int start = 0;

    while (scanf("%i %i", &position, &time) != EOF) {
        requests[requestCount].position = position;
        requests[requestCount].arrivalTime = time;
        requestCount++;
    }

    if (algorithm == 'F') {
        fcfs(start);
    } else if (algorithm == 'C') {
        cscan(start);
    }

    return 0;
}
