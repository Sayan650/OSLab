#include<stdio.h>        // printf scanf
#include<unistd.h>       // slepp
#include <sys/types.h>   // pid_t
#include<sys/wait.h>     // wait
#include <signal.h>      // signal kill
#include<stdlib.h>       // exit

//  SIGCHLD signal is send by the child to its parent just beore it terminates by explicitly/implicitly calling exit() system call
// the Default action defined for the SIGCHLD is it Ignore; hence the parent generally ignores it and the child remains Zombie
// parent can define custom-defined action (signal handler); howver it also can't reap the child 

void f1(int s)  // custom-defined handler for SIGCHLD
{
	printf("\n\t Recieved Signal %d Death of Child \n",s);
}

void main (void)
{
    signal(SIGCHLD,f1);  // custom handler for SIGCHLD
	
	pid_t x=fork();

	if(x>0) // parent part
	{
		//sleep(20000);
		for(;;) 
		{
		 	sleep(2); printf("\n\t within parent %d",getpid());
		}
	}
	else  // child part
	{
		sleep(2); printf("\n\t within child %d",getpid());
		exit(1);
	}

}