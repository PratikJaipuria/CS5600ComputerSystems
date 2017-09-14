#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    system("ps -A -o pid > pid.txt");
    return 0;
}