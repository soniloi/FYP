#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct timespec timespec;

void timespec_subtract(timespec * start, timespec * stop, timespec * interval)
{
	interval->tv_sec = stop->tv_sec - start->tv_sec; // Calculate seconds in the difference
	int nanos = stop->tv_nsec - start->tv_nsec; // Calculate nanoseconds in the difference, accounting for possible overflow
	if(nanos < 0)
	{
		nanos += 1000000000L;
		interval->tv_sec--;
	}
	interval->tv_nsec = nanos;
}

int main(int argc, char ** argv)
{
	if(argc != 3)
	{
		fprintf(stderr, "Usage: sieve <iterations> <size>\n");
		exit(1);
	}

	int iterations = atoi(argv[1]);
	if(iterations < 1)
	{
		fprintf(stderr, "Specify at least two iterations.\n");
		exit(1);
	}

	int size = atoi(argv[2]);
	if(size < 2)
	{
		fprintf(stderr, "Specify a positive integer size.\n");
		exit(1);
	}

	char * testname = "dummy";

	unsigned long long total_time = 0L;

	int bound = iterations + 1;
	int i;
	for(i = 0; i < bound; i++)
	{
		timespec start, stop, interval;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

		// Do stuff

		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
		timespec_subtract(&start, &stop, &interval);

		unsigned long long elapsed = interval.tv_sec * 1000000000L + interval.tv_nsec;
		if(i > 0) // Do not include the first iteration; data need to be paged in/cached, so the first iteration will always throw the result
			total_time += elapsed;
		//printf("Iteration %d: time elapsed: %llu microseconds\n", i+1, elapsed);

	}
	printf("%s where size = %d approximate average running time over %d iterations: %llu nanoseconds\n", testname, size, iterations, total_time/iterations);

	return 0;
}
