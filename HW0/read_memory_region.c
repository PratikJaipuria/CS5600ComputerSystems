#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

void main(){


    typedef struct MemoryRegion
    {
      void *startAddr;
      void *endAddr;
      int isReadable;
      int isWriteable;
      int isExecutabl;
    } MemoryRegion;

    FILE * bin;
    MemoryRegion myStruct;
    bin = fopen("Result", "rb");
    long unsigned sizeRO=0,sizeRW = 0;

    printf("\nStart Address       \t End Address \t isReadable \t isWriteable \t isExecutable\n");
    while(1) {
        fread(&myStruct,sizeof(struct MemoryRegion),1,bin);
        if(feof(bin)!=0)
            break;

        
        printf("\n%p   " ,myStruct.startAddr);
        printf("\t%2p\t\t" ,myStruct.endAddr);
        printf("%d\t\t",myStruct.isReadable );
        printf("%d\t\t",myStruct.isWriteable );
        printf("%d",myStruct.isExecutabl );


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


}
