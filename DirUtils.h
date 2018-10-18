#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>

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