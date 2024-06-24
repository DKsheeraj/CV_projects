#include <msocket.h> 
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
#include <sys/shm.h>
#include <sys/sem.h>

int toDeliver = 1;

int m_socket(int family, int type, int protocol) {
    int shmid = shmget(ftok(".", 60), sizeof(MTPSocket)*MAX_SOCKETS, 0666|IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    
    MTPSocket * SM = (MTPSocket *)shmat(shmid, 0, 0);

    int shmid2 = shmget(ftok(".", 11), sizeof(SOCK_info), 0666|IPC_CREAT);
    if (shmid2 == -1) {
        perror("shmget");
        exit(1);
    }

    SOCK_info * sock_info = (SOCK_info *)shmat(shmid2, 0, 0);

    struct sembuf pop, vop;
    pop.sem_num = 0;
    vop.sem_num = 0;    
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1;
    vop.sem_op = 1;

    int sem1 = semget(ftok(".", 3), 1, 0666|IPC_CREAT);
    int sem2 = semget(ftok(".", 4), 1, 0666|IPC_CREAT);

    int free_index = -1;

    for (int i = 0; i < MAX_SOCKETS; i++) {
        if (SM[i].is_free) {
            free_index = i;
            break;
        }
    }

    if(free_index == -1){
        errno = ENOBUFS;
        return -1;
    }


    V(sem1);
    P(sem2);


    if(sock_info->sockid == -1){
        errno = EINVAL;
    }
    else{
        SM[free_index].is_free = 0;
        SM[free_index].udp_socket_id = sock_info->sockid;
        SM[free_index].process_id = getpid();
    }

    sock_info->errorno = 0;
    sock_info->sockid = 0;
    sock_info->port = 0;

    for(int i = 0; i < 32; i++){
        sock_info->ip[i] = '\0';
    }

    return SM[free_index].udp_socket_id;


}

int m_bind(int sockfd, const char *src_ip, int src_port, const char *dest_ip, int dest_port){
    int shmid = shmget(ftok(".", 60), sizeof(MTPSocket)*MAX_SOCKETS, 0666|IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    
    MTPSocket* SM = (MTPSocket *)shmat(shmid, 0, 0);

    int shmid2 = shmget(ftok(".", 11), sizeof(SOCK_info), 0666|IPC_CREAT);
    if (shmid2 == -1) {
        perror("shmget");
        exit(1);
    }

    SOCK_info * sock_info = (SOCK_info *)shmat(shmid2, 0, 0);

    struct sembuf pop, vop;
    pop.sem_num = 0;
    vop.sem_num = 0;    
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1;
    vop.sem_op = 1;

    int sem1 = semget(ftok(".", 3), 1, 0666|IPC_CREAT);
    int sem2 = semget(ftok(".", 4), 1, 0666|IPC_CREAT);
    
    int cur_index = -1;

    for(int i = 0; i < MAX_SOCKETS; i++){
        if(SM[i].udp_socket_id == sockfd && SM[i].bound == 0){
            cur_index = i;
            SM[i].bound = 1;
            break;
        }
    }

    if(cur_index == -1){
        errno = ENOTBOUND;
        return -1;
    }

    strncpy(sock_info->ip, src_ip, strlen(src_ip));
    sock_info->port = src_port;
    sock_info->sockid = sockfd;

    V(sem1);
    P(sem2);

    if(sock_info->sockid == -1){
        errno = sock_info->errorno;
        sock_info->errorno = 0;
        sock_info->sockid = 0;
        sock_info->port = 0;
        for(int i = 0; i < 32; i++){
            sock_info->ip[i] = '\0';
        }
        return -1;
    }
    else{   
        SM[cur_index].destination_address.sin_family = AF_INET;
        SM[cur_index].destination_address.sin_port = htons(dest_port);
        inet_pton(AF_INET, dest_ip, &SM[cur_index].destination_address.sin_addr);
        sock_info->errorno = 0;
        sock_info->sockid = 0;
        sock_info->port = 0;
        for(int i = 0; i < 32; i++){
            sock_info->ip[i] = '\0';
        }
        return 0;
    }
}

int m_sendto(int sockfd, char *buf, size_t len, int seqno, int flags,const struct sockaddr *dest_addr, socklen_t addrlen){

    int shmid = shmget(ftok(".", 60), sizeof(MTPSocket)*MAX_SOCKETS, 0666|IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    
    MTPSocket* SM = (MTPSocket *)shmat(shmid, 0, 0);
    
    int is_bound = 0;
    int index = -1;
    for (int i = 0; i < MAX_SOCKETS; i++){
        if (SM[i].udp_socket_id == sockfd && !SM[i].is_free && SM[i].bound && SM[i].destination_address.sin_port == ((struct sockaddr_in *)dest_addr)->sin_port && SM[i].destination_address.sin_addr.s_addr == ((struct sockaddr_in *)dest_addr)->sin_addr.s_addr){
            is_bound = 1;
            index = i;
            break;
        }
    }
    if (!is_bound) {
        errno = ENOTBOUND;
        return -1;
    }

    int flag = 0;
    for(int i = 0; i < 10; i++){
        
        if(SM[index].send_buffer[i].header.sequence_number == -1){
            SM[index].send_buffer[i].header.sequence_number = seqno;
            strncpy(SM[index].send_buffer[i].data, buf, len);
            flag = 1;
            return len;
        }
    }   

    if(flag == 0){
        errno = ENOBUFS;
        return -1;
    } 

    return 0;
}

int m_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *dest_addr, socklen_t *addrlen){
    int shmid = shmget(ftok(".", 60), sizeof(MTPSocket)*MAX_SOCKETS, 0666|IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    
    MTPSocket* SM = (MTPSocket *)shmat(shmid, 0, 0);

    int is_bound = 0;
    int index = -1;
    for (int i = 0; i < MAX_SOCKETS; i++) {
        if (SM[i].udp_socket_id == sockfd && !SM[i].is_free && SM[i].bound && SM[i].destination_address.sin_port == ((struct sockaddr_in *)dest_addr)->sin_port && SM[i].destination_address.sin_addr.s_addr == ((struct sockaddr_in *)dest_addr)->sin_addr.s_addr){
            is_bound = 1;
            index = i;
            break;
        }
    }

    if (!is_bound) {
        errno = ENOTBOUND;
        return -1;
    }

    int flag = 0;
    

    for(int i = 0; i < 5; i++){
        if(SM[index].receive_buffer[i].header.sequence_number == toDeliver && SM[index].receive_buffer[i].header.flag == 1){
            strncpy(buf, SM[index].receive_buffer[i].data, len);
            SM[index].receive_buffer[i].header.sequence_number = -1;
            SM[index].receive_buffer[i].header.flag = 0;
            for(int k = 0; k < MAX_MESSAGE_SIZE; k++){
                SM[index].receive_buffer[i].data[k] = '\0';
            }
            flag = 1;
            toDeliver++;
            if(toDeliver == 16) toDeliver = 1;
            return len;
        }
    }

    if(flag == 0){
        errno = ENOMESG;
        return -1;
    }

    return 0;
}

int m_close(int sockfd){

    int shmid = shmget(ftok(".", 60), sizeof(MTPSocket)*MAX_SOCKETS, 0666|IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    
    MTPSocket* SM = (MTPSocket *)shmat(shmid, 0, 0);

    for (int i = 0; i < MAX_SOCKETS; i++) {
        if (SM[i].udp_socket_id == sockfd && !SM[i].is_free){
            close(sockfd);
            SM[i].is_free = 1;
            return 0;
        }
    }
    
    errno = EINVAL;
    return -1;
}
