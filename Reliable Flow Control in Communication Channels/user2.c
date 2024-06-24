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
    int sockfd;
    sockfd = m_socket(AF_INET, SOCK_MTP, 0);
    if(sockfd < 0){
        printf("Error in creating socket\n");
        return 0;
    }
    int bind_status = m_bind(sockfd, "127.0.0.1", 8081, "127.0.0.1", 8080);
    if(bind_status < 0){
        printf("Error in binding\n");
        return 0;
    }

    printf("Socket created and binded %d\n", sockfd);


    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(8080);
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    char buf[100];
    socklen_t dest_len;
    dest_len = sizeof(dest_addr);

    int r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);

    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);

    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);


    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);


    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);


    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);

    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);

    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);

    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);

    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);

    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);

    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);

    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);

    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);

    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);

    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);

    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);

    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);

    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);

    r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
    while(r == -1){
        r = m_recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&dest_addr, &dest_len);
        sleep(1);
    }

    printf("Data received: %s\n", buf);

    




}
