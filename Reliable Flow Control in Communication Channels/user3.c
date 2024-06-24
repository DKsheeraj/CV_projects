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
    int bind_status = m_bind(sockfd, "127.0.0.1", 8082, "127.0.0.1", 8083);
    if(bind_status < 0){
        printf("Error in binding\n");
        return 0;
    }

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(8083);
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    FILE *fp, *fp1;

    fp = fopen("sample.txt", "r");
    if (fp == NULL) {
        printf("Error opening input file.\n");
        return 1;
    }


    char buf[1024];
    int cntr = 1;

    while(!feof(fp)){
        for(int i = 0; i < 1024; i++){
            buf[i] = '\0';
        }
        fread(buf, sizeof(char), 1023, fp);
        int sb = -1;
        while(sb == -1){
            sleep(1); 
            sb = m_sendto(sockfd, buf, strlen(buf), cntr, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        }
        cntr++;
        if(cntr == 16) cntr = 1; 
        printf("Send status: %d\n", sb);

    }

    char *end = "#";
    int r = m_sendto(sockfd, end, 1, cntr, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    while(r == -1){
        sleep(1);
        r = m_sendto(sockfd, end, 1, cntr, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    }

    fclose(fp);

    printf("File copied successfully.\n");

    return 0;

    
}