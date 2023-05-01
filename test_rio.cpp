#include "rio.h"
#include <fcntl.h>
#include <string.h>
#include <iostream>

int main() {
	int fd = open("/home/kirin/Projects/stupid-server/test.txt", O_RDWR);
	const char *str = "hello server\n";

	rio_writen(fd, str, strlen(str));

	lseek(fd, 0, SEEK_SET);

	rio_buf test_buf;
	rio_buf_init(fd, &test_buf);

	char buf[1024];
	rio_realine_b(&test_buf, buf, 1024);
	std::cout << buf;
}
