#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

void manageMail();
void sendMail();
void quit();
void printacks(char * ack);

#define MAX_SUBJECT_LENGTH 50
#define MAX_BODY_LENGTH 500

struct sockaddr_in	smtpAddr, popAddr;
char command[100]; 
char from[100], to[100], subject[MAX_SUBJECT_LENGTH + 1], body[MAX_BODY_LENGTH + 1];
char *username, *password;

int main(int argc, char *argv[]){

    if(argc < 4){
        printf("*****Insufficient number of arguments*****\n");
        printf("You need to enter the server IP, port number and pop3 port number\n");
        exit(0);
    }

    
    char* servIP = argv[1];
    int smtp_portno = atoi(argv[2]);
    int pop_portno = atoi(argv[3]);

    smtpAddr.sin_family = AF_INET;
	inet_aton(servIP, &smtpAddr.sin_addr);
	smtpAddr.sin_port	= htons(smtp_portno);

    popAddr.sin_family = AF_INET;
    inet_aton(servIP, &popAddr.sin_addr);
    popAddr.sin_port = htons(pop_portno);

    username = (char *)malloc(100);
    printf("Enter username: ");
    scanf("%99s", username);

    password = (char *)malloc(100);
    printf("Enter password: ");
    scanf("%99s", password);

    int choice;

    do{
        printf("\nOptions:\n1. Manage Mail\n2. Send Mail\n3. Quit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                manageMail();
                break;

            case 2:
                sendMail();
                break;

            case 3:
                quit();
                printf("*****Ending the interaction*****\n");
                break;

            default:
                printf("Invalid option. Please enter a valid option.\n");
        }

    }while(choice != 3);

    return 0;
}

int validateEmailFormat(const char *email) {
    int atCount = 0;
    int dotCount = 0;

    if(strlen(email) == 0){
        return 0;
    }

    for (int i = 0; email[i] != '\0'; i++) {
        if (email[i] == '@') {
            atCount++;
            if (i == 0 || email[i + 1] == '\0') {
                return 0; // '@' should not be at the beginning or end
            }
        } else if (email[i] == '.') { //This is just done for additional format checking which is generally followed
            dotCount++;
            if (i == 0 || email[i + 1] == '\0' || email[i - 1] == '@') {
                return 0; // '.' should not be at the beginning, end, or after '@'
            }
        }
    }

    if(atCount == 1){
        return 1; // Valid email format
    }else{
        return 0; // Incorrect counts of '@'
    }
}


int enterMail() {
    
    // printf("From: ");
    getchar();
    // scanf("%99s", from);
    
    
    // printf("To: ");
    // scanf("%99s", to);
    
    // printf("Subject: ");
    // scanf(" %[^\n]s", subject);

    char from1[100], to1[100], subject1[MAX_SUBJECT_LENGTH + 1];
    char c;
    int i = 0, j;
    scanf("%c", &c);
    while(c != '\n'){
        from1[i] = c;
        i++;
        scanf("%c", &c);
    }

    from1[i] = '\0';
    for(j = 6; j < i; j++){
        from[j-6] = from1[j];
    }
    from[j-6] = '\0';

    i = 0;
    scanf("%c", &c);
    while(c != '\n'){
        to1[i] = c;
        i++;
        scanf("%c", &c);
    }
    to1[i] = '\0';
    for(j = 4; j < i; j++){
        to[j-4] = to1[j];
    }
    to[j-4] = '\0';

    i = 0;
    scanf("%c", &c);
    while(c != '\n'){
        subject1[i] = c;
        i++;
        scanf("%c", &c);
    }
    subject1[i] = '\0';

    for(j = 9; j < i; j++){
        subject[j-9] = subject1[j];
    }
    subject[j-9] = '\n';
    subject[j-8] = '\0';


    char line[MAX_BODY_LENGTH + 1];
    body[0] = '\0';
    
    while (1) {
        fgets(line, sizeof(line), stdin);
        
        if(strcmp(line, ".\n") == 0){
            break;
        }
        
        strncat(body, line, sizeof(body) - strlen(body) - 1);
        
        if (strlen(body) > MAX_BODY_LENGTH) {
            printf("*****Message body too long.*****\n");
            return 0;
        }
    }

    
    if (validateEmailFormat(from) && validateEmailFormat(to) && strlen(subject) > 0) {
        printf("\nEntered Mail:\n");
        printf("From: <%s>\nTo: <%s>\nSubject: %sMessage body: %s\n", from, to, subject, body);
        return 1;
    }else{
        printf("*****Incorrect format. Please enter the entire mail again.*****\n");
        return 0;
    }
}

