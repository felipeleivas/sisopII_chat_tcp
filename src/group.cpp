#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fstream>
#include <string>
#include <iostream>


#include "../lib/group.h"
#include "../lib/communication.h"
#include "../lib/packet.h"

using namespace std;

void print_connection_list(INT_LIST *int_list){
	while (int_list != NULL)
	{
		printf(" %d,", int_list->pid);
		int_list = int_list->next;
	}
}
void print_group_list(GROUP_LIST *group_list)
{
	printf("\n Groups: ");
	while (group_list != NULL)
	{
		printf("\n%s, connected_users: ", group_list->group->name);
		print_connection_list(group_list->group->connected_users);
		group_list = group_list->next;
	}
}

GROUP *find_group(GROUP_LIST *group_list, char *group_name)
{
	//TODO this is a critical section, and should be handle as so
	while (group_list != NULL)
	{
		if (strcmp(group_list->group->name, group_name) == 0)
		{
			return group_list->group;
		}
		group_list = group_list->next;
	}
	return NULL;
}

GROUP_LIST *add_group_list(GROUP_LIST *group_list, GROUP *group)
{
	//TODO this is a critical section, and should be handle as so
	GROUP_LIST *first_element_list = group_list;
	GROUP_LIST *new_group_list = (GROUP_LIST *)malloc(sizeof(GROUP_LIST));
	new_group_list->group = group;
	new_group_list->next = NULL;
	if (group_list != NULL)
	{

		GROUP_LIST *before = NULL;
		while (group_list != NULL)
		{
			before = group_list;
			group_list = group_list->next;
		}
		before->next = new_group_list;
		return first_element_list;
	}
	else
	{
		return new_group_list;
	}
}

USER *find_user(USER_LIST *user_list, char *user_name)
{
	//TODO this is a critical section, and should be handle as so
	while (user_list != NULL)
	{
		if (strcmp(user_list->user->name, user_name) == 0)
		{
			return user_list->user;
		}
		user_list = user_list->next;
	}
	return NULL;
}

USER_LIST *add_user_list(USER_LIST *user_list, USER *user)
{
	//TODO this is a critical section, and should be handle as so
	USER_LIST *first_element_list = user_list;
	USER_LIST *new_user_list = (USER_LIST *)malloc(sizeof(USER_LIST));
	new_user_list->user = user;
	new_user_list->next = NULL;
	if (user_list != NULL)
	{

		USER_LIST *before = NULL;
		while (user_list != NULL)
		{
			before = user_list;
			user_list = user_list->next;
		}
		before->next = new_user_list;
		return first_element_list;
	}
	else
	{
		return new_user_list;
	}
}

GROUP *create_new_group(char *group_name)
{
	GROUP *group = (GROUP*) malloc(sizeof(GROUP));
	group->name = group_name;
	group->connected_users = NULL;
	group->seqn = 0;
  pthread_mutex_t new_group_mutex = PTHREAD_MUTEX_INITIALIZER;
  group->group_mutex = new_group_mutex;
  // printf("\nCreated group %s\n", group_name);
	return group;
}

USER *create_new_user(char *user_name)
{
	USER *user = (USER*) malloc(sizeof(GROUP));
	user->name = user_name;
	user->connected_sockets = NULL;
  pthread_mutex_t new_user_mutex = PTHREAD_MUTEX_INITIALIZER;
  user->user_mutex = new_user_mutex;
  // printf("\nCreated group %s\n", group_name);
	return user;
}

INT_LIST *add_socket_list(INT_LIST *int_list, int socket)
{
	INT_LIST *first_element_list = int_list;
	INT_LIST *new_list = (INT_LIST *)malloc(sizeof(INT_LIST));
	new_list->pid = socket;
	new_list->next = NULL;
	if (int_list != NULL)
	{

		INT_LIST *before = NULL;
		while (int_list != NULL)
		{
			before = int_list;
			int_list = int_list->next;
		}
		before->next = new_list;
		return first_element_list;
	}
	else
	{
		return new_list;
	}
}


int count_elements(INT_LIST *int_list)
{
  int i = 0;
  while (int_list != NULL)
  {
    i++;
    int_list = int_list->next;
  }
	return i;
}


INT_LIST *remove_socket_list(INT_LIST *int_list, int socket)
{
  // printf("\n Before:");
	// print_connection_list(int_list);
	INT_LIST *first_element_list = int_list;
	INT_LIST *before = NULL;

	while (int_list != NULL)
	{
		if(int_list->pid == socket){
			if(before != NULL){
				before->next = int_list->next;
			}
			else{
				first_element_list = int_list->next;
			}
			free(int_list);
      int_list = before;
		}else{
			before = int_list;
		}
    if(int_list != NULL)
		  int_list = int_list->next;

	}
  // printf("\n After remove element %d:", socket);
	// print_connection_list(first_element_list);
	// printf("\n");
	return first_element_list;
}


void associate_socket_group(int socket, GROUP *group)
{
  pthread_mutex_lock(&group->group_mutex);
	group->connected_users = add_socket_list(group->connected_users, socket);
  pthread_mutex_unlock(&group->group_mutex);
}

void associate_socket_user(int socket, USER *user)
{
	user->connected_sockets = add_socket_list(user->connected_sockets, socket);
}

void send_message_to_group(GROUP *group, char *message)
{
  pthread_mutex_lock(&group->group_mutex);
	INT_LIST *socket_list = group->connected_users;
  fstream fs;
  string filename = "/Users/sap/Project/faculdade/sisopii/history/";
  filename.append(group->name);
  fs.open (filename , std::fstream::in | std::fstream::out | std::fstream::app);
  fs << message;
  fs.close();

	while (socket_list != NULL)
	{
		int socket = socket_list->pid;
    
		if(send_message(DATA_PACKET, socket, message, group->seqn) == -1){
			socket_list = remove_socket_list(group->connected_users, socket);
			group->connected_users = socket_list;
		}
    if(socket_list != NULL )
		  socket_list = socket_list->next;
	}
	group->seqn = group->seqn +1;
  pthread_mutex_unlock(&group->group_mutex);

}

void restore_message_for_user(int socket, GROUP *group){
  ifstream fs;
  string filename = "/Users/sap/Project/faculdade/sisopii/history/";
  filename.append(group->name);
  pthread_mutex_lock(&group->group_mutex);
  fs.open (filename);
  string line;
  while (getline(fs, line)) {
    line.append("\n");
    char message[line.size() +1];
    strcpy(message, line.c_str());
    send_message(DATA_PACKET, socket, message, group->seqn);
  }
  fs.close();
  pthread_mutex_unlock(&group->group_mutex);

}

