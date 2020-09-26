#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../lib/packet.h"

#define PORT 4000

int main(int argc, char *argv[])
{
  int sockfd, newsockfd, n;
  socklen_t clilen;
  char buffer[256];
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
      if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1)
      {
        
      }
      else{
          

      }
      pid = fork();
    }
    if (pid == 0)
    {
      while (1)
      {
        char packet_header[HEADER_SIZE];
        bzero(packet_header, HEADER_SIZE);

        n = 0;

        while (HEADER_SIZE > n)
        {
          int readBytes = read(newsockfd, packet_header, HEADER_SIZE);
          if (readBytes < 0)
          {
            printf("Erro reading header");
          }
          n += readBytes;
        }

        PACKET packet;
        deserialize_header(packet_header, &packet);

        n = 0;
        int message_length = packet.length;
        char message[message_length];

        while (n < message_length)
        {
          int readBytes = read(newsockfd, message, message_length);
          if (readBytes < 0)
          {
            printf("Erro reading message");
          }
          n += readBytes;
        }

        printf("Here is the message: %s\n", message);

        if (n < 0)
          printf("ERROR writing to socket");
      }
      close(newsockfd);
      return 0;
    }
  }
  close(sockfd);

  return 0;
}