void printacks(char * ack){
    printf("S: ");
    for(int i=0;i<strlen(ack);i++)
    {
        if(ack[i]!='\r' && ack[i]!='\n')printf("%c",ack[i]);
        else {
            printf("\n");
            break;
        }
    }
}

void printmessage(char * message){
    printf("C: ");
    for(int i=0;i<strlen(message);i++)
    {
        if(message[i]!='\r' && message[i]!='\n')   printf("%c",message[i]);
        else {
            printf("\n");
            break;
        }
    }
}


void manageMail(){

    int	sockfd ;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Unable to create socket\n");
		exit(0);
	}
    if((connect(sockfd, (struct sockaddr *) &popAddr,sizeof(popAddr))) < 0){
		perror("Unable to connect to server\n");
		exit(0);
	}

    char popMessage[1000], cliMessage[1000];

    /*RECEIVING SUCCESSFUL CONNECTION  FROM POP3 SERVER*/
    recv(sockfd, popMessage, 1000, 0);
    printacks(popMessage);

    /*SENDING USERNAME TO AUTHENTICATE THE USER*/
    sprintf(cliMessage, "USER %s\r\n", username);
    printf("C: USER %s\n" ,username);
    send(sockfd, cliMessage, strlen(cliMessage), 0);
    int len1 = recv(sockfd, popMessage, 1000, 0);
    printacks(popMessage);

    /*IF USER NOT FOUND*/
    if(strstr(popMessage, "+OK") == NULL){
        exit(0);
    }

    /*SENDING PASSWORD TO AUTHENTICATE THE USER*/
    printf("C: PASS %s\n", password);
    sprintf(cliMessage,"PASS %s\r\n",password);
    send(sockfd, cliMessage, strlen(cliMessage), 0);
    recv(sockfd, popMessage, 1000, 0);
    printacks(popMessage);

    /*IF PASSWORD NOT CORRECT*/
    if(strstr(popMessage, "+OK") == NULL){
        exit(0);
    }

    /*CLIENT GETS ACCESS TO VIEW THE MAILS FROM SERVER*/
    int choice;
    do{

        printf("C: STAT\n");
        sprintf(cliMessage,"STAT\r\n");
        send(sockfd, cliMessage, strlen(cliMessage), 0);
        recv(sockfd, popMessage, 1000, 0);
        printacks(popMessage);

        printf("C: LIST\n");
        sprintf(cliMessage,"LIST\r\n");
        send(sockfd, cliMessage, strlen(cliMessage), 0);



        int cnt = 0, st = 0, temp = 0, flag = 0;
        

        int mode = 0;
        int md = 0;
        while(1){
            int len = recv(sockfd, popMessage, 1000, 0);
            if(flag == 0 && len != 0){
                flag = 1;
            }
            int cntspecial = 0;
            for(int i = 0; i+2 < len; i++){
                /*AT LAST ./r/n IS SENT BY SERVER AFTER SENDING ALL THE NECESSARY INFORMATION*/
                if(mode == 1 && popMessage[i] != ')'){
                    printf("%c", popMessage[i]);
                    continue;
                }
                else if(mode == 1 && popMessage[i] == ')'){
                    mode = 0;
                    printf(")\n");
                    printf("--------------------------------------------------\n\n");
                    continue;
                }

                if(popMessage[i+1] == '\r' && popMessage[i+2] == '\n'&&popMessage[i] =='.'){
                    st = 1;
                    break;
                }

                if(popMessage[i] == '+'){
                    mode = 1;
                    printf("S: %c", popMessage[i]);
                    continue;
                }

                
                if(popMessage[i] != '\n' && popMessage[i] != '\r'){
                    /*MARKING THE START OF MAIL (<From> SO AS TO NUMBER THE MAILS)*/
                    if(popMessage[i] == '<' && popMessage[i+1] == 'F'){
                        if(temp != 0) printf("\n");
                        printf("%d.\t", temp+1);
                        temp++;
                        i = i+6;
                        continue;
                    }

                    if(popMessage[i] == '<' && popMessage[i+1] == 'R'){
                        printf("%c", popMessage[i]);
                        i = i+10;
                        continue;
                    }

                    if(popMessage[i] == '<' && popMessage[i+1] == 'S'){
                        printf("%c", popMessage[i]);
                        i = i+9;
                        continue;
                    }
                    if(popMessage[i] == '>'){
                        cntspecial++;
                        if(cntspecial != 1){
                            printf("%c", popMessage[i]);
                        }
                        if(cntspecial == 4) cntspecial = 0;
                        continue;
                    }

                    printf("%c", popMessage[i]);
                }
            }
            if(st == 1)break;
        }


        cnt = temp; //NUMBER OF MAILS AVAILABLE ON SERVER
        printf("\n\nEnter mail no. to see: ");
        scanf("%d", &choice);
        if(choice == -1){
            break;
        }

        while(choice > temp || choice  <= 0){
            printf("Mail no. out of range, give again\n");
            printf("Enter mail no. to see: ");
            scanf("%d", &choice);
        }
        printf("\n");


        /*ASKING THE SERVER TO LIST THE MESSAGE SIZE IN OCTETS*/
        int choice1 = htonl(choice);
        printf("C: LIST %d\n", choice);
        sprintf(cliMessage,"LIST %d\r\n", choice1);
        send(sockfd, cliMessage, strlen(cliMessage), 0);
        recv(sockfd, popMessage, 1000, 0);
        printacks(popMessage);
        
        /*ASKING THE SERVER TO RETRIVE THE MAIL*/
        choice1 = htonl(choice);
        printf("C: RETR %d\n", choice);
        sprintf(cliMessage,"RETR %d\r\n", choice1);
        send(sockfd, cliMessage, strlen(cliMessage), 0);
        
        /*RECEIVING THAT MAIL FROM SERVER AND PRINTING IT*/
        int ft = 0, modeRet = 0;
        while(1){
            int len = recv(sockfd, popMessage, 1000, 0);
            for(int i=0 ; i+3<len; i++){
                if(popMessage[i] == '+'){
                    modeRet = 1;
                    printf("S: %c", popMessage[i]);
                    continue;
                }else if (modeRet){
                    printf("%c", popMessage[i]);
                    if(i+3 == len-1){
                        printf("%c\n", popMessage[i+1]);
                        modeRet = 0;
                    }
                    continue;
                }
                if(popMessage[i] == '\r' && popMessage[i+1] == '\n' && popMessage[i+2] != '.'){
                    printf("\n");
                    i = i+1;
                }else if(popMessage[i] == '\n' && popMessage[i+1] == '.' && popMessage[i+2] == '\r' && popMessage[i+3] == '\n'){
                    printf(".\n");
                    ft = 1;
                    break;
                }else{
                    printf("%c", popMessage[i]);
                }
            }
            if(ft == 1)break;
        }

        /*TAKING ACTION INPUT FROM THE USER*/
        char whatToDo;
        printf("Give me the action: ");
        scanf(" %c", &whatToDo);
        if(whatToDo == 'd'){
            /*DELETE COMMAND SENT TO SERVER*/
            int client1 = htonl(choice);
            printf("C: DELE %d\n", choice);
            sprintf(cliMessage,"DELE %d\r\n", choice1);
            send(sockfd, cliMessage, strlen(cliMessage), 0);

            /*CORRECT THIS SERVER RESPONSE LATER*/
            recv(sockfd, popMessage, 1000, 0);
            printacks(popMessage);
        }else if(whatToDo == '9'){
            printf("C: RSET\n");
            sprintf(cliMessage,"RSET\r\n");
            send(sockfd, cliMessage, strlen(cliMessage), 0);

            recv(sockfd, popMessage, 1000, 0);
            printacks(popMessage);
        }

        
    }while(choice != -1);

    /*QUIT COMMAND SENT TO SERVER*/
    printf("C: QUIT\n");
    sprintf(cliMessage,"QUIT\r\n");
    send(sockfd, cliMessage, strlen(cliMessage), 0);
    recv(sockfd, popMessage, 1000, 0);
    printacks(popMessage);
    
    close(sockfd);
}



