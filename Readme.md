# FTP Client and Sever
## Description 
	A simple example of an FTP like client and server for CSC565. The client must be able to upload/download files, 
	view the contents of a remote directory, and gracefully quit the application. The server must support 5 simultaneous 
	connections and will run on port 12000. 

## Explanation
	For my solution to the ftp server/client I have a client program that makes a connection to a server socket and parses
	user commands and uses a sort of "protocol" to send the command to the server, execute the proper command, and return the
	correct result. The server binds to a port, listens, and accepts requests in an infinite loop. When a valid connection is 
	received the server forks into a new process to handle the clients requests allowing for multiple user access to the server. 
	The protocol used consists of first sending a command (ls,u,d) to the server followed by sending relevant information for that command. 
	In the case of "ls" that is the only information sent before the result is sent back to the client. In the case of upload (u) and
	download (d), after the command is sent the file name and file size is sent to create the corresponding buffer to receive the file. 
	To terminate sending that requires multiple sends from the socket or to indicate that an error has occured five null character "\0\0\0\0\0" 
	are sent over the socket as one message. By default if no command line arguments are passed into the server application it will run on port 
	12000 else it will run on any port you specify. Ex: ./ftp_server <port>. If no arguments are passed into the client application it will try
	to connect to 127.0.0.1:12000. Optionally you can pass in either ip address in dotted decimal format, port or both. If ip is the only argument 
	given it is assumed to run on port 12000. Ex: ./ftp_client <ip> <port>. 
## Known bugs
* Uploading/downloading binary files does not work fully. The entire contents of the file is transferred, however, in the case of images the image is corrupted and in the case of executables it will throw a permissions error when trying to execute it. For this reason the upload/download is limited to text based files for reliable transmission. 

## Directory contents
* ftp_client.c - the source code for the client application.
* ftp_server.c - the server used by the client to connect to. 
* ftp_utils.h  - common helper functions used between the server and client code.
* makefile - file used to clean, build, and run the project. 
* Readme.md - file used to descibe the git repo and project. 

## Building/Running 
* to build run the command ``make`` or ``make all``.
* to clean the project (remove built files) run ``make clean``.
* to clean the project and build it run ``make fresh``. This will also create a test directory and place the ftp_server program in it for testing.
* to pack the relevant project files into a tar execute the command ``make pack``.
