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
int socket_descriptor;
struct sockaddr_in address_struct; 


void socketSetupAndConnect() {
    socket_descriptor = socket(PF_INET, SOCK_STREAM, 0); /* create socket */
    memset(&address_struct, 0, sizeof(address_struct));    /* create & zero struct */

    address_struct.sin_family = AF_INET;    /* select internet protocol */
    address_struct.sin_port = htons(port);         /* set the port # */
    address_struct.sin_addr.s_addr = inet_addr(address); /* set the addr */
    int retcode = connect(socket_descriptor, &address_struct, sizeof(address_struct));
    if(retcode < 0){
        printf("Failed to connect with the server\n"); 
        exit(2);
    }
    printf("Connection to the server was successful\n"); 
}

int main(int argc, char **argv){
    
    //if a valid ip is given set the ip else default to 127.0.0.1
    if(argv[1] != NULL && sizeof(argv[1]) >= 7 ){
        address = argv[1];
    }
    //if a valid port number is given set it else default to 12000
    if(argv[2] != NULL && sizeof(argv[2]) > 0){
        port = atoi(argv[2]);
    }

    socketSetupAndConnect();
    char buffer[200];
    for(;;){
        printf("ftp> ");
        char input[20]; 
        scanf(" %[^\n]",input);

        if(strcmp(input,"ls client")==0){
            char *files[getNumFiles()]; 
            getDirectoryFiles(files);
            for(int i = 0; i < sizeof(files)/sizeof(char*); i++){
                printf("\t%d. %s\n",i,files[i]);
            }
            freeFileArray(files);

        } else if(strcmp(input,"ls server")==0){
            send(socket_descriptor, "ls", 2, 0); //send ls command
            int count = 0;
            //collect all of the messages from the server
            while(1){
                read(socket_descriptor,buffer,sizeof(buffer));
                if(buffer[0] == '\0' && buffer[1] == '\0' && buffer[2] == '\0' && buffer[3] == '\0' && buffer[4] == '\0'){  //if the first 5 chars is \0 then the server has stopped sending info
                    break;
                }
                printf("\t%d. %s\n",count,buffer);
                count++;
            }
        }

        if(strcmp(input,"quit")==0){
            break;
        }
    }
    close(socket_descriptor);
    return 0;
    
}