void sendMail(){

    int	sockfd ;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Unable to create socket\n");
		exit(0);
	}
    if ((connect(sockfd, (struct sockaddr *) &smtpAddr,sizeof(smtpAddr))) < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}

    /*CLIENT CONNECTION SUCCESS MESSAGE SENT TO SERVER*/
    char *message = (char *)malloc(1000);
    sprintf(message, "<client connects to SMTP port>\r\n");
    printmessage(message);
    send(sockfd, message, strlen(message), 0);
    
    int mailcheck = enterMail();
    /*IF MAIL ENTERED IN INCORRECT FORMAT*/
    if(mailcheck == 0){
        return;
    }

    char *domain = strchr(from, '@') + 1;
    char message2[1000];

    /*RECEIVING GREETING FROM SERVER*/
    recv(sockfd, message2, 1000, 0);
    printacks(message2);

    /*HELO MESSAGE AND REPLY*/
    sprintf(message, "HELO %s\r\n", domain);
    printmessage(message);
    send(sockfd, message, strlen(message), 0);
    recv(sockfd, message2, 1000, 0);
    printacks(message2);

    
    /*MAIL FROM MESSAGE AND REPLY*/
    sprintf(message, "MAIL FROM: <%s>\r\n", from);
    printmessage(message);
    send(sockfd, message, strlen(message), 0);
    recv(sockfd, message2, 1000, 0);
    printacks(message2);

    /*RCPT TO MESSAGE AND REPLY*/
    sprintf(message, "RCPT TO: <%s>\r\n", to);
    printmessage(message);
    send(sockfd, message, strlen(message), 0);
    int len = recv(sockfd, message2, 1000, 0);
    printacks(message2);

    /*IF USER IS NOT FOUND PRINT ERROR MESSAGE RECEIVED FROM SERVER*/
    for(int i = 0; i < len; i++){
        if(message2[i] == '5' && message2[i+1] == '5' && message2[i+2] == '0'){
            return;
        }
    }

    /*DATA MESSAGE AND REPLY*/
    sprintf(message, "DATA\r\n");
    printmessage(message);
    send(sockfd, message, strlen(message), 0);
    recv(sockfd, message2, 1000, 0);
    printacks(message2);

    /*SENDING MAIL TO SERVER*/
    sprintf(message, "From: <%s>\nTo: <%s>\nSubject: %s%s\n.\n", from, to, subject, body);
    send(sockfd, message, strlen(message), 0);
    printf("C: Mail sent successfully\n");
    
    /*RECEIVED STATUS FROM SERVER*/
    recv(sockfd, message2, 1000, 0);
    printacks(message2);

    /*QUIT MESSAGE AND REPLY*/
    sprintf(message, "QUIT\r\n");
    printmessage(message);
    send(sockfd, message, strlen(message), 0);
    recv(sockfd, message2, 1000, 0);
    printacks(message2);
}

void quit(){
    exit(0);
}


