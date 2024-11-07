#include<stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

int n=0;

/////////// different handler codes for SIGSTOP
void f1(int x)
{
	printf("\n\t Recieved Signal %d \n",x);
	printf("\n\t 'Bye'....I am dying...... \n");
	signal(SIGINT,SIG_DFL);
	//kill(getpid(),SIGINT);
}


/////////// handler code for SIGSTOP
void f5(int x)   
{
	printf("\n\t Recieved Signal %d \n",x);
	printf("\n\t Hey!!! you are forcefully stopping me.....\n");
	signal(SIGTSTP,SIG_IGN);
}



void main (void)
{
     signal(SIGINT,f1);	// custom handler defined for SIGINT signal
     signal(SIGTSTP,f5);  // custom handler defined for SIGSTOP signal
	
	printf("\n pid %d parId %d, grpId %d \n",getpid(),getppid(),getpgid(0));
	for(;;)
	{
        //sleep(20000); 
        pause();

	}
}





