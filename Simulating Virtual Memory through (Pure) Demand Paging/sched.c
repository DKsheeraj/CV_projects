#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<sys/msg.h>
#include<string.h>
#include<time.h>
#include<signal.h>
#include<math.h>


typedef struct mssgBuf{
	long type;
	pid_t pid;
}mssgBuf;

typedef struct sMC{
	long type;
	char mesg[30];
}sMC;

#define P(s) semop(s, &pop, 1)
#define V(s) semop(s, &vop, 1)

int main(int argc, char* argv[]){

	struct sembuf pop, vop;
    pop.sem_num = vop.sem_num = 0;
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1;
    vop.sem_op = 1;
 
	if(argc!=4){
		perror("Insufficent number of inputs provided, provide MQ1, MQ2 and k as command line arguments");
		exit(-1);
	}

	int sched_sem = semget(ftok(".", 9), 1, IPC_CREAT | 0666);
    if(sched_sem == -1){
        perror("semget");
        exit(1);
    }
    semctl(sched_sem, 0, SETVAL, 0);

	int MQ1, MQ2, k;

	MQ1 = atoi(argv[1]);			
	MQ2 = atoi(argv[2]);
	k = atoi(argv[3]);	

	int SigRec = semget(ftok(".", 7), 1, IPC_CREAT | 0666);
    if(SigRec == -1){
        perror("semget");
        exit(1);
    }
    semctl(SigRec, 0, SETVAL, 0);			

	int noOfPro = 0;

	while(noOfPro < k){

		mssgBuf mesg;
		sMC schMsgC;

		
		msgrcv(MQ1,&mesg,sizeof(mesg),1,0);			

		
		kill(mesg.pid, SIGUSR1);

		
		msgrcv(MQ2,&schMsgC,sizeof(schMsgC),1,0);


		if(strcmp(schMsgC.mesg,"PAGE FAULT HANDLED") == 0){

			
			mesg.type = 1;
			msgsnd(MQ1,&mesg,sizeof(mesg),0);
		}
		else{
			
			noOfPro++;
		}
	}

	
	// pid_t masterPID = getppid();
	// kill(masterPID, SIGUSR1);
	V(SigRec);
	pause();
	return 0;
}