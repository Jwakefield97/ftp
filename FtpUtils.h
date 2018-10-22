//********************************************************************
//
// Missouri State University Progammer: Jacob Wakefield
// Computer Networks
// Programming Project #0: FTP server
// October 23, 2018
// Instructor: Dr. Ajay K. Katangur
//
//********************************************************************

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int isFile(const char *path) {
    struct stat pathStat;
    stat(path, &pathStat);
    return S_ISREG(pathStat.st_mode);
}

//get num files in a dir 
int getNumFiles(){
    struct dirent *dp;
    const char *path="."; // Directory target
    DIR *dir = opendir(path); // Open the directory - dir contains a pointer to manage the dir

    int fileCount = 0; 
    while ((dp=readdir(dir)) != NULL){ // if dp is null, there's no more content to read
        if(strcmp(dp->d_name,".") != 0 && strcmp(dp->d_name,"..") != 0 && isFile(dp->d_name)){
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
        if(strcmp(dp->d_name,".") != 0 && strcmp(dp->d_name,"..") != 0 && isFile(dp->d_name)){
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

//get number of chars from file
unsigned int getFileSize(char *filename){
    int size;
    FILE * infile;
    infile = fopen(filename,"r");
    fseek(infile, 0L, SEEK_END);
    size = ftell(infile);
    fclose(infile);
    return size;
}

//get data lines from file and store in a char
void getFileData(char *filename, char *filecontents){
    FILE * infile;
    infile = fopen(filename,"rb");
    char ch; 
    int index = 0;
    while((ch = getc(infile)) != EOF){
        filecontents[index] = ch;
        index++;
    } 
    fclose(infile);
}

void saveFile(char *filename, char *buffer, int size){
    FILE *file; 
    file = fopen(filename, "w");
    fwrite(buffer, size, 1, file);
    fclose(file);
}

//TODO: add error handling 
//send a file by the number they chose over to the server
int sendFileOverSocket(int socketDescriptor, char *fileName, int bufferSize){
    unsigned int fileSize = getFileSize(fileName);
    char fileData[fileSize]; 
    getFileData(fileName,fileData);
    char buffer[fileSize];
    sprintf(buffer,"%d",fileSize);
    send(socketDescriptor, buffer, bufferSize,0);//send file size
    send(socketDescriptor, fileData, fileSize,0);//send file
    return 0;
}

void getFileFromSocket(int socketDescriptor, char *fileName, int fileSize){
    char buffer[fileSize];
    read(socketDescriptor,buffer,sizeof(buffer));
    saveFile(fileName,buffer,sizeof(buffer));
}