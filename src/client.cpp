#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <pthread.h>

#include "../lib/packet.h"
#include "../lib/communication.h"
// ESC-H, ESC-J (I remember using this sequence on VTs)
#define clear() printf("\033[H\033[J")

//ESC-BRACK-column;row (same here, used on terminals on an early intranet)
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))
#define remove_last_line() printf("%c[2K", 27)
#define move_cursor_line_up(x) printf("%c[%dA", 27, (x))
#define move_cursor_right(x) printf("%c[C", 27)


void* print_messages_from_group(void *socket_pointer){
  int socket = * (int *) socket_pointer;
  
  while(1){
    
    char *message = receive_message(socket);
    remove_last_line();
    move_cursor_line_up(1);
    remove_last_line();
    move_cursor_line_up(1);
    move_cursor_line_up(1);

    printf("\nMessage from group: %s", message);
    printf("\nEnter the message: \n");

  }
  close(socket);
}
int main(int argc, char *argv[])
{
	
	char buffer[256];
	if (argc < 5)
	{
		fprintf(stderr, "usage %s <username> <groupname> <server_ip_address> <port>\n", argv[0]);
		exit(0);
	}
	char *username = argv[1];
	char *groupName = argv[2];

	int sockfd = connect_to_server(argv[3], atoi(argv[4]));
	int seqn = 0;
	send_message(DATA_PACKET, sockfd, username, seqn);
	send_message(DATA_PACKET, sockfd, groupName, seqn);
	
	pthread_t group_connection_thread;
    pthread_create(&group_connection_thread, NULL, &print_messages_from_group, &sockfd);
	
	while (1)
	{
		bzero(buffer, 256);
		fgets(buffer, 256, stdin);

    remove_last_line();
    move_cursor_line_up(1);

		send_message(DATA_PACKET, sockfd, buffer, seqn);
	}
	close(sockfd);
	return 0;
}