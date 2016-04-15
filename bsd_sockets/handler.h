#ifndef _HANDLER_H
#define _HANDLER_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>




void handle_client(int client_socket,
		struct sockaddr_in *client_socket_addr, int local_server_fd);

#endif
