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


int main(int argc, char **argv){
    //if a valid ip is given set the ip else default to 127.0.0.1
    if(argv[1] != NULL && sizeof(argv[1]) >= 7 ){
        address = argv[1];
    }
    //if a valid port number is given set it else default to 12000
    if(argv[2] != NULL && sizeof(argv[2]) > 0){
        port = atoi(argv[2]);
    }

    int sd;
    sd = socket(PF_INET, SOCK_STREAM, 0); /* create socket */

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));    /* create & zero struct */
    addr.sin_family = AF_INET;    /* select internet protocol */
    addr.sin_port = htons(port);         /* set the port # */
    addr.sin_addr.s_addr = inet_addr(address); /* set the addr */
    
    int retcode = connect(sd, &addr, sizeof(addr));         /* connect! */
    if(retcode < 0){
         printf("Failed to connect with the host\n"); 
         exit(2);
    }  
    char s[200];
    FILE *fp;
    fp = fdopen(sd, "r+");         /* convert into stream */
    fprintf(fp, "GET / HTTP/1.0\n\n");      /* send request */
    fflush(fp);               /* ensure it got out */
    while ( fgets(s, sizeof(s), fp) != 0 ){  /* while not EOF ...*/
        fputs(s, stdout);           /*... print the data */
    }
    fclose(fp);
    return 0;
}