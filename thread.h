#ifndef THREAD_H
#define THREAD_H

#include "structures.h"

void initializeSemData(SEM_DATA* semData);

void initializeSharedData(SHARED_DATA* sharedPtr);

void initializeProducerData(PRODUCER_DATA* producerPtr, ProductType candyType, int produceTime, SEM_DATA* semPtr, SHARED_DATA* sharedPtr);

void initializeConsumerData(CONSUMER_DATA* consumerPtr, ConsumerType consumerName, int consumeTime, SEM_DATA* semPtr, SHARED_DATA* sharedPtr);

bool checkProduction(PRODUCER_DATA * producerPtr);

bool checkConsumption(CONSUMER_DATA * consumerPtr);

bool checkFrogCandy(PRODUCER_DATA* producerPtr);

int wait(long milliSec);

#endif // THREAD_H