//********************************************************************
//
// Missouri State University Progammer: Jacob Wakefield
// Computer Networks
// Programming Project #0: FTP server
// October 23, 2018
// Instructor: Dr. Ajay K. Katangur
//
//********************************************************************

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "FtpUtils.h"

const int Q_LEN = 5;  // number of waiting clients
int port = 12000; //default port

struct sockaddr_in serv_sin;    
struct sockaddr_in cli_sin;   
  
int sockListen;   // socket used for listening for connections

unsigned int addrLen;   // or socklen_t addrLen

//********************************************************************
//
// Set Up Server
//
// This function sets up the server socket, listens, and binds to 
// the server port.
//
// Return Value
// ------------
// Void
//
// Value Parameters
// ----------------
// None
//
// Reference Parameters
// --------------------
// None
//
// Local Variables
// ---------------
// None
//
//********************************************************************
void setUpServer(){
    // Setup address structure   
    bzero((char *) &serv_sin, sizeof(serv_sin));
    serv_sin.sin_family = AF_INET;
    serv_sin.sin_addr.s_addr = INADDR_ANY;   
    serv_sin.sin_port = htons(port);   // Setup listening socket   
    sockListen = socket(PF_INET, SOCK_STREAM, 0);

    if (sockListen < 0) {
        printf("Failed to create listening socket\n");
        exit(1);
    }

    if (bind(sockListen, (struct sockaddr *) &serv_sin, 
            sizeof(serv_sin)) < 0) {
        printf("Failed to bind listening socket to address \n");
        exit(1);
    }

    if (listen(sockListen, Q_LEN) < 0){
        printf("Failed to listen\n");
        exit(1);
    }
}
//********************************************************************
//
// Handle Connection
//
// This function handles incoming requests from a given connection. 
// It loops on the socket until the user quits or the connection is lost.
//
// Return Value
// ------------
// Void
//
// Value Parameters
// ----------------
// sockAccept       int     The socket descriptor of the new connection.   
//
// Reference Parameters
// --------------------
// None
//
// Local Variables
// ---------------
// buffer           char*   The buffer used to receive information from the socket.
// length           int     The length of the information sent to the socket.
//
//********************************************************************
void handleConnection(int sockAccept){
    char buffer[1000];
    int length;
    while (sockAccept > 0) {
        length = read(sockAccept, buffer, sizeof(buffer));
        if(length > 0){
            if(buffer[0] == 'l' && buffer[1] == 's'){
                char *files[getNumFiles()]; 
                getDirectoryFiles(files);
                for(int i = 0; i < sizeof(files)/sizeof(files[0]); i++){
                    write(sockAccept, files[i], sizeof(buffer));  // send the file name to the client

                    //if this is the last message being sent send only \0
                    if(i == (sizeof(files)/sizeof(files[0])-1)){
                        write(sockAccept, "\0\0\0\0\0", sizeof(buffer));  // Echo msg  
                    }
                }
                freeFileArray(files);
            } else if (buffer[0] == 'u'){ //a file is being uploaded
                read(sockAccept,buffer,sizeof(buffer)); //get file name
                char fileName[sizeof(buffer)]; 
                strcpy(fileName, buffer);

                read(sockAccept,buffer,sizeof(buffer)); //get file size
                getFileFromSocket(sockAccept,fileName,atoi(buffer));
                
            }else if (buffer[0] == 'd'){
                char *files[getNumFiles()]; 
                getDirectoryFiles(files);
                int fileChoice;

                read(sockAccept,buffer,sizeof(buffer)); //get file size
                fileChoice = atoi(buffer);
                printf("%d\n",fileChoice);
                write(sockAccept, files[fileChoice], sizeof(buffer)); //send filename

                sendFileOverSocket(sockAccept,files[fileChoice],sizeof(buffer));
                freeFileArray(files);
            } else {
                if(buffer[0] == '\0' && buffer[1] == '\0' && buffer[2] == '\0' && buffer[3] == '\0' && buffer[4] == '\0'){  //if the user has exited
                    break;
                }
            }

        bzero(buffer,1000);
        }else{
            break; //the client has disconnected 
        }
    }
    close(sockAccept);
}

//********************************************************************
//
// Main
//
// This function is the entry point to the application. 
//
// Return Value
// ------------
// int                  The exit value of the program. 
//
// Value Parameters
// ----------------
// argc     int         Number of arguments passed to the program
//
// Reference Parameters
// --------------------
// argv     char**      An array of char arrays containing the arguments passed to the program.
//
// Local Variables
// ---------------
// sockAccept           The new connection made to the server.
//
//********************************************************************
int main(int argc, char** argv) {
    //if a valid port number is given set it else default to 12000
    if(argv[1] != NULL && sizeof(argv[1]) > 0){
        port = atoi(argv[2]);
    }   
    setUpServer();//initialize server
    int sockAccept;   
    addrLen = sizeof(cli_sin);

    while (1){   
        fork();   //fork into a new process to handle the connection
        sockAccept = accept(sockListen,(struct sockaddr *) &cli_sin, (socklen_t *) &addrLen);
        if (sockAccept < 0){
            printf("Failed to accept connection\n");
            exit(1);
        }
        handleConnection(sockAccept);
    }
    return 0;
}