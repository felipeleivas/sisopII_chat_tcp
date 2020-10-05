#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <pthread.h>
#include <string>
#include <iostream> 

using namespace std;
#include "../lib/packet.h"
#include "../lib/communication.h"
// ESC-H, ESC-J (I remember using this sequence on VTs)
#define clear() printf("\033[H\033[J")

//ESC-BRACK-column;row (same here, used on terminals on an early intranet)
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))
#define remove_last_line() printf("%c[2K", 27)
#define move_cursor_line_up(x) printf("%c[%dA", 27, (x))
#define move_cursor_right(x) printf("%c[C", 27)

int sockfd;
int running = 1;
void* print_messages_from_group(void *socket_pointer){
  int socket = * (int *) socket_pointer;
  
  while(1){
    
    char *message = receive_message(socket);
    remove_last_line();
    move_cursor_line_up(1);
    remove_last_line();
    move_cursor_line_up(1);
    move_cursor_line_up(1);

    printf("\n%s", message);
    printf("\nEnter the message: \n");

  }
  close(socket);
}

void sigpipe_handler(int unused)
{
  printf("acabou");
  send_message(END_CONNECTION, sockfd, "", 0);
  close(sockfd);
  exit(0);
}

int isLetter(char c){
  return ((c >= 'a' && c<='z' ) || (c >= 'A' && c<='Z'));
}

int isNumber(char c){
  return (c >= '0' && c <= '9');
}

int has_error_name(char *prefix, char *name, int has_error){
  if(4 >strlen(name) || strlen(name)>20){

    string message = "";
    message.append(prefix);
    message.append(" must have at least 4 and at most 20 letters.");
    printf("%s\n", message.c_str());
    has_error = 1;
  }
  
  int has_error_format = 0;
  for(int i=0; i<strlen(name); i++){
    char c = name[i];
    if(i== 0){
      if(!isLetter(c)){
        has_error = 1;
        has_error_format = 1;
      }
    }
    else{
      if(!isLetter(c) && !isNumber(c) && c != '.'){
        has_error = 1;
        has_error_format = 1;
      }
    }
  }

  if(has_error_format == 1){
    string message = "";
    message.append(prefix);
    message.append(" should start with a letter and can only have letters, numbers and dot.");
    printf("%s\n", message.c_str());
  }

  return has_error;
}

int main(int argc, char *argv[])
{
  signal(SIGINT, sigpipe_handler);

	if (argc < 5)
	{
		fprintf(stderr, "usage %s <username> <groupname> <server_ip_address> <port>\n", argv[0]);
		exit(0);
	}
	char *username = argv[1];
	char *groupName = argv[2];
  int has_error = 0;
  has_error = has_error_name("Your username", username, has_error);
  has_error = has_error_name("Your groupname", groupName, has_error);
 
  if(has_error == 1){
    
  }
  else{
    sockfd = connect_to_server(argv[3], atoi(argv[4]));
    int seqn = 0;
    send_message(DATA_PACKET, sockfd, username, seqn);
    send_message(DATA_PACKET, sockfd, groupName, seqn);
    
    pthread_t group_connection_thread;
      pthread_create(&group_connection_thread, NULL, &print_messages_from_group, &sockfd);
    
      printf("\n\nEnter the message: \n");
    while (!feof(stdin))
    {
      string str;
      getline(cin, str); 
      char buffer[str.size() +1];
      strcpy(buffer, str.c_str());

      remove_last_line();
      move_cursor_line_up(1);

      send_message(DATA_PACKET, sockfd, buffer, seqn);
    }
    send_message(END_CONNECTION, sockfd, "", 0);

    close(sockfd);
  }
	return 0;
}