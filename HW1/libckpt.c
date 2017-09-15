#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void sighandler();

__attribute__ ((constructor))
void myconstructor()
{
	FILE *fptr;
	char ch;
	printf("DMTCP loaded...\n");
	fptr = fopen("/proc/self/maps", "r");
	
    
    if (fptr == NULL)
    {
        printf("Cannot open file \n");
        exit(0);
    }
    ch = fgetc(fptr);
    while (ch != EOF)
    {
        printf ("%c", ch);
        ch = fgetc(fptr);
    }
    fclose(fptr);
	
 	signal(SIGUSR2, sighandler);
}

void sighandler()
{
   printf("Caught signal coming out...\n");
   
}