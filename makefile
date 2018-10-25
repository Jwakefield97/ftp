all: ftp_client ftp_server

ftp_client: ftp_client.c
	gcc -Wall -g -o ftp_client ftp_client.c 

ftp_server: ftp_server.c
	gcc -Wall -g -o ftp_server ftp_server.c

clean: 
	rm -f ftp_client ftp_server
	rm -rf jwakefield
	rm -rf test
	rm -rf .vscode
	rm -f jwakefield.tgz

fresh: clean all
	mkdir test
	rm -f test/ftp_server
	cp ftp_server test/ftp_server

pack: clean
	mkdir jwakefield
	cp ftp_client.c jwakefield/ftp_client.c
	cp ftp_server.c jwakefield/ftp_server.c
	cp FtpUtils.h jwakefield/FtpUtils.h
	cp Makefile jwakefield/Makefile
	cp Readme.md jwakefield/Readme.md
	tar cvfz jwakefield.tgz jwakefield
	



