#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>


int iVal, jVal, kVal;
char* mat1, *mat2, *mat3;
int *arr4, *arr3, *ismul;



uint64_t totaltime = 0;
uint64_t ClockGetTime()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000000LL + (uint64_t)ts.tv_nsec / 1000LL;
}

int main(int argc, char * argv[])
{

    if(argc != 7){
        fprintf(stderr,"ERROR: Invalid Arguments\n" );
        fprintf(stderr,"USAGE: ./main.c iVal jVal kVal in1.txt in2.txt out.txt\n" );
        return EXIT_FAILURE;
    }
    iVal = atoi(argv[1]);
    jVal = atoi(argv[2]);
    kVal = atoi(argv[3]);
    mat1 = argv[4];
    mat2 = argv[5];
    mat3 = argv[6];


    //init to 0

    //int time_el = 5;
    pid_t pro1_pid = -1, pro2_pid = -1, sch_pid = -1;
    sch_pid = getpid();
    pro1_pid = fork();
    if (pro1_pid == 0){
        //child process
        execlp("./p1.out",argv[1], argv[2], argv[3], argv[4], argv[5],argv[6], NULL);

    } 
    //in parent
    pro2_pid = fork();
    if (pro2_pid == 0){
        //child process
        execlp("./p2.out",argv[1], argv[2], argv[3], argv[4], argv[5],argv[6], NULL);
    }
    //in parent
    int quantum_number = 2;

  
    //pause both processes
    kill(pro1_pid,SIGSTOP); // we are NOT killing the processes.
    kill(pro2_pid,SIGSTOP);

    kill(pro1_pid,SIGCONT);
   int quant1 = 0;
   int quant2 = 0;
  
    while(1)
    {

        printf("QUANTUM NUMBER: %d\n",quantum_number);
        if (quantum_number%2==0) //even implies pro1 should execute
        {
        	quant1++;
        	
            kill(pro2_pid,SIGSTOP);
            kill(pro1_pid,SIGCONT);
            
        }
        else
        {
        quant2++;
        
        int val = kill(pro1_pid,0);
       		if(val == 0) break;
       		
            kill(pro1_pid,SIGSTOP);
            kill(pro2_pid,SIGCONT);
        }
//        int flag = 0;
//
//        for(int i = 0; i < iVal * kVal; i++){
//            if(ismul[i] != -1){
//                flag += 1;
//            }
//        }
//        if(flag == 0 || timethreshold == 0){
//            break;
//        }
        quantum_number++;
       usleep(2);

    }
    
	printf("\nTime Quantum for one: %d, two: %d", quant1, quant2);
	printf("\n Turnaround time for one : %d", quant1*2);
	printf("\n Turnaround time for two : %d", quant2*2);
	
   /* shmdt(arr3);
    shmdt(arr4);
    shmctl(shmid3,IPC_RMID,NULL);
    shmctl(shmid4,IPC_RMID,NULL);*/

    wait(NULL);
    return 0;
}

