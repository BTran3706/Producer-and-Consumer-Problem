#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "thread.h"

extern "C" {
#include "io.h"
}

// Produce and add a candy to belt
ProductType incrementBelt(PRODUCER_DATA * producerPtr) {
    
    SHARED_DATA* sharedData = producerPtr->sharedPtr;

    // Type of candy about to be produced
    ProductType candyType = producerPtr->candyType;

    // Put candy on belt and move pointer
    sharedData->candiesOnBelt[sharedData->indexOfCandyProducedOnBelt] = candyType;
    sharedData->indexOfCandyProducedOnBelt = ((sharedData->indexOfCandyProducedOnBelt + 1) % CONVEYER_BELT_MAX); // Remainder of belt max of 10 to ensure producers aren't producing past max of belt

    // Increase total num of candies produced
	sharedData->totalProduced++;

    // Increase num of candy type produced (Frog or Escargot)
	sharedData->numOfCandyTypeProduced[candyType]++; 
  
    // Increase num of candy type on belt
    sharedData->numOfCandyTypeOnBelt[candyType]++;

    return candyType;
  
}

// Consume and remove a candy from belt
ProductType decrementBelt(CONSUMER_DATA * consumerPtr) {
    
    SHARED_DATA* sharedData = consumerPtr->sharedPtr;
	
	// Type of candy about to be consumed
    ProductType candyType = (ProductType) sharedData->candiesOnBelt[sharedData->indexOfCandyConsumedOnBelt];
	
    // Remove candy on belt and move pointer
	sharedData->indexOfCandyConsumedOnBelt = ((sharedData->indexOfCandyConsumedOnBelt + 1) % CONVEYER_BELT_MAX); // Remainder of belt max of 10 to ensure consumers aren't consuming candies past max of belt
	
	// Increase total num of candies consumed
	sharedData->totalConsumed++;
	
    // Increase num of candy type consumed by the consumer Lucy or Ethel
	consumerPtr->numOfCandyTypeConsumed[candyType]++;
	
	// Decrease num of candy type on belt
    sharedData->numOfCandyTypeOnBelt[candyType]--;

    return candyType;
  
}

// Producer task
void* produceTask(void* voidPtr) {

    // Typecast thread pointer
    PRODUCER_DATA *producerPtr = (PRODUCER_DATA*) voidPtr;
    SEM_DATA* semPtr = producerPtr->semPtr;
    SHARED_DATA* sharedData = producerPtr->sharedPtr;

    // Check production if total candy produced is less than max amount of 100
    while (checkProduction(producerPtr)) {
		
		// Sleep/wait for the amount of time it takes to produce the candy 
        wait(producerPtr->produceTime);
		
		// Stop production if the production task has been finished by other threads where the total candy produced is greater than or equal to max amount of 100
		if (!checkProduction(producerPtr)){
			return NULL;
		}
		
		// If num of frog candy on belt is not greater than or equal max amount of 3 at a time
        if (!checkFrogCandy(producerPtr)) {

            // Wait until at least 1 empty entry on the belt
            sem_wait(&(semPtr->empty));
      
            // Only 1 thread can modify the shared data at a time
            sem_wait(&(semPtr->mutex));
    
            // Update info after adding candy to belt
            ProductType producer = incrementBelt(producerPtr);

            // Print info
            io_add_type(producer, sharedData->numOfCandyTypeOnBelt, sharedData->numOfCandyTypeProduced);

            // Clear output stream
            fflush(stdout);

            // Exit critical Section 
            sem_post(&(semPtr->mutex));
            sem_post(&(semPtr->full));
        }
    }
  
    return NULL;
  
};

