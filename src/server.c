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
        // fprintf(stderr, "ERROR on accept \n");
      }
      else{
          // fprintf(stderr, "Connected \n");

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
        // fprintf(stderr, "Going into while");

        while (HEADER_SIZE > n)
        {
          // fprintf(stderr, "Going to read the header");

          int readBytes = read(newsockfd, packet_header, HEADER_SIZE);
          if (readBytes < 0)
          {
            // fprintf(stderr, "ERROR reading header from packet");
          }
          else
          {
            // fprintf(stderr, "Received %n bytes", readBytes);
            printf("Read %u", n);
          }
          n += readBytes;
        }

        PACKET packet;
        deserialize_header(packet_header, &packet);
        printf("Received data header: type: %u, seq: %u, length %u and timestamp %u", ntohs(packet.type), ntohs(packet.seqn), ntohs(packet.length), ntohs(packet.timestamp));

        n = 0;
        // int message_length = packet.length;
        int message_length = 256;
        char message[message_length];
        printf("\n message length = %u\n", message_length);
        while (n < message_length)
        {
          int readBytes = read(newsockfd, message, message_length);
          if (readBytes < 0)
          {
            // fprintf(stderr, "ERROR reading header from packet");
          }
          else{
            printf("Read payload: %u", n);
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