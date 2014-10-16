#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "multi-lookup.h"
#include "util.h"
#include "queue.h"

#define MINARGS 3
#define USAGE "<inputFilePath> <outputFilePath>"
#define SBUFSIZE 1025
#define INPUTFS "%1024s"

#define MAX_INPUT_FILES 10
#define REQUESTER_COUNT 10
#define RESOLVER_COUNT 2

//#define MULTI_LOOKUP_DEBUG

pthread_mutex_t queueLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t outputLock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t notFull = PTHREAD_COND_INITIALIZER;
pthread_cond_t notEmpty = PTHREAD_COND_INITIALIZER;

void PushElement(void* element)
{
	pthread_mutex_lock(&queueLock);
	while(queue_is_full(&q)) {
		pthread_cond_wait(&notFull, &queueLock);
	} 
	queue_push(&q, element);
	pthread_cond_signal(&notEmpty);
	pthread_mutex_unlock(&queueLock);
}

void* PopElement() 
{
	pthread_mutex_lock(&queueLock);
	while(queue_is_empty(&q) && cancelReads == 0) {
		pthread_cond_wait(&notEmpty, &queueLock);
	}
	
	void* element = NULL;
	if(cancelReads == 0) {
		element = queue_pop(&q);
		pthread_cond_signal(&notFull);
	}
	
	pthread_mutex_unlock(&queueLock);

	return element;
}

void WriteToFile(char* host, char* ip) 
{
	// Our output file is null
	if (FILEOUT == NULL) {
		printf("outfile err");
		exit(1);
	}
	
	// Write host and ip values to file
	pthread_mutex_lock(&outputLock);
	
	FILE* outputfp = fopen(FILEOUT, "a");
	

	fprintf(outputfp, "%s,%s\n", host, ip);
	fclose(outputfp);
	
	pthread_mutex_unlock(&outputLock);
	
		
#ifdef MULTI_LOOKUP_DEBUG
	printf("Wrote: <%s,%s>\n", host, ip);
#endif
}

void* Th_Request(void* filename) 
{
	char *f = (char*)filename;
    char hostname[SBUFSIZE];
    char errorstr[SBUFSIZE];

    FILE* inputfp = NULL; 
    inputfp = fopen(f, "r");	
    
    if(!inputfp){
        sprintf(errorstr, "Error Opening Input File: %s", f);
        perror(errorstr);
    }
    
    while(fscanf(inputfp, INPUTFS, hostname) > 0){
		char* v = malloc(strlen(hostname));
		strcpy(v, hostname);
		PushElement(v);
		
#ifdef MULTI_LOOKUP_DEBUG
		printf("Pushed: %s\n", v);
#endif
	
	}     
	
	fclose(inputfp);

	runningRequests--;
	
#ifdef MULTI_LOOKUP_DEBUG
	printf("Request Ended, %d Remaining\n", runningRequests);
#endif
    return NULL;
}

void* Th_Resolve()
{
	while(1) {
		void* element = PopElement();
		if (element == NULL) {
			break;
		}
		char* v = (char*)element;
	    char firstipstr[INET6_ADDRSTRLEN];
	    
#ifdef MULTI_LOOKUP_DEBUG
		printf("Popped: %s\n", v);
#endif

		if(dnslookup(v, firstipstr, sizeof(firstipstr)) != UTIL_FAILURE)
			WriteToFile(v, firstipstr);
		else 
			WriteToFile(v, "");
	}
	
#ifdef MULTI_LOOKUP_DEBUG
	printf("Resolve End\n");
#endif
	
	return NULL;
}

int main(int argc, char* argv[]){
	
    /* Check Arguments */
    if(argc < MINARGS){
		fprintf(stderr, "Not enough arguments: %d\n", (argc - 1));
		fprintf(stderr, "Usage:\n %s %s\n", argv[0], USAGE);
		return EXIT_FAILURE;
    }
    
	/* Vars */
    int i;
    char errorstr[SBUFSIZE];

    int NUM_THREADS = argc-2;
    if (NUM_THREADS > REQUESTER_COUNT) {
		printf("Too many files!\n");
		return EXIT_FAILURE;
	}
	
	FILEOUT = argv[argc-1];
	
	 /* Clear Outfile */
	 FILE* tmp = fopen(FILEOUT, "w");
	 if(!tmp) {
	    sprintf(errorstr, "Error Opening Output File: %s", FILEOUT);
        perror(errorstr);
    }
	
    pthread_t requesterPool[NUM_THREADS];
    pthread_t resolverPool[RESOLVER_COUNT];
    
    /* Initialize the Queue */
    queue_init(&q, 2);

	/* Create Threads */
	runningRequests = NUM_THREADS;

	for (i = 0; i < REQUESTER_COUNT; ++i) {
		if(i < NUM_THREADS)
			pthread_create(&requesterPool[i], NULL, Th_Request, argv[i + 1]);
		if(i < RESOLVER_COUNT) 
			pthread_create(&resolverPool[i], NULL, Th_Resolve, NULL);
	}

	
	while(1) {
		if(runningRequests == 0) {
			while(!queue_is_empty(&q));
#ifdef MULTI_LOOKUP_DEBUG
			printf("All Requests Pushed\n");
#endif
			/* Processing Done, Cancel Readers to avoid Deadlock */
			pthread_mutex_lock(&queueLock);
			cancelReads = 1;
			pthread_cond_broadcast(&notEmpty);
			pthread_mutex_unlock(&queueLock);
			break;
		}
	}

	
    /* Wait for Children */
    pthread_exit(NULL);

    return EXIT_SUCCESS;
}
