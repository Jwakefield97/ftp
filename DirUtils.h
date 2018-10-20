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

//get number of chars from file
unsigned int getFileSize(char *filename){
    FILE * infile;
    char line[501];
    infile = fopen(filename,"r");
    unsigned numChars;
    while (fgets(line,sizeof(line),infile)) {
        numChars += strlen(line);
    }
    fclose(infile);
    return numChars;
}

//get data lines from file and store in a char
void getFileData(char *filename, char *filecontents){
    FILE * infile;
    infile = fopen(filename,"r");
    char ch; 
    int index = 0;
    while((ch = getc(infile)) != EOF){
        filecontents[index] = ch;
        index++;
    } 
    fclose(infile);
}

//TODO: add error handling 
//send a file by the number they chose over to the server
int sendFileOverSocket(int socketDescriptor, int fileChoosen, int bufferSize){
    char *files[getNumFiles()]; 
    getDirectoryFiles(files);
    send(socketDescriptor, "u",bufferSize, 0); //send ls command
    while(1){
        //TODO: open file from file system and keep sending until all the bytes in the file are sent. make a function for it 

        unsigned int fileSize = getFileSize(files[fileChoosen]);
        char fileData[fileSize]; 
        getFileData(files[fileChoosen],fileData);

        while(1){
            char buffer[bufferSize];
            unsigned int lastIndex = 0;

            while(lastIndex < fileSize){
                //populate buffer
                for(int i =0; i < bufferSize; i++){
                    if(i+lastIndex < fileSize){
                        buffer[i] = fileData[i+lastIndex];
                    }
                }
                printf("%s",buffer);
                //send buffer
                send(socketDescriptor, buffer, bufferSize,0);
                //put the next index at the num chars alread sent
                lastIndex += bufferSize;
            }

            send(socketDescriptor, "\0\0\0\0\0", bufferSize,0);
            break;
        }
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