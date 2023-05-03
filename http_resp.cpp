#include "http_resp.h"

std::unordered_map<const char*, const char*> code_msg = {
	{"200", "OK"},
	{"301", "Moved Permanetly"},
	{"400", "Bad Reqeust"},
	{"404", "Not Found"},
	{"500", "Server Internal Error"}
};

void get_http_resp_header(char* buf, const char* code, const char* body) {
	char *ptr = buf;
	int len;
	int cont_len = strlen(body);
	ptr += sprintf(ptr, "HTTP/1.1 %s %s\r\n", code, code_msg[code]);
	ptr += sprintf(ptr, "Server: Stupid Server\r\n");
	ptr += sprintf(ptr, "Host: Stupid Server\r\n");
	ptr += sprintf(ptr, "Content-Length: %d\r\n", cont_len);
	ptr += sprintf(ptr, "\r\n");
	ptr += sprintf(ptr, "%s", body);
	printf("Reponse is:\n%s", buf);
}

