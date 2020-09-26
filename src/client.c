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
void send_message(int sockfd, char *message, int seqn)
{
  int message_len = strlen(message);
  PACKET packet = create_packet(DATA_PACKET, seqn++, message_len, (int)time(NULL), message);

  char *serialized_packet = realloc(NULL, (sizeof(char) * message_len) + HEADER_SIZE);
  int written_bytes = write(sockfd, serialize_packet(packet, serialized_packet), message_len + HEADER_SIZE);
  if (written_bytes < 0){
    printf("ERROR sending message\n");
  }
  free(serialized_packet);
}

int main(int argc, char *argv[])
{

  char buffer[256];
  if (argc < 5)
  {
    fprintf(stderr, "usage %s <username> <groupname> <server_ip_address> <port>\n", argv[0]);
    exit(0);
  }
  // char *username = argv[1];
  // char *groupName = argv[2];

  int sockfd = connect_to_server(argv[3], atoi(argv[4]));

  int seqn = 0;
  while (1)
  {
    printf("Enter the message: ");
    bzero(buffer, 256);
    fgets(buffer, 256, stdin);

    send_message(sockfd, buffer, seqn);
  }
  close(sockfd);
  return 0;
}