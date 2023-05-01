#define MAXSIZE 8192
#include <cstdlib>
#include <unistd.h>
#include <sys/errno.h>
#include <algorithm>
#include <malloc.h>
#include <cstring>

typedef struct {
	char buf[MAXSIZE];	
	char *ptr;
	int n_left;
	int fd;
} rio_buf;

int rio_readn(int fd, char* buf, int len);

int rio_writen(int fd, const char* buf, int len);

void rio_buf_init(int fd, rio_buf *rb);

int rio_readn_b(rio_buf* rb, char*buf, int len); 

int rio_realine_b(rio_buf* rb, char*buf, int max_len); 

