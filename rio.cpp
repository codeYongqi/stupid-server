#include "rio.h"

/**
 * blocking until readed given n bytes from file, or EOF 
 * @param 
 * @return: count of bytes readed
 */
int rio_readn(int fd, char* buf, int len) {
	int left = len, n_read = 0;
	char*ptr = buf;

	while (left > 0) {
		n_read = read(fd, buf, left);
		if (n_read < 0) {
			if (errno == EINTR) {
				n_read = 0;
			} else {
				return -1;
			}
		} else if (n_read == 0) {
			break;
		}

		left -= n_read;
		buf += n_read;
	}

	return len - left;
}

/**
 * blocking until wirte given n bytes to file
 * @param 
 * @return: count of bytes writed 
 */
int rio_writen(int fd, const char *buf, int len) {
	int left=len, n_write=0;
	const char*ptr = buf;

	while(left > 0) {
		n_write = write(fd, ptr, left);
		if (n_write <= 0) {
			if (errno == EINTR) 
				n_write = 0;
			else 
				return -1;	
		}

		left -= n_write;
		ptr += n_write;
	}

	return len - left;
}


void rio_buf_init(int fd, rio_buf *rb) {
	rb->fd = fd;
	rb->n_left = 0;
	rb->ptr = rb->buf;
	memset(rb->buf, 0, sizeof(rb->buf));
}

int rio_readn_b(rio_buf* rb, char*buf, int len) { 
	int left = len, n = 0;
	char*ptr = buf;

	while(left > 0) {
		if (rb->n_left == 0) {
			int n = read(rb->fd, rb->buf, sizeof(rb->buf));
			if (n < 0) {
				if (errno != EINTR)
				return -1;
			} else if (n == 0) {
				break;
			} else {
				rb->n_left = n;
				rb->ptr = rb->buf;
			}
		}	

		n = std::min(rb->n_left, left); 
		std::memcpy(buf, rb->ptr, n);
		left -= n;	
		ptr += n;
		rb->ptr += n;
		rb->n_left -= n;
	}
		
	return len - left;
}

/**
 *
 */
int rio_realine_b(rio_buf* rb, char*buf, int max_len) { 
	char c, *ptr = buf;
	int n;
	for(n = 1; n < max_len; n++) {
		int n_read = rio_readn_b(rb, &c, 1);
		if (n_read < 0) {
			return -1;
		} else if (n_read == 0) {
			if (n == 1) {
				return 0;
			} else{
				n = 0;
				break;
			}
		} else {
			*ptr++ = c;
			if (c == '\n') break;
		}
	}

	*ptr = 0;
	return n;
}

