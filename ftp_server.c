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
#include <signal.h>
#include "ftp_utils.h"

const int Q_LEN = 5;  // number of waiting clients
int port; //default port

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
        bzero(buffer,1000);
        length = read(sockAccept, buffer, sizeof(buffer));
        if(length > 0){
            if(buffer[0] == 'l' && buffer[1] == 's'){
                bzero(buffer,1000);
                int numFiles = getNumFiles();
                char *files[numFiles]; 
                getDirectoryFiles(files);
                int charSizeToSend = 0;
                for(int i = 0; i < sizeof(files)/sizeof(files[0]); i++){
                    charSizeToSend += strlen(files[i]);
                    charSizeToSend += 54; //number of possible file choices and the extra chars appended to the send buffer.
                }
                if(charSizeToSend < 1000){ //if char size is less than 1000 make the default size 1000
                    charSizeToSend += (1000 - charSizeToSend);
                }
                char sendBuffer[charSizeToSend];
                bzero(sendBuffer,charSizeToSend);
                for(int i = 0; i < sizeof(files)/sizeof(files[0]); i++){
                    char indexStr[50];
                    sprintf(indexStr,"%d",i);
                    strcat(sendBuffer,"\t");
                    strcat(sendBuffer,indexStr);
                    strcat(sendBuffer,". ");
                    strcat(sendBuffer,files[i]);
                    strcat(sendBuffer,"\n");
                }
                sprintf(buffer,"%d",charSizeToSend);
                write(sockAccept, buffer, sizeof(buffer)); //send number of chars being sent
                write(sockAccept, sendBuffer, sizeof(sendBuffer)); //send chars
                  

                freeFileArray(files,numFiles);
            } else if (buffer[0] == 'u'){ //a file is being uploaded
                read(sockAccept,buffer,sizeof(buffer)); //get file name
                char fileName[sizeof(buffer)]; 
                strcpy(fileName, buffer);

                read(sockAccept,buffer,sizeof(buffer)); //get file size
                getFileFromSocket(sockAccept,fileName,atoi(buffer));
                
            }else if (buffer[0] == 'd'){
                int numFiles = getNumFiles();
                char *files[numFiles]; 
                getDirectoryFiles(files);
                int fileChoice;

                read(sockAccept,buffer,sizeof(buffer)); //get file size
                fileChoice = atoi(buffer);
                if(fileChoice < numFiles){
                    write(sockAccept, files[fileChoice], sizeof(buffer)); //send filename
                    sendFileOverSocket(sockAccept,files[fileChoice],sizeof(buffer));
                }else{
                    write(sockAccept, "\0\0\0\0\0", sizeof(buffer)); //send error back indicating that the filechoice is not valid.
                }

                freeFileArray(files,numFiles);
            } else {
                if(buffer[0] == '\0' && buffer[1] == '\0' && buffer[2] == '\0' && buffer[3] == '\0' && buffer[4] == '\0'){  //if the user has exited
                    break;
                }
            }
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
    if(argv[1] != NULL){
        port = atoi(argv[1]);
    }else{
        printf("The incorrect number of arguments were passed in. Format: ./ftp_server <port>\n");
        exit(1);
    }
    setUpServer();//initialize server
    int sockAccept;   
    addrLen = sizeof(cli_sin);

    while (1){   
        fork();   //fork into a new process to handle the connection
        sockAccept = accept(sockListen,(struct sockaddr *) &cli_sin, (socklen_t *) &addrLen);
        if (sockAccept < 0){
            printf("Failed to accept connection\n");
        }else{
            handleConnection(sockAccept); //handle connection if it is successful
        }
    }
    return 0;
}