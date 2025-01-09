#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
    int sockfd, portno, newsockfd, clilen;
    struct sockaddr_in	cli_addr, serv_addr; 

    if(argc < 2){
        printf("Need to enter the port number too!\n");
        exit(0);
    }

    portno = atoi(argv[1]);

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Cannot create socket\n");
		exit(0);
	}

    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port	= htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		printf("Unable to bind local address\n");
		exit(0);
	}

	listen(sockfd, 5);

    while(1){
        clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen) ;

		if (newsockfd < 0) {
			printf("Accept error\n");
			exit(0);
		}
        else{
            char message[100];
            recv(newsockfd, message, 100, 0);
            printf("C: <client connects to SMTP port>\n");
            sprintf(message, "220 Service ready\r\n"); 
            printf("S: 220 iitkgp.ac.in Service ready\n");
            
            send(newsockfd, message, strlen(message), 0);
        }

        if(fork() == 0){
            close(sockfd);

            char message[1000], host[100];
            int cnt = 0;

            int reciepientfile;

            while(1){
                int len = recv(newsockfd, message, 1000, 0);
                

                message[len] = '\0';

                // char *ack = (char *)malloc(1000);
                char ack[1000];

                


                if(strstr(message,"HELO")!=NULL)
                {
                    printf("C: ");
                    for(int i=0;i<len;i++)
                    {
                        if(message[i]!='\r' && message[i]!='\n')   printf("%c",message[i]);
                        else {
                            printf("\n");
                            break;
                        }
                    }
                    sscanf(message, "HELO %[^\r\n]99s", host);
                    printf("S: 250 OK Hello %s\n",host);
                    sprintf(ack, "250 OK Hello %s\r\n",host);
                    
                    send(newsockfd, ack, strlen(ack), 0);
                    
                }
                else if(strstr(message,"MAIL")!=NULL)
                {
                    char sender[100];
                    sscanf(message, "MAIL FROM: %[^\r\n]99s", sender);

                    char senderTemp[100];
                    int cnt = 0;
                    for(int i = 0; i < strlen(sender); i++){
                        if(sender[i] != '\r' && sender[i] != '\n' && sender[i] != '<' && sender[i] != '>'){
                            senderTemp[cnt++] = sender[i];
                        }
                    }
                    senderTemp[cnt] = '\0';
                    printf("C: MAIL FROM: <%s>\n", senderTemp);
                    printf("S: 250 %s... Sender ok\n", senderTemp);

                    sprintf(ack, "250 %s... Sender ok\r\n", senderTemp);
                    
                    send(newsockfd, ack, strlen(ack), 0);

                    
                }
                else if(strstr(message,"RCPT")!=NULL)
                {
                    char recipient[100];
                    sscanf(message, "RCPT TO: %[^\r\n]99s", recipient);
                    
                    char recipientTemp[100];
                    int cnt = 0;
                    for(int i = 0; i < strlen(recipient); i++){
                        if(recipient[i] != '\r' && recipient[i] != '\n' && recipient[i] != '<' && recipient[i] != '>'){
                            recipientTemp[cnt++] = recipient[i];
                        }
                    }
                    recipientTemp[cnt] = '\0';
                    printf("C: RCPT TO: <%s>\n", recipientTemp);

                    char *user = strtok(recipientTemp, "@");

                    // strcat(user, ".txt");
                    char *pathname = (char *)malloc(100*(sizeof(char)));
                    strcpy(pathname, "./");
                    strcat(pathname, user);
                    strcat(pathname, "/mymailbox.txt");
                    
                    reciepientfile = open(pathname, O_RDWR |O_APPEND, 0644);
                    if(reciepientfile < 0){
                        printf("S: 550 No such user\n");
                        sprintf(ack, "Error in sending mail: <550 No such user>\r\n");
                        send(newsockfd, ack, strlen(ack), 0);
                        continue;
                    }
                    else{
                        printf("S: 250 root... Recipient ok\n");
                        // printf("250 root... Recipient ok \n");
                        sprintf(ack, "250 root... Recipient ok \n");
                        
                        send(newsockfd, ack, strlen(ack), 0);
                    }
                }
                else if(strstr(message,"DATA")!=NULL)
                {
                    printf("C: DATA\n");
                    printf("S: 354 Enter mail, end with '.' on a line by itself\n");
                    // printf("354 Enter mail, end with '.'' on a line by itself\n");
                    sprintf(ack, "354 Enter mail, end with '.' on a line by itself\n");
                    
                    send(newsockfd, ack, strlen(ack), 0);

                    char data[1000];
                    char body[10000];
                    int cnt = 0;
                    while(1)
                    {
                        int len = recv(newsockfd, data, 1000, 0);
                        // ;
                        

                        int fl = 0;
                        // data[len] = '\0';
                        for(int i = 0; i < len; i++){
                            body[cnt++] = data[i];
                        }

                        

                        if(body[cnt-1] == '\n' && body[cnt-2] == '.' && body[cnt-3] == '\n'){
                            body[cnt] = '\0';
                            fl = 1;

                            printf("C: ");

                            int st = 0;
                            
                            for(int i = 0; i < 1000; i++){
                                if(body[i] == '\n' && body[i+1] == '.' && body[i+2] == '\n'){
                                    printf(".\n");
                                    printf("S: 250 OK Message accepted for delivery\n");
                                    sprintf(ack, "250 OK Message accepted for delivery\r\n");
                                    close(reciepientfile);
                                    
                                    send(newsockfd, ack, strlen(ack), 0);
                                    break;
                                }
                                else if(body[i] == '\n' && body[i+1] != '.'){
                                    char temp[2];
                                    temp[0] = body[i];
                                    temp[1] = '\0';
                                    write(reciepientfile, temp, 1);
                                    printf("\nC: ");
                                    if(st == 7){
                                        char * timetxt = "Received: ";
                                        write(reciepientfile, timetxt, strlen(timetxt));
                                        char timestamp[50];
                                        time_t ltime; /* calendar time */
                                        ltime=time(NULL); /* get current cal time */
                                        if(ltime != -1){
                                            struct tm *timeinfo = localtime(&ltime);
                                            strftime(timestamp, sizeof(timestamp), "%Y-%m-%d : %H:%M", timeinfo);
                                            write(reciepientfile, timestamp, strlen(timestamp));
                                            write(reciepientfile, "\n", 1);
                                            st = 8;
                                        }
                                    }
                                }
                                else if(body[i] != '\n'){
                                    char temp[2];
                                    temp[0] = body[i];
                                    temp[1] = '\0';
                                    write(reciepientfile, temp, 1);

                                    

                                    if(body[i] == 'S' && st == 0) st = 1;
                                    // else st = 0;
                                    if(body[i] == 'u' && st == 1) st = 2;
                                    // else st = 0;
                                    if(body[i] == 'b' && st == 2) st = 3;
                                    // else st = 0;
                                    if(body[i] == 'j' && st == 3) st = 4;
                                    // else st = 0;
                                    if(body[i] == 'e' && st == 4) st = 5;
                                    // else st = 0;
                                    if(body[i] == 'c' && st == 5) st = 6;
                                    // else st = 0;
                                    if(body[i] == 't' && st == 6) st = 7;
                                    // else st = 0;

                                    printf("%c", body[i]);
                                }
                            // break;
                            }

                            break;
                            
                        }

                        
                        
                    }

                }
                else if(strstr(message,"QUIT")!=NULL)
                {
                    printf("C: QUIT\n");
                    printf("S: 221 %s closing connection", host);
                    sprintf(ack, "221 %s closing connection\r\n", host);
                    
                    send(newsockfd, ack, strlen(ack), 0);
                }

                
            }

            close(newsockfd);
        }else{
            close(newsockfd);
        }
    }


    return 0;
}