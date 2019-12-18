#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>

//funciton prototype.
void outOfTime();
void handleSignal();
void clearUp();
FILE* openFile(char*, char*);
int readFileToShm(FILE*, int);
void handleFork(int, int);


//Global variable
key_t SHMKEY = 1010;

//Start main function here
int main(int argc, char* argv[]) {
    int opt;
    int seconds = 120;
    //declare and assign input file name
    char* input_name = "input.txt";
    FILE* fp;
    //count the string line in file.
    int numStr;
    //shared memory id
    int shmid;
    //max of number process allowed
    int n = 20;
    
    if(argc < 2) { //No arguments
        printf("Invalid argument given, please see the README file");
    }
    
    //Start while loop and switch statement to parse the command line arguments.
    while((opt = getopt(argc, argv, "hf:n:")) != -1) {
        switch(opt) {
            case 'h':
                printf("\nThis program takes the following possible arguments\n");
                printf("\n");
                printf("  -h           : to display this help message\n");
                printf("  -f filename  : to use custom input file\n");
                printf("  -n x         : x = the amount of children allowed to exist at one time\n");
                printf("\n");
                return 0;
            case 'f':
                input_name = optarg;
                break;
            case 'n':
                n = atoi(optarg);
                break;
            default:
                break;
        }
    }
    //Get the shared memory ID
    shmid = shmget(SHMKEY, 100 * 80, IPC_CREAT | 0775);
    //If the shared memory id less than 0, there is an error.
    if(shmid < 0) {
        perror("./master: Error: shmget ");
        exit(EXIT_FAILURE);
    }
    //set up signal handlers for 2 min timout
    signal(SIGALRM, outOfTime);
    alarm(seconds);//120 seconds
    signal(SIGINT, handleSignal);
    
    fp = openFile(input_name, "r");
    numStr = readFileToShm(fp, shmid);//read input file into shared mem
    //close file after opening
    fclose(fp);
    //all the forks, execs, and waits occur here
    handleFork(numStr, n);
    //delete shared memory
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}

/*fopen with an error check*/
FILE* openFile(char* fname, char* opts) {
    FILE* fp = fopen(fname, opts);
    if(fp == NULL) {//error opening file
        perror("error opening file");
        exit(EXIT_FAILURE);
    }
    return fp;
}

//read lines from a given file pointer and copy them to shared memory with a given id
int readFileToShm(FILE* fp, int shmid) {
    //string buffer
    char buf[80];
    //number of strin line from file.
    int count = 0;
    char (*str)[100][80];//ptr to 2D arr of chars
    
    str = shmat(shmid, NULL, 0);//attach
    if(str < 0) {//error
        perror("./master: Error: shmat ");
        exit(EXIT_FAILURE);
    }
    while(fgets(buf, sizeof(buf), fp)) {//copy each line into shared memory
        strcpy((*str)[count++], buf);
    }
    shmdt(str);//detach
    return count;
}
//Handles all the forks, execs, waits, and creates a binary semaphore
void handleFork(int numStr, int n) {
    //for counting child process in system.
    int childRunning = 0;
    //for counting child terminated.
    int childTerminated = 0;
    //the index pass to child process.
    int index = numStr - 1;
    //string argument for exec
    char xx[5];
    pid_t pid;
    //sempahore for protecting the output files
    sem_t* sem;
    char* semName = "palin_sem";
    //if number of process greater than 20
    if(n > 20)
        n = 20;
    //create the semapore. starting semaphore value = 1
    sem = sem_open(semName, O_CREAT, 0644, 1);
    if(sem == SEM_FAILED) {
        perror("./master: Error: sem_open");
        exit(EXIT_FAILURE);
    }
    //Loop will be ended when enough child terminated.
    while(childTerminated < 5) {
  
        //not too many processes and strings line remaining
        if(childRunning < n && index >= 0) {
            //forking child
            pid = fork();
            if(pid < 0) {//Forking error
                perror("./master: Error: Forking error");
                exit(EXIT_FAILURE);
            }
            //Child process
            else if(pid == 0) {
                //get index as string xx
                sprintf(xx, "%d", index);
                execl("./palin", "palin", xx, (char*)NULL);//exec ./palin xx
            }
            //increment running process counter
            childRunning++;
            //decrement the index
            index = index - 1;        }
        //returns child pid if child terminated
        pid = waitpid(-1, NULL, WNOHANG);
       //child terminated
        if(pid > 0) {
            //increment process terminated counter
            childTerminated++;
            //decrement running process counter
            childRunning--;
        }
   }
    //all children are done so destroy semaphore
    sem_unlink(semName);
    
    return;
}

// Limit time SIGALRM handler
void outOfTime() {
    printf("Exceeding 2 minutes time limit\n");
    printf("Kill all remaining child processes\n");
    clearUp();
    exit(EXIT_SUCCESS);
}

//SIGINT handler
void handleSignal() {
    printf("Received (ctrl-C) signal\n");
    printf("Kill all the remaining child pocesses\n");
    clearUp();
    exit(EXIT_SUCCESS);
}
//delete shared memory, terminate children, and unlink the semaphore
void clearUp() {
    int shmid;
    shmid = shmget(SHMKEY, 100 * 80, IPC_CREAT | 0775);
    shmctl(shmid, IPC_RMID, NULL);
    sem_unlink("palin_sem");
    kill(0, SIGKILL);
    return;
}
