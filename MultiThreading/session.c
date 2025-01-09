#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <time.h>   
#include <unistd.h> 
#include <pthread.h>
#include <event.h>

pthread_mutex_t mutexP = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condP = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutexR = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condR = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutexS = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condS = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutexD = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condD = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


pthread_t doctor;

char nowTyp = 'E';
int treat = 2;
int nwp = 0, nwr = 0, nws = 0;
int reporterToken = 1, patientToken = 1, salesToken = 1;
int reporterTime = 0, patientTime = 0, salesTime = 0;
int curpatientToken = 0, cursalesToken = 0, curreporterToken = 0;
int curtime = 0;
int done = 0, d = 0;
int DonePat[26], DoneRep[10005], DoneSal[4];


typedef struct {
    char hours[10];
    char mins[10];
} TimeArray;

TimeArray* calculateTimeArray(int curtime) {
    TimeArray* timeArray = (TimeArray*)malloc(sizeof(TimeArray));
    int prev = 0;
    while (curtime < 0)
    {
        curtime += 60;
        prev++;
    }
    int hour = curtime / 60;
    hour -= prev; 
    hour = hour + 9;
    if (hour > 12) {
        hour = hour - 12;
    }
    sprintf(timeArray->hours, "%02d", hour);
    int min = curtime % 60;
    sprintf(timeArray->mins, "%02d", min);
    return timeArray;
}

