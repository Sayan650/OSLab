#include<stdio.h>        // printf scanf
#include<unistd.h>       // slepp
#include <sys/types.h>   // pid_t
#include<sys/wait.h>     // wait
#include <signal.h>      // signal kill
#include<stdlib.h>       // exit

//  parent can wait for its child to terminate by calling wait() system call
// takes the address of an interger to pass the child exit code and returns the child process Id
// importantly wait() ALSO REAPs the child

void f1(int s)  // custom-defined handler for SIGCHLD
{
	printf("\n\t Recieved Signal %d Death of Child \n",s);
}

void main (void)
{   int chld_exit_code;
    signal(SIGCHLD,f1);  // custom handler for SIGCHLD
	pid_t chid,x=fork();

	if(x>0)
	{
		//sleep(200);
		chid=wait(&chld_exit_code); // parent wants to proceed further ONLY IF child has terminated
		printf("\n\t Child Id %d exited passing code %d",chid,chld_exit_code/256);
		for(;;) 
		{
		 	sleep(2); printf("\n\t within parent %d",getpid());
		}
	}
	else
	{  //sleep(6); 
		printf("\n\t within child %d",getpid());
        //kill(getppid(),SIGCHLD);
        //for(;;) ;
		//exit(3);
	}
}