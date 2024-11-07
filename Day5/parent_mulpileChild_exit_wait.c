#include<stdio.h>        // printf scanf
#include<unistd.h>       // slepp
#include <sys/types.h>   // pid_t
#include<sys/wait.h>     // wait
#include <signal.h>      // signal kill
#include<stdlib.h>       // exit

//  parent waits for ANYONE child to terminate by calling wait() system call

void main (void)
{   
	int i,stat_code;
	pid_t chld_id;

    for(i=0;i<5;i++)
    	if(fork()==0)
    	{
    		sleep((5-i)*5);
    		printf("\n\t within child process Id %d\n",getpid());
    		
    		exit(i+1);
    	}
    
    chld_id=wait(&stat_code);
    printf("\n\t Parent %d sync with Child Id %d that exited passing code %d\n",getpid(),chld_id,stat_code/256);

    exit(1);
	
}