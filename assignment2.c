/***********************************************************************************/
//***********************************************************************************
//            *************NOTE**************
// This is a template for the subject of RTOS in University of Technology Sydney(UTS)
// Please complete the code based on the assignment requirement.

//***********************************************************************************
/***********************************************************************************/

/*
  To compile assign2_template-v3.c ensure that gcc is installed and run 
  the following command:

  gcc your_program.c -o your_ass-2 -lpthread -lrt -Wall
*/

#include  <pthread.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <stdio.h>
#include  <sys/types.h>
#include  <fcntl.h>
#include  <string.h>
#include  <sys/stat.h>
#include  <semaphore.h>
#include  <sys/time.h>

/* to be used for your memory allocation, write/read. man mmsp */
#define SHARED_MEM_NAME "/my_shared_memory"
#define SHARED_MEM_SIZE 1024

/* --- Structs --- */
typedef struct ThreadParams {
  int pipeFile[2]; // [0] for read and [1] for write. use pipe for data transfer from thread A to thread B
  sem_t sem_A, sem_B, sem_C; // the semphore
  char message[256];
  char inputFile[100]; // input file name
  char outputFile[100]; // output file name
} ThreadParams;

/* Global variables */
int sum = 1;
pthread_attr_t attr;
int shm_fd;// use shared memory for data transfer from thread B to Thread C 

/* --- Prototypes --- */
/* Initializes data and utilities used in thread params */
void initializeData(ThreadParams *params);
/* This thread reads data from data.txt and writes each line to a pipe */
void* ThreadA(void *params);
/* This thread reads data from pipe used in ThreadA and writes it to a shared variable */
void* ThreadB(void *params);
/* This thread reads from shared variable and outputs non-header text to src.txt */
void* ThreadC(void *params);

/* --- Main Code --- */
int main(int argc, char const *argv[]) {
  pthread_t tid[3]; // three threads
  ThreadParams params;

  pipe(params.pipeFile);
  strcpy(params.inputFile, argv[1]); // Maksym

  // Initialization
  initializeData(&params);
  // Create Threads
  pthread_create(&(tid[0]), &attr, &ThreadA, (void*)(&params));
  //TODO: add your code
  pthread_create(&(tid[1]), &attr, &ThreadB, (void*)(&params));
  pthread_create(&(tid[2]), &attr, &ThreadC, (void*)(&params));
  // Wait on threads to finish
  pthread_join(tid[0], NULL);
  //TODO: add your code
  pipe(params.pipeFile);
  strcpy(params.inputFile, argv[1]);
  pthread_join(tid[1], NULL);
  pthread_join(tid[2], NULL);
  return 0;
}

void initializeData(ThreadParams *params) {
  // Initialize Sempahores
  if(sem_init(&(params->sem_A), 0, 1) != 0) { // Set up Sem for thread A
    perror("error for init threa A");
    exit(1);
  }
  if(sem_init(&(params->sem_B), 0, 0) != 0) { // Set up Sem for thread B
    perror("error for init threa B");
    exit(1);
  }
  if(sem_init(&(params->sem_C), 0, 0) != 0) { // Set up Sem for thread C
    perror("error for init threa C");
    exit(1);
  }
// Initialize thread attributes 
  pthread_attr_init(&attr);
  //TODO: add your code
  shm_fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open fail");
    exit(1);
  }
  if(ftruncate(shm_fd, SHARED_MEM_SIZE) == -1){
    perror("ftruncate fail");
    exit(1);
  }
  return;
}

void* ThreadA(void *params) {
  //TODO: add your code
  ThreadParams* threadParams = (ThreadParams*)params;
  FILE *fptr;
  if ((fptr = fopen(threadParams->inputFile, "r")) == NULL) {
    printf("Error! opening file");
    // Program exits if file pointer returns NULL.
    exit(1);
  }
  printf("Thread A opened file - %s\n",threadParams->inputFile);
  printf("Thread A sends data to a pipe \n");
  while(fgets(threadParams->message, sizeof(threadParams->message) , fptr) != NULL) {
    write(threadParams->pipeFile[1],threadParams->message,strlen(threadParams->message) + 1);
    sem_post(&threadParams->sem_B);   
    sem_wait(&threadParams->sem_A);
  }
  fclose(fptr);
  printf("\nThread A signals EOF \n");
  strcpy(threadParams->message, "");
  write(threadParams->pipeFile[1], threadParams->message, strlen(threadParams->message) + 1);
  sem_post(&threadParams->sem_B);
  return 0;
}

void* ThreadB(void *params) {
  char ch[256];
  ThreadParams* threadParams = (ThreadParams*)params;

  char* sharedMemory = mmap(0, SHARED_MEM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (sharedMemory == MAP_FAILED) {
    perror("mmap fail");
    exit(1);
  }

  while (1){
    sem_wait(&threadParams->sem_B);
    int n = read(threadParams->pipeFile[0],ch,sizeof(ch));
    if (n==1){
      break;
    }
    printf("Thread B reads from a pipe: %s", ch); 
    strncpy(sharedMemory, ch, SHARED_MEM_SIZE);
    printf("Thread B writes to shared memory: %s", sharedMemory);
    sem_post(&threadParams->sem_C);
    sem_post(&threadParams->sem_A);
  }

  return 0;
}

void* ThreadC(void *params){
  return 0;
}