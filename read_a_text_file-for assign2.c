#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For exit() function

int main(int argc, char** argv) {
    char c[1000];
    FILE *fptr;
    char file_name[100];
    int sig;
    char check[10]="end_header";

/* Verify the correct number of arguments were passed in */
	if (argc != 2) {
		fprintf(stderr, "USAGE:./main.out data.txt\n");	
	}

    strcpy(file_name, argv[1]); //copy a string from the commond line


    if ((fptr = fopen(file_name, "r")) == NULL) {
        printf("Error! opening file");
        // Program exits if file pointer returns NULL.
        exit(1);
    }

    // reads text until newline is encountered
   printf("reading from the file:\n"); 
   
   sig=0;

   while (fgets(c, sizeof(c), fptr) != NULL) {
    //if the program read to the end of header
    //then, read the data region and print it to the console (sig==1)

    if (sig == 1)
        fputs(c, stdout);

    /* check whether this line is the end of header,
    the new line in array c contains "end_header\n"*/
    if ((sig==0) && strncmp(c, check, sizeof(check))==0)
    {
    //Yes. The end of header
        sig = 1;
    }			
}

    fclose(fptr);

    return 0;
}
