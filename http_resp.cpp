#include "http_resp.h"
#include <iostream>

std::unordered_map<const char*, const char*> code_msg = {
	{"200", "OK"},
	{"301", "Moved Permanetly"},
	{"400", "Bad Reqeust"},
	{"404", "Not Found"},
	{"500", "Server Internal Error"}
};

void get_code_msg(const char* code, char *msg) {
	const char *temp_msg;
	if (!strcmp(code, "200")) {
		temp_msg = "OK";
	} else if (!strcmp(code, "301")){
		temp_msg = "Moved Permanetly";
	} else if (!strcmp(code, "400")){
		temp_msg = "Bad Reqeust";
	} else if (!strcmp(code, "404")){
		temp_msg = "Not Found";
	} else if (!strcmp(code, "500")){
		temp_msg = "Server Internal Error";
	}
	memcpy(msg, temp_msg, strlen(temp_msg));
	msg[strlen(temp_msg)] = '\0';
}

void get_http_resp_header(char* buf, const char* code, const char* body) {
	char *ptr = buf;
	int len;
	int cont_len = strlen(body);
	char code_msg[127];
	get_code_msg(code, code_msg);
	ptr += sprintf(ptr, "HTTP/1.1 %s %s\r\n", code, code_msg);
	ptr += sprintf(ptr, "Server: Stupid Server\r\n");
	ptr += sprintf(ptr, "Host: Stupid Server\r\n");
	ptr += sprintf(ptr, "Content-Length: %d\r\n", cont_len);
	ptr += sprintf(ptr, "\r\n");
	ptr += sprintf(ptr, "%s", body);
}

void get_http_resp_header_n(char* buf, int cont_len) {
	char *ptr = buf;
	const char* code = "200";
	char code_msg[127];
	get_code_msg(code, code_msg);
	ptr += sprintf(ptr, "HTTP/1.1 %s %s\r\n", code, code_msg);
	ptr += sprintf(ptr, "Server: Stupid Server\r\n");
	ptr += sprintf(ptr, "Host: Stupid Server\r\n");
	ptr += sprintf(ptr, "Content-Length: %d\r\n", cont_len);
	ptr += sprintf(ptr, "\r\n");
}
