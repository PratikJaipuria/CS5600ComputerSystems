#include "malloc.h"

header_t *head;
header_t *tail;

//List of free buddy blocks
// buddy_blk_hdr[0]  ==> 8 bytes 
// buddy_blk_hdr[1]  ==> 16 bytes 
// buddy_blk_hdr[2]  ==> 32 bytes
//				.
//				.
//				.
// buddy_blk_hdr[9]  ==> 4096 bytes 

header_t *buddy_blk_hdr[CHUNK_SIZE];

size_t chunk_size[] = {8,16,32,64,128,256,512,1024,2048,4096};
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
	reqSize = find_apt_heap(reqSize + (sizeof(header_t) * 2));
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
	head -> size = heapSize;
	head -> isAvailable = 1;
	head -> prev = NULL;
	head -> next = NULL;

	buddy_blk_hdr[9] = head;
	// This should be total size of heap / just the requested one ??
	// buddy_blk_hdr[9] -> size = heapSize; //HEAP_SIZE;
	// buddy_blk_hdr[9] -> isAvailable = 1;
	// buddy_blk_hdr[9] -> prev = NULL;
	// buddy_blk_hdr[9] -> next = NULL;

	tail = (void*)head + heapSize - sizeof(header_t);

	tail -> size = heapSize;
	tail -> isAvailable = 1;
	tail -> prev = NULL;
	tail -> next = NULL;



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


// header_t *get_free_block(size_t size)
// {

// 	header_t *curr = head;
// 	while(curr){
// 		if(curr -> isAvailable == 0 && curr -> size >= size){
// 				return curr;
// 		}
// 		curr = curr -> next;
// 	}
// 	return NULL;
// }


void enque(header_t** buddy_blk ,int index ,  header_t* mem_addr)
{
	if(buddy_blk[index] == NULL){
		buddy_blk[index] = mem_addr;
		mem_addr -> prev = NULL;
		mem_addr -> next = NULL;
		return;
	}

	mem_addr -> next = buddy_blk[index];
	mem_addr -> prev = NULL;
	buddy_blk[index] -> prev = mem_addr;	
	buddy_blk[index] = mem_addr; 
	return;	
}

void* deque(header_t** buddy_blk ,int index)
{
	if(buddy_blk[index]){
	
	header_t * ret = buddy_blk[index];
	buddy_blk[index] = ret -> next;
	
	if(buddy_blk[index]  != NULL)
	{
		buddy_blk[index] -> prev = NULL;	
	}
	// mem_addr -> next = NULL;
	// mem_addr -> prev = NULL;
		return (void*)ret;
	}
	else{
		
		return NULL;
	}
}

// void deque_at_loc(header_t** buddy_blk, int index, header_t * mem_addr){
// 	if(buddy_blk[index] == NULL){
// 		return;
// 	}

// 	header_t temp = buddy_blk[index] ;
// 	while(temp)
// 	{
// 		if(temp -> next == mem_addr){
// 			temp = mem_addr -> next;
// 			if(mem_addr -> next != NULL){
// 				mem_addr -> next -> prev = temp;

// 			}
// 		}

// 		temp = temp->next;
// 	}

// }

header_t* split(header_t** buddy_blk , int index, size_t size)
{	

	size_t remaining = (buddy_blk[index] -> size);
	header_t * h1,* h2, *f1, *f2;
	if(remaining < size){
		return buddy_blk_hdr[index];
	}
	while(remaining > size ){
		remaining = remaining /2;
		
		h1 = buddy_blk[index];
		h1 -> size = remaining;
		h1 -> isAvailable = 1;
		
		f1 = (header_t *)((void *)buddy_blk[index] + remaining - sizeof(header_t));
		f1 -> size = remaining;
		f1 -> isAvailable = 1;
		
		index = get_chunk_sector(remaining);
		
		enque(buddy_blk,index,h1);

		h2 = (header_t *)((void *)buddy_blk[index] + remaining);
		h2 -> size = remaining;
		h2 -> isAvailable = 1;
		
		f2 = (header_t *)((void *)h2 + remaining - sizeof(header_t));
		f2 -> size = remaining;
		f2 -> isAvailable = 1;
			
		//if(remaining > size)
		buddy_blk[index] = h2;


	} 
	return h2;
}

header_t* find_fit(size_t reqSize)
{
	size_t adjusted_size = reqSize + (2 * sizeof(struct header_t));
	int index = get_chunk_sector(adjusted_size);
	if(buddy_blk_hdr[index] != NULL){
		void* retAddr = buddy_blk_hdr[index];
		buddy_blk_hdr[index] -> isAvailable = 0;
		deque(buddy_blk_hdr,index);
		return retAddr;
	}

	for(int i=0; i < CHUNK_SIZE ; i++){
		if( buddy_blk_hdr[i] != NULL && buddy_blk_hdr[i] -> size > adjusted_size ){
				return split(buddy_blk_hdr, i,adjusted_size);
				// return addr;
		}	

	}
}
void* malloc(size_t size)
{
	header_t* addr = find_fit(size);
	return (void*) (addr + 1);
}

void free(void * block)
{
	header_t* hdr_addr =  (header_t * )block - 1; 
	int index = get_chunk_sector(hdr_addr -> size);	
	enque(buddy_blk_hdr, index ,hdr_addr);

}

void *calloc(size_t nmemb, size_t size){
	return malloc(nmemb * size);
}

void *realloc(void *ptr, size_t size)
{
	free(ptr);
	return malloc(size);
}
// void free(void * block){
// 	header_t *header, *temp;
// 	void *prgmbrk;
	
// 	if(!block){
// 		return;
// 	}

// 	pthread_mutex_lock(&global_malloc_lock);
// 	header = (header_t *)block -1;
// 	prgmbrk = sbrk(0);

// 	if(prgmbrk == header->size + (char*) block){

// 		if(head != tail){
// 			temp = head;

// 			while(temp){
// 				if(temp -> next != tail){
// 					temp -> next = NULL;
// 					tail = temp;	
// 				}
				
// 				temp = temp -> next;
// 			}

// 		}else{
// 				head = NULL;
// 				tail = NULL;
// 		}

// 		sbrk(0 - header->size - sizeof(header_t));
// 		pthread_mutex_unlock(&global_malloc_lock);
// 		return;	
// 	}
	
// 	header -> isAvailable = 1;
// 	pthread_mutex_unlock(&global_malloc_lock);

// }