#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>

void verify(int argc, int n){
	if(optind != argc){
	   	fprintf(stderr, "Wrong argument");
        exit(2);
    }
    if(argc < 5){
    	fprintf(stderr, "Missing argument");
        exit(2);
    }
    if(argc > 5){
    	fprintf(stderr, "Invalid argument");
        exit(2);
    }
    
    if( n < 0){
    	printf("Bad argument, please use positive values for n \n"); 
        exit(EXIT_FAILURE);
    }

}

long unsigned computeFactorial(long unsigned n){
	long unsigned fact = 1;	
    while(n>0){
   		fact = fact * n;
    	n--;    
    }
    return fact;

}

int main(int argc, char *argv[]) {
    int option = 0;
    double result;
    int x = -1, n = -1;
    extern int optopt, opterr;
    
    while ((option = getopt(argc, argv,"x:n:")) != -1) {
        switch (option) {
        	case 'x' : 
             		x = atol(optarg);
                	break;
             case 'n' : 
             		n = atol(optarg);
                	break;
             case ':':       
                    fprintf(stderr,"Option -%c requires an operand\n", optopt);
                    break;
        	 case '?':
                    fprintf(stderr,"Unrecognized option: -%c\n", optopt);
                    break;
             default: 
             	printf("Wrong arguments, please use -x and -n to pass parameters\n"); 
                exit(EXIT_FAILURE);
        }
    }
    
    verify(argc,n);
    result = pow(x,n) / computeFactorial(n);

    if (isatty (1))
    	// fprintf(stdout, "%f\n", result);
    fprintf (stdout, "x^n / n! :  %f\n", result);
    // write(STDOUT_FILENO, &result, sizeof(result));	
	else
    fprintf (stdout, "%f",result);
    // write(STDOUT_FILENO, result, sizeof( double));	
    return 0;
}