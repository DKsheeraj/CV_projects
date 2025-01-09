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

#define FILENAME "user.txt"
#define MAX_LINE_LENGTH 100
#define MAX_SUBJECT_LENGTH 50
#define MAX_BODY_LENGTH 500
#define MAX_PATH_LENGTH 256

int totalsize = 0;

struct Mail {
    char from[MAX_LINE_LENGTH];
    char to[MAX_LINE_LENGTH];
    char subject[MAX_SUBJECT_LENGTH];
    char received[MAX_LINE_LENGTH];
    char message[MAX_BODY_LENGTH];
    int sz;
};


int authenticateUser(const char *username);
int authenticatePassword(const char *username, const char *password);
void extractMails(FILE *file, struct Mail mails[], int *numMails);

int main(int argc, char *argv[]){
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

    struct Mail mails[100];
    int numMails = 0, mailcount = 0;

    int toBeDeleted[100]; /* marks messages that need to be deleted from server */
    int countOfDeletable = 0;

    while(1){
        clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen) ;

		if (newsockfd < 0) {
			printf("Accept error\n");
			exit(0);
		}
        else{
            char message[100];
            
            sprintf(message, "+OK pop3 server ready\r\n"); 
            printf("S: +OK pop3 server ready\n");
            
            send(newsockfd, message, strlen(message), 0);
        }

        if(fork() == 0){
            close(sockfd);
            char message[1000], user[100], passwd[100];
            while(1){
                int len = recv(newsockfd, message, 1000, 0);
                char ack[1000];

                if(len == 0){
                    printf("Done!\n");
                    exit(0);
                }

                if(strstr(message,"USER")!=NULL){
                    printf("C: ");
                    for(int i=0;i<len;i++)
                    {
                        if(message[i]!='\r' && message[i]!='\n')   printf("%c",message[i]);
                        else {
                            printf("\n");
                            break;
                        }
                    }
                    sscanf(message, "USER %[^\r\n]99s", user);
                    int userfound = authenticateUser(user);
                    if(userfound){
                        printf("S: +OK %s found\n",user);
                        sprintf(ack, "+OK %s found\r\n",user);
                        send(newsockfd, ack, strlen(ack), 0);
                    }else{
                        printf("S: -ERR %s not found\n",user);
                        sprintf(ack, "-ERR %s not found\r\n",user);
                        send(newsockfd, ack, strlen(ack), 0);
                    }
                    
                }else if(strstr(message,"PASS")!=NULL){
                    printf("C: ");
                    for(int i=0;i<len;i++)
                    {
                        if(message[i]!='\r' && message[i]!='\n')   printf("%c",message[i]);
                        else {
                            printf("\n");
                            break;
                        }
                    }

                    sscanf(message, "PASS %[^\r\n]99s", passwd);
                    int matched = authenticatePassword(user, passwd);
                    if(matched){
                        printf("S: +OK Successfully logged in\n");
                        sprintf(ack, "+OK Successfully logged in\r\n");
                        send(newsockfd, ack, strlen(ack), 0);
                    }else{
                        printf("S: -ERR Wrong password\n");
                        sprintf(ack, "-ERR Wrong password\r\n");
                        send(newsockfd, ack, strlen(ack), 0);
                    }

                    
                }else if(strstr(message,"LIST")!=NULL){

                    
                    // printf("C: ");
                    int flagforDigit = 0;
                    for(int i=0;i<len;i++)
                    {
                        if(message[i]!='\r' && message[i]!='\n'){
                            if('0' <= message[i] && message[i] <= '9'){
                                flagforDigit = 1;
                            }
                            // printf("%c",message[i]);
                        }
                        else {
                            // printf("\n");
                            break;
                        }
                    }
                    if(flagforDigit == 1){
                        int index1 = 0;
                        for(int i = 0; i < len; i++){
                            if('0' <= message[i] && message[i] <= '9'){
                                index1 = index1*10 + (message[i] - '0');
                            }else{
                                if(message[i] == '\r') break;
                            }
                        }
                        index1;
                        int index = ntohl(index1);
                        printf("C: LIST %d\n", index);
                        --index;

                        sprintf(ack, "+OK %d %d\r\n", index+1, mails[index].sz);
                        send(newsockfd, ack, strlen(ack), 0);
                        printf("S: +OK %d %d\n", index+1, mails[index].sz);
                        continue;
                    }

                    printf("C: LIST\n");

                    sprintf(ack, "+OK %d messages (%d octets)\r\n",numMails, totalsize);
                    send(newsockfd, ack, strlen(ack), 0);
                    printf("S: +OK %d messages (%d octets)\n",numMails, totalsize);



                    for (int i = 0; i < numMails; i++) {
                        sprintf(ack, "<%s>    <%s>    <%s>\r\n",  mails[i].from, mails[i].received, mails[i].subject);
                        send(newsockfd, ack, strlen(ack), 0);
                    }

                    sprintf(ack, ".\r\n");
                    send(newsockfd, ack, strlen(ack), 0);
                    mailcount = numMails;
                    numMails = 0;
                }else if(strstr(message,"RETR")!=NULL){
                    // printf("C: ");
                    // for(int i=0;i<len;i++)
                    // {
                    //     if(message[i]!='\r' && message[i]!='\n')   printf("%c",message[i]);
                    //     else {
                    //         printf("\n");
                    //         break;
                    //     }
                    // }

                    int index1 = 0;
                    for(int i = 0; i < len; i++){
                        if('0' <= message[i] && message[i] <= '9'){
                            index1 = index1*10 + (message[i] - '0');
                        }else{
                            if(message[i] == '\r') break;
                        }
                    }
                    int index = ntohl(index1);
                    printf("C: RETR %d\n", index);
                    --index;

                    sprintf(ack, "+OK %d octets\r\n", mails[index].sz);
                    send(newsockfd, ack, strlen(ack), 0);
                    printf("S: +OK %d octets\n",mails[index].sz);

                    sprintf(ack, "%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n.\r\n", mails[index].from, mails[index].to, mails[index].subject, mails[index].received, mails[index].message);
                    send(newsockfd, ack, strlen(ack), 0);
                }else if(strstr(message, "DELE") != NULL){
                    // printf("C: ");
                    // for(int i=0;i<len;i++)
                    // {
                    //     if(message[i]!='\r' && message[i]!='\n')   printf("%c",message[i]);
                    //     else {
                    //         printf("\n");
                    //         break;
                    //     }
                    // }

                    int index1 = 0;
                    for(int i = 0; i < len; i++){
                        if('0' <= message[i] && message[i] <= '9'){
                            index1 = index1*10 + (message[i] - '0');
                        }else{
                            if(message[i] == '\r') break;
                        }
                    }
                    int index = ntohl(index1);
                    printf("C: DELE %d\n", index);
                    --index;
                    int already = 0;
                    for(int i = 0; i < countOfDeletable; i++){
                        if(toBeDeleted[i] == index){
                            already = 1;
                            break;
                        }
                    }

                    if(already == 0){
                        toBeDeleted[countOfDeletable++] = index;

                        sprintf(ack, "+OK message %d deleted\r\n", index+1);
                        printf("S: +OK message %d deleted\n", index+1);
                        send(newsockfd, ack, strlen(ack), 0);
                    }else{
                        sprintf(ack, "-ERR message %d already deleted\r\n", index+1);
                        send(newsockfd, ack, strlen(ack), 0);
                        printf("S: -ERR message %d already deleted\n", index+1);
                    }
                    
                }else if(strstr(message, "QUIT") != NULL){
                    printf("C: ");
                    for(int i=0;i<len;i++)
                    {
                        if(message[i]!='\r' && message[i]!='\n')   printf("%c",message[i]);
                        else {
                            printf("\n");
                            break;
                        }
                    }

                    char filePath[MAX_PATH_LENGTH];
                    snprintf(filePath, sizeof(filePath), "./%s/mymailbox.txt", user);

                    FILE *file = fopen(filePath, "w");
                    if (file == NULL) {
                        perror("Error opening file");
                        exit(0);
                    }

                    for(int t = 0; t < mailcount; t++){
                        int flg = 0;
                        for(int find = 0; find  < countOfDeletable; find++){
                            if(toBeDeleted[find] == t){
                                flg = 1;
                                break;
                            }
                        }
                        if(flg == 0){
                            fprintf(file, "%s\n%s\n%s\n%s\n%s", mails[t].from, mails[t].to, mails[t].subject, mails[t].received, mails[t].message);
                        }else{
                            continue;
                        }
                    }

                    sprintf(ack, "+OK pop3 server signing off, Good Bye!\r\n");
                    printf("S: +OK pop3 server signing off, Good Bye!\n");
                    send(newsockfd, ack, strlen(ack), 0);
                    close(newsockfd);
                    exit(0);
                }else if(strstr(message, "STAT") != NULL){
                    printf("C: ");
                    for(int i=0;i<len;i++)
                    {
                        if(message[i]!='\r' && message[i]!='\n')   printf("%c",message[i]);
                        else {
                            printf("\n");
                            break;
                        }
                    }

                    char filePath[MAX_PATH_LENGTH];
                    snprintf(filePath, sizeof(filePath), "./%s/mymailbox.txt", user);

                    FILE *file = fopen(filePath, "r");
                    if (file == NULL) {
                        perror("Error opening file");
                        exit(0);
                    }

                    

                    extractMails(file, mails, &numMails);
                    fclose(file);

                    sprintf(ack, "+OK %d %d\r\n", numMails, totalsize);
                    send(newsockfd, ack, strlen(ack), 0);
                    printf("S: +OK %d %d\n", numMails, totalsize);
                }else if(strstr(message, "RSET") != NULL){
                    printf("C: ");
                    for(int i=0;i<len;i++)
                    {
                        if(message[i]!='\r' && message[i]!='\n')   printf("%c",message[i]);
                        else {
                            printf("\n");
                            break;
                        }
                    }

                    for(int i = 0; i < countOfDeletable; i++){
                        toBeDeleted[i] = -1;
                    }
                    countOfDeletable = 0;


                    sprintf(ack, "+OK maildrop has %d messages (%d octets)\r\n", mailcount, totalsize);
                    send(newsockfd, ack, strlen(ack), 0);
                    printf("S: +OK maildrop has %d messages (%d octets)\n", mailcount, totalsize);
                }
                
            }

            close(newsockfd);
        }else{
            close(newsockfd);
        }

    }
}

