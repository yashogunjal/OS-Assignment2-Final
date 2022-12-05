#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>

#define getMax(x,y) (((x) >= (y)) ? (x) : (y))

#define MAX_SIZE 1000000
#define MEM_SIZE 5120
#define ARR_SIZE 1000
#define THREAD_NUM 2


//Global variables
FILE *fp1, *fp2;
int iVal,jVal,kVal;
char* file1, *file2, *file3;
int *arr1;
int *arr2;
int *read1;
int *read2;
int *offsetarray1, *offsetarray2;
int line1size = 0, line2size = 0;
int maxThreads;


//Time taken:
uint64_t totaltime = 0;
typedef struct files{
    int readStart1, readEnd1;
} thread_inp;

uint64_t ClockGetTime()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000000LL + (uint64_t)ts.tv_nsec / 1000LL;
}


void getLineIndex(FILE *fp, int *var){
    if(fp == NULL){
        fprintf(stderr, "ERROR: File not found\n");
        exit(EXIT_FAILURE);
    }
    char c;
    for (c = getc(fp); c != EOF ; c = getc(fp)) {

        if (c == '\n') {
            (*var)++;
        }
    }
}

void getOffset(FILE *fp, int offsetarr[]){
    if(fp == NULL){
        fprintf(stderr, "ERROR: File not found\n");
        exit(EXIT_FAILURE);
    }
    int temp = 0;
    int var = 0;
    char c;
    fseek(fp, 0, SEEK_SET);
    for (c = getc(fp); c != EOF ; c = getc(fp)) {
        temp++;
        if (c == '\n') {
            temp++;
            offsetarr[var] = temp;
            var++;

        }
    }
}

int arrIndex = 0;
int arrIndex2 = 0;
void* threadfun(void* args){
    thread_inp *inp = (thread_inp*) args;
    /*int* off1 = inp->off1, *off2 = inp->off2;*/
    int toreadS = inp->readStart1;
    int toreadE = inp->readEnd1;


    //line to store the values for file 1
    char *line = NULL;
    size_t len = 0;
    ssize_t readf1;

    //line to store the values for file 2
    char *line2 = NULL;
    size_t len2 = 0;
    ssize_t readf2;
   


//for loop to read the file
    for(int i = toreadS; i <= toreadE; i++){
        if(read1[i] == 0 && i < line1size){
       
            //FOR FILE ONE
            fseek(fp1, offsetarray1[i] - i - 1, SEEK_SET);
            

            readf1 = getline(&line, &len, fp1);

            
            //Process line and store in array
            char *token = strtok(line, " ");
            read1[i] = 1;
            int temp = 0;
            while(token != NULL){
                arr1[i*jVal+ temp] = atoi(token);
                /*printf("threadno: %d: %d\n", (int)toreadS, arr1[i*jVal+ temp]);*/
                token = strtok(NULL, " ");
                temp++;
            }

            //reset line
            line = NULL;

        }

        if(read2[i] == 0 && i < line1size){
            //FOR FILE TWO
            fseek(fp2, offsetarray2[i] - i - 1, SEEK_SET);
           

            readf2 = getline(&line2, &len2, fp2);

        
            int temp = 0;
            read2[i] = 1;
            char *token2 = strtok(line2, " ");
            while(token2 != NULL){
                arr2[i*jVal + temp] = atoi(token2);
                token2 = strtok(NULL, " ");
                temp++;
            }
            //Reset line2
            line2 = NULL;
        }
		
		
		

    }
    
  

    pthread_exit(NULL);


}

//MAIN FUNCTION

int main(int argc, char * argv[]){
    //Fork and exec
    if(argc != 8){
        fprintf(stderr,"ERROR: Invalid Arguments\n" );
        fprintf(stderr,"USAGE: ./group12_assignment2.out iVal jVal kVal in1.txt in2.txt out.txt\n" );
        return EXIT_FAILURE;
    }
    iVal = atoi(argv[1]);
    jVal = atoi(argv[2]);
    kVal = atoi(argv[3]);
    file1 = argv[4];
    file2 = argv[5];
    file3 = argv[6];
    int numOfthread = atoi(argv[7]);

    //Allocate memory for array1 and array2
    //create file pointers
    fp1 = fopen(file1, "r");
    fp2 = fopen(file2, "r");
    //transpose second matrix
    getLineIndex(fp1, &line1size);
    getLineIndex(fp2, &line2size);

    offsetarray1 = malloc(line1size * sizeof(int));
    offsetarray2 = malloc(line2size * sizeof(int));
    for (int p = 0; p < line1size; ++p) {
        offsetarray1[p] = 0;
        offsetarray2[p] = 0;
    }

    getOffset(fp1, offsetarray1);
    getOffset(fp2, offsetarray2);
    
    maxThreads = numOfthread;
    thread_inp *inp = malloc(maxThreads * sizeof(thread_inp));

    int prev = 0;
    int temp = getMax((line1size / maxThreads) , 1);
    for (int i = 0; i < maxThreads; ++i) {
        inp[i].readStart1 = prev;
        inp[i].readEnd1 = prev + temp;
        prev = prev + temp +1;

    }


    //create shared memory for array 1
    int shmid1 = shmget(1234,MEM_SIZE,0666|IPC_CREAT);
    arr1 = (int*) shmat(shmid1,(void*)0,0);

    //create shared memory for array 2, 3 and 4

    int shmid2 = shmget(1235, MEM_SIZE, 0666|IPC_CREAT);
    arr2 = (int*) shmat(shmid2, (void*)0, 0);
    
    int shmid3 = shmget(1246, MEM_SIZE, 0666|IPC_CREAT);
    read1 = (int*) shmat(shmid3, (void*)0, 0);
    
    int shmid4 = shmget(1248, MEM_SIZE, 0666|IPC_CREAT);
    read2 = (int*) shmat(shmid4, (void*)0, 0);

 uint64_t before = ClockGetTime();

    pthread_t thread_create[maxThreads];
    for (int i = 0; i < maxThreads; ++i) {
        pthread_create(&thread_create[i], NULL, threadfun, (void*)(inp + i));
    }
    for (int i = 0; i < maxThreads; ++i) {
        pthread_join(thread_create[i], NULL);
    }
    uint64_t after = ClockGetTime();
    totaltime = after - before;
    
       printf("%"PRIu64"\n",totaltime);


    FILE *time= fopen("time.txt", "a");
    fprintf(time,"%d,%"PRIu64"\n",maxThreads ,totaltime);
    fclose(time);
 

   printf("<-- Data Successfully Written to SHM -->\n");
   /* for (int i = 0; i < ARR_SIZE; ++i) {
        printf("%d ", shmseg[i]);
        printf("%d ", arr1[i]);
    }
    printf("\n");
    for (int i = 0; i < ARR_SIZE; ++i) {
        printf("%d ", arr2[i]);
    }
    */
    shmdt(arr1);
    shmdt(arr2);
    shmdt(read1);
    shmdt(read2);

    fclose(fp1);
    fclose(fp2);

    free(offsetarray1);
    free(offsetarray2);
    free(inp);
    return 0;



}