void WaitHere1()
{
    pthread_mutex_lock(&mutex);
    while (treat == 2)
    {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}

void WaitHere2()
{
    pthread_mutex_lock(&mutex);
    while (treat == 1)
    {
        pthread_cond_wait(&cond, &mutex);
    }
    treat = 2;
    pthread_mutex_unlock(&mutex);

}

void HandleDoctor(){
    pthread_mutex_lock(&mutexD);
    d = 1;
    pthread_cond_signal(&condD);
    pthread_mutex_unlock(&mutexD);
}

void HandleReporter(){
    pthread_mutex_lock(&mutexR);
    nwr--;
    curreporterToken = reporterToken;
    nowTyp = 'R';
    DoneRep[curreporterToken] = 1;
    pthread_cond_signal(&condR);
    pthread_mutex_unlock(&mutexR);
}

void HandlePatient(){
    pthread_mutex_lock(&mutexP);
    nwp--;
    curpatientToken = patientToken;
    nowTyp = 'P';
    DonePat[curpatientToken] = 1;
    pthread_cond_signal(&condP);
    pthread_mutex_unlock(&mutexP);
}

void HandleSales(){
    pthread_mutex_lock(&mutexS);
    nws--;
    cursalesToken = salesToken;
    nowTyp = 'S';
    DoneSal[cursalesToken] = 1;
    pthread_cond_signal(&condS);
    pthread_mutex_unlock(&mutexS);
}

void *Tdoctor(void *param)
{
    while (1)
    {
        pthread_mutex_lock(&mutexD);
        while (d == 0)
        {
            pthread_cond_wait(&condD, &mutexD);
        }
        
        TimeArray* timeArray = calculateTimeArray(curtime);

        printf("[%s:%s] Doctor has next visitor\n", timeArray->hours, timeArray->mins);
        fflush(stdout);


        d = 0;
        pthread_mutex_unlock(&mutexD);

        pthread_mutex_lock(&mutex);
        treat = 1;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
}

void *Tpatient(void *thispatientToken)
{
    pthread_mutex_lock(&mutexP);
    while (nowTyp != 'P' && curpatientToken != *((int *)thispatientToken) && DonePat[*((int *)thispatientToken)] == 0)
    {
        pthread_cond_wait(&condP, &mutexP);
    }
    
    TimeArray* timeArray = calculateTimeArray(curtime);
    int curtimeaft = curtime + patientTime;
    TimeArray* timeArrayAft = calculateTimeArray(curtimeaft);
    

    printf("[%s:%s] - [%s:%s] Patient %d in Doctors Chamber\n", timeArray->hours, timeArray->mins, timeArrayAft->hours, timeArrayAft->mins, patientToken);

    nowTyp = 'E';
    pthread_mutex_unlock(&mutexP);
    pthread_mutex_lock(&mutex);
    treat = 0;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *Treporter(void *thisreporterToken)
{
    pthread_mutex_lock(&mutexR);
    while (nowTyp != 'R' && curreporterToken != *((int *)thisreporterToken) && DoneRep[*((int *)thisreporterToken)] == 0)
    {
        pthread_cond_wait(&condR, &mutexR);
    }
    
    TimeArray* timeArray = calculateTimeArray(curtime);
    int curtimeaft = curtime + reporterTime;
    TimeArray* timeArrayAft = calculateTimeArray(curtimeaft);
    
    printf("[%s:%s] - [%s:%s] Reporter %d in Doctors Chamber\n", timeArray->hours, timeArray->mins, timeArrayAft->hours, timeArrayAft->mins, reporterToken);

    nowTyp = 'E';
    pthread_mutex_unlock(&mutexR);
    pthread_mutex_lock(&mutex);
    treat = 0;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *salesThread(void *thissalesToken)
{
    pthread_mutex_lock(&mutexS);
    while (nowTyp != 'S' && cursalesToken != *((int *)thissalesToken) && DoneSal[*((int *)thissalesToken)] == 0)
    {
        pthread_cond_wait(&condS, &mutexS);
    }
    
    TimeArray* timeArray = calculateTimeArray(curtime);
    int curtimeaft = curtime + salesTime;
    TimeArray* timeArrayAft = calculateTimeArray(curtimeaft);
    
    printf("[%s:%s] - [%s:%s] Sales Representative %d in Doctors Chamber\n", timeArray->hours, timeArray->mins, timeArrayAft->hours, timeArrayAft->mins, salesToken);

    nowTyp = 'E';
    pthread_mutex_unlock(&mutexS);
    pthread_mutex_lock(&mutex);
    treat = 0;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main()
{
    memset(DonePat, 0, sizeof(DonePat));
    memset(DoneRep, 0, sizeof(DoneRep));
    memset(DoneSal, 0, sizeof(DoneSal));

    eventQ E;
    eventQ E1, E2, E3;
    E = initEQ("ARRIVAL.txt");
    E1.n = 0, E2.n = 0, E3.n = 0;
    E1.Q = (event *)malloc(128 * sizeof(event));
    E2.Q = (event *)malloc(128 * sizeof(event));
    E3.Q = (event *)malloc(128 * sizeof(event));

    for (int i = 0; i < E.n; i++)
    {
        for (int j = 1; j < E.n; j++)
        {
            if (eventcmp(E.Q[j], E.Q[j - 1]) < 0)
            {
                event t = E.Q[j];
                E.Q[j] = E.Q[j - 1];
                E.Q[j - 1] = t;
            }
        }
    }

    pthread_create(&doctor, NULL, Tdoctor, NULL);
    int i = 0;
    int numexp = 0, numexs = 0;
    int numrep = 0;

    while (i < E.n)
    {
        if (E.Q[i].time <= curtime)
        {
            while (i < E.n && E.Q[i].time <= curtime)
            {
                if (E.Q[i].type == 'P')
                {
                    numexp++;
                    int timer = E.Q[i].time;
                    TimeArray *timeArray = calculateTimeArray(timer);
                    printf("\t\t[%s:%s] Patient %d arrives\n", timeArray->hours, timeArray->mins, numexp);
                    if (done == 1)
                    {
                        printf("\t\t[%s:%s] Patient %d leaves (session ended)\n", timeArray->hours, timeArray->mins, numexp);
                        i++;
                        continue;
                    }
                    if (numexp > 25)
                    {
                        printf("\t\t[%s:%s] Patient %d leaves (quota full)\n", timeArray->hours, timeArray->mins, numexp);
                        i++;
                        continue;
                    }
                    nwp++;
                    E2 = addevent(E2, E.Q[i]);
                    pthread_t patient;
                    pthread_create(&patient, NULL, Tpatient, (void *)&numexp);
                }
                else if (E.Q[i].type == 'R')
                {
                    numrep++;
                    int timer = E.Q[i].time;
                    
                    TimeArray *timeArray = calculateTimeArray(timer);

                    printf("\t\t[%s:%s] Reporter %d arrives\n", timeArray->hours, timeArray->mins, numrep);
                    if (done == 1)
                    {
                        printf("\t\t[%s:%s] Reporter %d leaves (session ended)\n", timeArray->hours, timeArray->mins, numrep);
                        i++;
                        continue;
                    }
                    pthread_t reporter;
                    nwr++;
                    E1 = addevent(E1, E.Q[i]);
                    pthread_create(&reporter, NULL, Treporter, (void *)&numrep);
                }
                else if (E.Q[i].type == 'S')
                {
                    numexs++;
                    int timer = E.Q[i].time;
                    
                    TimeArray *timeArray = calculateTimeArray(timer);

                    printf("\t\t[%s:%s] Sales Representative %d arrives\n", timeArray->hours, timeArray->mins, numexs);
                    if (done == 1)
                    {
                        printf("\t\t[%s:%s] Sales Representative %d leaves (session ended)\n", timeArray->hours, timeArray->mins, numexs);
                        i++;
                        continue;
                    }
                    if (numexs > 3)
                    {
                        printf("\t\t[%s:%s] Sales Representative %d leaves (quota full)\n", timeArray->hours, timeArray->mins, numexs);
                        i++;
                        continue;
                    }
                    nws++;
                    E3 = addevent(E3, E.Q[i]);
                    pthread_t sales;
                    pthread_create(&sales, NULL, salesThread, (void *)&numexs);
                }
                i++;
            }
        }
        else
        {

            if (patientToken == 26 && salesToken == 4 && nwr == 0 && done == 0)
            {
                done = 1;
                TimeArray *timeArray = calculateTimeArray(curtime);
                printf("[%s:%s] Doctor leaves\n", timeArray->hours, timeArray->mins);
            }
            else if (nwp + nws + nwr == 0 || done == 1) curtime = E.Q[i].time;
            else if (done == 0)
            {
                while (curtime < E.Q[i].time && (nwp + nws + nwr != 0))
                {

                    if (nwr > 0)
                    {
                        event e = E1.Q[0];
                        E1 = delevent(E1);
                        reporterTime = e.duration;
                        HandleDoctor();
                        WaitHere1();
                        // nowTyp = 'R';
                        // curreporterToken = reporterToken;
                        usleep(1000);
                        HandleReporter();
                        WaitHere2();
                        curtime += e.duration;
                        reporterToken++;
                    }
                    else if (nwp > 0)
                    {
                        event e = E2.Q[0];
                        E2 = delevent(E2);
                        patientTime = e.duration;
                        HandleDoctor();
                        WaitHere1();
                        HandlePatient();
                        WaitHere2();
                        curtime += e.duration;
                        patientToken++;
                    }
                    else if (nws > 0)
                    {
                        event e = E3.Q[0];
                        E3 = delevent(E3);
                        salesTime = e.duration;
                        HandleDoctor();
                        WaitHere1();
                        HandleSales();
                        WaitHere2();
                        curtime += e.duration;
                        salesToken++;
                    }
                }
            }
            else curtime = E.Q[i].time;
        }
    }

    while (done == 0 && (nwp + nws + nwr != 0))
    {
        if (nwr > 0)
        {
            event e = E1.Q[0];
            E1 = delevent(E1);
            reporterTime = e.duration;
            HandleDoctor();
            WaitHere1();
            // nowTyp = 'R';
            // curreporterToken = reporterToken;
            usleep(1000);
            HandleReporter();
            WaitHere2();
            curtime += e.duration;
            reporterToken++;
        }
        else if (nwp > 0)
        {
            event e = E2.Q[0];
            E2 = delevent(E2);
            patientTime = e.duration;
            HandleDoctor();
            WaitHere1();
            HandlePatient();
            WaitHere2();
            curtime += e.duration;
            patientToken++;
        }
        else if (nws > 0)
        {
            event e = E3.Q[0];
            E3 = delevent(E3);
            salesTime = e.duration;
            HandleDoctor();
            WaitHere1();
            HandleSales();
            WaitHere2();
            curtime += e.duration;
            salesToken++;
        }
    }
}