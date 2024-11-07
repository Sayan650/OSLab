#include<stdio.h>        // printf scanf
#include<unistd.h>       // slepp
#include <sys/types.h>   // pid_t
#include<sys/wait.h>     // wait
#include <signal.h>      // signal kill
#include<stdlib.h>       // exit

void main (void)
{   
	pid_t child_pid, wpid;
	int status = 0;

	//Father code (before child processes start)

	for (int id=0; id<5; id++) {
    	if ((child_pid = fork()) == 0) {
        	   sleep((5-id)*2);
        	   printf("\n\t Child running %d",getpid());
        	   exit(id);  //child code
    	}
	}

	//wpid = wait(&status); // [A] this way parent WAITS for ANY ONE of the child process to exit

	while((wpid = wait(&status)) > 0); // [B} this way, the parent WAITS for ALL IT's child processes to exit

	sleep(1);
	printf("\n\t Parent %d synchronized with child death (ret Child Id %d ....ret Status %d.... )",getpid(),wpid,status);
	
}