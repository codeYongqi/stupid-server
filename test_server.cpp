#include "open_sock_fd.h"
#include "rio.h"
#include <sys/socket.h>
#include <memory>
#include <csignal>
#include "http_resp.h"
#include <unistd.h>
#include <sys/mman.h>

#define MAXLINE 1024

using namespace std;

int listenfd;

void sig_handler(int sig) {
	printf("close fd:%d", listenfd);
	if (listenfd >= 0) {
		close(listenfd);
	}
	exit(1);
}

typedef struct {
	fd_set activeset;
	fd_set readyset;
	int maxi;
	int maxfd;
	int n_ready;
} pool;

void init_pool(int listenfd, pool* client_pool) {
	client_pool->maxi = -1;
	client_pool->maxfd = listenfd;
	client_pool->n_ready = 0;
	FD_ZERO(&client_pool->activeset);
	FD_SET(listenfd, &client_pool->activeset);
	FD_ZERO(&client_pool->readyset);
}

void add_client(int connfd, pool* client_pool) {
	FD_SET(connfd, &client_pool->activeset);
	if (connfd > client_pool->maxfd) {
		client_pool->maxfd = connfd;
	}
}

void check_clients(pool* client_pool) {
	int i;
	for(i = 0; i < FD_SETSIZE && i != listenfd; i++) {
		if(FD_ISSET(i, &client_pool->readyset)) {
			serve(connfd);//TODO:				
			FD_CLR(i, &client_pool->activeset);
		}	
	}
}

void serve(int connfd, char* uri) {
	char *ptr, *query_string;
	if ((ptr = strstr(uri, "static")) != NULL) {
		// /static/home.html
		ptr += 5;
		*ptr = '.';
		int fd = open(ptr, O_RDONLY, 0);
		struct stat stat_buf;
		stat(ptr, &stat_buf);
		int filesize = stat_buf.st_size;
		char *src = (char *)mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
		rio_writen(fd, src, filesize);
	} else if ((ptr = strstr(uri, "bin")) != NULL){
		// /bin/adder?1&2
		char filepath[PATH_MAX];		
		
		ptr += 2;
		*ptr = '.';
		strcpy(filepath, ptr);
		query_string = strchr(filepath, '?');
		*query_string = '\0';
		query_string += 1;
		struct stat stat_buf;
		stat(filepath, &stat_buf);

		if (S_ISREG(stat_buf.st_mode)) {
			setenv("QUERY_STRING", query_string, 1);
			execve(filepath, NULL, environ);
		}
			
	} else {
		char buf[8192];	
		const char *body = "<h>Given Path is not found</h>";
		get_http_resp_header(buf, "404", body);
		rio_writen(connfd, buf, strlen(buf));
	}
	fflush(stdout);
}

int main(int argc, char **argv) {

	if (argc == 1) {
		printf("usage port <1>\n");
	}

	// start server
	int fd = open_listenfd(argv[1]);
	if (fd < 0) {
		std::cerr << "failed to open_listenfd \n";
		exit(1);
	}
	printf("server is litened on:%s\n", argv[1]);

	listenfd = fd;
	signal(SIGINT, sig_handler);
	struct sockaddr_in client_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	pool client_pool;

	char buf[MAXLINE];
	int connfd;
	while(1) {
		fd_set readfds;
		select(listenfd, &readfds, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &readfds)) {
			connfd = accept(client_pool.maxfd+1, (struct sockaddr*)&client_addr, &addrlen);
			if (connfd < 0) {
				fprintf(stderr, "failed to accept request");
				continue;
			}
			add_client(connfd, &client_pool);
		}

		check_clients(&client_pool);

		rio_buf r_buf;
		rio_buf_init(accept_fd, &r_buf);
		int len;

		while ((len = rio_realine_b(&r_buf, buf, MAXLINE)) > 0){
			printf("receive %d bytes from client\n", len);
			printf("%s", buf);
		}
	}
}
