#include <stdlib.h>
#include <stdio.h> 
#include <time.h>
#include "thread.h"

// Initialize semaphore with mutex, empty, and full
void initializeSemData(SEM_DATA* semData) {
    
    sem_init(&(semData->mutex), 0, 1); // Mutual exclusion
    sem_init(&(semData->empty), 0, CONVEYER_BELT_MAX); // Num empty buffer entries
    sem_init(&(semData->full), 0, 0); // Num full buffer entries
  
};

// Initialize shared data with all the candies on belt, num of each candy type produced, and num of each candy type on belt
void initializeSharedData(SHARED_DATA* sharedPtr) {
    
    sharedPtr->totalConsumed = sharedPtr->totalProduced = sharedPtr->indexOfCandyProducedOnBelt = sharedPtr->indexOfCandyConsumedOnBelt = 0;
  
    for (int i = 0; i < CONVEYER_BELT_MAX; i++) {
        sharedPtr->candiesOnBelt[i] = 0;
    }
  
    for (int i = 0; i < ProductTypeN; i++) { // For ProductTypeN, 0 is frogCandy and 1 is escargot candy
        sharedPtr->numOfCandyTypeProduced[i] = 0;
    }
  
    for (int i = 0; i < ProductTypeN; i++) {
        sharedPtr->numOfCandyTypeOnBelt[i] = 0;
    }
  
}

// Initialize producer data with the candy type, the time it takes for the candy to be produced, and pointers to the semaphore and shared data 
void initializeProducerData(PRODUCER_DATA* producerPtr, ProductType candyType, int produceTime, SEM_DATA* semPtr, SHARED_DATA* sharedPtr){

    producerPtr->candyType = candyType;
    producerPtr->produceTime = produceTime;
    producerPtr->semPtr = semPtr;
	producerPtr->sharedPtr = sharedPtr;
    
};

// Initialize consumer data with the consumer name, time it takes for the consumer to consume the candy, num of each candy type consumed, and pointers to the semaphore and shared data 
void initializeConsumerData(CONSUMER_DATA* consumerPtr, ConsumerType consumerName, int consumeTime, SEM_DATA* semPtr, SHARED_DATA* sharedPtr) {

    consumerPtr->consumerName = consumerName;
    consumerPtr->consumeTime = consumeTime;

    for (int i = 0; i < ConsumerTypeN; i++) { // For ConsumerTypeN, 0 is Lucy and 1 is Ethel
      consumerPtr->numOfCandyTypeConsumed[i] = 0;
    }
    
    consumerPtr->semPtr = semPtr;
    consumerPtr->sharedPtr = sharedPtr;
    
};

// Check if total candy produced is within the limit of 100
bool checkProduction(PRODUCER_DATA* producerPtr) {
  
    SHARED_DATA* sharedData = producerPtr->sharedPtr;
  
    return sharedData->totalProduced < MAX_CANDY;
  
}

// Check if total candy consumed is within the limit of 100
bool checkConsumption(CONSUMER_DATA * consumerPtr) {
    
    SHARED_DATA* sharedData = consumerPtr->sharedPtr;

    return (sharedData->totalConsumed < MAX_CANDY);
  
}

// Check if frog candy on belt is within the limit of 3 at a time
bool checkFrogCandy(PRODUCER_DATA* producerPtr) {
    
    bool isFrogCandyProducer = (producerPtr->candyType == FrogBite);
    int numFrogCandyOnBelt = producerPtr->sharedPtr->numOfCandyTypeOnBelt[FrogBite];

    return (isFrogCandyProducer && numFrogCandyOnBelt >= MAX_FROGCANDY_ON_BELT);
  
}

// Sleep a specified amount of time in milliseconds to simulate the amount of the time it takes to produce or consume a candy
int wait(long milliSec) {
    
    struct timespec timeSlept;

    timeSlept.tv_sec = milliSec / 1000;
    timeSlept.tv_nsec = (milliSec % 1000) * 1000000;

    return nanosleep(&timeSlept, &timeSlept);
    
}