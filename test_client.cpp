#include "rio.h"
#include "open_sock_fd.h"

void send_http_get_request(int connfd, const char *uri) {
	char buf[8192];
	char *ptr = buf;
	int len;
	ptr += sprintf(ptr, "GET %s HTTP/1.1\r\n", uri);
	ptr += sprintf(ptr, "User-agent: Stupid Agent\r\n");
	ptr += sprintf(ptr, "Host: Stupid Server\r\n");
	ptr += sprintf(ptr, "\r\n");
	printf("Test header is:\n%s", buf);
	rio_writen(connfd, buf, strlen(buf));
}

int main(int argc, char **argv) {
	int fd = open_clientfd("127.0.0.1", "8080");
	send_http_get_request(fd, argv[1]);
	int len;
	rio_buf buf;
	char string[1024];
	rio_buf_init(fd, &buf);
	while((len = rio_realine_b(&buf, string ,1024)) > 0) {
		printf("%s", string);
	}
}

