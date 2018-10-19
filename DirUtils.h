#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>

//struct used to store info about a given file
struct file_info {
    char **data;
    char *fileName;
    long long  int numLines; 
    long long  int numChars; 
};

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


//get data lines from file and store in a char
struct file_info getFileData(char *filename){
    FILE * infile;
    char line[501];
    int llen;
    infile = fopen(filename,"r");

    struct file_info file; 
    file.fileName = filename; 
    file.data = NULL;
    file.numLines = 0; 
    file.numChars = 0;

    while (fgets(line,501,infile)) {
        // Allocate memory for pointer to line just added
        file.data = realloc(file.data,(file.numLines+1) * sizeof(char *));
        // And allocate memory for that line itself!
        llen = strlen(line);
        file.numChars += strlen(line);
        file.data[file.numLines] = calloc(sizeof(char),llen+1);
        // Copy the line just read into that memory
        strcpy(file.data[file.numLines],line);
        file.numLines++;
    }
    fclose(infile);
    return file;
}

//TODO: add error handling 
//send a file by the number they chose over to the server
int sendFileOverSocket(int socketDescriptor, int fileChoosen, int bufferSize){
    char *files[getNumFiles()]; 
    getDirectoryFiles(files);
    send(socketDescriptor, "u",bufferSize, 0); //send ls command
    while(1){
        //TODO: open file from file system and keep sending until all the bytes in the file are sent. make a function for it 
        struct file_info file = getFileData(files[fileChoosen]);
        printf("num chars: %lld\n",file.numChars);
        while(1){

            send(socketDescriptor, files[fileChoosen], bufferSize,0);

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