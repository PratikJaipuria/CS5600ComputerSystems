#include "malloc.h"

header_t *head, *tail;

//List of free buddy blocks
// buddy_blk_hdr[0]  ==> 8 bytes 
// buddy_blk_hdr[1]  ==> 16 bytes 
// buddy_blk_hdr[2]  ==> 32 bytes
//				.
//				.
//				.
// buddy_blk_hdr[9]  ==> 4096 bytes 
header_t *buddy_blk_hdr[10];


//Request memory from heap of size 4K bytes


size_t find_apt_heap(size_t reqSize){
	int reqHeap = 1;
	while(reqSize > sysconf(_SC_PAGESIZE)* reqHeap){
		reqHeap++;
	}
	return sysconf(_SC_PAGESIZE)* reqHeap;
}

void *request_memory(size_t reqSize)
{

	void *new_mem = sbrk(reqSize);
	if(new_mem  == (void *) -1)
        {
            errno = ENOMEM;
            printf("\nsbrk failed with error : %d",errno);
            return NULL;
        }	
        return new_mem;
}


// Initialize heap for the first time and later per requirement
void alloc_memory(size_t reqSize)
{
	size_t heapSize = find_apt_heap(reqSize);
	head = request_memory(heapSize);
	buddy_blk_hdr[9] = head;
	// This should be total size of heap / just the requested one ??
	buddy_blk_hdr[9] -> size = heapSize; //HEAP_SIZE;
	buddy_blk_hdr[9] -> isAvailable = 1;
	buddy_blk_hdr[9] -> prev = NULL;
	buddy_blk_hdr[9] -> next = NULL;
	

}

void init(){
	alloc_memory(1);
	int i;
	for(i=0; i < 9 ;i++){

		buddy_blk_hdr[i] = NULL;
		// buddy_blk_hdr[i] -> size = 0; 
		// buddy_blk_hdr[i] -> isAvailable = 1;	
		// buddy_blk_hdr[i] -> prev = NULL;
		// buddy_blk_hdr[i] -> next = NULL;
	}
}

__attribute__ ((constructor))
static void myconstructor()
{	
	init();
}


// Identify the chunk size that can fit the request -> BEST FIT
int get_chunk_sector(size_t size){

	int ret_val = 0;
	int suitable_chunk = 8;
	while(suitable_chunk < size)
	{
		suitable_chunk = suitable_chunk * 2;
		ret_val++;
	}
	return ret_val;
}


header_t *get_free_block(size_t size)
{

	header_t *curr = head;
	while(curr){
		if(curr -> isAvailable == 0 && curr -> size >= size){
				return curr;
		}
		curr = curr -> next;
	}
	return NULL;
}

void* malloc(size_t size)
{
	init();
	int index = get_chunk_sector(size);

	buddy_blk_hdr[index] = buddy_blk_hdr[9];
	buddy_blk_hdr[index] -> size = size; //HEAP_SIZE;
	buddy_blk_hdr[index] -> isAvailable = 1;
	buddy_blk_hdr[index] -> prev = NULL;
	buddy_blk_hdr[index] -> next = NULL;

	buddy_blk_hdr[9] -> size = (buddy_blk_hdr[9] -> size) - size; 

	return buddy_blk_hdr[index];
	// size_t total_size;


}

void free(void * block){
	header_t *header, *temp;
	void *prgmbrk;
	
	if(!block){
		return;
	}

	pthread_mutex_lock(&global_malloc_lock);
	header = (header_t *)block -1;
	prgmbrk = sbrk(0);

	if(prgmbrk == header->size + (char*) block){

		if(head != tail){
			temp = head;

			while(temp){
				if(temp -> next != tail){
					temp -> next = NULL;
					tail = temp;	
				}
				
				temp = temp -> next;
			}

		}else{
				head = NULL;
				tail = NULL;
		}

		sbrk(0 - header->size - sizeof(header_t));
		pthread_mutex_unlock(&global_malloc_lock);
		return;	
	}
	
	header -> isAvailable = 1;
	pthread_mutex_unlock(&global_malloc_lock);

}