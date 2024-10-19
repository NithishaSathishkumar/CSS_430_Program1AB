/*
Nithisha Sathishkumar
CSS 430 Program1A
Professor Peng
*/

#include <sys/types.h> // for fork, wait
#include <sys/wait.h>  // for wait
#include <unistd.h>    // for fork, pipe, dup, close
#include <stdio.h>     // for NULL, perror
#include <stdlib.h>    // for exit
#include <iostream>    // for cout

using namespace std;

//Constant Variable 
const int INVAILD = -1; // invalid number
const int SIZE = 2; // Number of pipes 

//Function Calls
int newProcess(); 
void changeIO(int inputFileDescriptor, int outputFileDescriptor);
void deletePipe(int index, int fds[SIZE][SIZE], bool write, bool read);
void failPipe(int fds[SIZE][SIZE]);

int main(int argc, char** argv) {
    int fds[SIZE][SIZE]; //store pipe    
    int newPid; //empty variable for storing Process ID

    if ( argc != 2 ) {
        cerr << "Usage: processes command" << endl;
        exit(INVAILD);
    }

    newPid = newProcess(); //create child process

    failPipe(fds); //check if the pipe is created sucessfully

    if (newPid == 0) { // if process is a child
        newPid = newProcess(); //create grandchild process

        if (newPid == 0) {//child of grandchild
            newPid = newProcess(); //create great grandchild process

            if (newPid == 0) { //child of great grandchild 
                //Close Unwanted pipes for great grandchild process
                deletePipe(0, fds, false, true); 
                deletePipe(1, fds, true, true);  

                //cout << "Great-grandchild process executing ps -A" << endl;
                
                //redirect input to pipe output
                changeIO(fds[0][1], 1);

                execlp("ps", "ps", "-A", (char*) 0); //excute ps -A command 
                perror("Failed at ps"); //if the ps -A command fails
                exit(INVAILD);//exit out of the program
            
            } else {//parent of great grandchild
                //Close Unwanted pipes for grandchild process
                deletePipe(0, fds, true, false);  
                deletePipe(1, fds, false, true);

                //cout << "Grandchild process executing grep " << argv[1] << endl;

                //redirect input to pipe output
                changeIO(fds[0][0], 0);
                changeIO(fds[1][1], 1);
                
                wait(NULL); //waiting for the grandchild process to complete
                
                execlp("grep", "grep", argv[1], (char *)0); //excute grep command 

                perror("Failed at grep"); //if the grep command fails
                exit(INVAILD); //exit out of the program
            }
        } else { //parent of the grandchild
            //Close Unwanted pipes for child process
            deletePipe(0, fds, true, true);  
            deletePipe(1, fds, true, false); 

            //redirect input to pipe output
            changeIO(fds[1][0], 0);

            wait(NULL); //waiting for the child process to complete

            //cout << "Child process executing wc -l" << endl;
            execlp("wc", "wc", "-l", (char*) 0); //excute wc -l command 

            perror("Failed at wc -l"); //if the wc -l command fails
            exit(INVAILD); //exit out of the program
        }
    } else { //Parent Processes
        wait(NULL); //waiting for the child process to complete
        //cout << "Parent process" << endl;
        cout << "commands completed" << endl; //print if the process is successfully completed 
    }

    return 0;
}

//Using fork create new process
int newProcess() {
    int newPid = fork(); //new process
    if (newPid < 0) { //if the process id is less than 0, exit out of the program
        perror("Error: Fork pid's less than 0");
        exit(INVAILD);
    }
    return newPid;
}

//Change the fds for I/O redirection
void changeIO(int inputFileDescriptor, int outputFileDescriptor) {
    int check = dup2(inputFileDescriptor, outputFileDescriptor); //get the value of the dup2
    if(check == INVAILD) { //if the dups == -1, exit of the of the program
        perror("Failed at dup2");
        exit(INVAILD);
    }
}

//Check if the pipe was created successfully 
void failPipe(int fds[SIZE][SIZE]){
    for (int i = 0; i < SIZE; i++) {
        if (pipe(fds[i]) != 0) { // fds is equal to 0, exit out of the program
            perror("Failed at pipes");
            exit(INVAILD);
        }
    }
}

//close ends of the pipes 
void deletePipe(int index, int fds[SIZE][SIZE], bool write, bool read) {
    if (write) {// close the write of pipe
        close(fds[index][1]);
    }
    
    if (read) {// close the read of pipe
        close(fds[index][0]); 
    }
}