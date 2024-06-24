#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include <sys/sem.h>
#include<time.h>
#include<signal.h>

pthread_t thread_R, thread_S, thread_G;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *thread_R_func(void *arg);
void *thread_S_func(void *arg);
void *thread_G_func(void *arg);
int dropMessage(float P);

int main(){
    int shmid = shmget(ftok(".", 60), sizeof(MTPSocket)*MAX_SOCKETS, 0666|IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    MTPSocket *SM = (MTPSocket *)shmat(shmid, 0, 0);

    

    //INITIALISING THE SHARED MEMORY
    for (int i = 0; i < MAX_SOCKETS; i++) {
        SM[i].is_free = 1;
        for(int j = 0; j < 10; j++){
            SM[i].send_buffer[j].header.sequence_number = -1;
        } 
        for(int j = 0; j < 5; j++){
            SM[i].receive_buffer[j].header.sequence_number = -1;
            SM[i].receive_buffer[j].header.flag = 0;
        }

        SM[i].rwnd.windowSize = 5;
        int packetno = 1;
        for(int j = 0; j < maxseqarray; j++){
            SM[i].rwnd.sequenceNumber[j] = packetno;
            packetno += 1;
            if(packetno == 16) packetno = 1;
        }
        SM[i].rwnd.left = 0;
        SM[i].rwnd.right = MAX_WINDOW_SIZE-1;
        SM[i].noSpace = 0;

        SM[i].swnd.windowSize = 5;
        packetno = 1;
        for(int j = 0; j < maxseqarray; j++){
            SM[i].swnd.sequenceNumber[j] = packetno;
            packetno += 1;
            if(packetno == 16) packetno = 1;
        }
        SM[i].swnd.left = SM[i].swnd.right = -1;
        SM[i].lastSent = 0;
        SM[i].bound = 0;
    }


    int shmid2 = shmget(ftok(".", 11), sizeof(SOCK_info)*1, 0666|IPC_CREAT);
    if (shmid2 == -1) {
        perror("shmget");
        exit(1);
    }



    SOCK_info * sock_info = (SOCK_info *)shmat(shmid2, 0, 0);
    sock_info->sockid = 0;
    sock_info->port = 0;
    for(int i = 0; i < 32; i++){
        sock_info->ip[i] = '\0';
    }
    sock_info->errorno = 0;

    struct sembuf pop, vop;
    pop.sem_num = 0;
    vop.sem_num = 0;    
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1;
    vop.sem_op = 1;

    int sem1 = semget(ftok(".", 3), 1, 0666|IPC_CREAT);
    semctl(sem1, 0, SETVAL, 0);

    int sem2 = semget(ftok(".", 4), 1, 0666|IPC_CREAT);
    semctl(sem2, 0, SETVAL, 0);

    int sem3 = semget(ftok(".", 1), 1, 0666|IPC_CREAT);
    semctl(sem3, 0, SETVAL, 1);

    int sem4 = semget(ftok(".", 2), 1, 0666|IPC_CREAT);
    semctl(sem4, 0, SETVAL, 1);

    int sem5 = semget(ftok(".", 5), 1, 0666|IPC_CREAT);
    semctl(sem5, 0, SETVAL, 1);

    int sem6 = semget(ftok(".", 6), 1, 0666|IPC_CREAT);
    semctl(sem6, 0, SETVAL, 1);

    int sem7 = semget(ftok(".", 7), 1, 0666|IPC_CREAT);
    semctl(sem7, 0, SETVAL, 1);

    int sem8 = semget(ftok(".", 8), 1, 0666|IPC_CREAT);
    semctl(sem8, 0, SETVAL, 1);



    srand(time(NULL));

    pthread_create(&thread_R, NULL, thread_R_func, NULL);
    pthread_create(&thread_S, NULL, thread_S_func, NULL);
    pthread_create(&thread_G, NULL, thread_G_func, NULL);

    
    while(1){
        P(sem1);
        if(sock_info->sockid == 0 && sock_info->errorno == 0 && sock_info->port == 0){
            int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if(sockfd < 0){
                sock_info->errorno = errno;
                sock_info->sockid = -1;
            }
            else{
                sock_info->sockid = sockfd;
            }
            V(sem2);

        }
        else{
            int sockfd = sock_info->sockid;
            struct sockaddr_in selfaddr;
            selfaddr.sin_family = AF_INET;
            selfaddr.sin_port = htons(sock_info->port);
            inet_pton(AF_INET, sock_info->ip, &selfaddr.sin_addr);
            if(bind(sockfd, (struct sockaddr *)&selfaddr, sizeof(selfaddr)) < 0){
                sock_info->errorno = errno;
                sock_info->sockid = -1;
                perror("bind");
            }
            V(sem2);
        }
    }

    pthread_join(thread_R, NULL);
    pthread_join(thread_S, NULL);
    pthread_join(thread_G, NULL);

    return 0;
    
}

void *thread_R_func(void *arg) {
    fd_set readfds;
    struct timeval timeout;
    
    int activity;
    char buffer[MAX_MESSAGE_SIZE];
    int max_fd;

    struct sembuf pop, vop;
    pop.sem_num = 0;
    vop.sem_num = 0;    
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1;
    vop.sem_op = 1;

    int shmid = shmget(ftok(".", 60), sizeof(MTPSocket)*MAX_SOCKETS, 0666|IPC_CREAT);
    if(shmid == -1){
        perror("shmget");
        exit(1);
    }

    MTPSocket *SM = (MTPSocket *)shmat(shmid, (void *)0, 0);

    int sem3 = semget(ftok(".", 1), 1, 0666|IPC_CREAT);
    if(sem3 == -1){ // release s3
        perror("semget");
        exit(1);
    }

    int sem4 = semget(ftok(".", 2), 1, 0666|IPC_CREAT);
    if(sem4 == -1){
        perror("semget");
        exit(1);
    }

    int sem5 = semget(ftok(".", 5), 1, 0666|IPC_CREAT);
    if(sem5 == -1){
        perror("semget");
        exit(1);
    }

    


    while (1) {
        /*INITIALISING readfds*/
        FD_ZERO(&readfds);                           
        max_fd = -1;

        /*WAITING ON SEMAPHORE TO GET ACCESS TO TABLE*/
        P(sem3);

        /*IF A PARTICULAR SOCKET IS BOUND WITH OTHER WE ADD THAT TO readfds*/
        for (int i = 0; i < MAX_SOCKETS; i++) {
            if (!SM[i].is_free) {
                FD_SET(SM[i].udp_socket_id, &readfds);
                if (SM[i].udp_socket_id > max_fd) {
                    max_fd = SM[i].udp_socket_id;
                }
            }
        }

        /*RELEASING THE SEMAPHORE AGAIN*/
        V(sem3);
        

        /*SETTING TIMEOUT FOR SELECT CALL TO BE 10s*/
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        /*USING SELECT CALL TO KNOW WHAT ALL SOCKETS ARE READY FOR RECEIVING*/
        activity = select(max_fd + 1, &readfds, NULL, NULL, &timeout);
        if(activity < 0){
            perror("select");
            sleep(2);
            continue;
        }

        if(activity == 0){
            //TIMEOUT HAPPENED
            
            /*WAITING FOR ACCESS OVER TABLE*/
            P(sem5);

            //SENDING ACKS FOR THOSE SOCKETS WHOSE nospace FLAG IS SET BUT NOW THERE IS AVAILABLE SPACE
            for(int i = 0; i < MAX_SOCKETS; i++){
                if(!SM[i].is_free){
                    int freeSize = 0;
                    printf("\t\t\t\tCurrently present: ");

                    /*CALCULATING FREE SIZE OF THAT PARTICULAR SOCKETS'S RECEIVE BUFFER*/
                    for(int j = 0; j < 5; j++){
                        if(SM[i].receive_buffer[j].header.sequence_number == -1){
                            freeSize += 1;
                        }else{
                            printf("[%d %d] ", SM[i].receive_buffer[j].header.sequence_number, SM[i].receive_buffer[j].header.flag);
                        }
                    }
                    printf("\n");

                    if(freeSize > 0){
                        /*RESETING THE nospace FLAG IF FREE SIZE IS NOT 0*/
                        SM[i].noSpace = 0;

                        /*FINDING THE SEQUENCE OF NUMBER OF LAST INORDER MESSAGE SENT*/
                        int index = SM[i].rwnd.left-1, sqnumForAck;

                        /*IF ITS THE START OF WINDOW WE SEND ACK FOR 0 MESSAGE WHICH ACTUALLY DOESNT EXIST*/
                        if(index == -1) sqnumForAck  = 0;
                        else sqnumForAck = SM[i].rwnd.sequenceNumber[index];

                        /*SENDING THE ACK FOR THE LASTINORDER MESSAGE ALONG WITH UPDATED RECEIVE BUFFER SIZE*/
                        char ack[20];
                        /*ACK FORMAT CLEARLY MENTIONED IN DOCUMENT*/
                        for(int jj = 0; jj < 20; jj++) ack[jj] = '\0';
                        ack[0] = '#';
                        for(int ii = 0; ii < 4; ii++){
                            if((1<<ii)&sqnumForAck) ack[4-ii] = '1';
                            else ack[4-ii] = '0';
                        }
                        ack[5] = '#';
                        for(int ii = 0; ii < 4; ii++){
                            if((1<<ii)&freeSize) ack[9-ii] = '1';
                            else ack[9-ii] = '0';
                        }
                        ack[10] = '\0';

                        /*SENDING ACK TO THE SENDER*/
                        int n = sendto(SM[i].udp_socket_id, ack, strlen(ack), 0, (struct sockaddr *)&SM[i].destination_address, sizeof(SM[i].destination_address));
                        printf("\t\t\t\tSending ACK : %s\n", ack);
                        if(n < 0){
                            perror("sendto");
                            continue;
                        }

                        /*WRT TO FREE SIZE WE UPDATE THE RIGHT POINTER OF RECEIVER WINDOW AND NEW RECEIVE BUFFER SIZE*/
                        SM[i].rwnd.right = SM[i].rwnd.left + freeSize - 1;
                        SM[i].rwnd.windowSize = freeSize;
                        SM[i].noSpace = 0;
                    }
                }
            }
            V(sem5);


            //GO TO NEXT ITERATION AND FRESHLY CHECK FOR NEW SOCKETS GETTING CREATED
            continue;
        }

        
        /*WAITING FOR ACCESS TO SM TABLE*/
        P(sem3);

        /*CHECKING FOR EACH SOCKET IF THAT IS READY TO RECEIVE MESSAGES*/
        for (int i = 0; i < MAX_SOCKETS; i++) {
            if(!SM[i].is_free && FD_ISSET(SM[i].udp_socket_id, &readfds)){
                printf("\t\t\t\tDealing with socket: %d in receiver thread\n", SM[i].udp_socket_id);
                int sock = SM[i].udp_socket_id;
                char buffer[MAX_MESSAGE_SIZE+7];
                for(int j = 0; j < MAX_MESSAGE_SIZE+7; j++){
                    buffer[j] = '\0';
                }
                int addrlen = sizeof(SM[i].destination_address);
                int len = recvfrom(sock, buffer, MAX_MESSAGE_SIZE+7, 0, (struct sockaddr*)&SM[i].destination_address, &addrlen);
                printf("\t\t\t\tReceived Data : %s\n\n", buffer);
                
                /*DROPMESSAGE FUNCTION DECIDES WHETHER MESSAGE IS DROPPED OR NOT*/
                int decider = dropMessage(p);
                if(decider == 1) continue;

                //HANDLING THE ACK MESSAGE
                if(buffer[0] == '#'){
                    if(len <= 13){
                        int sqnum = 0, windowsSizeForReceiver = 0;

                        /*FINDING THE SEQUENCE NUMBER OF MESSAGE FOR WHICH ACK IS RECEIVED*/
                        for(int ii = 1; ii < 5; ii++){
                            if(buffer[ii] == '1') sqnum += (1<<(4-ii));
                        }

                        /*FINDING THE NEW SIZE OF RECEIVE BUFFER*/
                        for(int ii = 6; ii <= 9; ii++){
                            if(buffer[ii] == '1') windowsSizeForReceiver += (1<<(9-ii));
                        }
                       
                        /*FINDING IF THE RECEIVED ACK IS FOR SOME MESSAGE IN THE SENDER WINDOW*/
                        int pp1, flag11 = 0;
                        for(pp1 = SM[i].swnd.left; pp1 <= SM[i].swnd.right; pp1++){
                            if(pp1 < 0) continue;
                            if(SM[i].swnd.sequenceNumber[pp1] == sqnum){
                                flag11 = 1;
                                break;
                            }
                        }

                        /*REMOVING MESSAGES BETWEEN LEFT POINTER AND THE POINTER WHICH POINTS TO ACKED MESSAGE(INCLUSIVE) FROM SEND BUFFER*/
                        /*WE REMOVE THE MESSAGES IN THAT WAY BECAUSE IF WE RECEIVE AN ACK FOR A LATER MESSAGE FOR A LATER MESSAGE BUT NOT FOR EARLIER
                        MESSAGES THAT INDICATES THAT PREVIOUS ACKS ARE LOST AS MESSAGES ARE ALWAYS ACKED INORDER BY THE RECEIVER*/
                        if(flag11 == 1){
                            for(int jj = SM[i].swnd.left; jj <= pp1; jj++){
                                if(jj < 0) continue;
                                for(int j = 0; j < 10; j++){
                                    if(SM[i].send_buffer[j].header.sequence_number == SM[i].swnd.sequenceNumber[jj]){
                                        SM[i].send_buffer[j].header.sequence_number = -1;
                                        for(int k = 0; k < MAX_MESSAGE_SIZE; k++){
                                            SM[i].send_buffer[j].data[k] = '\0';
                                        }
                                        break;
                                    }
                                }
                            }
                        }

                        // UPDATING THE LEFT POINTER OF THE SEND WINDOW IF THE RECEIVED ACK IS THE NEXT EXPECTED ACK
                        if(flag11 == 1){
                            SM[i].swnd.left = pp1+1;
                        }
                        SM[i].swnd.windowSize = windowsSizeForReceiver;

                    }
                    continue;
                }

                //HANDLING THE DATA MESSAGE
                int sqnum = 0;
                for(int ii = 0; ii < 4; ii++){
                    if(buffer[ii] == '1') sqnum += (1<<(3-ii));
                }
                char *data = buffer + 4;
                int pointer, found = 0;

                /*CHECKING WHETHER THE MESSAGE IS EXPECTIED TO COME IN THE RECEIVER WINDOW*/
                for(pointer = SM[i].rwnd.left; pointer <= SM[i].rwnd.right; pointer++){
                    if(SM[i].rwnd.sequenceNumber[pointer] == sqnum){
                        found = 1;
                        break;
                    }
                }

                int isDuplicate = 0;
                //DUPLICATE MESSAGE CAN BE OF TWO TYPES:
                //    1. THE MESSAGE IS IN RWND BUT IS NOT YET REMOVED FROM RECEIVE BUFFER AS THERE ARE PREVIOUS INORDER MESSAGES YET TO BE RECEIVED
                //    2. THE MESSAGE IS PRESENT BEFORE THE LEFT POINTER OF RWND

                //CHECK FOR THE FIRST TYPE OF DUPLICATE MESSAGE
                for(int j = 0; j < 5; j++){
                    if(SM[i].receive_buffer[j].header.sequence_number == sqnum && sqnum != SM[i].rwnd.sequenceNumber[SM[i].rwnd.left]){
                        isDuplicate = 1;
                        break;
                    }
                }

                //CHECK FOR THE SECOND TYPE OF DUPLICATE MESSAGE
                int lastReceivedIndex = SM[i].rwnd.left-1;
                if(lastReceivedIndex != -1){
                    int lastReceived = SM[i].rwnd.sequenceNumber[lastReceivedIndex];
                    if(lastReceived == 1){
                        if(lastReceivedIndex >= 15) lastReceived = 15;
                    }
                    if(sqnum <= lastReceived){
                        isDuplicate = 1;
                    }
                }

                //IT IS DECLARED TO BE OF TYPE 2 BUT THE MESSAGE IS TOO FAR FROM THE CURRENT WINDOW OR RATHER IT 
                //IS EXPECTED TO BE RECEIVED IN CURRENT WINDOW
                for(int jj = SM[i].rwnd.left; jj <= SM[i].rwnd.right; jj++){
                    if(jj < 0) continue;
                    if(SM[i].rwnd.sequenceNumber[jj] == sqnum){
                        isDuplicate = 0;
                        break;
                    }
                }

                if(isDuplicate == 1){
                    //SEND AN ACK FOR THE LAST INORDER MESSAGE DESPITE THE RECEIVED MESSAGE SENDER SENDS
                    char ack[20];
                    ack[0] = '#';
                    int index = SM[i].rwnd.left-1, sqnumForAck;
                    if(index == -1) sqnumForAck  = 0;
                    else sqnumForAck = SM[i].rwnd.sequenceNumber[index];
                    for(int ii = 0; ii < 4; ii++){
                        if((1<<ii)&sqnumForAck) ack[4-ii] = '1';
                        else ack[4-ii] = '0';
                    }
                    ack[5] = '#';
                    for(int ii = 0; ii < 4; ii++){
                        if((1<<ii)&SM[i].rwnd.windowSize) ack[9-ii] = '1';
                        else ack[9-ii] = '0';
                    }
                    ack[10] = '\0';

                    int n = sendto(SM[i].udp_socket_id, ack, strlen(ack), 0, (struct sockaddr *)&SM[i].destination_address, sizeof(SM[i].destination_address));
                    printf("\t\t\t\tSending ACK : %s\n", ack);
                    if(n < 0){
                        perror("sendto");
                        continue;
                    }
                    continue;
                }

                //DROP THE MESSAGE IF THE MESSAGE IS OUT OF ORDER
                if(found == 0){
                    printf("\t\t\t\tMessage out of order\n");
                    continue;
                }

                //IF THE RECEIVED MESSAGE IS THE NEXT EXPECTED MESSAGE UPDATE THE LEFT POINTER
                if(pointer == SM[i].rwnd.left){
                    SM[i].rwnd.left += 1;
                }

                //CHECKING IF THE MESSAGE IS ALREADY THERE IN THE RECEIVE BUFFER SO THAT WE NEED NOT STORE IT AGAIN
                int already = 0;
                for(int j = 0; j < 5; j++){
                    if(SM[i].receive_buffer[j].header.sequence_number == sqnum){
                        if(pointer+1 == SM[i].rwnd.left){
                            SM[i].receive_buffer[j].header.flag = 1;
                        }
                        already = 1;
                        break;
                    }
                }

                //STORING THE MESSAGE IN THE RECEIVE BUFFER IF ALREADY NOT PRESENT
                int spaceFound = 0;
                if(already == 0){
                    for(int j = 0; j < 5; j++){
                        if(SM[i].receive_buffer[j].header.sequence_number == -1){
                            SM[i].receive_buffer[j].header.sequence_number = sqnum;
                            strncpy(SM[i].receive_buffer[j].data, data, strlen(data));
                            if(pointer+1 == SM[i].rwnd.left){
                                SM[i].receive_buffer[j].header.flag = 1;
                            }
                            spaceFound = 1;
                            break;
                        }
                    }
                }

                //CALCULATING THE FREE SIZE OF BUFFER
                int free_size = 0;
                printf("\t\t\t\tCurrent messages in receive buffer: ");
                for(int j = 0; j < 5; j++){
                    if(SM[i].receive_buffer[j].header.sequence_number == -1){
                        free_size += 1;
                    }else{
                        printf("[%d %d] ", SM[i].receive_buffer[j].header.sequence_number, SM[i].receive_buffer[j].header.flag);
                    }
                }
                printf("\n");
                if(free_size == 0){
                    SM[i].noSpace = 1;
                }
                printf("\n");

                //UPDATING THE RIGHT POINTER ACCORDINGLY
                if(free_size == 0) SM[i].rwnd.right = SM[i].rwnd.left;
                else SM[i].rwnd.right = SM[i].rwnd.left + free_size - 1;
                SM[i].rwnd.windowSize = free_size;

                if(pointer+1 == SM[i].rwnd.left && already == 0 && spaceFound == 0){
                    SM[i].rwnd.left--;
                    SM[i].rwnd.right--;
                }

                //SEND AN ACK FOR THE LAST INORDER MESSAGE DESPITE THE RECEIVED MESSAGE SENDER SENDS
                char ack[20];
                ack[0] = '#';
                int index = SM[i].rwnd.left-1, sqnumForAck;
                if(index == -1) sqnumForAck  = 0;
                else sqnumForAck = SM[i].rwnd.sequenceNumber[index];
                for(int ii = 0; ii < 4; ii++){
                    if((1<<ii)&sqnumForAck) ack[4-ii] = '1';
                    else ack[4-ii] = '0';
                }
                ack[5] = '#';
                for(int ii = 0; ii < 4; ii++){
                    if((1<<ii)&free_size) ack[9-ii] = '1';
                    else ack[9-ii] = '0';
                }
                ack[10] = '\0';

                int n = sendto(SM[i].udp_socket_id, ack, strlen(ack), 0, (struct sockaddr *)&SM[i].destination_address, sizeof(SM[i].destination_address));
                printf("\t\t\t\tSending ACK : %s\n", ack);
                if(n < 0){
                    perror("sendto");
                    continue;
                }
                
            }
        }
        V(sem3);

    }
    return NULL;
}

void *thread_S_func(void * arg){
    int shmid = shmget(ftok(".", 60), sizeof(MTPSocket)*MAX_SOCKETS, 0666|IPC_CREAT);
    if(shmid == -1){
        perror("shmget");
        exit(1);
    }

    struct sembuf pop, vop;
    pop.sem_num = 0;
    vop.sem_num = 0;    
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1;
    vop.sem_op = 1;

    int sem6 = semget(ftok(".", 6), 1, 0666|IPC_CREAT);
    if(sem6 == -1){
        perror("semget");
        exit(1);
    }

    int sem7 = semget(ftok(".", 7), 1, 0666|IPC_CREAT);
    if(sem7 == -1){
        perror("semget");
        exit(1);
    }

    int sem8 = semget(ftok(".", 8), 1, 0666|IPC_CREAT);
    if(sem8 == -1){
        perror("semget");
        exit(1);
    }

    MTPSocket *SM = (MTPSocket *)shmat(shmid, 0, 0);

    while(1){
        sleep(T/2);

        time_t curtime = time(NULL);

        /*WAITING FOR ACCESS*/
        P(sem6);

        /*RESENDING MESSAGES IN SENDER WINDOW AFTER TIME OUT*/
        for(int i = 0; i < MAX_SOCKETS; i++){
            if(!SM[i].is_free){
                if(curtime - SM[i].lastSent > T){
                    int seqnoOfStartMessage;
                    if(SM[i].swnd.left == -1){
                        seqnoOfStartMessage = SM[i].swnd.sequenceNumber[0];
                    }else{
                        seqnoOfStartMessage = SM[i].swnd.sequenceNumber[SM[i].swnd.left];
                    }
                    
                    if(SM[i].swnd.right == -1){
                        continue;
                    }
                    int seqnoOfEndMessage =  SM[i].swnd.sequenceNumber[SM[i].swnd.right];

                    for(int j = 0; j < 10; j++){
                        printf("%d ", SM[i].send_buffer[j].header.sequence_number);
                    }
                    printf("\n\n");

                    for(int pointer = SM[i].swnd.left; pointer <= SM[i].swnd.right; pointer++){
                        if(pointer < 0) continue;
                        int mssg = SM[i].swnd.sequenceNumber[pointer];
                        for(int j = 0; j < 10; j++){
                            if(SM[i].send_buffer[j].header.sequence_number == mssg){
                                char buffer[MAX_MESSAGE_SIZE+7];
                                for(int jj = 0; jj < MAX_MESSAGE_SIZE+7; jj++){
                                    buffer[jj] = '\0';
                                }
                                int sqnum = SM[i].send_buffer[j].header.sequence_number;
                                for(int ii = 0; ii < 4; ii++){
                                    if((1<<ii)&sqnum) buffer[3-ii] = '1';
                                    else buffer[3-ii] = '0';
                                }
                                buffer[4] = '\0';
                                strcat(buffer, SM[i].send_buffer[j].data);
                                printf("Resending Data : %s\n\n", buffer);
                                int n = sendto(SM[i].udp_socket_id, buffer, strlen(buffer), 0, (struct sockaddr *)&SM[i].destination_address, sizeof(SM[i].destination_address));
                                if(n < 0){
                                    perror("sendto");
                                    continue;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
        V(sem6);

        P(sem7);
        /*CHECKING IF THERE IS ANY PENDING MESSAGE IN SENDER BUFFER IF SO WE SEND IT ACCORDING TO THE RECEIVE BUFFER
        SIZE AVAILABLE*/
        for(int i = 0; i < MAX_SOCKETS; i++){
            if(!SM[i].is_free){
                int flag = 0;
                int noOfMessagesSent = 0;
                if(SM[i].swnd.right != -1){
                    if(SM[i].swnd.left != -1) noOfMessagesSent = SM[i].swnd.right - SM[i].swnd.left + 1;
                    else noOfMessagesSent = SM[i].swnd.right + 1;
                }
                int maxExtraMessages = SM[i].swnd.windowSize - noOfMessagesSent;
                if(maxExtraMessages == 0) continue;
                int seqnoOfStartMessage = SM[i].swnd.sequenceNumber[SM[i].swnd.right+1];
                int seqnoOfEndMessage =  SM[i].swnd.sequenceNumber[SM[i].swnd.right+maxExtraMessages];
                int leftEnd = SM[i].swnd.right+1;
                int rightEnd = SM[i].swnd.right+maxExtraMessages;
                for(int pointer = leftEnd; pointer <= rightEnd; pointer++){
                    int mssg = SM[i].swnd.sequenceNumber[pointer];
                    for(int j = 0; j < 10; j++){
                        if(SM[i].send_buffer[j].header.sequence_number == mssg){
                            flag = 1;
                            char buffer[MAX_MESSAGE_SIZE+7];
                            for(int jj = 0; jj < MAX_MESSAGE_SIZE+7; jj++){
                                buffer[jj] = '\0';
                            }
                            int sqnum = SM[i].send_buffer[j].header.sequence_number;
                            for(int ii = 0; ii < 4; ii++){
                                if((1<<ii)&sqnum) buffer[3-ii] = '1';
                                else buffer[3-ii] = '0';
                            }
                            buffer[4] = '\0';
                            strcat(buffer, SM[i].send_buffer[j].data);
                            printf("Sending Data : %s\n\n", buffer);

                            int n = sendto(SM[i].udp_socket_id, buffer, strlen(buffer), 0, (struct sockaddr *)&SM[i].destination_address, sizeof(SM[i].destination_address));
                            if(n < 0){
                                perror("sendto");
                                continue;
                            }
                            SM[i].swnd.right += 1;
                            break;
                        }

                    }
                
                }
                if(flag == 0){
                    printf("No message to send\n");
                }else{
                    time_t curtime = time(NULL);
                    SM[i].lastSent = curtime;
                }
            }
        }
        V(sem7);
    }
    

    return NULL;
}

void *thread_G_func(void *arg) {

    int shmid = shmget(ftok(".", 60), sizeof(MTPSocket)*MAX_SOCKETS, 0666|IPC_CREAT);
    if(shmid == -1){
        perror("shmget");
        exit(1);
    }
    MTPSocket* SM = (MTPSocket *)shmat(shmid, 0, 0);
    while (1) {
        pthread_mutex_lock(&lock);

        for(int i = 0; i < MAX_SOCKETS; i++){
            if(!SM[i].is_free){
                int res = kill(SM[i].process_id, 0);
                if(res != 0){
                    SM[i].is_free = 1;
                    for(int j = 0; j < 10; j++){
                        SM[i].send_buffer[j].header.sequence_number = -1;
                    } 
                    for(int j = 0; j < 5; j++){
                        SM[i].receive_buffer[j].header.sequence_number = -1;
                        SM[i].receive_buffer[j].header.flag = 0;
                    }

                    SM[i].rwnd.windowSize = 5;
                    int packetno = 1;
                    for(int j = 0; j < maxseqarray; j++){
                        SM[i].rwnd.sequenceNumber[j] = packetno;
                        packetno += 1;
                        if(packetno == 16) packetno = 1;
                    }
                    SM[i].rwnd.left = 0;
                    SM[i].rwnd.right = MAX_WINDOW_SIZE-1;
                    SM[i].noSpace = 0;

                    SM[i].swnd.windowSize = 5;
                    packetno = 1;
                    for(int j = 0; j < maxseqarray; j++){
                        SM[i].swnd.sequenceNumber[j] = packetno;
                        packetno += 1;
                        if(packetno == 16) packetno = 1;
                    }
                    SM[i].swnd.left = SM[i].swnd.right = -1;
                    SM[i].lastSent = 0;
                    SM[i].bound = 0;
                
                }
            }
        }


        pthread_mutex_unlock(&lock);
        sleep(60);
    }
    return NULL;
}

int dropMessage(float P) {
    float rand_num = (float)rand() / RAND_MAX;
    printf("\t\t\t\tRandom number: %f\n\n", rand_num);
    if (rand_num < P) {
        return 1;
    } else {
        return 0;
    }
}