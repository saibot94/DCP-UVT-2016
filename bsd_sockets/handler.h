#ifndef _HANDLER_H
#define _HANDLER_H


#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>


#define SOCKET_PATH "/tmp/my_socket_path"


void handle_client(int client_socket,
		struct sockaddr_in *client_socket_addr);

#endif
