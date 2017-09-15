#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void sighandler();

__attribute__ ((constructor))
void myconstructor()
{
	printf("DMTCP loaded...\n");
 	signal(SIGUSR2, sighandler);
}

void sighandler()
{
   printf("Caught signal coming out...\n");
   
}