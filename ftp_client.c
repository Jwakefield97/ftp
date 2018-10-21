#include <stdio.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include "DirUtils.h"


char *address = "127.0.0.1";
int port = 12000;
int socketDescriptor;
struct sockaddr_in addressStruct; 


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

//extract the file number from input buffer
//TODO: fix truncation after 6 int places
int getInputNumber(char *input){
    char number[50];
    int numberIndex = 0;
    for(int i = 0; i < sizeof(input)/sizeof(input[0]); i++){
        if(input[i] != 'u' && input[i] != 'd' && input[i] != '\0'){
            number[numberIndex] = input[i];
            numberIndex++;
        }
    }
    return atoi(number);
}

int main(int argc, char **argv){
    // struct file_info file;
    // file = getFileData("test.txt");
    // printf("num chars %lld\n",file.numChars);

    // for(int i =0; i < file.numLines; i++){
    //     printf("%d: %s\n",i,file.data[i]);
    // }
    
    //if a valid ip is given set the ip else default to 127.0.0.1
    if(argv[1] != NULL && sizeof(argv[1]) >= 7 ){
        address = argv[1];
    }
    //if a valid port number is given set it else default to 12000
    if(argv[2] != NULL && sizeof(argv[2]) > 0){
        port = atoi(argv[2]);
    }

    socketSetupAndConnect();
    char buffer[1000];
    for(;;){
        printf("ftp> ");
        char input[50]; 

        
        fgets(input,sizeof(input),stdin);
        //insert \0 to prevent premature exit of loop
        //look up how to prevent fputs from inserting a newline
        fputs("\0",stdin);

        if(strcmp(input,"exit")==0){
            break;
        }

        if(strcmp(input,"ls client\n")==0){
            char *files[getNumFiles()]; 
            getDirectoryFiles(files);
            for(int i = 0; i < sizeof(files)/sizeof(char*); i++){
                printf("\t%d. %s\n",i,files[i]);
            }
            freeFileArray(files);

        }else if(strcmp(input,"ls server\n")==0){
            send(socketDescriptor, "ls", 2, 0); //send ls command
            int count = 0;
            //collect all of the messages from the server
            while(1){
                read(socketDescriptor,buffer,sizeof(buffer));
                if(buffer[0] == '\0' && buffer[1] == '\0' && buffer[2] == '\0' && buffer[3] == '\0' && buffer[4] == '\0'){  //if the first 5 chars is \0 then the server has stopped sending info
                    break;
                }
                printf("\t%d. %s\n",count,buffer);
                count++;
            }
        }else if(input[0] == 'u') {
            sendFileOverSocket(socketDescriptor,getInputNumber(input),sizeof(buffer));
            printf("uploading\n");
        }else if(input[0] == 'd'){
            getFileFromServer(socketDescriptor,getInputNumber(input),sizeof(buffer));
        }

    }
    close(socketDescriptor);
    return 0;
    
}