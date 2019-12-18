#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>

#include "shm.h"
#define PERM (S_IWUSR | S_IRUSR| S_IRGRP | S_IROTH)


int main(int argc, char const *argv[])
{
	key_t key;
	char* infile;
	infile = "Input.txt";
	char* file_palin;
	file_palin = "palin.out";
	char* file_nonpalin;
	file_nonpalin = "nopalin.out";
	FILE *fpalin, *fnopalin;
	int shmid, i,r1,r2;
	sharedInfo *shinfo;
	pid_t childpid = getpid();
	int n = 10;
	key = 100290;

void Print_Palin( char* palinFilemane, char stringP[])
{
    fpalin = fopen(palinFilemane,"a");      
   	if ( fpalin )
   		{
   			fprintf(fpalin, "%ld\t",childpid );
   			fprintf(fpalin, "%d\t",shinfo->index );
			fputs(stringP,fpalin);   
			fputs("\n",fpalin);

    	}
   	else
    	{
    	   	printf("Failed to open the file\n");
        }
   	fclose(fpalin);
}


void Print_NonPalin( char* nopalinFilename, char stringN[])
{
    fnopalin = fopen(nopalinFilename,"a");
   	if ( fnopalin )
   	{
   		fprintf(fnopalin, "%ld\t",childpid );
   		fprintf(fnopalin, "%d\t",shinfo->index );
		fputs(stringN,fnopalin);
		fputs("\n",fnopalin); 			
    }
   	else
    {
       	printf("Failed to open the file\n");
    }
	   	fclose(fnopalin);
}



void isPalindrome(char str[])
{

    int first = 0;
    int last = strlen(str) - 1;
 
    while (last > first)
    {
        if (str[first++] != str[last--])
        {
        	r1 = rand() % 3;
        	sleep(r1);
            Print_Palin(file_nonpalin,str);
            r2 = rand() % 3;
        	sleep(r2);
            return;
        }
    }
    r1 = rand() % 3;
    sleep(r1);
    if(sizeof(str) > 0)
    	Print_NonPalin(file_palin, str);
    r2 = rand() % 3;
    sleep(r2);

}

void process(const int i ) 
{
	int j;
    time_t t = time(0);
	
	do {
		do {
			fprintf(stderr, "Executing code to enter critical section at  %s \n",ctime(&t) );
			shinfo->flag[i] = want_in;
			j = shinfo->turn;
			
 		while ( j != i )
 			j = ( shinfo->flag[j] != idle ) ? shinfo->turn : ( j + 1 ) % n;

		
 		shinfo->flag[i] = in_cs;

	for ( j = 0; j < n; j++ )
 		if ( ( j != i ) && ( shinfo->flag[j] == in_cs ) )
 			break;
 } 
 
 while ( ( j < n ) || ( shinfo->turn != i && shinfo->flag[shinfo->turn] != idle ) );

 shinfo->turn = i;
 
 
 
 
 fprintf(stderr, "%s Entering critical section process %d\n", ctime(&t),i);
 
 isPalindrome(shinfo->mylist[shinfo->index]);  
 	shinfo->index++;
 
 
 
 	 fprintf(stderr, "%s Executing code to Exit from critical section %d\n ",ctime(&t),i );
 j = (shinfo->turn + 1) % n;
 while (shinfo->flag[j] == idle)
 	j = (j + 1) % n;
 

 shinfo->turn = j;
 shinfo->flag[i] = idle;
 
 } 
 while ( 1 );
 }
	shmid = shmget(key, 2500*sizeof(shinfo), PERM | IPC_EXCL);
	if ((shmid == -1) && (errno != EEXIST)) 
	{
		perror("Unable to read shared memory");
		return -1;
	} 
	else
	{
		shinfo = (sharedInfo*)shmat(shmid,NULL,0);
		if (shinfo == (void*)-1)
		{
			printf("Cannot attach shared memory\n");
			return -1;
		}

		for (i = 0; i < 5; i++ )
		{

			int n= atoi(argv[0]);
			int xx= atoi(argv[1]);
			process(n);
	 
	shmdt(shinfo);
	shmctl(shmid, IPC_RMID, NULL);
	if (shmdt(shinfo) == -1) 
	{
        fprintf(stderr, " Error while detaching shared memory\n");
        exit(1);
    }
    if ((shmctl(shmid, IPC_RMID, NULL) == -1) && !0)
	{
		fprintf(stderr, " Error while removing shared memory\n");
        exit(1);
	}
	  kill(getpid(), SIGKILL);
		}    }
		shmdt(shinfo);
	shmctl(shmid, IPC_RMID, NULL);
	return 0;
}

