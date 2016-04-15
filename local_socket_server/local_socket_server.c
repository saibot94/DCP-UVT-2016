#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>


const char* SOCKET_PATH = "/Users/chris/my_socket";

#define MAX_PENDING 10

int create_server() {
 struct sockaddr_un addr;
 int fd;

 if ((fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
  perror("Failed to create server socket");
  return fd;
 }

 memset(&addr, 0, sizeof(addr));

 addr.sun_family = AF_LOCAL;

 unlink(SOCKET_PATH);
 strcpy(addr.sun_path, SOCKET_PATH);

 int optval = 1;
 //setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
 
 // setsockopt(
	// 	fd,
	// 	SOL_SOCKET,
	// 	SO_REUSEADDR,
	// 	&optval,
	// 	sizeof(optval));

 if (bind(fd, (struct sockaddr *) &(addr),
                              sizeof(addr)) < 0) {
  perror("Failed to bind server socket");
  return -1;
 }

 
 if (listen(fd, MAX_PENDING) < 0) {
  perror("Failed to listen on server socket");
  return -1;
 }

 printf("Created socket, listening to : %d\n", fd);
 /* Add handler to handle events on fd */

 return fd;
}

static int recv_file_descriptor(
  int socket) /* Socket from which the file descriptor is read */
{
 int sent_fd;
 struct msghdr message;
 struct iovec iov[1];
 struct cmsghdr *control_message = NULL;
 char ctrl_buf[CMSG_SPACE(sizeof(int))];
 char data[1];
 int res;

 memset(&message, 0, sizeof(struct msghdr));
 memset(ctrl_buf, 0, CMSG_SPACE(sizeof(int)));

 /* For the dummy data */
 iov[0].iov_base = data;
 iov[0].iov_len = sizeof(data);

 message.msg_name = NULL;
 message.msg_namelen = 0;
 message.msg_control = ctrl_buf;
 message.msg_controllen = CMSG_SPACE(sizeof(int));
 message.msg_iov = iov;
 message.msg_iovlen = 1;

 if((res = recvmsg(socket, &message, 0)) <= 0)
  return res;

 /* Iterate through header to find if there is a file descriptor */
 for(control_message = CMSG_FIRSTHDR(&message);
     control_message != NULL;
     control_message = CMSG_NXTHDR(&message,
                                   control_message))
 {
  if( (control_message->cmsg_level == SOL_SOCKET) &&
      (control_message->cmsg_type == SCM_RIGHTS) )
  {
   return *((int *) CMSG_DATA(control_message));
  }
 }

 return -1;
}

#define MAX_LINE_SIZE 255

void handle_client_file(int client_fd){
	FILE* f = fdopen(client_fd, "r+");
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

void accept_client(int server_fd){
	while(1){
		// init part
		struct sockaddr_in *client_socket_address;
		socklen_t *client_socket_len;

		client_socket_address = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
		client_socket_len = (socklen_t *)malloc(sizeof(socklen_t));
		if(client_socket_address == NULL || client_socket_len == NULL){
			perror("Unexpected");
			exit(1);
		}

		// accept the client's connection
		int client_socket = accept(server_fd, (struct sockaddr *)client_socket_address, 
			client_socket_len);
		printf("Accepted connection over local socket!\n");

		sleep(1);
		int client_fd = recv_file_descriptor(client_socket);

		// int ret = fork();
		// if(ret == -1){
		// 	perror("Unexpected");
		// 	exit(1);			
		// }

		// if(ret == 0){
		// 	// child handling over here
		fprintf(stderr, "Handling things on my side... ");
		handle_client_file(client_fd);
		// }
		// else{
		// 	//close()
		// }
		printf("Finished with one client!\n");
		
	}
}

int main(int argc, char const *argv[])
{
	int server_fd = create_server();
	accept_client(server_fd);
	return 0;
}