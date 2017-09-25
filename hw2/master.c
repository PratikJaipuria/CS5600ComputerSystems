#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <sys/epoll.h>

#define READ_END 0
#define WRITE_END 1

struct option longopts[] = {
   { "worker_path",    required_argument, NULL, 'f' },
   { "num_workers",    required_argument, NULL, 'w' },
   { "wait_mechanism", required_argument, NULL, 'm' },   
   { 0, 0, 0 }
  };


int Integer2String(int value,char *ptr)
     {
        int temp;
        if(ptr==NULL)
            return 0;   
        
        if(value==0)
        {   
            *ptr='0';
            return 1;
        }

        if(value<0)
        {
            value*=(-1);    
            *ptr++='-';
            
        }
        for(temp=value;temp>0;temp/=10,ptr++);
        *ptr='\0';
        for(temp=value;temp>0;temp/=10)
        {
            *--ptr=temp%10+'0';
            
        }
        return 0;
     }



void verify(int argc){
	if(optind != argc){
	   	fprintf(stderr, "Wrong argument");
        exit(2);
    }
    if(argc < 11){
    	fprintf(stderr, "Missing argument");
        exit(2);
    }
    if(argc > 11){
    	fprintf(stderr, "Invalid argument");
        exit(2);
    }
    
    
}



