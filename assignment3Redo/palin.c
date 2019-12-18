#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

//function prototypes
int isPalin(char str[]);
FILE* fileOpen(char*, char*);

//start main functions
int main(int argc, char* argv[]){
    int shmid;
    key_t shmkey = 1010;
    //string that will attach to the shared memory.
    char (*strings)[100][80];
    //string index
    int index;
    int pid;
    FILE *fp;
    //two output file names
    char *fnames[2] = {"nopalin.out", "palin.out"};
    //loop iter control
    int i;
    sem_t *sem;//semaphore to protect the output files
    char *sem_name = "palin_sem";
    int isPalin_arr[5] = {0, 0, 0, 0, 0};
    pid = getpid();
    index = atoi(argv[1]);//get passed arg as int
    
    //Shared Memory
    shmid = shmget(shmkey, 100 * 80, IPC_CREAT | 0775);
    if(shmid < 0) {//handle error
        perror("./palin: Error: shmget");
        exit(EXIT_FAILURE);
    }
    
    //attach to shared memory
    strings = shmat(shmid, NULL, 0);
    //error
    if(strings < 0) {
        perror("./palin: Error: shmat");
        exit(EXIT_FAILURE);
    }
    
    //Determine if the string is palindromes
    for(i = 0; i < 5; i++) {
        //if((index - i) < 0)
           // break;
        //else {
            isPalin_arr[i] = isPalin((*strings)[index-i]);
        //}
    }
    
    //Semaphore & Critical section
    sem = sem_open(sem_name, 0);//open semaphore created by master
    if(sem == SEM_FAILED) {
        perror("./palin: Error: sem_open");
        exit(EXIT_FAILURE);
    }
    
    //No more than 5 process in critical section
    for(i = 0; i < 5; i++) {
        //sleep(rand() % 4);//sleep for 0-3 sec
        fprintf(stderr, "%-6d Entering the critical section: sem_wait()\n", pid);
        sem_wait(sem);//wait for turn
        
        // CRITICAL SECTION
        fprintf(stderr, "%-6d Inside the critical section\n", pid);
        sleep(2);//sleep for 2 more sec
        //opening the correct file for writing string to.
        fp = fileOpen(fnames[isPalin_arr[i]], "a");
        fprintf(fp, "%-6d %s\n", pid, (*strings)[index-i]);//write to file
        fclose(fp);//done with file so close
        sleep(2);//sleep for 2 more sec

        fprintf(stderr, "%-6d Exiting the critical section: sem_post()\n", pid);
        //The end of CRITICAL SECTION
        sem_post(sem);//done with crit section so signal the semaphore
    }
    exit(EXIT_SUCCESS);
}

//check if given string is palindrome.
int isPalin(char str[]) {
    int lo = 0;
    int hi = strlen(str) - 1;
    
    //remove the end line
    if(str[hi] == '\n') {
        str[hi] = '\0';
        hi--;
    }
    while(hi > lo) {
        if(str[lo++] != str[hi--])
            return 0;
    }
    return 1;
}

//open file function definition
FILE* fileOpen(char* fname, char* option) {
    char error[128];
    FILE* fp = fopen(fname, option);
    if(fp == NULL) {
        perror(error);
        exit(EXIT_FAILURE);
    }
    return fp;
}
