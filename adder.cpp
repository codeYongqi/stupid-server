#include "http_resp.h"
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv) {
	char *query_string = getenv("QUERY_STRING");
	char *ptr;
	if ((ptr = strchr(query_string, '&')) != NULL) {
		*ptr = '\0';	
	}
	
	char params[2][128];
	strncpy(params[0], query_string, 127);
	strncpy(params[1], ptr+1, 127);

	int a = atoi(params[0]), b = atoi(params[1]);

	char resp[1024];
	char buf[1024];
	ptr = buf;
	
	ptr += sprintf(ptr, "<!DOCTYPE html>\n");
	ptr += sprintf(ptr, "<html>\n");
	ptr += sprintf(ptr, "  <h>Adder</h>\n");
	ptr += sprintf(ptr, "  <p>%d+%d = %d</p>\n", a, b, a+b);
	ptr += sprintf(ptr, "</html>\n");
	get_http_resp_header(resp, "200", buf);
	rio_writen(STDOUT_FILENO, resp, strlen(resp));
}

