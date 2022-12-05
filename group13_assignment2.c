#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int iVal, jVal, kVal;
char* mat1, *mat2, *mat3;
int *arr4, *arr3, *ismul;

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
   
    for( int i = 0; i < 1000; i++)
    {

        printf("QUANTUM NUMBER: %d\n",quantum_number);
        if (quantum_number%2==0) //even implies pro1 should execute
        {
            kill(pro2_pid,SIGSTOP);
            kill(pro1_pid,SIGCONT);
        }
        else
        {
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
        usleep(1);

    }

   /* shmdt(arr3);
    shmdt(arr4);
    shmctl(shmid3,IPC_RMID,NULL);
    shmctl(shmid4,IPC_RMID,NULL);*/

    wait(NULL);
    return 0;
}

