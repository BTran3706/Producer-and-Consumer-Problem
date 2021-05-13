#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <semaphore.h>
#include "production.h"

#define MAX_CANDY 100
#define CONVEYER_BELT_MAX 10
#define MAX_FROGCANDY_ON_BELT 3

// Time it take to produce and consume gotten from command line
struct PRODUCE_AND_CONSUME_TIMES {
    int ethelTime;
	int lucyTime;
	int frogCandyTime;
	int escargotCandyTime;
};

struct SEM_DATA {
    sem_t mutex; // Mutual exclusion
    sem_t empty; // Num empty buffer entries 
    sem_t full; // Num full buffer entries
};

struct SHARED_DATA {
    int totalProduced; // Total num of candies produced
	int numOfCandyTypeProduced[ProductTypeN]; // Array of num of each candy type produced
	int totalConsumed; // Total num of candies consumed
	int candiesOnBelt[CONVEYER_BELT_MAX]; // Array of all the candies on the belt
    int numOfCandyTypeOnBelt[ProductTypeN]; // Array of num of each candy type on belt
    int indexOfCandyProducedOnBelt, indexOfCandyConsumedOnBelt; // Position of candy on belt for production and consumption
};

struct PRODUCER_DATA {
    SEM_DATA* semPtr; // Pointer to semaphore data
    SHARED_DATA* sharedPtr; // Pointer to shared data
	ProductType candyType; // Type of candy
    int produceTime; // Time it takes candy to be produced
};

struct CONSUMER_DATA {
    SEM_DATA* semPtr; // Pointer to semaphore data
    SHARED_DATA* sharedPtr;  // Pointer to shared data
	ConsumerType consumerName; // Name of consumer          
    int consumeTime; // Time it takes for consumer to consume the candy
    int numOfCandyTypeConsumed[ProductTypeN]; // Array of num of candy type consumer consumed
};

#endif // STRUCTURES_H