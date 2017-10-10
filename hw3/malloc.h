#include <sys/mman.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

#define HEAP_SIZE 1
#define CHUNK_SIZE 10

// global declaration of mutex 
pthread_mutex_t global_malloc_lock;


// defination of header structure
typedef struct header_t{
	size_t size;
	unsigned isAvailable;
	struct header_t *next;
	struct header_t *prev;
} header_t;








