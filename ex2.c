#include <pthread.h>
#include <unistd.h>
#include "circle.h"

#define NUMBER_OF_THREADS 4

/* shared variable for the threads to update */
volatile int buffer = -1;
pthread_mutex_t buffer_mutex;
pthread_cond_t read_from_buffer;

void *worker_main()
{
	printf("Thread: Starting\n");
	for (;;){
		int roll = circle();
		pthread_mutex_lock(&buffer_mutex);
		if (buffer == 2) {
			goto exit;
		};
		buffer = roll;
        pthread_cond_signal(&read_from_buffer);
		pthread_mutex_unlock(&buffer_mutex);
	}
exit: 
	pthread_mutex_unlock(&buffer_mutex);
	printf("Thread: Terminating\n");
    return 0;
}

int
main(void)
{
    pthread_mutex_init(&buffer_mutex, NULL);
	pthread_cond_init(&read_from_buffer, NULL);

    pthread_t workers[NUMBER_OF_THREADS];

	for (int i = 0; i < NUMBER_OF_THREADS; i++) {
		pthread_create( &workers[i],
		0,
		worker_main,
		0);
	}

	/* number of samples */
	int niters = 1000000;
	int count = 0;

	sleep(1);
	for (int i = 0; i < niters; i++) {
        pthread_mutex_lock(&buffer_mutex);
        pthread_cond_wait(&read_from_buffer, &buffer_mutex);
		if (buffer == 1) {
			count++;
		}
        pthread_mutex_unlock(&buffer_mutex);
    	if (i % (niters/100) == 0) 
			printf("%d/%d: %f\n", i, niters, (double)count / i * 4);
	}

	buffer = 2;

	printf("The value of pi is: %f\n", (double)count / niters * 4);

	for (int i = 0; i < NUMBER_OF_THREADS; i++) {
		void *result;
		pthread_join( workers[i], &result);
	}

    pthread_mutex_destroy(&buffer_mutex);
	pthread_cond_destroy(&read_from_buffer);

	return 0;
}
