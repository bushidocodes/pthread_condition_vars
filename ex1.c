#include <pthread.h>
#include "circle.h"

#define NUMBER_OF_THREADS 4

/* shared variable for the threads to update */
volatile int var = -1;
pthread_mutex_t mutex;

void *worker_main(void *raw_n)
{
	printf("%p", raw_n);
	// printf("Hello from %ld\n", pthread_self());
	
	for (;;){
		while (var != -1) ; // spin while waiting for workers
		pthread_mutex_lock(&mutex);
		if (var == 2) {
			pthread_mutex_unlock(&mutex);
			break;
		};
		var = circle();
		pthread_mutex_unlock(&mutex);
	}
    return 0;
}

int
main(void)
{
	pthread_mutex_init( &mutex, NULL);
    pthread_t workers[NUMBER_OF_THREADS];

	for (int i = 0; i < NUMBER_OF_THREADS; i++) {
		pthread_create( &workers[i],
		0,
		worker_main,
		0);
	}

	/* number of samples */
	int niters = 100000;
	int count = 0;

	for (int i = 0; i < niters; i++) {
		printf("%f\n", (double)count / i * 4);
		while (var == -1) ; // spin while waiting for workers
		
		if (var == 1) {
			count++;
		}

		pthread_mutex_lock(&mutex);
		var = -1;
		pthread_mutex_unlock(&mutex);
	}

	pthread_mutex_lock(&mutex);
	var = 2;
	pthread_mutex_unlock(&mutex);

	printf("The value of pi is: %f\n", (double)count / niters * 4);

	// TODO: Fix this....
	// for (int i = 0; i < NUMBER_OF_THREADS; i++) {
	// 	int result;
	// 	pthread_join( &workers[i], &result);
	// }

	return 0;
}
