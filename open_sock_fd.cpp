#include "open_sock_fd.h"

int open_clientfd(const char *hostname, const char *port) {
	int fd = -1, ret;
	struct addrinfo hint, *res;
	memset(&hint, 0, sizeof(struct addrinfo));
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_NUMERICSERV | AI_ADDRCONFIG;
	
	ret = getaddrinfo(hostname, port, &hint, &res);
	if (ret != 0) {
		std::cerr << "failed to get addrinfo\n";
	}

	for(struct addrinfo *it = res; it; it = it->ai_next) {
		if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			std::cerr << "failed to create socket fd\n";
			continue;
		} 

		ret = connect(fd, it->ai_addr, it->ai_addrlen);
		if (ret != 0) {
			std::cerr << "failed to connect to " << hostname << "\n";
			continue;
		} else {
			break;
		}
	}

	if (fd < 0) {
		return -1;	
	} else {
		return fd;
	}
}

int open_listenfd(const char *port) {
	int fd = -1, ret;
	struct addrinfo hint, *res;
	memset(&hint, 0, sizeof(struct addrinfo));
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_NUMERICSERV | AI_PASSIVE;
	
	ret = getaddrinfo(NULL, port, &hint, &res);
	if (ret != 0) {
		std::cerr << "failed to get addrinfo\n";
	}

	for(struct addrinfo *it = res; it; it = it->ai_next) {
		if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			std::cerr << "failed to create socket fd\n";
			continue;
		} 

		if ((ret = bind(fd, it->ai_addr, it->ai_addrlen)) == 0) {
			if ((ret = listen(fd, 1024)) == 0 ) {
				break;
			} else {
				std::cerr << "failed to listen to socket fd\n";
				continue;
			}
		} else {
			std::cerr << "failed to bind to socket fd\n";
			fd = -1;
		}
	}

	if (fd < 0) {
		return -1;	
	} else {
		return fd;
	}
}

