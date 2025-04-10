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
#include  <sys/mman.h>
#include  <fcntl.h>
#include  <sys/types.h>
#include  <sys/stat.h>

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
  pthread_t tid[3]; // Three threads
  ThreadParams params;
  pipe(params.pipeFile);

  strcpy(params.inputFile, argv[1]); // input file
  strcpy(params.outputFile, argv[2]); // output file

  // Initialization
  initializeData(&params);

  // Create Threads
  pthread_create(&(tid[0]), &attr, &ThreadA, (void*)(&params));
  pthread_create(&(tid[1]), &attr, &ThreadB, (void*)(&params));
  pthread_create(&(tid[2]), &attr, &ThreadC, (void*)(&params));

  // Wait on threads to finish
  pthread_join(tid[0], NULL);
  pthread_join(tid[1], NULL);
  pthread_join(tid[2], NULL);
  
  // Clean up
  close(shm_fd);
  shm_unlink(SHARED_MEM_NAME);
  close(params.pipeFile[0]);
  close(params.pipeFile[1]);
  return 0;
}

void initializeData(ThreadParams *params) {
  // Initialize Sempahores
  if(sem_init(&(params->sem_A), 0, 0) != 0) { // Set up Sem for thread A
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

  // Shared memory
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
  ThreadParams* threadParams = (ThreadParams*)params;
  FILE *fptr;
  if ((fptr = fopen(threadParams->inputFile, "r")) == NULL) {
    printf("Error! opening file");
    // Program exits if file pointer returns NULL.
    exit(1);
  }

  // Loop for writting lines into a pipe one by one
  while(fgets(threadParams->message, sizeof(threadParams->message) , fptr) != NULL) {
    write(threadParams->pipeFile[1],threadParams->message,strlen(threadParams->message) + 1);
    sem_post(&threadParams->sem_B); 
    sem_wait(&threadParams->sem_A);
  }

  // Exit program if no more lines left in the input file
  if (fgets(threadParams->message, sizeof(threadParams->message), fptr) == NULL){
    exit(1);
  }

  fclose(fptr);
  return 0;
}

void* ThreadB(void *params) {
  char ch[256];
  ThreadParams* threadParams = (ThreadParams*)params;

  // Map shared memory for writing
  char* sharedMemory = mmap(0, SHARED_MEM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (sharedMemory == MAP_FAILED) {
    perror("mmap fail");
    exit(1);
  }

  // Loop for reading a line from a pipe and copying a line to shared memory
  while (1) {
    sem_wait(&threadParams->sem_B);  // Wait for Thread A to post (i.e., data in the pipe)
    read(threadParams->pipeFile[0], ch, sizeof(ch));
    // Copy data into shared memory
    strncpy(sharedMemory, ch, SHARED_MEM_SIZE);
    // Post to Thread C to signal that there is new data in shared memory
    sem_post(&threadParams->sem_C);
  }

  return 0;
}

void* ThreadC(void *params){
ThreadParams* threadParams = (ThreadParams*)params;
  FILE *fptr;
  int startWriting = 0;

  // Open the output file for writing
  if ((fptr = fopen(threadParams->outputFile, "w")) == NULL) {
    perror("Error opening output file");
    exit(1);
  }

  // Map shared memory for reading
  char* sharedMemory = mmap(0, SHARED_MEM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
  if (sharedMemory == MAP_FAILED) {
    perror("mmap failed in ThreadC");
    exit(1);
  }

  while (1) {
    sem_wait(&threadParams->sem_C);

    // Check if shared memory is empty
    if (strlen(sharedMemory) == 0) {
      exit(1);
    }

    // Start writing lines to output file after reaching end of header
    if (strncmp(sharedMemory, "end_header", strlen("end_header")) == 0){
      startWriting = 1;
    }

    if(startWriting && strncmp(sharedMemory, "end_header", strlen("end_header")) != 0) {
      fputs(sharedMemory, fptr);
      fputs(sharedMemory, stdout);
    }

    sem_post(&threadParams->sem_A);
  }

  fclose(fptr);
  return 0;
}