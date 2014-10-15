#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "util.h"
#include "queue.h"

#define MINARGS 3
#define USAGE "<inputFilePath> <outputFilePath>"
#define SBUFSIZE 1025
#define INPUTFS "%1024s"

#define MAX_INPUT_FILES 10

pthread_mutex_t queueLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t outputLock = PTHREAD_MUTEX_INITIALIZER;

static queue q;

void* Th_Request(void* filename) 
{
    char hostname[SBUFSIZE];
    char errorstr[SBUFSIZE];

    FILE* inputfp = fopen(filename, 'r');	
    
    
    if(!inputfp){
        sprintf(errorstr, "Error Opening Input File: %s", filename);
        perror(errorstr);
    }
    
    char* dom = fscanf(inputfp, INPUTFS, hostname);
    printf("%s\n", filename);
    // Spin if queue is full
    while(queue_is_full(&q) == 1) 
    {
	pthread_mutex_lock(&queueLock);	
	queue_push(&q, (void*)dom);
	pthread_mutex_unlock(&queueLock);
    }
    
    return NULL;
}

void* Th_Resolve(void* filename)
{
	printf("%s", filename);
	
    /* Open Output File 
    outputfp = fopen(argv[(argc-1)], "w");
    if(!outputfp){
	perror("Error Opening Output File");
	return EXIT_FAILURE;
    }
    fclose(outputfp);
	*/
	return NULL;
}

int main(int argc, char* argv[]){

    /* Local Vars */

    int i;
    int NUM_THREADS = argc-2;

    // My Vars
    pthread_t requesterPool[NUM_THREADS];
    pthread_t resolverPool[NUM_THREADS];
    
    queue_init(&q, NUM_THREADS);

    for (i = 0; i < NUM_THREADS; ++i) 
    {
 	char* filename = argv[i + 1];
	fopen(filename, 'r');
	//pthread_create(&requesterPool[i], NULL, Th_Request, filename);
	//pthread_create(&resolverPool[i], NULL, Th_Resolve, (void*)(argv[NUM_THREADS-1]));
    }

    /* Check Arguments */
    if(argc < MINARGS){
	fprintf(stderr, "Not enough arguments: %d\n", (argc - 1));
	fprintf(stderr, "Usage:\n %s %s\n", argv[0], USAGE);
	return EXIT_FAILURE;
    }



    // Split here
    

    /* Close Output File */

    queue_cleanup(&q);

    pthread_exit(NULL);

    return EXIT_SUCCESS;
}
/*
void blah()
{

    for(i=1; i<(argc-1); i++){
	

	inputfp = fopen(argv[i], "r");
	if(!inputfp){
	    sprintf(errorstr, "Error Opening Input File: %s", argv[i]);
	    perror(errorstr);
	    break;
	}	


	while(fscanf(inputfp, INPUTFS, hostname) > 0){
	

	    if(dnslookup(hostname, firstipstr, sizeof(firstipstr))
	       == UTIL_FAILURE){
		fprintf(stderr, "dnslookup error: %s\n", hostname);
		strncpy(firstipstr, "", sizeof(firstipstr));
	    }
	

	    fprintf(outputfp, "%s,%s\n", hostname, firstipstr);
	}


	fclose(inputfp);
    }
}
*/
