/*Dara Lim
 *CS4760 Assignment 1
 */
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>

int main(int argc, char *argv[])
{
    int option;
    int numFork;
    char *execFileName;
    char *inputFile = 0;
    char *outputFile = 0;
    
    /*Get the executable file name*/
    /*execFileName = argv[0];*/
    
    /*Run getopt function until it return -1*/
    while((option = getopt(argc, argv, "o:hi:")) != -1)
    {
        switch(option)
        {
            case 'h':
                printf(" Error: Run program this way ");
                printf("%s -i inputFile -o outputFile \n", argv[0]);
                break;
            case 'i':
                inputFile = optarg;
                break;
            case 'o':
                outputFile = optarg;
                break;
            default:
                break;
        }
        
    }
    
    /*if the input and output files are not specified, the default is assigned*/
    if(inputFile == 0){
        inputFile = "input.dat";
        
    }
    if(outputFile == 0){
        outputFile = "output.dat";
        
    }
    
    /*Open file for reading and writing*/
    FILE *fpIn;
    FILE *fpOut;
    fpIn = fopen(inputFile, "r");
    if(fpIn == NULL){
        fprintf(stderr, "Opening file failed!\n");
        exit(1);
    }
    fpOut = fopen(outputFile, "w");
    if(fpOut == NULL){
        fprintf(stderr, "Cannot Write to output file!\n");
        exit(1);
    }
    
    /*Get the first number from input file and use it as iteration control*/
    fscanf(fpIn, "%d", &numFork);
    int arrChild[numFork];
    for(int j = 0; j < numFork; j++){
        /*Child process*/
        if(fork() == 0){
            
            /*declare some variables and re-initialized every iteration*/
            size_t l = 0;
            ssize_t str;
            char *strLine = NULL;
            int arrSize;
            
            /*fprintf(fpOut, "Process ID: ");*/
            fprintf(fpOut, "%u: ", getpid());
    
            /*skip unwanted line from file*/
            for(int k = 0; k < (2*j + 1); k++){
                str = getline(&strLine, &l, fpIn);
            }
            fscanf(fpIn, "%d", &arrSize);
            
            /*Create array of size arrSize and re-initialize every iteration*/
            int arr[arrSize];
            for(int index = 0; index < arrSize; index++){
                fscanf(fpIn, "%d", &arr[index]);
            }
            
            /*Print out the array element in reverse order*/
            for(int r = arrSize - 1; r >= 0; r--){
                fprintf(fpOut, "%d ", arr[r]);
            }
    
            fprintf(fpOut, "\n");
            exit(0);
        }
    }
   
    /*Print out parent process ID*/
    fprintf(fpOut, "The parent process ID: ");
    fprintf(fpOut, "%u\n", getppid());
    
    for(int i = 0; i < numFork; i++)
    wait(NULL);
    fclose(fpIn);
    fclose(fpOut);
    return 0;
}
