# MultithreadedFilestatServer
Statistical Analysis from .dat Files in Directory Structures
This project in C language is designed to compute statistics (such as mean, standard deviation, and others) of numbers present in text files with the extension ".dat" within complex directory structures.

Program Functionality
The Master process recursively explores the specified directory structure.
Worker threads read the names of ".dat" files from a shared buffer, perform the required calculations, and send the results to the Collector.
The Collector, acting as a server via sockets, receives data from the Workers and prints a formatted table with the calculated statistics.
Implementation Structure
The Master receives as command-line parameters the initial directory name and the number of Worker threads to use.
Worker threads perform the necessary calculations on ".dat" files and send the results to the Collector via sockets.
Main Components
Master: Explores directories, acquires names of ".dat" files, and communicates with Worker threads.
Worker: Executes required calculations on ".dat" files and sends results to the Collector via sockets.
Collector: Acts as a server, receives results from Worker threads via sockets, and prints a table with requested statistic values.
Usage and Adaptability
This program can be adapted to compute various statistics beyond mean and standard deviation. By modifying the functionality of Worker threads, it is possible to perform custom statistical calculations, such as variance, median, or other statistical measures.

Notes
The program employs a complex structure of processes, threads, and sockets to manage communication and parallel computations, providing flexibility to customize calculations based on specific requirements.

Instructions
Compile the program using the provided Makefile.
Verify the correctness of the obtained results by following the instructions in the included README.
