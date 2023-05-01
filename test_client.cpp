#include "rio.h"
#include "open_sock_fd.h"

int main() {
	int fd = open_clientfd("127.0.0.1", "8080");
	char buf[1024];
	
	while(1) {
		std::cin >> buf;
		rio_writen(fd, buf, strlen(buf));
	}

}
