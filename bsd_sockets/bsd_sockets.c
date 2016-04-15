#include <stdio.h>
#include <stdlib.h>
// Take a host and port number, create an echo server from that 

#include "handler.h"


const char* SOCKET_PATH = "/Users/chris/my_socket";


int connect_server_locally() {
 struct sockaddr_un addr;
 int fd;

 if ((fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
  perror("Failed to create client socket");
  return fd;
 }

 memset(&addr, 0, sizeof(addr));

 addr.sun_family = AF_LOCAL;
 strcpy(addr.sun_path, SOCKET_PATH);
 
 int optval = 1;
 setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
 
 setsockopt(
		fd,
		SOL_SOCKET,
		SO_REUSEADDR,
		&optval,
		sizeof(optval));


 if (connect(fd,
             (struct sockaddr *) &(addr),
             sizeof(addr)) < 0) {
  perror("Failed to connect to server");
  return -1;
 }


 /* Add handler to handle events */

 return fd;
}

int main(int argc, const char **argv){
	int port_number, server_socket, ret;
	const char *ip_address = NULL;
	int socket_opt = 1;
	struct sockaddr_in server_socket_address;
	if(argc != 3){
		fprintf(stderr, "Please call it like: app <ip_addr> <port_number>\n");
		exit(1);	
	}
	port_number = strtol(argv[2], NULL, 10);
	if(port_number == 0){
		fprintf(stderr, "Invalid port number!\n");
		exit(1);	
	}
	ip_address = argv[1];
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket == -1){
		perror("Unexpected");
		exit(1);
	}
	
	
	ret = inet_pton(AF_INET, ip_address, &server_socket_address.sin_addr);
	if(ret == 0){
		fprintf(stderr, "The provided address is not correct: %s\n",ip_address);
		exit(1);
	}
	else if(ret == -1){
		perror("Unexpected");
		exit(1);
	}
	setsockopt(
		server_socket,
		SOL_SOCKET,
		SO_REUSEADDR,
		&socket_opt,
		sizeof(socket_opt)
	);
	server_socket_address.sin_family = AF_INET;
	server_socket_address.sin_port = htons(port_number);
	

	ret = bind(server_socket, (const struct sockaddr *) &server_socket_address, 
				sizeof(server_socket_address));
	if(ret != 0){
		perror("Unable to bind");
		exit(1);
	}
	fprintf(stderr, "Socket bound successfully!\n");
	ret = listen(server_socket, 10);
	if(ret != 0){
		perror("Unable to listen to bound socket");
		exit(1);
	}

	while(1){
		// Handle requests
		int client_socket, client_port;
		char* client_address_str;
		struct sockaddr_in *client_socket_address;
		socklen_t *client_socket_len;

		client_socket_address = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
		client_socket_len = (socklen_t *)malloc(sizeof(socklen_t));
		if(client_socket_address == NULL || client_socket_len == NULL){
			perror("Unexpected");
			exit(1);
		}

		*client_socket_len = sizeof(struct sockaddr_in);
		client_socket = accept(server_socket, (struct sockaddr *)client_socket_address, 
			client_socket_len);

		int local_server_fd = connect_server_locally();

		if(client_socket == 0){
			perror("Unexpected error");
			exit(1);	
		}
		fprintf(stderr, "Got a new client!\n");

		ret = fork();
		if(ret == -1){
			perror("Unexpected");
			exit(1);			
		}
		if(ret == 0){
		  handle_client(client_socket, client_socket_address, local_server_fd);
		  close(local_server_fd);
		  break;
		}
		else{
		    fprintf(stderr, "Handling client... \n");	
			client_port = ntohs(client_socket_address->sin_port);		
			client_address_str = inet_ntoa(client_socket_address->sin_addr);		
			fprintf(stderr, "Client address: %s\n Client port: %d\n", client_address_str, client_port);
			close(client_socket);
			close(local_server_fd);
		}
	}
	return 0;
}
