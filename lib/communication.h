#ifndef COMMUNICTION_HEADER
#define COMMUNICTION_HEADER

void send_message(int data_type, int sockfd, char *message, int seqn);
void read_header(int socket, PACKET *packet);
char* receive_message(int socket);
int connect_to_server(char *server_ip, int port);
#endif