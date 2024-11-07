#include<stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>

pid_t x[2]={0,0};

/////////// different handler codes for SIGSTOP
void f1(int x)
{
	printf("\n\t Recieved Signal %d \n",x);

	//signal(SIGQUIT,SIG_IGN);
	kill(x[0],SIGQUIT);
	kill(x[1],SIGQUIT);
}



void main (void)
{
    
    int i=0;


	//signal(SIGINT,f1);
    
     do
     { 
     	
     	x[i]=fork();
        i++;
     }while((x[i]>0)&&(i<2));

    
	if(x[1]>0)
	{
		//printf("\n\t within parent child %d ppid %d",getpid(),x[0]);
	    //printf("\n\t within parent child %d ppid %d",getpid(),x[1]);
		//sleep(2);
		kill(x[0],SIGKILL);
	    kill(x[1],SIGKILL);
	    for(;;);
	}
	else
	{
		printf("\n\t within child %d ppid %d",getpid(),getppid());
		for(;;);
	}
	

}