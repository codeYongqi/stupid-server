#include <netdb.h>
#include <string.h>
#include <iostream>

int open_clientfd(const char *hostname, const char *port);

int open_listenfd(const char *port);
