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
ucontext_t* ucp;
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
	if (fp == -1)
	{	
		printf("open failed, read errno = %d\n", errno);
		exit(1);
    }
    
   
    ucontext_t* ucp = malloc(sizeof(ucontext_t));
	int r = getcontext(ucp);
	if(r ==-1)
	{	
		printf("getContext failed");
		exit(1);
	}
	if(flag==1){
	 return;
	}
	flag++;
	
	write(fp,(void *)ucp,(size_t)sizeof(ucontext_t));

	if (fptr == NULL)
    {
        printf("Cannot open file \n");
        exit(EXIT_FAILURE);
    }

    while (getline(&line, &len, fptr) != -1) {
        int n = 1;
        token = strtok(line, "-");
    	    
	    while( token != NULL ) 
		   {
		  
		   	  if(n==1){
		   	  	mr.startAddr = (void *)hex2int(token);	
		  
		   	  }
		   	  if(n==2){
		   	  	mr.endAddr = (void *)hex2int(token);	
		   	  }
		   	  if(n==3){
				   	  		if(token[0] == 'r'){
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
			printf("Found Match!!!!\n");
			break;
		}

		if (mr.isReadable == 1 ) {
					
		    if (write(fp,&mr, sizeof(mr)) < 0){
		    	printf("MemoryRegion writing error\n");
		    }
		
		    int writes = write(fp ,mr.startAddr,mr.endAddr - mr.startAddr);
		    if ( writes == -1 )
			{
  	    			printf("Error in Data Writing with error %d\n" , errno);
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
   printf("Caught signal coming out...\n");
   write_context_to_ckpt_header();
     
}

__attribute__ ((constructor))
static void myconstructor()
{	
	printf("DMTCP loaded...\n");
	signal(SIGUSR2, sighandler);
}



