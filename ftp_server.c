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
int port = 12000;

void getFileFromSocket(int socketDescriptor, char *fileName){
    char buffer[1000];
    while(1){
        //TODO: keep reading until all the bytes in the file are sent (i.e. 5 \0 characters are sent).
        read(socketDescriptor,buffer,sizeof(buffer));

        if(buffer[0] == '\0' && buffer[1] == '\0' && buffer[2] == '\0' && buffer[3] == '\0' && buffer[4] == '\0'){  //if the first 5 chars is \0 then the server has stopped sending info
            return;
        }else{
            printf("%s\n",buffer);
            saveFile(fileName,buffer);
        }
    }
}

int main(int argc, char** argv) {
    //if a valid port number is given set it else default to 12000
    if(argv[1] != NULL && sizeof(argv[1]) > 0){
        port = atoi(argv[2]);
    }  
    
 
    struct sockaddr_in serv_sin;   
    struct sockaddr_in cli_sin;   
    char buffer[1000];  
    int sockListen;   
    int sockAccept;   
    unsigned int addrLen;   // or socklen_t addrLen
    int length;

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
    addrLen = sizeof(cli_sin);
    while (1){      
        sockAccept = accept(sockListen,(struct sockaddr *) &cli_sin, (socklen_t *) &addrLen);
        if (sockAccept < 0){
            printf("Failed to accept connection\n");
            exit(1);
        }
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
                    getFileFromSocket(sockAccept,fileName);
                    
                    //TODO: after the entire file is collected, store the collected info on the file system.
                }
            } else if (buffer[0] == 'd'){
                //TODO: add code to handle opening and sending a file by name.
            } else {
                break; //invalid command break out of the loop
            }
        }
        close(sockAccept);
    }
    return 0;
}