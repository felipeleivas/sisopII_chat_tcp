#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <string>
using namespace std;

void sigpipe_handler(int unused)
{
  
}

#include "../lib/packet.h"
#include "../lib/group.h"
#include "../lib/communication.h"

#define PORT 4000

GROUP_LIST *group_list = NULL;
USER_LIST *user_list = NULL;

pthread_mutex_t find_group_mutex = PTHREAD_MUTEX_INITIALIZER, find_user_mutex = PTHREAD_MUTEX_INITIALIZER;


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

USER *create_user(char *username)
{
	
  USER *new_user = create_new_user(username);
	user_list = add_user_list(user_list, new_user);
  
	return new_user;
}

void send_group_name_message(char* groupname, int socket){

  string group_welcome_message = "GROUP: ";
  group_welcome_message.append(groupname);
  group_welcome_message.append("\n");

  char bufferAux[group_welcome_message.size()];
  strcpy(bufferAux, group_welcome_message.c_str());

  send_message(DATA_PACKET, socket, bufferAux, 0);
}

void send_max_connections_message(char* username, int socket){

  string group_welcome_message = "The user: ";
  group_welcome_message.append(username);
  group_welcome_message.append("is already loged in two devices, which is the maximum allowed");
  group_welcome_message.append("\n");

  char bufferAux[group_welcome_message.size()];
  strcpy(bufferAux, group_welcome_message.c_str());

  send_message(DATA_PACKET, socket, bufferAux, 0);
}

void send_welcome_user_message_group(char* username, char* groupname, GROUP* group){
  string welcome_message = "User ";
  welcome_message.append(username);
  welcome_message.append(" has joined the group ");
  welcome_message.append(groupname);
  welcome_message.append("\n");

  char buffer[welcome_message.size()];
  strcpy(buffer, welcome_message.c_str());
  send_message_to_group(group, buffer);
}

void send_goodbye_user_message_group(char* username, char* groupname, GROUP* group){
  string welcome_message = "User ";
  welcome_message.append(username);
  welcome_message.append(" has left the group ");
  welcome_message.append(groupname);
  welcome_message.append("\n");

  char buffer[welcome_message.size()];
  strcpy(buffer, welcome_message.c_str());
  send_message_to_group(group, buffer);
}

void  send_message_from_user(char* username, char* message, GROUP* group){
  string str_message = "[";
  str_message.append(username);
  str_message.append("]:");
  str_message.append(message);
  char buffer[str_message.size()];
  strcpy(buffer, str_message.c_str());
  send_message_to_group(group, buffer);
}

void* handle_connection_with_client(void *socket_pointer)
{
	int socket = *(int *)socket_pointer;
	char *username = receive_message(socket);
	char *groupname = receive_message(socket);
  
  pthread_mutex_lock(&find_user_mutex);
	USER *found_user = find_user(user_list, username);
	if (found_user == NULL)
	{
		found_user = create_user(username);
	}
  pthread_mutex_unlock(&find_user_mutex);
  
  pthread_mutex_lock(&found_user->user_mutex);
  if(count_elements(found_user->connected_sockets) >= 2){
    pthread_mutex_unlock(&found_user->user_mutex);
    send_max_connections_message(username, socket);
  }
  else{
    associate_socket_user(socket, found_user);
    pthread_mutex_unlock(&found_user->user_mutex);
  
    pthread_mutex_lock(&find_group_mutex);
    GROUP *found_group = find_group(group_list, groupname);
    if (found_group == NULL)
    {
      found_group = create_group(groupname);
    }
    pthread_mutex_unlock(&find_group_mutex);

    send_group_name_message(groupname, socket);
    restore_message_for_user(socket, found_group);
    associate_socket_group(socket, found_group);
    send_welcome_user_message_group(username, groupname, found_group);
    
    int connection_is_alive = 1;
    while (connection_is_alive == 1)
    {

      char *message = receive_message(socket);
      if (message != NULL)
      {
        send_message_from_user(username, message, found_group);
        free(message);
      }
      else{
        connection_is_alive = 0;
      }
    }

    pthread_mutex_lock(&found_group->group_mutex);
    found_group->connected_users = remove_socket_list(found_group->connected_users, socket);
    pthread_mutex_unlock(&found_group->group_mutex);

    pthread_mutex_lock(&found_user->user_mutex);
    found_user->connected_sockets = remove_socket_list(found_user->connected_sockets, socket);
    pthread_mutex_unlock(&found_user->user_mutex);

    send_goodbye_user_message_group(username, groupname, found_group);
  }
	free(username);
	// free(groupname);
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
    signal(SIGPIPE, sigpipe_handler);

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