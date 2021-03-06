#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <math.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/times.h>

#define TEST_DEPTH 10000 // "Resolution" of tests
#define REPEAT 500 // Number of times each process runs a test

void DoTests();
void RecordResults(double time, double userTime, double systemTime, char* filename);

void sieve();
float pi();

inline double dist(double x0, double y0, double x1, double y1) { return sqrt(pow((x1-x0),2) + pow((y1-y0),2)); }
inline double zeroDist(double x, double y) { return dist(0, 0, x, y); }

/*
 * Args: 
 * 0 - File
 * 1 - Children To Spawn
 * 2 - Scheduler To Use
 * 3 - Filename To Output Report
 */
int main(int argc, char* argv[]) 
{
		if (argc < 3) {
			printf("Not enough args");
			return 1;
		}
		
		time_t start;
		time_t end;
		struct tms runningTime;
		
		int repeat = atoi(argv[1]); // Times to repeat test
		int sched_type = atoi(argv[2]); // Scheduler Type
		int sched_policy = SCHED_OTHER;
		
		char* filename;
		if(argc == 4) {
			filename = argv[3];
		} else {
			filename = "test.txt";
		}
		
		double totalTime;
		double userTime;
		double systemTime;
		
		struct sched_param param;
		
		switch(sched_type) {
			case 0:
				sched_policy = SCHED_OTHER;
				break;
			case 1:
				sched_policy = SCHED_FIFO;
				break;
			case 2:
				sched_policy = SCHED_RR;
				break;
			default:
				printf("Bad Schedule Type\n");
				return 1;
		}
		
		param.sched_priority = sched_get_priority_max(sched_policy);
		if(sched_setscheduler(0, sched_policy, &param)) {
			perror("Error setting policy");
			return 1;
		}
		
		time(&start);
		DoTests(repeat);
		time(&end);
		times(&runningTime);
		
		totalTime = ((double)difftime(end, start));
		userTime = ((double)(runningTime.tms_utime))/CLOCKS_PER_SEC;
		systemTime = ((double)(runningTime.tms_stime))/CLOCKS_PER_SEC;
		
		RecordResults(totalTime, userTime, systemTime, filename);
		
		return 0;
}
// Adapted from github.com/chalmerj/comp410-MinixBenchmarkSuite
void sieve()
{
	int i, j;
	char list[TEST_DEPTH];
	
	for (i = 2; i * i <= TEST_DEPTH; i++) {
		if (!list[i]) {
			for(j = i + i; j < TEST_DEPTH; j += i) {
				list[j] = 1;
			}
		}
	}
}

// Adapted from github.com/asayler/CU-CS3753-PA3
float pi() 
{
	long i;
	double x, y;
	double inCircle = 0.0;
	double inSquare = 0.0;
	
	double pCircle = 0.0;
	double pi = 0.0;
	
	for (i = 0; i < TEST_DEPTH; ++i) {
		x = (random() % ((RAND_MAX / 2) * 2)) - (RAND_MAX / 2);
		y = (random() % ((RAND_MAX / 2) * 2)) - (RAND_MAX / 2);
		if(zeroDist(x,y) < RAND_MAX / 2) {
			inCircle++;
		}
		inSquare++;
	}
	
	pCircle = inCircle / inSquare;
	pi = pCircle * 4.0;
	return pi;
}

void DoTests (int instances) 
{
	int i, j;
	pid_t pid;
	for (i = 0; i < instances; ++i) {
		pid = fork();
		if(pid == 0) {
			for(j = 0; j < REPEAT; ++j) {
				sieve(TEST_DEPTH);
				pi(TEST_DEPTH);
			}
			return; 
		} 
	}
	while(wait(NULL) > 0);
}


void RecordResults(double time, double userTime, double systemTime, char* filename) {
	FILE *f = fopen(filename, "w");
	if(f != NULL) 
	{
		fprintf(f, "{\"time\":%f,\"userTime\":%f,\"systemTime\":%f}", time, userTime, systemTime);
	}
	fclose(f);
}
