#include <stdio.h>
int main(int argc, char **argv){
    char *address = argv[1];
    int port = atoi(argv[2]);
    printf("%s %d",address,port);
   return 0;
}