#include <stdio.h>
#include <stdlib.h>
// Take a host and port number, create an echo server from that 

#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>


int main(int argc, const char **argv){
	int port_number, server_socket, ret;
	const char *ip_address = NULL;
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

	server_socket_address.sin_family = AF_INET;
	server_socket_address.sin_port = htons(port_number);
	ret = bind(server_socket, (const struct sockaddr *) &server_socket_address, 
				sizeof(server_socket_address));
	if(ret != 0){
		perror("Unable to bind");
		exit(1);
	}
	fprintf(stderr, "Socket bound successfully!\n");
	return 0;
}