// Consumer task
void* consumeTask(void* voidPtr) {

    // Typecast thread pointer
	CONSUMER_DATA *consumerPtr = (CONSUMER_DATA *) voidPtr;
    SEM_DATA* semPtr = consumerPtr->semPtr;
    SHARED_DATA* sharedData = consumerPtr->sharedPtr;

    // Check consumption for if total candy consumed is less than max amount of 100
    while (checkConsumption(consumerPtr)) {
		
		// Sleep/wait for the amount of time it takes to consume the candy 
		wait(consumerPtr->consumeTime);
		
		// Stop consumption if the consumption task has been finished by other threads where the total candy consumed is greater than or equal to max of amount of 100
		if (!checkConsumption(consumerPtr)){
			return NULL;
		}
		
		// Wait until at least 1 empty entry on the belt
		sem_wait(&(semPtr->full)); 
        
		// Only 1 thread can modify the shared data at a time
		sem_wait(&(semPtr->mutex));
 
        // Update info after removing from belt
        ProductType product = decrementBelt(consumerPtr); 

        // Print info
        io_remove_type(consumerPtr->consumerName, product, sharedData->numOfCandyTypeOnBelt, consumerPtr->numOfCandyTypeConsumed);
        
        // Clear output stream
		fflush(stdout);
		
		// Exit critical Section 
        sem_post(&(semPtr->mutex));
        sem_post(&(semPtr->empty));
    }
  
    return NULL;
  
};

// Process arguments for how long it takes to produce and consume candy
void getArguments(int argc, char* argv[], PRODUCE_AND_CONSUME_TIMES* arguments) {
    
    int option;

    while ((option = getopt(argc, argv, "E:L:f:e:")) != -1) {
        switch (option) {
            case 'E': // Specifies milliseconds consumer Ethel requires to consume a candy
                arguments->ethelTime = atol(optarg);
                break;
            case 'L': // Specifies milliseconds consumer Lucy requires to consume a candy
                arguments->lucyTime = atol(optarg);
                break;
            case 'f': // Specifies milliseconds required to produce frog candy
                arguments->frogCandyTime = atol(optarg);
                break;
            case 'e': // Specifies milliseconds required to produce escargot candy
                arguments->escargotCandyTime = atol(optarg);   
                break;
            default:
                exit(EXIT_FAILURE);
        }   
    }
    
}

// Main method to run and simulate the producer consumer problem in the Mizzo Candy Factory
void runMizzoCandyFactory(PRODUCE_AND_CONSUME_TIMES* arguments) {

    // Initialize data for semaphores
    SEM_DATA semData;                               
    initializeSemData(&semData);

    // Initialize shared data between all threads
	SHARED_DATA sharedData;
    initializeSharedData(&sharedData);

    // Initialize data for producer threads with one handling frog candy and the other handling escargot candy
	PRODUCER_DATA frogCandyData, escargotCandyData; 
    initializeProducerData(&frogCandyData, FrogBite, arguments->frogCandyTime, &semData, &sharedData);
    initializeProducerData(&escargotCandyData, Escargot, arguments->escargotCandyTime, &semData, &sharedData);

    // Initialize data for consumer threads with one handling Lucy and the other handling Ethel
	CONSUMER_DATA lucyData, ethelData;
    initializeConsumerData(&lucyData, Lucy, arguments->lucyTime, &semData, &sharedData);
    initializeConsumerData(&ethelData, Ethel, arguments->ethelTime, &semData, &sharedData);

    pthread_t frogCandyThread, escargotCandyThread, lucyThread, ethelThread;
	void *threadPtr;

    // Create all threads and excecute their task
	pthread_create(&frogCandyThread, NULL, produceTask, &frogCandyData);
    pthread_create(&escargotCandyThread, NULL, produceTask, &escargotCandyData);
    pthread_create(&lucyThread, NULL, consumeTask, &lucyData);
    pthread_create(&ethelThread, NULL, consumeTask, &ethelData);

    // Wait for threads to exit
	pthread_join(frogCandyThread, &threadPtr);
    pthread_join(escargotCandyThread, &threadPtr);
    pthread_join(lucyThread, &threadPtr);
    pthread_join(ethelThread, &threadPtr);

    // Array of pointers to numOfCandyTypeConsumed arrays for the consumers Lucy and Ehtel
    int* numCandyConsumed[ConsumerTypeN];
    numCandyConsumed[Lucy] = lucyData.numOfCandyTypeConsumed;
    numCandyConsumed[Ethel] = ethelData.numOfCandyTypeConsumed;

    // Report how many candies of each type frog and escargot are produced and how many candies the consumers Lucy and Ethel consumed
    io_production_report(sharedData.numOfCandyTypeProduced, numCandyConsumed);
    
}

int main(int argc, char* argv[]) {

    // Time it take to produce and consume gotten from command line
	PRODUCE_AND_CONSUME_TIMES arguments = {};
	
	// Read in arguments from command line
    getArguments(argc, argv, &arguments);
    
    runMizzoCandyFactory(&arguments);

    return 0;
    
}