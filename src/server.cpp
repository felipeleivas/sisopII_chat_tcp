#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>

void sigpipe_handler(int unused)
{
}

#include "../lib/packet.h"
#include "../lib/group.h"
#include "../lib/communication.h"

#define PORT 4000

GROUP_LIST *group_list = NULL;

pthread_mutex_t find_group_mutex = PTHREAD_MUTEX_INITIALIZER;


void print_message(void *arg)
{
	char *string = (char *)arg;
	printf("%s", string);
}

GROUP *create_group(char *group_name)
{
	
  GROUP *new_group = create_new_group(group_name);
	group_list = add_group_list(group_list, new_group);
  
	return new_group;
}

void* handle_connection_with_client(void *socket_pointer)
{
	int socket = *(int *)socket_pointer;
	char *username = receive_message(socket);
	char *groupname = receive_message(socket);
  
  pthread_mutex_lock(&find_group_mutex);
	GROUP *found_group = find_group(group_list, groupname);
	if (found_group == NULL)
	{
		found_group = create_group(groupname);
	}

  pthread_mutex_unlock(&find_group_mutex);

	associate_socket_group(socket, found_group);
  char buffer[70];
  sprintf(buffer, "User %s joined group: %s", username, groupname);
  sleep(1);
  send_message_to_group(found_group, buffer);

	print_group_list(group_list);
	int connection_is_alive = 1;
	while (connection_is_alive)
	{

		char *message = receive_message(socket);
		if (message != NULL)
		{
			send_message_to_group(found_group, message);
			free(message);
		}
		else{
			connection_is_alive = 0;
		}
	}
	free(username);
	free(groupname);
	close(socket);
}

int bind_server_to_socket()
{
	int sockfd;
	struct sockaddr_in serv_addr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		fprintf(stderr, "ERROR opening socket");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr.sin_zero), 8);

	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		fprintf(stderr, "ERROR on binding");
		exit(-1);
	}
	return sockfd;
}

int accept_connection(int sockfd)
{
	int newsockfd;
	struct sockaddr_in cli_addr;

	socklen_t clilen = sizeof(struct sockaddr_in);
	if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1)
	{
		printf("Could not accept this connection");
	}
	return newsockfd;
}

int main(int argc, char *argv[])
{
	// sigaction(SIGPIPE, &(struct sigaction){sigpipe_handler}, NULL);

	int sockfd = bind_server_to_socket();

	listen(sockfd, 5);

	while (1)
	{
		int newsockfd = accept_connection(sockfd);
		if(newsockfd >= 0 ){
			pthread_t client_connection_thread;
			pthread_create(&client_connection_thread, NULL, &handle_connection_with_client, &newsockfd);
		}
	}
	close(sockfd);

	return 0;
}