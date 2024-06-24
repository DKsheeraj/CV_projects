#ifndef GLOBALS_H
#define GLOBALS_H
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sem.h>
#include <netinet/in.h>

#define SOCK_MTP 3
#define MAX_SOCKETS 25
#define MAX_MESSAGE_SIZE 1024
#define MAX_WINDOW_SIZE 5 
#define EINVAL 22
#define ENOBUFS 105
#define ENOTBOUND 113
#define ENOMESG 122
#define maxseqarray 10000

#define T 5
#define p 0.4


#define P(s) semop(s, &pop, 1);
#define V(s) semop(s, &vop, 1);


typedef struct {
    int sequence_number;
    int flag;
} MTPHeader;

typedef struct {
    MTPHeader header;
    char data[MAX_MESSAGE_SIZE];
} Message;

typedef struct Window{
    int windowSize;
    int sequenceNumber[maxseqarray];
    int left;
    int right;
} Window;

typedef struct MTPSocket{
    int is_free;
    pid_t process_id;
    int udp_socket_id;
    int bound;
    struct sockaddr_in destination_address;
    Message send_buffer[10];
    Message receive_buffer[5];
    Window swnd;
    Window rwnd;
    int lastSent;
    int noSpace;
} MTPSocket;

typedef struct SOCK_info{
    int sockid;
    int port;
    char ip[32];
    int errorno;
} SOCK_info;



int m_socket(int family, int type, int protocol);
int m_bind(int sockfd, const char *src_ip, int src_port, const char *dest_ip, int dest_port);
int m_sendto(int sockfd, char *buf, size_t len, int seqno,  int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
int m_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t * addrlen);
int m_close(int mtp_socket_id);





