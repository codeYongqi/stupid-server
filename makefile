.PHONY: clean
CC = g++
CFLASG = 

all: test_server test_client

test_server: test_server.o open_sock_fd.h http_resp.h rio.h open_sock_fd.o http_resp.o rio.o
	$(CC) -o test_server test_server.o open_sock_fd.h http_resp.h rio.h open_sock_fd.o http_resp.o rio.o

test_server.o: test_server.cpp open_sock_fd.h http_resp.h rio.h open_sock_fd.o http_resp.o rio.o

http_resp.o:  http_resp.h rio.h open_sock_fd.h rio.o open_sock_fd.o

test_client: test_client.o rio.o open_sock_fd.o
	$(CC) -o test_client test_client.o rio.o open_sock_fd.o

test_client.o: rio.h open_sock_fd.h rio.o open_sock_fd.o

rio.o: rio.h

open_sock_fd.o: open_sock_fd.cpp open_sock_fd.h


clean:
	@echo "Start Cleaning"
	-rm *.o 
	-rm test_client
	-rm *.gch
	-rm *.swp
	-rm test_server
	@echo "Ending"
	


