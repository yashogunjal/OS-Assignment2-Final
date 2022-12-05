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

#define getMax(x,y) (((x) >= (y)) ? (x) : (y))

#define MAXSIZE 1000
#define ARR_SIZE 2500
#define MEM_SIZE 5120
#define THREAD_NUM 3
// Program to multiply the two elements
int* arr1, * arr2;
int* read1, *read2;
int iVal, jVal, kVal;
char * file1, * file2, * file3;
long *ans;

int indexV = 0;



int maxThreads;

typedef struct thread{
    int elemS, elemE;
}thread_args;

void* multiplyFun(void* args) {
    thread_args *t = (thread_args *) args;
    int elemE = t->elemE;
    int elemS = t->elemS;
    
    for (int i = elemS; i <= elemE; ++i) {
        if(read1[i/iVal] == 1 && read2[i/kVal] == 1){
        int rowno = i/iVal;
        int colno = i%kVal;
        long sum = 0;
      
        if(rowno < iVal && colno < kVal) {
    
            for (int j = 0; j < jVal; ++j) {
                int num1 = arr1[rowno * jVal + j];
                int num2 = arr2[colno * jVal + j];
                sum += num1 * num2;
            }
            ans[i] = sum;
       	}
        }
        
    }

    pthread_exit(NULL);

}


int main(int argc, char * argv[]){
    if(argc != 6){
        fprintf(stderr,"Usage: ./a.out <i> <j> <k> in1.txt in2.txt out.txt");
        return EXIT_FAILURE;
    }
    iVal = atoi(argv[0]);
    jVal = atoi(argv[1]);
    kVal = atoi(argv[2]);
    file1 = argv[3];
    file2 = argv[4];
    file3 = argv[5];

    
    //CREATE SHARED MEMORY
    int shmid1 = shmget(1234,MEM_SIZE,0666|IPC_CREAT);
    arr1 = (int*) shmat(shmid1,(void*)0,0);

    int shmid2 = shmget(1235,MEM_SIZE,0666|IPC_CREAT);
    arr2 = (int*) shmat(shmid2,(void*)0,0);
    
    int shmid3 = shmget(1246, MEM_SIZE, 0666|IPC_CREAT);
    read1 = (int*) shmat(shmid3, (void*)0, 0);
    
    int shmid4 = shmget(1248, MEM_SIZE, 0666|IPC_CREAT);
    read2 = (int*) shmat(shmid4, (void*)0, 0);
/*  printf("Arra1 is: \n");
    for (int i = 0; i < iVal*jVal; ++i) {
        printf("%d ", arr1[i]);
        if(i%jVal == jVal-1){
            printf("\n");
        }
    }
    printf("Arra2 is: \n");
    for (int i = 0; i < jVal*kVal; ++i) {
        printf("%d ", arr2[i]);
        if(i%jVal == jVal-1){
            printf("\n");
        }
    }
    */

    ans = (long *) malloc(iVal*kVal*sizeof(long ));


    maxThreads = THREAD_NUM;
    thread_args *inp_t = malloc(maxThreads* sizeof (thread_args));
    
    int prev = 0;
    int var = ((iVal*kVal) / maxThreads);
    int temp = getMax(var, 1);
    for (int i = 0; i < maxThreads; ++i) {
        inp_t[i].elemS = prev;
        inp_t[i].elemE = prev + temp;
        prev = prev + temp + 1;
      
    }
    
   
    //Create threads
    pthread_t *threads = malloc(maxThreads * sizeof(pthread_t));

    for (int i = 0; i < maxThreads; ++i) {
        pthread_create(&threads[i], NULL, multiplyFun, (void*)(inp_t + i));
    }
    for (int i = 0; i < maxThreads; ++i) {
        pthread_join(threads[i], NULL);
    }


    FILE *outfp = fopen("out.txt", "w");
    for (int i = 0; i < iVal; ++i) {
        for (int j = 0; j < kVal; ++j) {

            fprintf(outfp, "%ld", ans[i*kVal + j]);
            if(j != kVal-1){
                fprintf(outfp, " ");
            }

        }
        fprintf(outfp, "\n");
    }


    shmdt(arr1);
    shmdt(arr2);
    shmdt(read1);
    shmdt(read2);

    free(threads);
    free(inp_t);
    free(ans);
    fclose(outfp);

    return 0;
}


