#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <math.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/times.h>

#define TEST_DEPTH 500 // "Resolution" of tests
#define REPEAT 1 // Number of times each process runs a test

void DoTests();
void RecordResults(double time, double userTime, double systemTime, char* filename);

int ExpensiveRead();
void ExpensiveWrite();

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
int ExpensiveRead() {
	unsigned int v, i = 0;
	FILE *f;
	
	for(i = 0; i < TEST_DEPTH; ++i) {
		f = fopen("/dev/urandom", "r");
		fread(&v, sizeof(v), 1, f);
		fclose(f);
		i += v;
	}
	return i;
}

void ExpensiveWrite() {
	
	int i;
	FILE *f;
	for (i = 0; i < TEST_DEPTH; ++i) {
		f = fopen("/dev/null", "w");
		fprintf(f, "%d", ExpensiveRead());
		fclose(f);
	}
}

void DoTests (int instances) 
{
	int i, j;
	pid_t pid;
	for (i = 0; i < instances; ++i) {
		pid = fork();
		if(pid == 0) {
			for(j = 0; j < REPEAT; ++j) {
				ExpensiveWrite();
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