int authenticateUser(const char *username) {
   FILE *file = fopen(FILENAME, "r");

    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    char line[MAX_LINE_LENGTH];
    int userFound = 0;

    
    while (fgets(line, sizeof(line), file) != NULL) {
        
        size_t lineLength = strlen(line);
        if (lineLength > 0 && line[lineLength - 1] == '\n') {
            line[lineLength - 1] = '\0';
        }

        char savedUsername[MAX_LINE_LENGTH];
        char savedPassword[MAX_LINE_LENGTH];
        if (sscanf(line, "%s %s", savedUsername, savedPassword) == 2) {
            
            if (strcmp(savedUsername, username) == 0) {
                userFound = 1;
                break;
            }
        }
    }
    fclose(file);

    return userFound;
}

int authenticatePassword(const char *username, const char *password) {
    FILE *file = fopen(FILENAME, "r");

    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    char line[MAX_LINE_LENGTH];
    int passwordMatch = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        size_t lineLength = strlen(line);
        if (lineLength > 0 && line[lineLength - 1] == '\n') {
            line[lineLength - 1] = '\0';
        }

        char savedUsername[MAX_LINE_LENGTH];
        char savedPassword[MAX_LINE_LENGTH];
        if (sscanf(line, "%s %s", savedUsername, savedPassword) == 2) {
            if (strcmp(savedUsername, username) == 0 && strcmp(savedPassword, password) == 0) {
                passwordMatch = 1;
                break;
            }
        }
    }

    fclose(file);

    return passwordMatch;
}

