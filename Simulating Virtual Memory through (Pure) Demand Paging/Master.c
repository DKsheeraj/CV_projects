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


int done = 0;

typedef struct pageTable{
	int frame;
	int valid;
}pageTable;


typedef struct processMap {
	pid_t pid;
	int pages;
}processMap;



void handler(int sig){

	if(sig == SIGUSR1){
		done = 1;
	}
}

#define P(s) semop(s, &pop, 1)
#define V(s) semop(s, &vop, 1)

int main(int argc, char* argv[]){

	if(argc!=4){
		printf("Usage: %s <k> <m> <f>\n", argv[0]);
		exit(-1);
	}

	struct sembuf pop, vop;
    pop.sem_num = vop.sem_num = 0;
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1;
    vop.sem_op = 1;

	signal(SIGUSR1, handler);

	srand(time(NULL));

	int k,m,f;

	k = atoi(argv[1]);
	m = atoi(argv[2]);
	f = atoi(argv[3]);

	key_t sm1_key, sm2_key, sm3_key;
	key_t mq1_key, mq2_key, mq3_key;
	int SM1, SM2, SM3, MQ1, MQ2, MQ3;
	
	sm1_key = ftok(".",50);
	sm2_key = ftok(".",80);
	sm3_key = ftok(".",110);
	mq1_key = ftok(".",140);
	mq2_key = ftok(".",170);
	mq3_key = ftok(".",200);
	
	SM1 = shmget(sm1_key, k*m*sizeof(pageTable), IPC_CREAT|0666);
	if(SM1 == -1){
		perror("Error in creating shared memory for SM1");
		exit(-1);
	}


	SM2 = shmget(sm2_key, f*sizeof(int), IPC_CREAT|0666);
	if(SM2 == -1){
		perror("Error in creating shared memory for SM2");
		exit(-1);
	}

	SM3 = shmget(sm3_key, k*sizeof(processMap), IPC_CREAT|0666);
	if(SM3 == -1){
		perror("Error in creating shared memory for SM3");
		exit(-1);
	}


	MQ1 = msgget(mq1_key, IPC_CREAT|0666);
	if(MQ1 == -1){
		perror("Error in creating message queue for MQ1");
		exit(-1);
	}

	MQ2 = msgget(mq2_key, IPC_CREAT|0666);
	if(MQ2 == -1){
		perror("Error in creating message queue for MQ2");
		exit(-1);
	}

	MQ3 = msgget(mq3_key, IPC_CREAT|0666);
	if(MQ3 == -1){
		perror("Error in creating message queue for MQ3");
		exit(-1);
	}

	int SigRec = semget(ftok(".", 7), 1, IPC_CREAT | 0666);
    if(SigRec == -1){
        perror("semget");
        exit(1);
    }
    semctl(SigRec, 0, SETVAL, 0);

	int mmu_sem = semget(ftok(".", 8), 1, IPC_CREAT | 0666);
    if(mmu_sem == -1){
        perror("semget");
        exit(1);
    }
    semctl(mmu_sem, 0, SETVAL, 0);

    int sched_sem = semget(ftok(".", 9), 1, IPC_CREAT | 0666);
    if(sched_sem == -1){
        perror("semget");
        exit(1);
    }
    semctl(sched_sem, 0, SETVAL, 0);


	pageTable* pg = (pageTable*)shmat(SM1,NULL,0);
	int* freeFrame = (int*)shmat(SM2,NULL,0);
	processMap* pMap = (processMap*)shmat(SM3,NULL,0);

	
	for(int i=0; i<k; i++){
		pMap[i].pid = -1;
		pMap[i].pages = -1;
	}

	for(int i=0; i<k; i++){
		for(int j=0; j<m; j++){
			pg[i*m + j].frame = -1;
			pg[i*m + j].valid = 0;
		}
	}	

	for(int i=0; i<f; i++)
		freeFrame[i] = 1;

	if(shmdt((const void *)pg) < 0){
		perror("Error in detaching shared memory for page tables");
		exit(-1);
	}

	if(shmdt((const void *)freeFrame) < 0){
		perror("Error in detaching shared memory for free frame list");
		exit(-1);
	}


	char SM1_str[100], SM2_str[100], SM3_str[100], MQ1_str[100], MQ2_str[100], MQ3_str[100], k_str[100], m_str[100], f_str[100];
	sprintf(SM1_str, "%d", SM1);
	sprintf(SM2_str, "%d", SM2);
	sprintf(SM3_str, "%d", SM3);
	sprintf(MQ1_str, "%d", MQ1);
	sprintf(MQ2_str, "%d", MQ2);
	sprintf(MQ3_str, "%d", MQ3);
	sprintf(k_str, "%d", k);
	sprintf(m_str, "%d", m);
	sprintf(f_str, "%d", f);

	


	pid_t sch_pid = fork();

	if(sch_pid == 0){
		
		char** args = malloc(5 * sizeof(char*));
		if (args == NULL) {
			perror("Memory allocation error");
			return EXIT_FAILURE;
		}

		int mq1StrLen = strlen(MQ1_str);
		int mq2StrLen = strlen(MQ2_str);
		int KStrLen = strlen(k_str);

		args[0] = malloc(15 * sizeof(char));
		args[1] = malloc((mq1StrLen + 1) * sizeof(char));
		args[2] = malloc((mq2StrLen + 1) * sizeof(char));
		args[3] = malloc((KStrLen + 1) * sizeof(char));
		args[4] = NULL;

		if (args[0] == NULL || args[1] == NULL || args[2] == NULL || args[3] == NULL) {
			perror("Memory allocation error");
			return EXIT_FAILURE;
		}

		strcpy(args[0], "./sched");
		strcpy(args[1], MQ1_str);
		strcpy(args[2], MQ2_str);
		strcpy(args[3], k_str);

		if(execlp("./sched", "./sched", MQ1_str, MQ2_str, k_str, NULL)<0){
			perror("Error in execvp in creating scheduler");
			exit(-1);
		}

		// if(execvp(args[0], args)<0){
		// 	perror("Error in execvp in creating scheduler");
		// 	exit(-1);
		// }				
	}

	pid_t mmu = fork();

	if(mmu == 0){
		
		char** args = malloc(14 * sizeof(char*));
		if (args == NULL) {
			perror("Memory allocation error");
			return EXIT_FAILURE;
		}

		int mq2StrLen = strlen(MQ2_str);
		int mq3StrLen = strlen(MQ3_str);
		int sm1StrLen = strlen(SM1_str);
		int sm2StrLen = strlen(SM2_str);
		int sm3StrLen = strlen(SM3_str);
		int KStrLen = strlen(k_str);
		int MStrLen = strlen(m_str);
		int FStrLen = strlen(f_str);

		args[0] = malloc(10 * sizeof(char));
		args[1] = malloc(10 * sizeof(char));
		args[2] = malloc(5 * sizeof(char));
		args[3] = malloc(10 * sizeof(char));
		args[4] = malloc((mq2StrLen + 1) * sizeof(char));
		args[5] = malloc((mq3StrLen + 1) * sizeof(char));
		args[6] = malloc((sm1StrLen + 1) * sizeof(char));
		args[7] = malloc((sm2StrLen + 1) * sizeof(char));
		args[8] = malloc((sm3StrLen + 1) * sizeof(char));
		args[9] = malloc((KStrLen + 1) * sizeof(char));
		args[10] = malloc((MStrLen + 1) * sizeof(char));
		args[11] = malloc((FStrLen + 1) * sizeof(char));
		args[12] = NULL;

		if (args[0] == NULL || args[1] == NULL || args[2] == NULL || args[3] == NULL ||
			args[4] == NULL || args[5] == NULL || args[6] == NULL || args[7] == NULL ||
			args[8] == NULL || args[9] == NULL || args[10] == NULL || args[11] == NULL) {
			perror("Memory allocation error");
			return EXIT_FAILURE;
		}

		// strcpy(args[0], "xterm");
		// strcpy(args[1], "-hold");
		// strcpy(args[2], "-e");
		// strcpy(args[3], "./MMU");
		// strcpy(args[4], MQ2_str);
		// strcpy(args[5], MQ3_str);
		// strcpy(args[6], SM1_str);
		// strcpy(args[7], SM2_str);
		// strcpy(args[8], SM3_str);
		// strcpy(args[9], k_str);
		// strcpy(args[10], m_str);
		// strcpy(args[11], f_str);

		if(execlp("xterm", "xterm", "-hold", "-e", "./MMU", MQ2_str, MQ3_str, SM1_str, SM2_str, SM3_str, k_str, m_str, f_str, NULL)<0){
			perror("Error in execvp in creating mmu");
			exit(-1);
		}

		// if(execlp(args[0], args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], NULL)<0){
		// 	perror("Error in execvp in creating mmu");
		// 	exit(-1);
		// }


		// if(execvp(args[0], args)<0){
		// 	perror("Error in execvp in creating mmu");
		// 	exit(-1);
		// }
	}

	pid_t P[k];

	
	

	for(int i=0; i<k ; i++){
		int mi = rand()%m + 1;
		pMap[i].pages = mi;

		
		int li = rand()%(8*mi + 1) + 2*mi;
		char pageRefString[10000];

        pageRefString[0] = '\0';

        for(int j = 0; j < li; j++){
            double val = ((double)rand())/(double)RAND_MAX;

            double temp = (-2.0)*log(val);

            int pnum = (int)(temp*mi);

            char tem_pp[100];
            sprintf(tem_pp, "%d", pnum);

            strcat(pageRefString, tem_pp);

            if(j < li - 1){
                strcat(pageRefString, " ");
            }
        }

		P[i] = fork();

		if(P[i] == 0){

			pMap[i].pid = getpid();

		
			char *args[5];

			int pageRefStringLength = strlen(pageRefString);
			int mq1StrLength = strlen(MQ1_str);
			int mq3StrLength = strlen(MQ3_str);

			args[0] = malloc(10 * sizeof(char)); 
			args[1] = malloc((pageRefStringLength + 1) * sizeof(char));
			args[2] = malloc((mq1StrLength + 1) * sizeof(char));
			args[3] = malloc((mq3StrLength + 1) * sizeof(char));
			args[4] = NULL;

			if (args[0] == NULL || args[1] == NULL || args[2] == NULL || args[3] == NULL) {
				perror("Memory allocation error");
				return EXIT_FAILURE;
			}

			// strcpy(args[0], "./process");
			// strcpy(args[1], pageRefString);
			// strcpy(args[2], MQ1_str);
			// strcpy(args[3], MQ3_str);

			if(execlp("./process", "./process", pageRefString, MQ1_str, MQ3_str, NULL)<0){
				perror("Error in execvp in creating process");
				exit(-1);
			}
			// if(execvp(args[0], args)<0){
			// 	perror("Error in execvp in creating process");
			// 	exit(-1);
			// }
		}

		usleep(250000);
	}


	if(shmdt((const void *)pMap) < 0){
		perror("Error in detaching shared memory for pMap");
		exit(-1);
	}

	
	// if(done == 0)
	// 	pause();


	P(SigRec);

	
	kill(sch_pid, SIGKILL);
	kill(mmu, SIGKILL);

	
	
	shmctl(SM1,IPC_RMID,NULL);
	shmctl(SM2,IPC_RMID,NULL);
	shmctl(SM3,IPC_RMID,NULL);

	 
    msgctl(MQ1, IPC_RMID, NULL);
    msgctl(MQ2, IPC_RMID, NULL);
	msgctl(MQ3, IPC_RMID, NULL);

	

	FILE * f1 = fopen("pageFault.txt", "r");
    FILE * f2 = fopen("frequency.txt", "w");


    if(f1 == NULL || f2 == NULL){
        perror("fopen");
        exit(1);
    }

    char buffer[1000], line[1000];
    int pos, pid;

    fgets(buffer, sizeof(buffer), f1);
	fgets(buffer, sizeof(buffer), f1);


	int mapForProcCount[k+1][2];
	for(int i=0; i<=k; i++){
		mapForProcCount[i][0] = -1;
		mapForProcCount[i][1] = 0;
	}

    while (fgets(buffer, sizeof(buffer), f1) != NULL) {
        if (strlen(buffer) > 0) {
            sscanf(buffer+22, "%d", &pid);
			int f = 0;
            for(int i = 0; i < k+1; i++){
				if(mapForProcCount[i][0] == pid){
					mapForProcCount[i][1]++;
					f = 1;
					break;
				}
			}

			if(f == 0){
				for(int i = 0; i < k+1; i++){
					if(mapForProcCount[i][0] == -1){
						mapForProcCount[i][0] = pid;
						mapForProcCount[i][1]++;
						break;
					}
				}
			}
		}
	}

	for(int i = 0; i < k+1; i++){
		if(mapForProcCount[i][0] != -1){
			fprintf(f2, "Total count of page faults incurred by the process identified with the process ID: %d is %d\n", mapForProcCount[i][0], mapForProcCount[i][1]);
		}
	}

	fprintf(f2, "\n");
	fclose(f1);
	fclose(f2);
	return 0;
}