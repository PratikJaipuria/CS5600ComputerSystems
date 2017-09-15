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