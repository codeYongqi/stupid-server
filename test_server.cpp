#include "open_sock_fd.h"
#include <sys/socket.h>
#include <memory>
#include <csignal>
#include "http_resp.h"
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

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
	rio_buf client_buf[FD_SETSIZE];	
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
	rio_buf_init(connfd, &client_pool->client_buf[connfd]);

	if (connfd > client_pool->maxfd) {
		client_pool->maxfd = connfd;
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
	} else if ((ptr = strstr(uri, "cgi-bin")) != NULL){
		// /bin/adder?1&2
		char filepath[PATH_MAX];		
		
		ptr += 6;
		*ptr = '.';
		strcpy(filepath, ptr);
		query_string = strchr(filepath, '?');
		*query_string = '\0';
		query_string += 1;
		struct stat stat_buf;
		stat(filepath, &stat_buf);

		char *argv[2];
		argv[0] = filepath;
		argv[1] = NULL;
		if (S_ISREG(stat_buf.st_mode)) {
			setenv("QUERY_STRING", query_string, 1);
			if (fork() == 0) {
				dup2(connfd, STDOUT_FILENO);
				close(connfd);
				execve(filepath, argv, environ);
			} else {
				close(connfd);
			}
		}
			
	} else {
		char buf[8192];	
		const char *body = "<h>Given Path is not found</h>";
		get_http_resp_header(buf, "404", body);
		rio_writen(connfd, buf, strlen(buf));
	}
	fflush(stdout);
}

/**
 * 
 */
void read_header(int connfd, pool* client_pool) {
	int len;
	char buf[1024];
	char method[10];
	char uri[PATH_MAX];
	char version[10];
	int cnt = 0;
	while((len = rio_realine_b(&client_pool->client_buf[connfd], buf, 1024)) > 0) {
		if(strcmp(buf, "\r\n") == 0) {
			break;
		}

		if (cnt == 0){
			char *ptr = strchr(buf, ' ');
			*ptr = '\0';
			strcpy(method, buf);
			ptr += 1;
			char *ptr2 = strchr(ptr, ' ');
			*ptr2 = '\0';
			strcpy(uri, ptr);
			ptr2 += 1;
			char *ptr3 = strchr(ptr2, '\r');
			*ptr3 = '\0';
			strcpy(version, ptr2);

			serve(connfd, uri);
		}

		printf("receive %d bytes from client\n", len);
		printf("%s", buf);
		cnt++;
	}
}

void check_clients(pool* client_pool) {
	int i;
	for(i = 0; i <= client_pool -> maxfd ; i++) {
		if (i == listenfd) continue;
		if(FD_ISSET(i, &client_pool->readyset)) {
			read_header(i, client_pool);
			FD_CLR(i, &client_pool->activeset);
		}	
	}
}

pool client_pool;

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
	init_pool(listenfd, &client_pool);

	char buf[MAXLINE];
	int connfd;
	while(1) {
		client_pool.readyset = client_pool.activeset;
		select(client_pool.maxfd + 1, &client_pool.readyset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &client_pool.readyset)) {
			connfd = accept(listenfd, (struct sockaddr*)&client_addr, &addrlen);
			if (connfd < 0) {
				fprintf(stderr, "failed to accept request");
				continue;
			}
			add_client(connfd, &client_pool);
		}

		check_clients(&client_pool);
	}
}
