#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define BOXES_PER_DWELLER 5
#define TRUCK_ROOM 10
#define MIN_WEIGHT 5
#define MAX_WEIGHT 50
#define MIN_DWELLER_TIME 1
#define MAX_DWELLER_TIME 7
#define MIN_MOVER_TIME 1
#define MAX_MOVER_TIME 3
#define MIN_TRUCKER_TIME 1
#define MAX_TRUCKER_TIME 3
#define MIN_TRIP_TIME 5
#define MAX_TRIP_TIME 10

#define RANDOM(a,b, seed) (a+rand_r(&seed)%(b-a))

#define R 0
#define W 1

int dwellerToMover[2];
int moverToTrucker[2];

typedef struct params
{
	int id;
	int randomSeed;
} threadParams;

typedef struct box 
{
	int weight;
} movingBox;

typedef struct truck
{
	int room;
	int totalWeight;
} movingTruck;


void* dwellerFunction(void* param)
{
	threadParams* params = (threadParams*) param;
	printf("Hello from house dweller %d\n", params->id);
	int timeToPack;
	movingBox *box = malloc(sizeof(movingBox));
	for(int i = 0; i < BOXES_PER_DWELLER; i++)
	{
		timeToPack = RANDOM(MIN_DWELLER_TIME, MAX_DWELLER_TIME, params->randomSeed);
		sleep(timeToPack);
		box->weight = RANDOM(MIN_WEIGHT, MAX_WEIGHT, params->randomSeed);
		write(dwellerToMover[W], box, sizeof(movingBox));
		printf("House dweller %d created a box that weighs %d in %d units of time\n", params->id, box->weight, timeToPack); 
		//House dweller 1 created a box that weighs 17 in 5 units of time
	}
	printf("House dweller %d is done packing!\n", params->id);
	free(box);
	pthread_exit(NULL);
}

void* moverFunction(void* param)
{
	threadParams* params = (threadParams*) param;
	printf("Hello from mover %d\n", params->id);
	movingBox *box = malloc(sizeof(movingBox));
	int timeToMove;
	while(read(dwellerToMover[R], box, sizeof(movingBox)) != 0)
	{
		timeToMove = RANDOM(MIN_MOVER_TIME, MAX_MOVER_TIME, params->randomSeed);
		sleep(timeToMove);
		write(moverToTrucker[W], box, sizeof(movingBox));
		printf("Mover %d brought down a box that weighs %d in %d units of time\n", params->id, box->weight, timeToMove);
	}

	printf("Mover %d is done moving boxes downstairs!\n", params->id);
	free(box);
	pthread_exit(NULL);
}

void* truckerFunction(void* param)
{
	threadParams* params = (threadParams*) param;
	printf("Hello from truck driver %d\n", params->id);
	movingBox *box = malloc(sizeof(movingBox));
	movingTruck *truck = malloc(sizeof(movingTruck));
	int timeToLoad;
	int roundTripTime;
	truck->room = TRUCK_ROOM;
	truck->totalWeight = 0;
	while(read(moverToTrucker[R], box, sizeof(movingBox)) != 0)
	{
		timeToLoad = RANDOM(MIN_TRUCKER_TIME, MAX_TRUCKER_TIME, params->randomSeed);
		sleep(timeToLoad);
		truck->totalWeight = truck->totalWeight + box->weight;
		truck->room = truck->room - 1;
		printf("Trucker %d loaded up a box that weighs %d to the truck, took %d units of time, room left:%d\n", params->id, box->weight, timeToLoad, truck->room);
		if(truck->room == 0)
		{
			roundTripTime = RANDOM(MIN_TRIP_TIME, MAX_TRIP_TIME, params->randomSeed);
			printf("Full truck %d with load of %d units of mass departed, round trip time will take %d units of time\n", params->id, truck->totalWeight, roundTripTime);
			sleep(roundTripTime);
			truck->totalWeight = 0;
			truck->room = TRUCK_ROOM;
		}
	}
	if(truck->room != TRUCK_ROOM)
	{
		roundTripTime = RANDOM(MIN_TRIP_TIME, MAX_TRIP_TIME, params->randomSeed);
		printf("Not full truck %d with load of %d units of mass departed, one way trip will take %d units of time\n", params->id, truck->totalWeight, roundTripTime);
		truck->totalWeight = 0;
		truck->room = TRUCK_ROOM;
	}
	printf("Trucker %d is finished!\n", params->id);
	free(box);
	free(truck);
	pthread_exit(NULL);
}

int main()
{
	int dwellers, movers, truckers;

	if(pipe(dwellerToMover) < 0 || pipe(moverToTrucker) < 0)
		exit(EXIT_FAILURE);

	printf("Enter the number of dwellers: ");
	scanf("%d", &dwellers);
	
	printf("Enter the number of movers: ");
	scanf("%d", &movers);

	printf("Enter the number of truckers: ");
	scanf("%d", &truckers);

	printf("There are %d dwellers, %d movers, and %d truckers\n", dwellers, movers, truckers);


	threadParams *dwellerParams = malloc(dwellers * sizeof(threadParams));
	pthread_t *dwellerThreads = malloc(dwellers * sizeof(pthread_t));
	

	for(int i = 0; i < dwellers; i++)
	{
		dwellerParams[i].id = i;
		dwellerParams[i].randomSeed = rand();
		pthread_create(&dwellerThreads[i], NULL, dwellerFunction, (void*)&dwellerParams[i]);

	}

	pthread_t *moverThreads = malloc(movers * sizeof(pthread_t));
	threadParams *moverParams = malloc(movers * sizeof(threadParams));
	for(int i = 0; i < movers; i++)
	{
		moverParams[i].id = i;
		moverParams[i].randomSeed = rand();
		pthread_create(&moverThreads[i], NULL, moverFunction, (void*)&moverParams[i]);
	}

	pthread_t *truckerThreads = malloc(truckers * sizeof(pthread_t));
	threadParams *truckerParams = malloc(truckers * sizeof(threadParams));
	for(int i = 0; i < truckers; i++)
	{
		truckerParams[i].id = i;
		truckerParams[i].randomSeed = rand();
		pthread_create(&truckerThreads[i], NULL, truckerFunction, (void*)&truckerParams[i]);
	}

	for(int i = 0; i < dwellers; i++)
		pthread_join(dwellerThreads[i], NULL);

	close(dwellerToMover[W]);
	
	for(int i = 0; i < movers; i++)
		pthread_join(moverThreads[i], NULL);

	close(dwellerToMover[R]);
	close(moverToTrucker[W]);

	for(int i = 0; i < truckers; i++)
		pthread_join(truckerThreads[i], NULL);

	close(moverToTrucker[R]);

	free(dwellerThreads);
	free(dwellerParams);
	free(moverThreads);
	free(moverParams);
	free(truckerThreads);
	free(truckerParams);

	printf("Moving is finished!\n");
	return 0;
}