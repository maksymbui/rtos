/******************************************************************************* 
The assignment 3 for subject 48450 (RTOS) in University of Technology Sydney(UTS) 
This is a template of Program_2.c template. Please complete the code based on 
the assignment 3 requirement. Assignment 3 

------------------------------Program_2.c template------------------------------
*******************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

//Number of pagefaults in the program
int pageFaults = 0;

//Function declaration
void SignalHandler(int signal);

/**
 Main routine for the program. In charge of setting up threads and the FIFO.

 @param argc Number of arguments passed to the program.
 @param argv array of values passed to the program.
 @return returns 0 upon completion.
 */
int main(int argc, char* argv[])
{

	if (argc != 2){
		fprintf(stderr, "Usage: %s <size of frame>\n", argv[0]);
		return 1;
	}

	//Register Ctrl+c(SIGINT) signal and call the signal handler for the function.
	//add your code here
	
    int i;
	// reference number
	int REFERENCESTRINGLENGTH=24;
	//Argument from the user on the frame size, such as 4 frames in the document
	int frameSize = atoi(argv[1]);
	//Frame where we will be storing the references. -1 is equivalent to an empty value
	int frame[REFERENCESTRINGLENGTH];
	//Reference string from the assignment outline
	int referenceString[24] = {7,0,1,2,0,3,0,4,2,3,0,3,0,3,2,1,2,0,1,7,0,1,7,5};
	//Next position to write a new value to.
	int nextWritePosition = 0;
	//Boolean value for whether there is a match or not.
	bool match = false;
	// //Current value of the reference string.
	// int currentValue;
	//Current index of the frame string
	int frameIdx;

	if (frameSize <= 0 || frameSize > REFERENCESTRINGLENGTH){
		fprintf(stderr, "Given frame should be in a range from 1 to 24");
		return 1;
	}

	//Initialise the empty frame with -1 to simulate empty values.
	for(i = 0; i < frameSize; i++){
		frame[i] = -1;
	}
	printf("Initializing an empty frame. \t |");
	printf("\tFrame: [");
	for (frameIdx = 0; frameIdx < frameSize; frameIdx++){
		if (frameIdx != 0) printf(", ");
		if (frame[frameIdx] == -1){
			printf("_ ");
		} else {
			printf("Error initializing an empty frame");
			return 1;
		}
	}
	printf("]\n");

	//Loop through the reference string values.
	for(i = 0; i < REFERENCESTRINGLENGTH; i++){
		for(frameIdx = 0; frameIdx < frameSize; frameIdx++){
			if (referenceString[i] == frame[frameIdx]){
				match = true;
				break;
			}
		}
		if (match){
			printf("Page [%d] is loaded in frame \t | ", referenceString[i]);
			printf("\tFrame: [");
			match = false;
		} else {
			printf("Page [%d] caused a page fault. \t | ", referenceString[i]);
			printf("\tFrame: [");
			pageFaults++;
			frame[nextWritePosition] = referenceString[i];
			if (nextWritePosition == frameSize - 1){
				nextWritePosition = 0;
			}else{
				nextWritePosition++;
			}
		}

		for (frameIdx = 0; frameIdx < frameSize; frameIdx++){
			if (frameIdx != 0) printf(", ");
			if (frame[frameIdx] == -1){
				printf("_ ");
			} else {
				printf("%d ", frame[frameIdx]);
			}
		}
		printf("]\n");
	}


	signal(SIGINT, SignalHandler);
	printf("Press Ctrl+C\n");
	//Sit here until the ctrl+c signal is given by the user.
	while(1){
		sleep(1);
	}

	return 0;
}

/**
 Performs the final print when the signal is received by the program.

 @param signal An integer values for the signal passed to the function.
 */
void SignalHandler(int signal)
{
	printf("\nTotal page faults: %d\n", pageFaults);
	exit(0);
}
