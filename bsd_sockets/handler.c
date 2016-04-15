#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "handler.h"

#define MAX_LINE_SIZE 255

int connect_server() {
 struct sockaddr_un addr;
 int fd;

 if ((fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
  perror("Failed to create client socket");
  return fd;
 }

 memset(&addr, 0, sizeof(addr));

 addr.sun_family = AF_LOCAL;
 strcpy(addr.sun_path, SOCKET_PATH);

 if (connect(fd,
             (struct sockaddr *) &(addr),
             sizeof(addr)) < 0) {
  perror("Failed to connect to server");
  return -1;
 }


 /* Add handler to handle events */

 return fd;
}

static int
send_file_descriptor(
  int socket, /* Socket through which the file descriptor is passed */
  int fd_to_send) /* File descriptor to be passed, could be another socket */
{
 struct msghdr message;
 struct iovec iov[1];
 struct cmsghdr *control_message = NULL;
 char ctrl_buf[CMSG_SPACE(sizeof(int))];
 char data[1];

 memset(&message, 0, sizeof(struct msghdr));
 memset(ctrl_buf, 0, CMSG_SPACE(sizeof(int)));

 /* We are passing at least one byte of data so that recvmsg() will not return 0 */
 data[0] = ' ';
 iov[0].iov_base = data;
 iov[0].iov_len = sizeof(data);

 message.msg_name = NULL;
 message.msg_namelen = 0;
 message.msg_iov = iov;
 message.msg_iovlen = 1;
 message.msg_controllen =  CMSG_SPACE(sizeof(int));
 message.msg_control = ctrl_buf;

 control_message = CMSG_FIRSTHDR(&message);
 control_message->cmsg_level = SOL_SOCKET;
 control_message->cmsg_type = SCM_RIGHTS;
 control_message->cmsg_len = CMSG_LEN(sizeof(int));

 *((int *) CMSG_DATA(control_message)) = fd_to_send;

 return sendmsg(socket, &message, 0);
}

// Local handler that runs on the server
void handle_client(int client_socket,
		struct sockaddr_in *client_socket_addr) {

	int local_server = connect_server();
	int ret = send_file_descriptor(local_server, client_socket);
	if(ret == -1){
		perror("Something went wrong sending over unix sockets");
	}


	}
