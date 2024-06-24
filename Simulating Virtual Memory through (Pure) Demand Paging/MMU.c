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


int TS = 0;
int MQ2, MQ3, SM1, SM2, SM3, k, m, f;
int** timeStampForPages;

typedef struct mPFB{
	long type;
	pid_t pid;				
	int info;				
}mPFB;

typedef struct sMC{
	long type;
	char mesg[30];
}sMC;


typedef struct pageTable {
	int frame;
	int valid;			
}pageTable;


typedef struct processMap {
	pid_t processId;
	int numPages;
}processMap;



int processToIndex[10000];
int ind = 0;

pageTable* pg;
int* freeFrame;
processMap* pMap;

void Insert(int processidentity){
	int f = 0;
	for(int i = 0; i < 10000; i++){
		if(processToIndex[i] == processidentity){
			f = 1;
			break;
		}
	}
	if(f == 0){
		processToIndex[ind] = processidentity;
		ind++;
	}
}

int getIndex(int processidentity){
	for(int i = 0; i < 10000; i++){
		if(processToIndex[i] == processidentity){
			return i;
		}
	}
	return -1;
}

void updateFreeFrameList(int processidentity){
	
	int index  = getIndex(processidentity);

	for(int j = 0; j < m; j++){
		if(pg[index*m + j].valid == 1){
			freeFrame[pg[index*m + j].frame] = 1;
			pg[index*m + j].valid = 0;
		}	
	}
}

int checkvalid(int pageNo, int processidentity){

	for(int i = 0; i<k ; i++){
		if(pMap[i].processId == processidentity){

			if(pageNo < pMap[i].numPages && pageNo >= 0){
				return 0;
			}
			break;
		}
	}
	return -1;
}

void sendToPro(mPFB mssgPFB, int frame){
	mssgPFB.type = mssgPFB.pid;
	mssgPFB.info = frame;

	msgsnd(MQ3, &mssgPFB, sizeof(mssgPFB), 0);
}




int checkForPage(mPFB mssgPFB){

	int index = getIndex((int)mssgPFB.pid);
	int pgno = mssgPFB.info;

	if(pg[index*m+pgno].valid == 0){
		return -1;
	}



	timeStampForPages[index][pgno] = TS;

	sendToPro(mssgPFB, pg[index*m+pgno].frame);

	return 0;
}

int getFreeFrame(){
	for(int i=0; i<f; i++){
		if(freeFrame[i] == 1){
			return i;
		}
	}
	return -1;
}

void PFH(int pgno, pid_t pid){

	int index = getIndex((int)pid);
	int fr = getFreeFrame();

	if(fr < 0){
		

		int temp11;
		int temp12 = -1, req = -1;

		for(int j=0; j<m; j++){
			if(pg[index*m+j].valid == 1){

				temp11 = TS - timeStampForPages[index][j];

				if(temp12 < temp11){
					temp12 = temp11;
					req = j;
				}
			}
		}

		if(temp12 != -1){
	
			pg[index*m + pgno].frame = pg[index*m + req].frame;
			pg[index*m + req].valid = 0;
			pg[index*m + pgno].valid = 1;

			timeStampForPages[index][pgno] = TS;

		}
		

	}
	else{

		pg[index*m+pgno].frame = fr;
		pg[index*m+pgno].valid = 1;

		
		freeFrame[fr] = 0;

		timeStampForPages[index][pgno] = TS;
	
	}

}

#define P(s) semop(s, &pop, 1)
#define V(s) semop(s, &vop, 1)

