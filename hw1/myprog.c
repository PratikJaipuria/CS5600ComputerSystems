Pratik Jaipuria

libckpt.c
==========
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <ucontext.h>
#include <fcntl.h>
#include <errno.h>

typedef struct MemoryRegion
	{
	  void *startAddr;
	  void *endAddr;
	  int isReadable;
	  int isWriteable;
	  int isExecutabl;
	} MemoryRegion;

void sighandler();
void write_context_to_ckpt_header();
long unsigned hex2int(char *);
ucontext_t* cp;
MemoryRegion mr;
int flag = 0;

long unsigned hex2int(char *str) {  
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
	return intVal;   
}

void write_context_to_ckpt_header(){
	FILE *fptr;
	int fp;
	char * line = NULL;
    size_t len = 0;
  	const char s[2] = " ";
    char *token;    
	fptr = fopen("/proc/self/maps", "r");
	fp = open("ckpt",O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
	if (fp == -1){	
		printf("open failed, read errno = %d\n", errno);
		exit(1);	
    }  
    ucontext_t* cp = malloc(sizeof(ucontext_t));
	int getcontext_success = getcontext(cp);
	if(getcontext_success ==-1)	{	
		printf("getContext failed");
		exit(1);
	}
	if(flag==1){
	 return;
	}
	flag++;	
	write(fp,(void *)cp,(size_t)sizeof(ucontext_t));
	if (fptr == NULL)
    {   printf("Cannot open file \n");
        exit(EXIT_FAILURE);
    }
    while (getline(&line, &len, fptr) != -1) {
        int n = 1;
        token = strtok(line, "-");    	    
	    while( token != NULL ) 
		   {  if(n==1){
		   	  	mr.startAddr = (void *)hex2int(token);	
		  		}
		   	  if(n==2){
		   	  	mr.endAddr = (void *)hex2int(token);	
		   	  	}
		   	  if(n==3){		if(token[0] == 'r'){
				   	  			mr.isReadable = 1;
				   	  		}else{
				   	  			mr.isReadable = 0;
				   	  		}
				   	  		
				   	  		if(token[1] == 'w'){
				   	  			mr.isWriteable = 1;
				   	  		}else{
				   	  			mr.isWriteable = 0;
				   	  		}

				   	  		if(token[2] == 'x'){
				   	  			mr.isExecutabl = 1;
				   	  		}else{
				   	  			mr.isExecutabl = 0;
			   	  			}
		   	  			}
                           if(n==7){
		   	  	break;
		   	   }
		   	  n++;
		   	  token = strtok(NULL, s);
		   }
		if(strstr(token, "[vsyscall]") != NULL){
			break;}
		if (mr.isReadable == 1) {
		    if (write(fp,&mr, sizeof(mr)) < 0){
		    	printf("MemoryRegion writing error\n");
		    }		
		    int writes = write(fp ,mr.startAddr,mr.endAddr - mr.startAddr);
		    if ( writes == -1 )
			{  printf("Error in Data Writing with error %d\n" , errno);
			}
                    }
	}   
    if (line)
        free(line);
    fclose(fptr);
    close(fp);
}
void sighandler()
{
   write_context_to_ckpt_header();
}
__attribute__ ((constructor))
static void myconstructor()
{	
    signal(SIGUSR2, sighandler);
}



restart.c
=========

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <ucontext.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>

long unsigned hex2int(char *str) {
  
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
    return intVal;   
}

typedef struct MemoryRegion
	{
	  void *startAddr;
	  void *endAddr;
	  int isReadable;
	  int isWriteable;
	  int isExecutabl;
	} MemoryRegion;

    
void my_restore_memory(char* ckpt, MemoryRegion mr){
	
    int unmap = munmap((void *)mr.startAddr, mr.endAddr - mr.startAddr);   	
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
                    break;
	        }	        
	        eofCheck = read(bin,&myStruct,sizeof(myStruct));
	        if(eofCheck < 0){
	        	printf("Read error in MemoryRegion \n");
	        	break;
	        }	        
	        if(myStruct.isReadable == 1){
	        	mapped = mmap(myStruct.startAddr, myStruct.endAddr - myStruct.startAddr , PROT_READ | PROT_WRITE |PROT_EXEC , MAP_FIXED | MAP_PRIVATE| MAP_ANONYMOUS , -1, 0);
				if(mapped == MAP_FAILED){
				printf("MAP_FAILED for checkpoint image data with error %d\n",errno);
				}

				eofCheck = read(bin, mapped, myStruct.endAddr - myStruct.startAddr);
				if(eofCheck < 0){
				printf("Read error in Data Size with error code %d \n", errno);
				break;
				}                  
                }       
    }
    close(bin);
	if(setcontext(cp)==-1){
		printf("Set Context Failed\n");
	}
}


void main(int argc, char * argv [])
{	void* stackMemory = (void *)0x5300000;
	void* mapped;
	FILE *fptr;
	char * line = NULL;
	size_t len = 0;
	MemoryRegion mr;
	char *token;
	const char s[2] = " ";  	
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
		   	  	mr.startAddr = (void*)hex2int(token);	
		   	  }
		   	  if(n==2){
		   	  	mr.endAddr = (void *)hex2int(token);	
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
   mapped = mmap(stackMemory, mr.endAddr - mr.startAddr , PROT_READ | PROT_WRITE | PROT_EXEC, MAP_FIXED| MAP_PRIVATE | MAP_ANONYMOUS , -1, 0);
   if (mapped == MAP_FAILED){
    	printf("Map Failed for creating stack for current program with error code %d\n", errno);
	}
   void* endofStack = mapped + (mr.endAddr - mr.startAddr);
   asm volatile ("mov %0,%%rsp" : : "g" (endofStack) : "memory"); 
   my_restore_memory(argv[1], mr);   
}

myprog.c
========
#include<stdio.h>
#include<unistd.h>

int main()
{
	int i = 0;
	while(1){
		printf("%d ",i);
		sleep(1);
		i++;
		fflush(stdout);
	}
	return 0;
}


