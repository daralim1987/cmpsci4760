#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "shm.h"
#define PERM (S_IWUSR | S_IRUSR| S_IRGRP | S_IROTH)


int main(int argc, char * const argv[])
{
	int child_count = 10;
	key_t key;
	 char* infile;
	infile = "Input.txt";
	
	char* file_palin;
	file_palin = "palin.out";
	
	char* file_nonpalin;
	file_nonpalin = "nopalin.out";
	FILE *fpalin, *fnopalin;
	
	int shmid;
	sharedInfo *shinfo;
	pid_t childpid[19];
	char file_str[100][100];
	int itr=0;
	int totalStr = 0;    
	char words[100][100];
	char *arg1, *arg2, *arg3, *arg4;
	arg1 = (char*)malloc(40);
	arg2 = (char*)malloc(40);
	arg3 = (char*)malloc(40);		
	arg4 = (char*)malloc(40);		
	int seconds = 60;		
	int option;		
	int max_writes = 5;
		
		
while ((option = getopt(argc, argv,"hf:s:n:w:")) != -1) 
{	
  switch (option) 
  {		
   case 'h' :		
           printf("Usage executable -f {filename in double quotes} -n {num of processes } -s {num of seconds} -w{max_writes}\n");
         break;	
    case 'f':		
                infile = optarg;		
                break;
	case 's' : seconds = atoi(optarg);		
           		break;	 
   case 'n' : child_count = atoi(optarg);		
           		break;				
    case 'w':		
                max_writes = atoi(optarg);		
                break;		
   default:		
  		
           fprintf(stderr, "%s: ",argv[0]);		
           perror("Invalid option. Use -h option to check the usage.");		
           break;;		
 }		
}

void interruptHandler(int SIG){
  signal(SIGQUIT, SIG_IGN);
  signal(SIGINT, SIG_IGN);

  if(SIG == SIGINT)
   {
    fprintf(stderr, "\nCTRL-C encoutered, killing processes\n");
  	}

  if(SIG == SIGALRM) 
  {
    fprintf(stderr, "Master has timed out. killing processes\n");
  }
kill(-getpgrp(), SIGQUIT);
    shmdt(shinfo);
	shmctl(shmid, IPC_RMID, NULL);
}

	signal(SIGINT, interruptHandler); 
	signal(SIGALRM, interruptHandler);
	alarm(seconds);

	FILE *fp = fopen(infile, "r");

	key = 100290;	
	shmid = shmget(key, 5500*sizeof(shinfo), PERM |IPC_CREAT |IPC_EXCL);
	if ((shmid == -1) && (errno != EEXIST))
	{
		perror("Unable to create shared memory");
		return -1;
	}
	if (shmid == -1)
	{
		printf("Shared Memory Already created");
		return -1;
	}
	else
	{
		int i;
		shinfo = (sharedInfo*)shmat(shmid,NULL,0);
		if (shinfo == (void*)-1)
			return -1;
		shinfo->index = 0;  
		shinfo->turn = 0;
    	
		if(fp==NULL)
        	perror("File does not exist");
    	else
    	{
    		while(fgets(file_str[itr], 100, fp))
    		{
        	file_str[itr][strlen(file_str[itr]) - 1] = '\0';
        	itr++;
    		}
        totalStr = itr;

    	for(itr = 0; itr < totalStr; itr++)
    		{ 
        	strcpy(shinfo->mylist[itr],file_str[itr]);
    		}
    		itr = 0;
			
    	}
		
    	int id;
		for (id = 1; id < child_count; id++)
        {
        	if((childpid[id-1] = fork())<=0) 
        		break;
        }
        if (childpid[id-1]==-1)
        {
          	printf("Failed to fork:%d",i);
             	return 1;
        }
        if (childpid[id-1]==0)
        { 
  			sprintf(arg1, "%d", id);
        	int a;
        	for ( a = 0; a < totalStr; ++a)
        	{
        		sprintf(arg2, "%d", a);
           	}
				execl("palin", arg1, arg2, NULL);
        }

        wait(NULL);
shmdt(shinfo);
shmdt(file_str);
shmctl(shmid, IPC_RMID, NULL);
	}	

	kill(getpid(), SIGTERM);
	return 0;
}