int main(int argc, char* argv[]){

	struct sembuf pop, vop;
    pop.sem_num = vop.sem_num = 0;
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1;
    vop.sem_op = 1;

	if(argc!=9){
		perror("Insufficent number of inputs provided, provide MQ2, MQ3, SM1, SM2, SM3, k, m, f as command line arguments");
		exit(-1);
	}

	MQ2 = atoi(argv[1]);
	MQ3 = atoi(argv[2]);
	SM1 = atoi(argv[3]);
	SM2 = atoi(argv[4]);
	SM3 = atoi(argv[5]);
	k = atoi(argv[6]);
	m = atoi(argv[7]);
	f = atoi(argv[8]);	

	timeStampForPages = (int **)malloc(k*sizeof(int*));
	for(int i=0; i<k; i++){
		timeStampForPages[i] = (int *)malloc(m*sizeof(int));
	}

	for(int i=0; i<k; i++){
		for(int j=0; j<m; j++){
			timeStampForPages[i][j] = -1;
		}
	}

	int mmu_sem = semget(ftok(".", 8), 1, IPC_CREAT | 0666);
    if(mmu_sem == -1){
        perror("semget");
        exit(1);
    }
    semctl(mmu_sem, 0, SETVAL, 0);

	
	pg = (pageTable*)shmat(SM1,NULL,0);
	freeFrame = (int*)shmat(SM2,NULL,0);
	pMap = (processMap*)shmat(SM3,NULL,0);
 
	mPFB mssgPFB;
	sMC smccont;

	int noOfPro = 0;


	FILE *f1, *f2, *f4;

   
    f1 = fopen("pageFault.txt", "w");
    if (f1 == NULL) {
        printf("Error opening pageFault.txt.\n");
        return 1;
    }

    
    f2 = fopen("globalOrdering.txt", "w");
    if (f2 == NULL) {
        printf("Error opening globalOrdering.txt.\n");
        fclose(f1); 
        return 1;
    }

	f4 = fopen("invalidPageReference.txt", "w");
	if (f4 == NULL) {
		printf("Error opening invalidPageReference.txt.\n");
		fclose(f1); 
		fclose(f2); 
		return 1;
	}

	
    fprintf(f1, "\nPAGE FAULT SEQUENCE (pi,xi) : \n");
    fprintf(f2, "\nGLOBAL ORDERING (ti,pi,xi) : \n");



	while(noOfPro < k){
		
		msgrcv(MQ3, &mssgPFB, sizeof(mssgPFB), 1, 0);



		fprintf(f2, "(%d,%d,%d)\n", TS, mssgPFB.pid, mssgPFB.info);
		printf("(%d,%d,%d)\n", TS, mssgPFB.pid, mssgPFB.info);


		Insert(mssgPFB.pid);

		if(mssgPFB.info == -9){
			
			updateFreeFrameList(mssgPFB.pid);

			smccont.type = 1;
			strcpy(smccont.mesg,"TERMINATED");

			
			msgsnd(MQ2, &smccont, sizeof(smccont), 0);

			
			noOfPro++;
		}
		else if(mssgPFB.info == -2){
			printf("TRYING TO ACCESS INVALID PAGE REFERENCE\n");
			smccont.type = 2;
			strcpy(smccont.mesg,"TERMINATED");

			msgsnd(MQ2, &smccont, sizeof(smccont), 0);
		}
		else{
			
			if(checkvalid(mssgPFB.info, mssgPFB.pid) < 0){

				fprintf(f4, "INVALID PAGE REFERENCE : (%d,%d)\n", mssgPFB.pid, mssgPFB.info);
				printf("INVALID PAGE REFERENCE : (%d,%d)\n", mssgPFB.pid, mssgPFB.info);

				sendToPro(mssgPFB, -2);

				updateFreeFrameList(mssgPFB.pid);

				smccont.type = 1;
				strcpy(smccont.mesg,"TERMINATED");

				
				msgsnd(MQ2, &smccont, sizeof(smccont), 0);

			
				noOfPro++;
			}
			else{
				
				

					
					
					if(checkForPage(mssgPFB) < 0 ){

						
						sendToPro(mssgPFB, -1);

						
						fprintf(f1, "PAGE FAULT OCCURED : (%d,%d)\n", mssgPFB.pid, mssgPFB.info);
						printf("PAGE FAULT OCCURED : (%d,%d)\n", mssgPFB.pid, mssgPFB.info);

						
						PFH(mssgPFB.info, mssgPFB.pid);						
						
						

						smccont.type = 1;
						strcpy(smccont.mesg,"PAGE FAULT HANDLED");

						msgsnd(MQ2, &smccont, sizeof(smccont), 0);
					}
				

			}	
		}	

		
		TS++;
	}


	fprintf(f1, "\n");

	if(shmdt((const void *)pg) < 0){
		perror("Error in detaching shared memory for page tables");
		exit(-1);
	}

	if(shmdt((const void *)freeFrame) < 0){
		perror("Error in detaching shared memory for free frame list");
		exit(-1);
	}
	if(shmdt((const void *)pMap) < 0){
		perror("Error in detaching shared memory for pMap");
		exit(-1);
	}

	
	for(int i = 0; i < k; i++){
		free(timeStampForPages[i]);
	}
	free(timeStampForPages);

	fprintf(f2, "\n");
	fclose(f1);
	fclose(f2);
	fclose(f4);
	
	pause();

	return 0;
}