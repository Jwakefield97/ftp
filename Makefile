all: ftp_client ftp_server

ftp_client: ftp_client.c
	gcc -Wall -g -o ftp_client ftp_client.c 

ftp_server: ftp_server.c
	gcc -Wall -g -o ftp_server ftp_server.c

clean: 
	rm -f ftp_client ftp_server
	rm -rf test
	rm -rf .vscode
	rm -f jsw5227.tgz

fresh: clean all
	mkdir test
	rm -f test/ftp_server
	cp ftp_server test/ftp_server

pack: clean
	mkdir jsw5227
	cp ftp_client.c jsw5227/ftp_client.c
	cp ftp_server.c jsw5227/ftp_server.c
	cp ftp_utils.h jsw5227/ftp_utils.h
	cp Makefile jsw5227/Makefile
	cp Readme.md jsw5227/Readme.md
	tar cvfz jsw5227.tgz jsw5227
	rm -rf jsw5227
	



