#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_M 3
#define MAX_N 4

typedef struct timespec timespec;

const int correct[MAX_M+1][MAX_N+1] = { {1, 2, 3, 4, 5},
                                        {2, 3, 4, 5, 6},
                                        {3, 5, 7, 9, 11},
                                        {5, 13, 29, 61, 125} };

/* Ackermann function */
int ackermann(int m, int n)
{
	if(m == 0)
		return n+1;

	else if(n == 0)
		return ackermann(m-1, 1);

	else
		return ackermann(m-1, ackermann(m, n-1));
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
	if(argc != 4)
	{
		fprintf(stderr, "Usage: ackermann <iterations> <m> <n>\n");
		exit(1);
	}

	int iterations = atoi(argv[1]);
	if(iterations < 2)
	{
		fprintf(stderr, "Specify at least two iterations.\n");
		exit(1);
	}

	int m = atoi(argv[2]);
	if(m < 0 || m > MAX_M)
	{
		fprintf(stderr, "m must be between 0 and %d inclusive.\n", MAX_M);
		exit(1);
	}

	int n = atoi(argv[3]);
	if(n < 0 || n > MAX_N)
	{
		fprintf(stderr, "n must be between 0 and %d inclusive.\n", MAX_N);
		exit(1);
	}

	unsigned long long total_time = 0L;

	int bound = iterations + 1;
	int i;
	for(i = 0; i < bound; i++)
	{
		timespec start, stop, interval;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

		int ack = ackermann(m, n);

		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
		timespec_subtract(&start, &stop, &interval);

		//printf("result = %d\n", ack);
		if(ack != correct[m][n])
			printf("Error in iteration %d: function returns %d, correct result is %d\n", i, ack, correct[m][n]);

		unsigned long long elapsed = interval.tv_sec * 1000000000L + interval.tv_nsec;
		if(i > 0) // Do not include the first iteration; data need to be paged in/cached, so the first iteration will always throw the result
			total_time += elapsed;

		//printf("Iteration %d: time elapsed: %llu microseconds\n", i+1, elapsed);
	}
	printf("ackermann(%d, %d) approximate average running time over %d iterations: %llu microseconds\n", m, n, iterations, total_time/iterations);
}

