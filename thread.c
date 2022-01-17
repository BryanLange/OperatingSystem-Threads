#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

/*------------------------------------------------------------------- 
   About: This program uses two levels of pthreads to find
          the minimum of N numbers.
   Compile: $ gcc -pthread thread.c -o thread
   Execute: $ ./thread
-------------------------------------------------------------------*/

#define N 10000	         // number of random numbers to generate(must be even) 
#define NUM_L1THREADS 5  // number of threads at level 1
#define NUM_L2THREADS 20 // number of threads at level 2


// global variables
int randNums[N];
int rangeL1, rangeL2;

// prototypes
void *runner1(void *param);
void *runner2(void *param);

// struct to hold thread data
typedef struct threadData {
	int startIndex;
	int endIndex;
	int result;
} threadData;


int main() {

	// get starting time of thread search
	struct timeval t1, t2;
	double elapsedTime;
	gettimeofday(&t1, NULL);
	

	//-compute size of range for each level
	//-the range represents the number of array elements to be 
	// searched by a thread at its level
	rangeL1 = N / NUM_L1THREADS;
	rangeL2 = rangeL1 / NUM_L2THREADS;


	// generate N random numbers
	time_t t;
	srand((unsigned) time(&t));
	for(int i=0; i<N; i++) {
		randNums[i] = rand();
	}
	

	//-create threadData struct for each level 1 thread
	//-compute level 2 search ranges
	//-each struct will hold the minimum value of the
	// range between startIndex and endIndex
	threadData threadData[NUM_L1THREADS];
	for(int i=0; i<NUM_L1THREADS; i++) {
		int lowerBound = i * rangeL1;
		threadData[i].startIndex = lowerBound;
		threadData[i].endIndex = lowerBound + rangeL1 - 1;
	} 
	

	//-create level 1 threads using pthread_create()
	//-NULL used as attribute parameter instead of:
	//	pthread_attr_t attr; pthread_attr_init(&attr);
	//-each threadData[i] struct is passed as a pointer parameter
	// to the corresponding thread[i]
	pthread_t threads[NUM_L1THREADS];
	for(int i=0; i<NUM_L1THREADS; i++) {
		pthread_create(&threads[i], NULL, runner1,
					(void *)&threadData[i]);		
	}

	// wait for level 1 threads to finish
	for(int i=0; i<NUM_L1THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	
	
	// find minimum value of level 1 threads(minimum of N)
	int min = threadData[0].result;
	for(int i=1; i<NUM_L1THREADS; i++) {
		if(threadData[i].result < min) {
			min = threadData[i].result;
		}
	}
	
	// compute elasped time
	gettimeofday(&t2, NULL);
	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;

	
	// display minimum value and time taken
	printf("\nThreaded search:\n");
	printf("\tThe mininum is: %d\n", min);	
	printf("\tTime taken: %fms.\n", elapsedTime);



	// LINEAR SEARCH --------------------------------------------
	// get starting time
	gettimeofday(&t1, NULL);
	

	// find minimum value
	int minLin = randNums[0];
	for(int i=1; i<N; i++) {
		if(randNums[i] < minLin) {
			minLin = randNums[i];
		}
	}
	
	// compute elapsed time 
	gettimeofday(&t2, NULL);
	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;
	

	// display minimum value and time taken	
	printf("Linear search:\n");
	printf("\tThe mininum is: %d\n", minLin);
	printf("\tTime taken: %fms.\n\n", elapsedTime);
		
	return 0;
} // end main



// runner function for level 1 threads
void *runner1(void *param) {
	
	// cast param to threadData struct
	threadData *data = (threadData *) param;
	
	
	//-create threadData struct for each level 2 thread
	//-compute level 2 search ranges
	//-each struct will hold the minimum value of the
	// range between structs's startIndex and endIndex
	threadData threadData[NUM_L2THREADS];
	for(int i=0; i<NUM_L2THREADS; i++) {
		int lowerBound = data->startIndex + i * rangeL2;
		threadData[i].startIndex = lowerBound;
		threadData[i].endIndex = lowerBound + rangeL2 - 1;
	}

	//-create level 2 threads
	//-each threadData[i] struct is passed as a pointer parameter
	// to the corresponding thread[i]
	pthread_t threads[NUM_L2THREADS];
	for(int i=0; i<NUM_L2THREADS; i++) {
		pthread_create(&threads[i], NULL, runner2,
					(void *) &threadData[i]);		
	}

	// wait for level 2 threads to finish
	for(int i=0; i<NUM_L2THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	// find minimum value of level 2 threads
	int min = threadData[0].result;
	for(int i=1; i<NUM_L2THREADS; i++) {
		if(threadData[i].result < min) {
			min = threadData[i].result;
		}
	}
	
	// set 'result' of .this thread to minimum of level 2 threads
	data->result = min;
	
	pthread_exit(0);
} // end runner1()



// runner function for level 2 threads
void *runner2(void *param) {
	
	// cast param to threadData struct
	threadData *data = (threadData *) param;

	// find minimum of startIndex and endIndex range
	int min = randNums[data->endIndex];
	for(int i=data->startIndex; i<data->endIndex; i++) {
		if(randNums[i] < min) {
			min = randNums[i];
		}
	}

	// set 'result' of .this thread to minimum of search range
	data->result = min;

	pthread_exit(0);
} // end runner2()
