#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#include "../lib/packet.h"

int send_message(int data_type, int sockfd, char *message, int seqn)
{
  int message_len = strlen(message);
  PACKET packet = create_packet(data_type, seqn++, message_len, (int)time(NULL), message);

  char *serialized_packet = (char*) realloc(NULL, (sizeof(char) * message_len) + HEADER_SIZE);
  int written_bytes = write(sockfd, serialize_packet(packet, serialized_packet), message_len + HEADER_SIZE);
  if (written_bytes < 0){
    printf("ERROR sending message\n");
    return -1;
  }
  free(serialized_packet);
  return 0;
}

int read_message(int socket, char *message_holder, int length){
    int total_read_bytes = 0;
    while (length > total_read_bytes)
    {
      int read_bytes = read(socket, message_holder, length);
      if (read_bytes < 0)
      {
        printf("Erro reading message");
        return -1;
      }
      total_read_bytes += read_bytes;
    }
    return 0;
}


int read_header(int socket, PACKET *packet){
    char packet_header[HEADER_SIZE];
    bzero(packet_header, HEADER_SIZE);
    if(read_message(socket, packet_header, HEADER_SIZE) <0){
      return -1;
    }
    deserialize_header(packet_header, packet);
    return 0;
}

char* receive_message(int socket){
  PACKET packet;
  read_header(socket, &packet);
  if(packet.type == END_CONNECTION){
    return NULL;
  }
  int message_length = packet.length;
  char *return_message = (char*) realloc(NULL,(sizeof(char) * message_length) + 1);
  return_message[message_length]='\0';
  if(read_message(socket, return_message, message_length) == -1){
    return NULL;
  }

  return return_message;
}


int connect_to_server(char *server_ip, int port)
{
  int sockfd;
  struct sockaddr_in serv_addr;
  struct hostent *server = gethostbyname(server_ip);
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    printf("ERROR opening socket\n");

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
  bzero(&(serv_addr.sin_zero), 8);

  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    printf("ERROR connecting\n");
  return sockfd;
}