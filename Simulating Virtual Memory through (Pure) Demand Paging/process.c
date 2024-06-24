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

typedef struct mPFB{
	long type;
	pid_t pid;				
	int info;				
}mPFB;

int done = 0;

void handler(int sig){
	if(sig == SIGUSR1){
		
		done = 1;
	}

	
	signal(SIGUSR1, handler);
}

int main(int argc, char* argv[]){

	

	if(argc!=4){
		perror("Insufficent number of inputs provided, provide page reference string, MQ1 and MQ3 as command line arguments");
		exit(-1);
	}

	
	signal(SIGUSR1, handler);

	int MQ1, MQ3;
	pid_t myID = getpid();

	char *ref_str = argv[1];			
	MQ1 = atoi(argv[2]);				
	MQ3 = atoi(argv[3]);

	int pages[10000];
    int ctr = 0;
    int length;

    char *token = strtok(ref_str, " ");
    while(token != NULL){
        pages[ctr++] = atoi(token);
        token = strtok(NULL, " ");
    }
    length = ctr;

	int temp[length];
	for(int i=0; i<length; i++){
		temp[i] = pages[i];
	}

	mssgBuf mesg;
	mesg.type = 1;
	mesg.pid = myID;

	
    msgsnd(MQ1, &mesg, sizeof(mesg), 0);

    
    if(done == 0)
 	   pause();

 	done = 0;

 	mPFB msgPFB;
	int pagecnt = 0;

 	while(pagecnt < length){

	 	 

 		msgPFB.type = 1;
 		msgPFB.pid = myID;

		msgPFB.info = temp[pagecnt];

	 	
	 	msgsnd(MQ3, &msgPFB, sizeof(msgPFB), 0);

	 	
		msgrcv(MQ3, &msgPFB, sizeof(msgPFB), myID, 0);

		
		if(msgPFB.info >= 0){

			pagecnt++;
		}
		else if(msgPFB.info == -1){

		
			if(done == 0)
				pause();

			done = 0;	
		}
		else if(msgPFB.info == -2){
			
			return 0;
		}
		else{	
			perror("Invalid integer returned!, should be either >=0 or -1 or -2\n");
			exit(-1);
		}
 	}

 	
 	msgPFB.type = 1;
 	msgPFB.pid = myID;
	msgPFB.info = -9;

	
	msgsnd(MQ3, &msgPFB, sizeof(msgPFB), 0);

	return 0;
}