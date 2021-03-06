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
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include "ftp_utils.h"


char *address; //address of the ftp server
int port; //port of the ftp server
int socketDescriptor; //socket descriptor of the open socket with the server
struct sockaddr_in addressStruct; //address struct of the server 

//********************************************************************
//
// Socket Setup and Connect 
//
// This function is used to setup the socket and make the initial connection 
// to the server.
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
void socketSetupAndConnect() {
    socketDescriptor = socket(PF_INET, SOCK_STREAM, 0); /* create socket */
    memset(&addressStruct, 0, sizeof(addressStruct));    /* create & zero struct */

    addressStruct.sin_family = AF_INET;    /* select internet protocol */
    addressStruct.sin_port = htons(port);         /* set the port # */
    addressStruct.sin_addr.s_addr = inet_addr(address); /* set the addr */
    int retcode = connect(socketDescriptor, (struct sockaddr*) &addressStruct, sizeof(addressStruct));
    if(retcode < 0){
        printf("Failed to connect with the server\n"); 
        exit(2);
    }
    printf("Connection to the server was successful\n"); 
}

//********************************************************************
//
// Get Input Number 
//
// This function gets the number entered with the command. 
// ex: "u 12" would get the number 12 from the input.
//
// Return Value
// ------------
// int                  The extracted number from the input buffer.
//
// Value Parameters
// ----------------
// None
//
// Reference Parameters
// --------------------
// input        char*   The input buffer used to collect the command from the user.
//
// Local Variables
// ---------------
// number       char*   The char array used to store only the number chars.
// numberIndex  int     The index of the next place in the char number array.
//
//********************************************************************
int getInputNumber(char *input){
    char number[50];
    int numberIndex = 0;
    for(int i = 0; i < strlen(input); i++){
        if(input[i] != 'u' && input[i] != 'd' && input[i] != '\0'){
            number[numberIndex] = input[i];
            numberIndex++;
        }
    }
    return atoi(number);
}

//********************************************************************
//
// Interpret Command
//
// This function interprets the command entered by the user and makes the 
// corresponding network calls.
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
// buffer   char*       A char array used to collect information from the socket.
// input    char*       A char array used to store commands entered by the user.
//
//********************************************************************
void interpretCommand(){
    char buffer[1000];
    bzero(buffer,1000);
    char input[50]; 
    fgets(input,sizeof(input),stdin);

    if(strcmp(input,"bye\n")==0){
        send(socketDescriptor, "\0\0\0\0\0", sizeof(buffer),0);  // Echo msg
        exit(1);
    }

    if(strcmp(input,"ls client\n")==0){
        int numFiles = getNumFiles();
        char *files[numFiles]; 
        getDirectoryFiles(files);
        for(int i = 0; i < sizeof(files)/sizeof(char*); i++){
            printf("\t%d. %s\n",i,files[i]);
        }
        freeFileArray(files,numFiles);

    }else if(strcmp(input,"ls server\n")==0){
        send(socketDescriptor, "ls", sizeof(buffer), 0); //send ls command
        bzero(buffer,1000);

        //collect all of the messages from the server
        read(socketDescriptor,buffer,sizeof(buffer)); //get number of chars being sent
        
        char recvBuffer[atoi(buffer)];
        read(socketDescriptor,recvBuffer,sizeof(buffer)); //get number of chars being sent
        printf("%s\n",recvBuffer);
            
    }else if(input[0] == 'u') {
        int fileNumber = getInputNumber(input);
        int numFiles = getNumFiles();
        if(fileNumber >= 0 && fileNumber < numFiles){
            char *files[numFiles]; 
            getDirectoryFiles(files);
            send(socketDescriptor, "u",sizeof(buffer), 0); //send u command
            send(socketDescriptor, files[fileNumber], sizeof(buffer), 0); //send filename

            sendFileOverSocket(socketDescriptor,files[fileNumber],sizeof(buffer)); //send file
            freeFileArray(files,numFiles);
        }
    }else if(input[0] == 'd'){
        int fileNumber = getInputNumber(input);//get the file number choosen. not currently being used (needs to be sent to server)
        if(fileNumber >= 0){
            send(socketDescriptor, "d",sizeof(buffer), 0); //send d command
            sprintf(buffer,"%d",fileNumber);
            send(socketDescriptor, buffer,sizeof(buffer), 0); //send file choice
            bzero(buffer,1000);
            read(socketDescriptor,buffer,sizeof(buffer)); //get file name
            if(buffer[0] != '\0' && buffer[1] != '\0' && buffer[2] != '\0' && buffer[3] != '\0' && buffer[4] != '\0'){  //if the file option was invalid
                //extract filename 
                char fileName[sizeof(buffer)]; 
                strcpy(fileName, buffer);
                bzero(buffer,1000);

                read(socketDescriptor,buffer,sizeof(buffer)); //get file size
                getFileFromSocket(socketDescriptor,fileName,atoi(buffer));
            }
            
        }
    }
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
// None
//
//********************************************************************
int main(int argc, char **argv){
    
    //if a valid ip is given set the ip else default to 127.0.0.1
    if(argv[1] != NULL && argv[2] != NULL){
        address = argv[1];
        port = atoi(argv[2]);
    }else{
        printf("The incorrect number of arguments were passed in. Format: ./ftp_client <ip> <port>\n");
        exit(1);
    }
    socketSetupAndConnect();
    
    for(;;){
        printf("ftp> ");
        interpretCommand();
    }
    close(socketDescriptor);
    return 0;
    
}
