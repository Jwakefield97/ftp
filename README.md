# FTP Client and Sever
## Description 
	A simple example of an FTP like client and server for CSC565. The client must be able to upload/download files, 
	view the contents of a remote directory, and gracefully quit the application. The server must support 5 simultaneous 
	connections and will run on port 12000. 

## Explanation

## Known bugs

## Directory contents
* ftp_client.c - the source code for the client application.
* ftp_server.c - the server used by the client to connect to. 
* makefile - file used to clean, build, and run the project. 
* .gitignore - file used to prevent git from tracking certain files. 
* README.md - file used to descibe the git repo and project. 

## Building/Running 
* to build run the command ``make`` or ``make all``.
* to clean the project (remove built files) run ``make clean``.
* to clean the project and build it run ``make fresh``.
* to run the project execute ``make run``. This will start the server then the client. Optionally you can execute them separately without the use of make.