int main(int argc, char *argv[]){
	char *worker_file, *mechanism;
	int num_worker;
	int option = 0;
    char* x;
    char* n;
                     
	while ((option = getopt_long(argc, argv, "x:n:f:", longopts, NULL)) != -1) {
        switch (option) {
        	case 'x' : 
             		x = optarg;
                	break;
             case 'n' : 
             		n = optarg;
                	break;
             case 'f' : 
             		worker_file = optarg;             		
             		break; 	
             case 'm' : 
             		mechanism = optarg;             		 
             		break; 
             case 'w' : 
             		num_worker = atoi(optarg);             		
             		break; 
             case '?':       
             default: 
             	printf("Wrong arguments, please use appropriate parameters\n"); 
                exit(EXIT_FAILURE);
        }
    }

    verify(argc);
    int processesCount =  num_worker;
	int nCount = atoi(n);    
    pid_t pid[nCount+1];  
    int fd[nCount][2];
    int proceed = 0;
    int counter = 0;
    int track_fds = 0;
    
	   if(strcmp(mechanism, "epoll") == 0){
	   		

	   		for(int i=0 ; i < nCount; i++){
					pipe(fd[i]);
			}
	   		int efd;
	   		struct epoll_event eventN[nCount];
  			struct epoll_event events[processesCount];

  			efd =  epoll_create(1);	
  			if (efd == -1) {
               perror("epoll_create");
               exit(EXIT_FAILURE);
           }
           int proceed  = 0;
           int counter = 0;
           int track = 0;
           double epollResult = 0;
           // events=malloc(sizeof(struct epoll_eventN) * (n+1));

           for(int i=0 ; i < nCount ; i++){

           		eventN[i].events = EPOLLIN;
           		eventN[i].data.fd = fd[i][READ_END];
           		
           		// checkedFD[i] = 0;
           		if (epoll_ctl(efd, EPOLL_CTL_ADD,fd[i][READ_END],&eventN[i]) == -1) {
                           perror("epoll_ctl: error");
                           exit(EXIT_FAILURE);
                       }

           }	

           for (int j = 0 ; j < processesCount ; j++){

           					
           						// pipe(fd[j]);
							    int pid = fork();
							    if (pid == -1){
							    	printf("Filure in parent %d\n", errno);
							    }	
							    char num[100];
							    Integer2String(j, num);
							  	if(pid == 0)
							    {
							            close(fd[j][READ_END]);
							            close(STDOUT_FILENO);
							    		// dup2(fd[counter][READ_END], STDIN_FILENO);
							        	dup2(fd[j][WRITE_END], STDOUT_FILENO);
							        	execl(worker_file,worker_file,"-x",x,"-n",num, NULL);

							    }
							    // checkedFD[j] = 0;
							    // counter++;
							    proceed++;
							    
           		}

           track_fds=0;
           int readyProcesses;

           while(counter  <= nCount){

           		readyProcesses = epoll_wait(efd, events, processesCount, -1 );
           		if(readyProcesses == -1 ){
           			perror("Epoll wait");	
           		}

           		for(int k = 0 ; k < readyProcesses ; k++ ){
           						// epoll_ctl(efd, EPOLL_CTL_DEL, events[k].data.fd, NULL);
           						char * readWorker = malloc(sizeof(double));
			       				read(fd[counter][READ_END],readWorker, sizeof(double));
						    	// printf("%d %s\n",counter,readWorker);
			       				epollResult += atof(readWorker);
								counter++;
								track++;
           				}

           			
           			
				for (int j = 0 ; j < processesCount ; j++){

           					
           						if(counter > nCount){
							 		break;  	
							    }
							    int pid = fork();
							    if (pid == -1){
							    	printf("Filure in parent %d\n", errno);
							    }	
							    char num[100];
							    Integer2String(proceed, num);
							  	if(pid == 0)
							    {
							            close(fd[j][READ_END]);
							            close(STDOUT_FILENO);
							    		// dup2(fd[proceed][READ_END], STDIN_FILENO);
							        	dup2(fd[proceed][WRITE_END], STDOUT_FILENO);
							        	execl(worker_file,worker_file,"-x",x,"-n",num, NULL);

							    }
							    proceed++;
							    track++;						    
							   
							 
							    
           		}
           	}

           printf("Epoll result = %f\n", epollResult);

        }

        if(strcmp(mechanism,"select") == 0){

	int processesCount =  num_worker;
  int nCount = atoi(n);    
    pid_t pid[nCount+1];  
    int fd[nCount+1][2];

    for(int i=0 ; i < processesCount; i++){

      pipe(fd[i]);
      pid[i] = fork();
      int status;
      if (pid[i] == -1){
        printf("Filure in parent %d\n", errno);
      } 
      char num[1000];
      Integer2String(i, num);
      if(pid[i] == 0)
      {
              close(fd[i][READ_END]);
              close(STDOUT_FILENO);
            dup2(fd[i][WRITE_END], STDOUT_FILENO);
            execl(worker_file,worker_file,"-x",x,"-n",num, NULL);
      }
    
    }
    int proceed = 0;
    int counter = processesCount;
    // select_wait(processesCount,fd,nCount,worker_file,x, pid);
    fd_set rfds;
    struct timeval tv;
    int retval;
    int checkedFD[nCount];
  /* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO(&rfds);   
    int maxfd = 0;
     for (int j=0; j < processesCount; j++) {
          
          FD_SET(fd[j][READ_END], &rfds);
        maxfd = (maxfd>fd[j][READ_END])?maxfd:fd[j][READ_END];
        checkedFD[j] = 0;
    }

     /* Wait up to five seconds. */
     tv.tv_sec = 5;
     tv.tv_usec = 0;
     int track_fds = 0;
     double result = 0;
     while(proceed <= nCount ){

     retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);
       /* Don't rely on the value of tv now! */
       if (retval == -1)
           perror("select()");
       else if (retval){
          // printf("Data is available now.\n");
          for (int k = 0 ; k < processesCount ; k++){

            if(FD_ISSET(fd[k][READ_END], &rfds)){
              char * readWorker = malloc(sizeof(double));
              read(fd[k][READ_END],readWorker, sizeof(double));

            printf("%d %s\n",k,readWorker);
            result += atof(readWorker); 
            checkedFD[k] = 1;
            track_fds++;
       //    
            }
          }
            FD_ZERO(&rfds); 

            for(int l=0;l<processesCount;l++){
          if(checkedFD[l]==0){
            FD_SET(fd[l][READ_END],&rfds);
          }
          else{
            
            pipe(fd[l]);
              pid[l] = fork();
              // printf("Created new process \n");
              // int status;
              if (pid[l] == -1){
                printf("Filure in parent %d\n", errno);
              } 
              char num[100];
              Integer2String(counter, num);
              if(pid[l] == 0)
              {
                      // printf("Executing new process\n");
                      close(fd[l][READ_END]);
                      close(STDOUT_FILENO);
                    dup2(fd[l][WRITE_END], STDOUT_FILENO);
                    execl(worker_file,worker_file,"-x",x,"-n",num, NULL);

              }
              FD_ZERO(&rfds); 
              checkedFD[l] = 0;
            counter++;
            proceed++;
            track_fds--;
              tv.tv_sec = 5;
              tv.tv_usec = 0;
          }
        }
        for (int j=0; j < processesCount; j++) {
                  FD_SET(fd[j][READ_END], &rfds);
              maxfd = (maxfd>fd[j][READ_END])?maxfd:fd[j][READ_END];
                
            }

        }
    else
           printf("No data within five seconds.\n");

       // exit(EXIT_SUCCESS);

     }
     printf("Select Answer = %f\n", result);
       }




return 0;

	   }
	   	   
   