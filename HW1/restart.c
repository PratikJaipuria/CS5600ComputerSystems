#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <ucontext.h>
#include <fcntl.h>
#include <sys/mman.h>


void* hex2int(char *str) {
    
   str = str + 2 ;
	
   unsigned long intVal = 0;
   int i = 0;
   
   while(1){
   	
   	 switch(str[i]){
   	 	case 'a':
   	 		intVal += 10; 
   	 		break;
   	 	case 'b':
   	 		intVal += 11;	
   	 		break;
   	 	case 'c':
   	 		intVal += 12;	   	 		
   	 		break;	
   	 	case 'd':
   	 		intVal += 13;    	 		
   	 		break;
   	 	case 'e':
   	 		intVal += 14;	   	 		
   	 		break;
   	 	case 'f':
   	 		intVal += 15;	   	 		
   	 		break;		
   	 	default:
   	 		intVal += (int)str[i] - 48;	
   	 		
   	 }
 	 
   	 i++;
   	 if(str[i] == '\0'){
   	 	break;
   	 }
   	 intVal = intVal << 4;
   }
   void* p ;
   p = (long unsigned *)intVal;	
   return p;
}

typedef struct MemoryRegion
	{
	  void *startAddr;
	  void *endAddr;
	  int isReadable;
	  int isWriteable;
	  int isExecutabl;
	} MemoryRegion;


void my_restore_memory(MemoryRegion mr, char* ckpt){
	
	int unmap = munmap(mr.startAddr, mr.endAddr - mr.startAddr);
   	if(unmap == -1){
   	printf("Unmap current stack failed \n");
    }

	void * mapped;
	int bin;
    void *data;
	ucontext_t readContext, *cp = &readContext;
    MemoryRegion myStruct;
    bin = open(ckpt, O_RDONLY);
    
    if(bin == -1){
    	printf("Checkpoint image failed\n");
    }
    
    

    int eofCheck = 0;
    eofCheck = read(bin, cp,sizeof(readContext));
    if(eofCheck == 0){
    	printf("Read error in Context \n");
     }
    
    while(1) {
    
	        if(eofCheck == 0){
	        	printf("End of File Reached\n");
	        	break;
	        }
	        
	        eofCheck = read(bin,&myStruct,sizeof(myStruct));
	        
	        if(eofCheck < 0){
	        	printf("Read error in MemoryRegion \n");
	        	break;
	        }
	        
			mapped = mmap(myStruct.startAddr, myStruct.endAddr - myStruct.startAddr , PROT_READ | PROT_WRITE | PROT_EXEC , MAP_PRIVATE| MAP_FIXED | MAP_ANONYMOUS , -1, 0);
			if(mapped == MAP_FAILED){
				printf("MAP_FAILED for checkpoint image data \n");
			}

			eofCheck = read(bin, mapped, myStruct.endAddr - myStruct.startAddr);
			if(eofCheck < 0){
				printf("Read error in Data Size \n");
				break;
			}	

		}

    close(bin);

	if(setcontext(cp)==-1){
		printf("Set Context Failed\n");
	}
}

void main(int argc, char * argv [])
{
	void* stackMemory = (void *)0x5300000;
	char * mapped;
	FILE *fptr;
	char * line = NULL;
	size_t len = 0;
	MemoryRegion mr;
	char *token;
	const char s[2] = " ";
   	printf("Restoring...\n");

   if(argc < 2){
    	printf("Context file not in the input\n");
    }
	   
	fptr = fopen("/proc/self/maps", "r");
    if (fptr == NULL)
    {
        printf("Cannot open file \n");
        exit(EXIT_FAILURE);
    }

   
    while (getline(&line, &len, fptr) != -1)
     {
        int n = 1;    	    
        token = strtok(line, "-");
	    while( token != NULL ) 
		   {		
			  if(n==1){
		   	  	mr.startAddr = hex2int(token);	
		   	  }
		   	  if(n==2){
		   	  	mr.endAddr = hex2int(token);	
		   	  }

		   	  if(n==7){
		   	  	if(strstr(token, "stack") != NULL)
		   	  	break;
		   	  }

		   	  n++;
		   	  token = strtok(NULL, s);

		   }
	}


    fclose(fptr);
    if (line)
        free(line);
   mapped = mmap(stackMemory, mr.endAddr - mr.startAddr , PROT_READ | PROT_WRITE | PROT_EXEC, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS , -1, 0);
    
    if (mapped == MAP_FAILED){
    	printf("Map Failed for creating stack for current program \n");
	}

   void* endofStack = mapped + (mr.endAddr - mr.startAddr);

   asm volatile ("mov %0,%%rsp" : : "g" (endofStack) : "memory"); 

   my_restore_memory(mr, argv[1]);
   
}



