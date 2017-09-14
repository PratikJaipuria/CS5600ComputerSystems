#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

/**
 * hex2int
 * take a hex string and convert it to a 32bit number (max 8 hex digits)
 */
void* hex2int(char *str) {
    

// char* str = "8BC";
   unsigned long intVal = 0;
   int i = 0;
   
   while(1){
   	 // printf("%c", str[i]);
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

int main(int argc, char * argv []){

	typedef struct MemoryRegion
	{
	  void *startAddr;
	  void *endAddr;
	  int isReadable;
	  int isWriteable;
	  int isExecutabl;
	} MemoryRegion;
	// system("cat /proc/self/maps > Result.txt");
	MemoryRegion mr;
    FILE * fp ,*fptr;
    char * line = NULL;
    char * pids = NULL;
    size_t len = 0;
    size_t len_pid = 0;
	int i = 0;    
    const char s[2] = " ";
    char *token;
    void *startAddr,*endAddr;
    long unsigned countRW = 0, countRO = 0;
    if(argc < 2){
    	pids = "self/maps";
    }else{
    	
    	pids = argv[1];	
    	strcat(pids,"/maps"); 	
    }
    
    fptr = fopen("Result","wb+");
   	
    char command[50] = "cat /proc/";
    char strC[80];
    strcpy(strC,command);
	strcat(strC,pids);
	 
    fp = popen(strC, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while (getline(&line, &len, fp) != -1) {
        int n = 1;
        token = strtok(line, "-");
    	    
	    while( token != NULL ) 
		   {
		   	  if(n==4){
		   	  	break;
		   	  }
		   	  if(n==1){
		   	  	mr.startAddr = hex2int(token);	
		   	  	// startAddr =  hex2int(token);
		   	  	// mr.startAddr = startAddr;
		   	  	// printf("Printing pointer startAddr %lu\n", hex2int(token));

		   	  }
		   	  if(n==2){
		   	  	mr.endAddr = hex2int(token);	
		   	  	// endAddr =  hex2int(token);
		   	  	// mr.endAddr = endAddr;
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

		   	  		if(token[0] == 'r' && token[1] == 'w'){
		   	  			countRW += mr.endAddr - mr.startAddr;
		   	  			
		   	  		}else if(token[0] == 'r' && token[1] == '-'){
		   	  			countRO += mr.endAddr - mr.startAddr;
		   	  			
		   	  		}

		   	  		if(token[2] == 'x'){
		   	  			mr.isExecutabl = 1;
		   	  		}else{
		   	  			mr.isExecutabl = 0;
		   	  		}
		   	  }
		   	  n++;
		   	  token = strtok(NULL, s);

		   }
		

		// printf("Memory Range Info :\n%p\n", mr.startAddr);
		// printf("%p\n", mr.endAddr);
		// printf("%d\n", mr.isReadable);
		// printf("%d\n", mr.isWriteable);
		// printf("%d\n", mr.isExecutabl);

		
		// fwrite(&mr, sizeof(struct MemoryRegion), 1, fptr);	

		MemoryRegion *object=malloc(sizeof(MemoryRegion));
		// strcpy(object->day,"Good day");
		object->startAddr = mr.startAddr;
		object->endAddr = mr.endAddr;
		object->isReadable = mr.isReadable;
		object->isWriteable = mr.isWriteable;
		object->isExecutabl = mr.isExecutabl;
		// FILE * file= fopen("output", "wb");
		if (fptr != NULL) {
		    fwrite(object, sizeof(struct MemoryRegion), 1, fptr);
		    // fclose(file);
}

    }

    
    printf("ReadOnly = %luKB \n",countRO/1024);
    printf("Read&Write = %luKB\n",countRW/1024);

    

    
    fclose(fp);
    fclose(fptr);
    if (line)
        free(line);
    
    FILE * bin;
    MemoryRegion myStruct;
    bin = fopen("Result", "rb");
    long unsigned sizeRO=0,sizeRW = 0;


    while(1) {
        fread(&myStruct,sizeof(struct MemoryRegion),1,bin);
        if(feof(bin)!=0)
            break;
        
        // printf("Reading: \n startAddr%p\n" ,myStruct.startAddr);
        // printf("endAddr%p\n" ,myStruct.endAddr);
        // printf("isReadable%d\n",myStruct.isReadable );
        // printf("isWriteable%d\n",myStruct.isWriteable );
        // printf("isExecutabl%d\n",myStruct.isExecutabl );


        if(myStruct.isReadable ==1 && myStruct.isWriteable==1){
        	sizeRW += myStruct.endAddr - myStruct.startAddr;

        }else if(myStruct.isReadable ==1 && myStruct.isWriteable==0){
        	sizeRO += myStruct.endAddr - myStruct.startAddr;	
        }
        // printf("%lu" ,&(myStruct.endAddr));
        
    }

    printf("ReadOnly = %luKB\n",sizeRO/1024);
    printf("Read&Write = %luKB \n",sizeRW/1024);

    fclose(bin);

    exit(EXIT_SUCCESS);

	return 0;
}