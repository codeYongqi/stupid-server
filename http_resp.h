#include "rio.h"
#include "open_sock_fd.h"
#include "limits.h"
#include <unordered_map>
#include <sys/stat.h>

void get_http_resp_header(char* buf, const char* code, const char* body);
