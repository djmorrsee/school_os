#ifndef __MULTI_LOOKUP
#define __MULTI_LOOKUP

#include "queue.h"

// Global Output file
char* FILEOUT;

// Globabl Queue
queue q;

// Global Thread Flags
int runningRequests = 0;
int cancelReads = 0;

// Thread safe methods for pushing and popping elements from a queue
void PushElement(void* element);
void* PopElement();

// PThread Methods (not inherantly t-safe)
void* Th_Request(void* filename);
void* Th_Resolve();

// Thread safe method for appending data to OUTFILE
void WriteToFile(char* host, char* ip);

#endif
