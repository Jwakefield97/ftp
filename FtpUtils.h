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

//********************************************************************
//
// Is File 
//
// This function checks to see if a file is a normal file or a directory.
//
// Return Value
// ------------
// int                          0 if the file is a directory and 1 if it is a normal file.
//
// Value Parameters
// ----------------
// None
//
// Reference Parameters
// --------------------
// path         char*           The path to the file. In this case just the filename.
//
// Local Variables
// ---------------
// pathStat     struct stat     The struct holding the information about the dir.   
//
//********************************************************************
int isFile(const char *path) {
    struct stat pathStat;
    stat(path, &pathStat);
    return S_ISREG(pathStat.st_mode);
}

//********************************************************************
//
// Get Number of Files
//
// This function gets the number of files in a directory. 
//
// Return Value
// ------------
// int                          The number of files in the current directory.
//
// Value Parameters
// ----------------
// None
//
// Reference Parameters
// --------------------
// None
//
// Local Variables
// ---------------
// dp           struct dirent*   A dirent directory pointer.
// path         char*            The path to the directory.
// dir          DIR              A directory pointer to the opened directory.
// fileCount    int              The running count of the files in a directory.
//
//********************************************************************
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


//********************************************************************
//
// Get Directory Files
//
// This function gets the filenames from the current directory and stores
// them in an char**.
//
// Return Value
// ------------
// Void
//
// Value Parameters
// ----------------
// None
//
// Reference Parameters
// --------------------
// fileList     char**          The char array used to store the file names in.
//
// Local Variables
// ---------------
// dp           struct dirent*   A dirent directory pointer.
// path         char*            The path to the directory.
// dir          DIR              A directory pointer to the opened directory.
// count        int              The count of valid files. Used to index the fileList.
//
//********************************************************************
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

//********************************************************************
//
// Free File Array
//
// This function is used to free up space that is dynamcially allocated 
// by the getDirectoryFiles function.
//
// Return Value
// ------------
// Void
//
// Value Parameters
// ----------------
// None
//
// Reference Parameters
// --------------------
// array    char**      The array of filenames to deallocate.
//
// Local Variables
// ---------------
// None
//
//********************************************************************
void freeFileArray(char **array){
    for(int i = 0; i < sizeof(array)/sizeof(char*); i++){
        free(array[i]);
    }
}

//********************************************************************
//
// Get File Size
//
// This function gets the size of the file in bytes.
//
// Return Value
// ------------
// unsigned int        The size of the file in bytes.
//
// Value Parameters
// ----------------
// None
//
// Reference Parameters
// --------------------
// filename     char*   The char array containing the filename of the file to get the size of.
//
// Local Variables
// ---------------
// size         int     The running count of the file size.
// infile       FILE    The file pointer to the file to get the size of.
//
//********************************************************************
unsigned int getFileSize(char *filename){
    int size;
    FILE * infile;
    infile = fopen(filename,"r");
    fseek(infile, 0L, SEEK_END);
    size = ftell(infile);
    fclose(infile);
    return size;
}

//********************************************************************
//
// Get File Data
//
// This function gets the data contents of a file and puts it into a char array.
//
// Return Value
// ------------
// Void
//
// Value Parameters
// ----------------
// None
//
// Reference Parameters
// --------------------
// filename         char*   The filename of the file to get the contents of.
// filecontents     char*   The char array to store the contents in.
//
// Local Variables
// ---------------
// infile           FILE    The file pointer to the file to get the data from.
// ch               char    The current char that is being processed.
// index            int     The index of the filecontents array to store the current char in.
//
//********************************************************************
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

//********************************************************************
//
// Save File
//
// This function used to save a file from a buffer.
//
// Return Value
// ------------
// Void
//
// Value Parameters
// ----------------
// size         int     The size of the buffer to be saved into the file.
//
// Reference Parameters
// --------------------
// filename     char*   The name of the file to save the buffer under. 
// buffer       char*   The buffer to save into the file.   
// Local Variables
// ---------------
// file         FILE    The file pointer to the file to modify.
//
//********************************************************************
void saveFile(char *filename, char *buffer, int size){
    FILE *file; 
    file = fopen(filename, "w");
    fwrite(buffer, size, 1, file);
    fclose(file);
}

//********************************************************************
//
// Send File Over Socket
//
// This function is used to send the filename, file size, and file contents to
// another socket.
//
// Return Value
// ------------
// Void
//
// Value Parameters
// ----------------
// socketDescriptor     int             The socket descriptor of the socket to send the file to.
// bufferSize           int             The size of the buffer on the server socket.
//
// Reference Parameters
// --------------------
// fileName             char*           The filename of the file to send.
//
// Local Variables
// ---------------
// fileSize             unsigned int    The size of the file to send.   
// fileData             char*           The array of the data from the file.
// buffer               char*           The buffer used to send the file size.
//
//********************************************************************
void sendFileOverSocket(int socketDescriptor, char *fileName, int bufferSize){
    unsigned int fileSize = getFileSize(fileName);
    char fileData[fileSize]; 
    getFileData(fileName,fileData);
    char buffer[fileSize];
    sprintf(buffer,"%d",fileSize);
    send(socketDescriptor, buffer, bufferSize,0);//send file size
    send(socketDescriptor, fileData, fileSize,0);//send file
}

//********************************************************************
//
// Get File From Socket
//
// This function is used to get the file from the socket and save the file.
//
// Return Value
// ------------
// Void
//
// Value Parameters
// ----------------
// socketDescriptor     int     The socket to receive the file from.
// fileSize             int     The size of the file being received.
//
// Reference Parameters
// --------------------
// fileName             char*   The name of the file being saved.
//
// Local Variables
// ---------------
// buffer               char*   The buffer used to get the file from the socket.
//
//********************************************************************
void getFileFromSocket(int socketDescriptor, char *fileName, int fileSize){
    char buffer[fileSize];
    read(socketDescriptor,buffer,sizeof(buffer));
    saveFile(fileName,buffer,sizeof(buffer));
}