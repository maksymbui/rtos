/*******************************************************************************
The assignment 3 for subject 48450 (RTOS) in University of Technology Sydney(UTS)
This is a template of Program_1.c template. Please complete the code based on
the assignment 3 requirement. Assignment 3
------------------------------Program_1.c template------------------------------
*******************************************************************************/
#include <pthread.h> /* pthread functions and data structures for pipe */
#include <unistd.h> /* for POSIX API */
#include <stdlib.h> /* for exit() function */
#include <stdio.h> /* standard I/O routines */
#include <stdbool.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>

typedef struct RR_Params {
    int timeQuantum;
} ThreadParams;

typedef struct{
    int pid;
    int arrive_t, wait_t, burst_t, turnaround_t, remain_t;
    bool ready;
} process;

char outputFile[100]; // output file name
int pipeFile[2];
sem_t sem_RR;
pthread_t thread1, thread2;
process processes[8];

void input_processes();
void process_RR(int tq);

/* this function calculates Round Robin (RR) with a time quantum of 4, writes
waiting time and turn-around time to the FIFO */
void *worker1(void *params){
    ThreadParams* threadParams = (ThreadParams*)params;
    input_processes();
    process_RR(threadParams->timeQuantum);

    double wait_sum = 0;
    double turn_sum = 0;
    for (int i = 0; i < 7; i++){
        wait_sum += processes[i].wait_t;
        turn_sum += processes[i].turnaround_t;
    }
    double values[2];
    values[0] = wait_sum/7;
    values[1] = turn_sum/7;
    write(pipeFile[1], values, sizeof(values));
    sem_post(&sem_RR);	
}

/* reads the waiting time and turn-around time through the FIFO and writes to text
file */
void *worker2(){
    sem_wait(&sem_RR);
    FILE *fptr;
    double values[2];
    read(pipeFile[0], values, sizeof(values));
    if ((fptr = fopen(outputFile, "w")) == NULL) {
        perror("Error opening output file");
        exit(1);
    }
    fprintf(fptr,"Average wait time: %fs\n", values[0]);
	fprintf(fptr,"Average turnaround time: %fs\n", values[1]);
    fclose(fptr);
    return NULL;
}

/* this main function creates named pipe and threads */
int main(int argc, char* argv[]){

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <time quantum> <output file>\n", argv[0]);
        return 1;
    }

    if (atoi(argv[1]) <= 0){
        fprintf(stderr, "Time quantum should be an integer greater than 0\n");
        return 1;
    }

    /* initialize the parameters */
    ThreadParams params;
    pthread_t tid[2];
    sem_init(&sem_RR, 0, 0);
    strcpy(outputFile, argv[2]);
    params.timeQuantum = atoi(argv[1]);

    /* creating a named pipe(RR) with read/write permission */
    pipe(pipeFile);

    /* create threads */
    if(pthread_create(&thread1, NULL, (void *)worker1, (void*)(&params))!=0){
        printf("Thread 1 created error\n");
        return -1;
    }
    if(pthread_create(&thread2, NULL, (void *)worker2, NULL)!=0){
        printf("Thread 2 created error\n");
        return -2;
    }

    /* wait for the thread to exit */
    if(pthread_join(thread1, NULL)!=0){
	    printf("join thread 1 error\n");
	    return -3;
	}
	if(pthread_join(thread2, NULL)!=0){
	    printf("join thread 2 error\n");
	    return -4;
	}

    sem_destroy(&sem_RR);
    close(pipeFile[0]);
    close(pipeFile[1]);
    return 0;
}

void input_processes() {
	processes[0].pid = 1; processes[0].arrive_t = 8; processes[0].burst_t = 10;
	processes[1].pid = 2; processes[1].arrive_t = 10; processes[1].burst_t = 3;
	processes[2].pid = 3; processes[2].arrive_t = 14; processes[2].burst_t = 7;
	processes[3].pid = 4; processes[3].arrive_t = 9; processes[3].burst_t = 5;
	processes[4].pid = 5; processes[4].arrive_t = 16; processes[4].burst_t = 4;
	processes[5].pid = 6; processes[5].arrive_t = 21; processes[5].burst_t = 6;
	processes[6].pid = 7; processes[6].arrive_t = 26; processes[6].burst_t = 2;

    //Initialise remaining time to be same as burst time
	for (int i = 0; i < 7; i++) {
		processes[i].remain_t = processes[i].burst_t;
	}
}

void process_RR(int tq){
    int remain = 7, front = 0, rear = 0, time = 0;
    int queue[14];
    int process_time = -1;
    int current_process = 1;

    while(remain > 0){
        for(int i = 0; i < 7; i++){
            if (processes[i].arrive_t == time && processes[i].remain_t > 0 && !processes[i].ready){
                queue[rear++] = i;
                processes[i].ready = true;
            }
        }

        if (front == rear){
            time ++;
            continue;
        }

        current_process = queue[front];
        if (process_time == -1){
            process_time = (processes[current_process].remain_t < tq) ? processes[current_process].remain_t : tq;
        }

        if (process_time > 0){
            processes[current_process].remain_t--;
        }
        process_time--;
        if (process_time == 0){
            if (processes[current_process].remain_t == 0){
                remain--;
                processes[current_process].turnaround_t = time - processes[current_process].arrive_t + 1;
                processes[current_process].wait_t = processes[current_process].turnaround_t - processes[current_process].burst_t;

            } else if (processes[current_process].remain_t > 0) {
                queue[rear++] = current_process;
            }
            front++;
            process_time--;
        }
        time ++;
    }
}