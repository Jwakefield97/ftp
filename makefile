all: ftp_client ftp_server

ftp_client: ftp_client.c
	gcc -Wall -g -o ftp_client ftp_client.c 

ftp_server: ftp_server.c
	gcc -Wall -g -o ftp_server ftp_server.c

clean: 
	rm -f ftp_client ftp_server

fresh: clean all
 
run: 
	./ftp_server & ./ftp_client