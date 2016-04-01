#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "handler.h"

#define MAX_LINE_SIZE 255
void handle_client(int client_socket,
		struct sockaddr_in *client_socket_addr) {

	FILE* f = fdopen(client_socket, "r+");
	char line[MAX_LINE_SIZE];
	
	while(!feof(f)){
		if(fgets(line, MAX_LINE_SIZE, f) == NULL){
			fprintf(stderr, "Could not read from client!");
			goto end;
		}
		if(strncmp(line, "/exit", 5) == 0){
			fprintf(f, "Bye!\n");
			goto end;
		}
		fprintf(f, "%s\n", line);
		fflush(f);		
	}
		

	end:
	 fflush(f);
	 fclose(f);
}
