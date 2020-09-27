#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "../lib/packet.h"
#include "../lib/group.h"

#define PORT 4000


GROUP_LIST* group_list = NULL;
void read_message(int socket, char *message_holder, int length){
    int total_read_bytes = 0;
    while (length > total_read_bytes)
    {
      int read_bytes = read(socket, message_holder, length);
      if (read_bytes < 0)
      {
        printf("Erro reading message");
      }
      total_read_bytes += read_bytes;
    }
}

void print_message(void * arg){
    char* string = (char*) arg;
    printf("%s", string);

}
void read_header(int socket, PACKET *packet){
    char packet_header[HEADER_SIZE];
    bzero(packet_header, HEADER_SIZE);
    read_message(socket, packet_header, HEADER_SIZE);
    deserialize_header(packet_header, packet);
}

char* receive_message_from_client(int socket){
  PACKET packet;
  read_header(socket, &packet);
  int message_length = packet.length;
  char *message = realloc(NULL, (sizeof(char) * message_length) + 1);
  message[message_length]='\0';
  read_message(socket, message, message_length);
  return message;
}

GROUP* create_group(char* group_name){
    GROUP* found_group = malloc(sizeof(GROUP));
    found_group->name = group_name;
    found_group->connected_users = NULL;
    group_list = add_group_list(group_list, found_group);
    return found_group;
}

void handle_connection_with_client(void *socket_pointer){
  int socket = * (int *) socket_pointer;
  char* username = receive_message_from_client(socket);
  char* groupname = receive_message_from_client(socket);
  
  GROUP* found_group = find_group(group_list, groupname);
  if(found_group == NULL){
    found_group = create_group(groupname);
  }

  associate_socket_group(socket, found_group);

  print_group_list(group_list);

  while(1){
    
    char *message = receive_message_from_client(socket);
    printf("\nHere is the message: %s", message);

  }
  close(socket);
}

int bind_server_to_socket(){
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

int accept_connection(int sockfd ){
  int newsockfd;
  struct sockaddr_in cli_addr;

  socklen_t clilen = sizeof(struct sockaddr_in);
  if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1){
    printf("Could not accept this connection");
  }
  return newsockfd;
}

int main(int argc, char *argv[])
{
  int sockfd = bind_server_to_socket();

  listen(sockfd, 5);

  while (1)
  {
    int newsockfd = accept_connection(sockfd);
    pthread_t client_connection_thread;
    pthread_create(&client_connection_thread, NULL, (void *) handle_connection_with_client, &newsockfd);

  }
  close(sockfd);

  return 0;
}