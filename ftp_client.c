#include <stdio.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

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
    FILE *file_ptr;
    file_ptr = fdopen(socket_descriptor, "r+");         /* convert into stream */
    // fprintf(file_ptr, "GET / HTTP/1.0\n\n");      /* send request */
    // fflush(file_ptr);               /* ensure it got out */
    // while (fgets(buffer, sizeof(buffer), file_ptr) != 0 ){  /* while not EOF ...*/
    //     fputs(buffer, stdout);           /*... print the data */
    // }
    for(;;){
        char input[20]; 
        scanf("%s",input);
        fprintf(file_ptr, "%s\n", input);      /* send request */
        fflush(file_ptr);
        if(strcmp(input,"quit")==0){
            break;
        }
    }

    fclose(file_ptr);
    return 0;
}