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


int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
	
    char buffer[256];
    if (argc < 5) {
		fprintf(stderr,"usage %s <username> <groupname> <server_ip_address> <port>\n", argv[0]);
		exit(0);
    }
  char* username = argv[1];  
  char* groupName = argv[2];  
	server = gethostbyname(argv[3]);
	int port = atoi(argv[4]);

	if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
        printf("ERROR opening socket\n");
    
	serv_addr.sin_family = AF_INET;     
	serv_addr.sin_port = htons(port);    
	serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
	bzero(&(serv_addr.sin_zero), 8);     
	
    
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        printf("ERROR connecting\n");
    int seqn = 0;
    while(1){
        printf("Enter the message: ");
        bzero(buffer, 256);
        fgets(buffer, 256, stdin);

        PACKET packet = create_packet(DATA_PACKET, seqn++, strlen(buffer),(int)time(NULL), buffer ); 
        PACKET other_packet;
      
    	/* write in the socket */
        char *serialized_packet = realloc(NULL, sizeof(char) * (strlen(buffer) + 4 * sizeof(uint16_t)));
    	  n = write(sockfd, serialize_packet(packet, serialized_packet), strlen(buffer) + HEADER_SIZE);
        if (n < 0) 
    		  printf("ERROR writing to socket\n");
        free(serialized_packet);
    	    	
    }
	close(sockfd);
    return 0;
}