void extractMails(FILE *file, struct Mail mails[], int *numMails) {
    char line[MAX_LINE_LENGTH];
    int mailIndex = -1;

    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, "From:", 5) == 0) {
            mailIndex++;
            mails[mailIndex].sz = 0;
            strcpy(mails[mailIndex].from, line);
            mails[mailIndex].from[strcspn(mails[mailIndex].from, "\n")] = '\0';

            fgets(mails[mailIndex].to, sizeof(mails[mailIndex].to), file);
            fgets(mails[mailIndex].subject, sizeof(mails[mailIndex].subject), file);
            fgets(mails[mailIndex].received, sizeof(mails[mailIndex].received), file);

            mails[mailIndex].to[strcspn(mails[mailIndex].to, "\n")] = '\0';
            mails[mailIndex].subject[strcspn(mails[mailIndex].subject, "\n")] = '\0';
            mails[mailIndex].received[strcspn(mails[mailIndex].received, "\n")] = '\0';

            strcpy(mails[mailIndex].message, "");

        } else if (mailIndex >= 0) {
            if (strcmp(line, "\n") == 0) {
                continue;
            }
            strcat(mails[mailIndex].message, line);
        }
    }
    totalsize = 0;
    for (int i = 0; i <= mailIndex; i++) {
        mails[i].sz = strlen(mails[i].from) + strlen(mails[i].to) + strlen(mails[i].subject) + strlen(mails[i].received) + strlen(mails[i].message);

        totalsize += mails[i].sz;
        
    }

    *numMails = mailIndex + 1;
}
