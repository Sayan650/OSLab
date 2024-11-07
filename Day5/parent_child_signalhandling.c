#include<stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>


/////////// different handler codes for SIGSTOP
pid_t x;

void f1(int s)
{
	printf("\n\t Recieved Signal %d \n",s);
	kill(x,SIGINT);
	signal(SIGINT,SIG_DFL);
}



void main (void)
{
     

	signal(SIGINT,f1);
	
	x=fork();

	if(x>0)
	{
		//sleep(5);
		while(1)
		{	printf("\n\t within parent %d",getpid());
		    sleep(1);
		}
		
	}
	else
	{
		signal(SIGINT,SIG_DFL);
		while(1)
		{
			printf("\n\t within child %d",getpid());
			sleep(1);
		}
	}

}