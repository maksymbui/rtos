# Real-Time Operating Systems Assignment 2: File Processing with Threads, Pipes, and Semaphores

## Overview

This assignment involves creating a real-time file processing program in C that utilizes threads, pipes, semaphores, and shared memory. The goal is to read data from a file, process it through multiple threads, and output the processed data into another file. The task also includes experimenting with and analyzing the behavior of threads and synchronization mechanisms in the absence of semaphores.

## Key Concepts

- **Threads**: You are required to implement three threads (A, B, and C) that sequentially process data.
- **Pipes**: Threads A, B, and C communicate through pipes to pass data between them.
- **Semaphores**: To ensure mutual exclusion and synchronization among threads, semaphores will be used.
- **Shared Memory**: You will also explore shared memory for data exchange, and analyze the behavior of threads without semaphores.

## Program Overview

The program will process the content of a file named `data.txt` and write the processed data into a new file called `output.txt`. The file `data.txt` contains a header region and a content region. The header region should be discarded, and only the content region should be written to `output.txt`.

### File Format of `data.txt`

ply
format ascii 1.0
comment VCGLIB generated
element vertex 5
property float x
property float y
property float z
element face 0
property list uchar int vertex_indices
end_header
-0.999323 5.07845 17.0996 
-0.551401 5.14165 15.803 
-0.937731 5.09894 16.1129 
-0.962823 4.11493 16.7939 
-0.292709 3.10348 22.8929

### Thread Operations

1. **Thread A**: Reads one line of data from the `data.txt` file and writes it to a pipe.
2. **Thread B**: Reads data from the pipe and passes it to Thread C.
3. **Thread C**: Processes the data:
   - Identifies whether the line is from the header or content region.
   - If the line is from the content region, writes it to the `output.txt` file.
   - Discards lines from the header region.
   - Identifies the end of the header using a smart detection method.
4. **Threads Synchronization**: The threads A, B, and C will run sequentially (one thread at a time, blocked when not executing) in a loop, processing one line at a time (A → B → C).

### Key Requirements

- Implement **three threads**: A, B, and C.
- Use **pipes** to pass data between threads.
- Apply **semantics** to ensure proper synchronization and mutual exclusion among threads.
- Detect the end of the **file header region** using a smart solution in Thread C.
- Implement **error handling** to deal with the end of the file when Thread A finishes reading.

### Additional Task: Shared Memory Experiment

- Write a program that reads information from shared memory and outputs it to your monitor.
- **Experiment**: Run the program without semaphores and observe the behavior of threads and pipes. Summarize your findings in your assignment report.

## Assignment Structure

- **`data.txt`**: The input file to be read and processed.
- **`output.txt`**: The output file where the processed data will be written.
- **C Code**: Implement the logic as described using C programming language.
- **Makefile**: If you're using a Makefile, ensure to include it with your submission.

## Submission Guidelines

1. **Code Submission**:
   - Upload the `C` code file(s).
   - If a Makefile is used for compilation, submit the `Makefile` as well.

2. **Report Submission**:
   - Write a report summarizing the implementation and the results of your experiment (with and without semaphores).
   - Include your analysis and observation, particularly focusing on the effect of synchronization mechanisms.
   
3. **Group Work**:
   - This assignment can be completed individually or in pairs.
   - If working in pairs, both students will receive the same mark.
   - All group changes must be approved by the subject coordinator.

4. **Deadline**: The assignment is due on **11th April 2025 at 23:59 PM**.

## Grading Criteria

- **Correct Implementation**: All threads must function correctly with proper synchronization, file reading/writing, and pipe usage.
- **Efficiency**: Proper use of semaphores, pipes, and threads, ensuring mutual exclusion and optimal execution.
- **Analysis**: Clear and detailed analysis of the experimental part (effect of semaphores).
- **Code Quality**: Well-structured code with comments explaining key logic and synchronization mechanisms.
- **Timeliness**: Submission by the specified deadline.

## Conclusion

This assignment will help you develop an understanding of real-time operating system concepts like multithreading, inter-thread communication (using pipes), and synchronization (using semaphores). By completing this assignment, you will enhance your skills in handling real-time tasks and memory management in C programming.

Good luck, and remember to adhere to the technical report template when writing your assignment report!

---

If you need further clarification or help during the implementation, don't hesitate to reach out.
