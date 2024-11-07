#include<stdio.h>        // printf scanf
#include<unistd.h>       // sleep
#include <sys/types.h>   // pid_t
#include<sys/wait.h>     // wait
//#include <signal.h>      // signal kill
#include<stdlib.h>       // exit

void main (void)
{   
	pid_t child_pid[100], wpid;
	int status = 0;


	for (int id=0; id<5; id++) {
    	if ((child_pid[id] = fork()) == 0) {
        	   sleep((5-id)*2);
        	   printf("\n\t Child running %d",getpid());
        	   exit(id+1);  //child code
    	}
	}

    //wpid = waitpid(child_pid[1],&status,0); // [A] this way parent WAITS for SPECIFIED child process to exit
	 //wpid = waitpid(-1,&status,0); // [B] this way parent WAITS for ANY_ONE of the child process to exit
    //wpid = waitpid(child_pid[1],&status,WNOHANG); // [C] this way parent RETURNS IMMEDIATELY if child with specified pid has not terminated
    //wpid = waitpid(-1,&status,WNOHANG); // [D] this way parent RETURNS IMMEDIATELY if none of the child has terminated
    while((wpid=waitpid(-1,&status,0)) > 0); // [E] this way parent WAITS for ALL of it's the child process to exit
    
	printf("\n\t Parent synchronized with child death (returned pid %d ....returned status %d... )",wpid,status);
	
}