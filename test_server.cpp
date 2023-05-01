#include "open_sock_fd.h"
#include "rio.h"
#include <sys/socket.h>

#define MAXLINE 1024

int main() {
	int fd = open_listenfd("8080");
	struct sockaddr_in client_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	if (fd < 0) {
		std::cerr << "failed to open_listenfd \n";
		exit(1);
	}

	char buf[MAXLINE];
	
	int accept_fd = accept(fd, (struct sockaddr*)&client_addr, &addrlen);
	if (accept_fd < 0) {
		std::cerr << "failed to accept request\n";
		exit(1);
	}

	rio_buf r_buf;
	rio_buf_init(accept_fd, &r_buf);
	int len;

	//TODO: need to close fd when exited or get signal
	while ((len = rio_realine_b(&r_buf, buf, MAXLINE)) >= 0){
		printf("%s", buf);
		printf("\n");
	}
}
