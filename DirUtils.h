#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>

//get num files in a dir 
int getNumFiles(){
    struct dirent *dp;
    const char *path="."; // Directory target
    DIR *dir = opendir(path); // Open the directory - dir contains a pointer to manage the dir
    int fileCount = 0; 
    while ((dp=readdir(dir)) != NULL){ // if dp is null, there's no more content to read
        if(strcmp(dp->d_name,".") != 0 && strcmp(dp->d_name,"..") != 0){
            fileCount++;
        }
    }
    return fileCount;
}

//get file names from dir
void getDirectoryFiles(char **fileList){
    struct dirent *dp;
    const char *path="."; // Directory target
    DIR *dir = opendir(path); // Open the directory - dir contains a pointer to manage the dir
    int count = 0;
    
    while((dp=readdir(dir)) != NULL){
        if(strcmp(dp->d_name,".") != 0 && strcmp(dp->d_name,"..") != 0){
            fileList[count] = (char*) malloc (strlen(dp->d_name)+1);
            strncpy(fileList[count],dp->d_name,strlen(dp->d_name));
            count++;
        }
    }
}

//free the array used to hold file names
void freeFileArray(char **array){
    for(int i = 0; i < sizeof(array)/sizeof(char*); i++){
        free(array[i]);
    }
}

//get the number of lines in a file
int getNumberOfLinesinFile(char *filename) {
    FILE * infile = fopen(filename,"r");
    int counter = 0;
    char line[501];
    while (fgets(line,501,infile)) {
        counter++;
    }
    return counter;
}

//get data lines from file and store in a char **
char** getFileData(char *filename){
    FILE * infile;
    char line[501];
    int llen;
    int counter = 0;
    char ** info = NULL;
    infile = fopen(filename,"r");

    while (fgets(line,501,infile)) {
        // Allocate memory for pointer to line just added
        info = realloc(info,(counter+1) * sizeof(char *));
        // And allocate memory for that line itself!
        llen = strlen(line);
        info[counter] = calloc(sizeof(char),llen+1);
        // Copy the line just read into that memory
        strcpy(info[counter],line);
        counter++;
    }
    return info;
}

//TODO: add error handling 
//send a file by the number they chose over to the server
int sendFileOverSocket(int socketDescriptor, int fileChoosen, int bufferSize){
    char *files[getNumFiles()]; 
    getDirectoryFiles(files);
    while(1){
        //TODO: open file from file system and keep sending until all the bytes in the file are sent. make a function for it 
        //see -> https://www.daniweb.com/programming/software-development/threads/66574/read-file-with-unknown-buffer-length
        send(socketDescriptor, "u",bufferSize, 0); //send ls command
        send(socketDescriptor, files[fileChoosen], bufferSize,0);
        send(socketDescriptor, "\0\0\0\0\0", bufferSize,0);
        break;
    }
    freeFileArray(files);
    return 0;
}

//TODO: add error handling
int getFileFromServer(int socketDescriptor, int fileChoosen, int bufferSize){
    printf("getting file {%d} from server",fileChoosen);
    return 0; 
}