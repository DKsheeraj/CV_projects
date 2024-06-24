#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <msocket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/shm.h>



int main(){
    int sockfd = m_socket(AF_INET, SOCK_MTP, 0);
    if(sockfd < 0){
        printf("Error in creating socket\n");
        printf("%d\n", errno);
        return 0;
    }
    int bind_status = m_bind(sockfd, "127.0.0.1", 8080, "127.0.0.1", 8081);
    if(bind_status < 0){
        printf("Error in binding\n");
        return 0;
    }

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(8081);
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // char buf[MAX_MESSAGE_SIZE], 

    char* buf = "Hello";
    char *buf2 = "World";
    char *buf3 = "This";
    char *buf4 = "is";
    char *buf5 = "user1";
    char *buf6 = "sending";
    char *buf7 = "data";
    

    char *buf8 = "Ramoji";
    char *buf9 = "Film";
    char *buf10 = "City";
    char *buf11 = "Hyderabad";
    char *buf12 = "Telangana";
    char *buf13 = "India";
    char *buf14 = "TheWorld";
    char *buf15 = "Earth";
    char *buf16 = "SolarSystem";
    char *buf17 = "MilkyWay";
    char *buf18 = "Universe";
    char *buf19 = "Galaxy";
    char *buf20 = "Planet";

    
    printf("%d", sockfd);
    printf("PID = %d\n", getpid());
    
    m_sendto(sockfd, buf, 5, 1, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    m_sendto(sockfd, buf2, 5, 2, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    m_sendto(sockfd, buf3, 4, 3, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    m_sendto(sockfd, buf4, 2, 4, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    m_sendto(sockfd, buf5, 5, 5, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    m_sendto(sockfd, buf6, 7, 6, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    m_sendto(sockfd, buf7, 4, 7, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    m_sendto(sockfd, buf8, 6, 8, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    m_sendto(sockfd, buf9, 4, 9, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    m_sendto(sockfd, buf10, 4, 10, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));


    int r = m_sendto(sockfd, buf11, 9, 11, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    while(r != 9){
        sleep(1);
        r = m_sendto(sockfd, buf11, 9, 11, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    }
    r = m_sendto(sockfd, buf12, 9, 12, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    while(r != 9){
        sleep(1);
        r = m_sendto(sockfd, buf12, 9, 12, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    }

    r = m_sendto(sockfd, buf13, 5, 13, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    while(r != 5){
        sleep(1);
        r = m_sendto(sockfd, buf13, 5, 13, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    }
    r = m_sendto(sockfd, buf14, 8, 14, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    while(r != 8){
        sleep(1);
        r = m_sendto(sockfd, buf14, 8, 14, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    }

    r = m_sendto(sockfd, buf15, 5, 15, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    while(r != 5){
        sleep(1);
        r = m_sendto(sockfd, buf15, 5, 15, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    }
    r = m_sendto(sockfd, buf16, 11, 1, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    while(r != 11){
        sleep(1);
        r = m_sendto(sockfd, buf16, 11, 1, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    }
    r = m_sendto(sockfd, buf17, 8, 2, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    while(r != 8){
        sleep(1);
        r = m_sendto(sockfd, buf17, 8, 2, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    }
    r = m_sendto(sockfd, buf18, 8, 3, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    while(r != 8){
        sleep(1);
        r = m_sendto(sockfd, buf18, 8, 3, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    }

    r = m_sendto(sockfd, buf19, 6, 4, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    while(r != 6){
        sleep(1);
        r = m_sendto(sockfd, buf19, 6, 4, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    }
    r = m_sendto(sockfd, buf20, 6, 5, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    while(r != 6){
        sleep(1);
        r = m_sendto(sockfd, buf20, 6, 5, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    }

    int shmid = shmget(ftok(".", 60), sizeof(MTPSocket)*MAX_SOCKETS, 0666|IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    
}