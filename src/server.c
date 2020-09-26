#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../lib/packet.h"

#define PORT 4000

void read_message(int socket, char *message_holder, int length){
    int total_read_bytes = 0;
    while (length > total_read_bytes)
    {
      int read_bytes = read(socket, message_holder, length);
      if (read_bytes < 0)
      {
        printf("Erro reading header");
      }
      total_read_bytes += read_bytes;
    }
}

void read_header(int socket, PACKET *packet){
    char packet_header[HEADER_SIZE];
    bzero(packet_header, HEADER_SIZE);
    read_message(socket, packet_header, HEADER_SIZE);
    deserialize_header(packet_header, packet);
}

void receive_message_from_client(int socket){
  PACKET packet;
  read_header(socket, &packet);

  int n=0;
  int message_length = packet.length;
  char message[message_length];
  read_message(socket, message, message_length);

  printf("Here is the message: %s\n", message);

  if (n < 0)
  printf("ERROR writing to socket");
}

int main(int argc, char *argv[])
{
  int sockfd;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;

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

  listen(sockfd, 5);
  // fprintf(stderr, "listen\n");
  printf("listen\n");
  int pid = 1;
  while (1)
  {

    int newsockfd;
    if (pid > 0)
    {
      clilen = sizeof(struct sockaddr_in);
      if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1){}
      pid = fork();
    }
    if (pid == 0)
    {
      while (1)
      {
        receive_message_from_client(newsockfd);
      }
      close(newsockfd);
      return 0;
    }
  }
  close(sockfd);

  return 0;
}