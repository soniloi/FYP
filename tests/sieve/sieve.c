#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LEN_CORRECT 128

typedef struct timespec timespec;

const int correct[LEN_CORRECT] = {0, 0, 1, 1, 0, 1, 0, 1, // 0-7
                                  0, 0, 0, 1, 0, 1, 0, 0, // 8-15
                                  0, 1, 0, 1, 0, 0, 0, 1, // 16-23
                                  0, 0, 0, 0, 0, 1, 0, 1, // 24-31
                                  0, 0, 0, 0, 0, 1, 0, 0, // 32-39
                                  0, 1, 0, 1, 0, 0, 0, 1, // 40-47
                                  0, 0, 0, 0, 0, 1, 0, 0, // 48-55
                                  0, 0, 0, 1, 0, 1, 0, 0, // 56-63
                                  0, 0, 0, 1, 0, 0, 0, 1, // 64-71
                                  0, 1, 0, 0, 0, 0, 0, 1, // 72-79
                                  0, 0, 0, 1, 0, 0, 0, 0, // 80-87
                                  0, 1, 0, 0, 0, 0, 0, 0, // 88-95
                                  0, 1, 0, 0, 0, 1, 0, 1, // 96-103
                                  0, 0, 0, 1, 0, 1, 0, 0, // 104-111
                                  0, 1, 0, 0, 0, 0, 0, 0, // 112-119
                                  0, 0, 0, 0, 0, 0, 0, 1}; // 120-127

int * sieve(int n) {
	int * isprime;
	isprime = malloc(sizeof(int) * n);

	int i, j;
	for(i = 2; i < n; i++)
	{
		isprime[i] = 1;
	}
	isprime[0] = 0;
	isprime[1] = 0;

	int bound = sqrt(n) + 1; // Allow for possible FP rounding errors
	for(i = 2; i <= bound; i++)
	{
		if(isprime[i])
		{
			for(j = i+i; j < n; j+=i)
			{
				isprime[j] = 0;
			}
		}
	}
	/*
	int k;
	for(k = 1; k < n; k++)
	{
		if(isprime[k])
		printf("%d ", k);
	}
	printf("\n");
	*/
	return isprime;
}

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
	if(iterations < 2)
	{
		fprintf(stderr, "Specify at least two iterations.\n");
		exit(1);
	}

	int size = atoi(argv[2]);
	if(size < 1)
	{
		fprintf(stderr, "Specify a positive integer range.\n");
		exit(1);
	}

	unsigned long long total_time = 0L;

	int bound = iterations + 1;
	int i;
	for(i = 0; i < bound; i++)
	{
		timespec start, stop, interval;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

		int * isprime;
		isprime = sieve(size);

		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
		timespec_subtract(&start, &stop, &interval);

		unsigned long long elapsed = interval.tv_sec * 1000000000L + interval.tv_nsec;
		if(i > 0) // Do not include the first iteration; data need to be paged in/cached, so the first iteration will always throw the result
			total_time += elapsed;

		//printf("Iteration %d: time elapsed: %llu microseconds\n", i+1, elapsed);

		// Test output for correctness
		int max = LEN_CORRECT;
		if(size < max)
			max = size;
		int j;
		for(j = 0; j < max; j++)
		{
			if(isprime[j] != correct[j])
				printf("Error on iteration %d: program calculates %d=%d, correct is %d=%d\n", i, j, isprime[j], j, correct[j]);
		}

		free(isprime);
	}
	printf("Sieve of Eratosthenes where size = %d approximate average running time over %d iterations: %llu nanoseconds\n", size, iterations, total_time/iterations);

	return 0;
}
