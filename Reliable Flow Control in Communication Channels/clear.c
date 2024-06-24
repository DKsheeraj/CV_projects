#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/select.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include <msocket.h>

int main(){

    int shmid = shmget(ftok(".", 60), sizeof(MTPSocket)*MAX_SOCKETS, 0666|IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    MTPSocket* SM = (MTPSocket *)shmat(shmid, 0, 0);

    shmctl(shmid, IPC_RMID, NULL);

    int shmid2 = shmget(ftok(".", 11), sizeof(SOCK_info), 0666|IPC_CREAT);
    if (shmid2 == -1) {
        perror("shmget");
        exit(1);
    }

    shmctl(shmid2, IPC_RMID, NULL);

    int sem1 = semget(ftok(".", 3), 1, 0666|IPC_CREAT);
    int sem2 = semget(ftok(".", 4), 1, 0666|IPC_CREAT);
    int sem3 = semget(ftok(".", 1), 1, 0666|IPC_CREAT);
    int sem4 = semget(ftok(".", 2), 1, 0666|IPC_CREAT);
    int sem5 = semget(ftok(".", 5), 1, 0666|IPC_CREAT);
    int sem6 = semget(ftok(".", 6), 1, 0666|IPC_CREAT);
    int sem7 = semget(ftok(".", 7), 1, 0666|IPC_CREAT);
    int sem8 = semget(ftok(".", 8), 1, 0666|IPC_CREAT);


    semctl(sem1, 0, IPC_RMID, 0);
    semctl(sem2, 0, IPC_RMID, 0);
    semctl(sem3, 0, IPC_RMID, 0);
    semctl(sem4, 0, IPC_RMID, 0);
    semctl(sem5, 0, IPC_RMID, 0);
    semctl(sem6, 0, IPC_RMID, 0);
    semctl(sem7, 0, IPC_RMID, 0);
    semctl(sem8, 0, IPC_RMID, 0);

    return 0;

}