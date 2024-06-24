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
    int bind_status = m_bind(sockfd, "127.0.0.1", 8083, "127.0.0.1", 8082);
    if(bind_status < 0){
        printf("Error in binding\n");
        return 0;
    }

    printf("Socket created and binded %d\n", sockfd);

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(8082);
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    socklen_t dest_len;
    dest_len = sizeof(dest_addr);

    
    
    
    char buf[1024];
    int r;
    
    int ct = 1, ctr = 0;
    FILE *fp = fopen("output.txt", "a");
    while(1){
        sleep(1); 
        for(int i = 0; i < 1024; i++){
            buf[i] = '\0';
        }
        r = m_recvfrom(sockfd, buf, 1024, 0, (struct sockaddr *)&dest_addr, &dest_len);
        while(r == -1){
            r = m_recvfrom(sockfd, buf, 1024, 0, (struct sockaddr *)&dest_addr, &dest_len);
            sleep(1);
        }


        
        if(r != -1){
            ctr++;
            printf("Recv status: %d %d\n", (int)strlen(buf), ctr);
            printf("%s\n", buf);
            ct++;
            if(buf[0] == '#' && strlen(buf) == 1) break;
            int x = fwrite(buf, sizeof(char), strlen(buf), fp);
            printf("Write status: %d\n", x);
            // break;
            
        }
        
    }

    fclose(fp);

    return 0;

}
