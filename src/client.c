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

#define PORT 4000

int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
	
    char buffer[256];
    if (argc < 2) {
		fprintf(stderr,"usage %s hostname\n", argv[0]);
		exit(0);
    }
	
	server = gethostbyname(argv[1]);
	if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
        printf("ERROR opening socket\n");
    
	serv_addr.sin_family = AF_INET;     
	serv_addr.sin_port = htons(PORT);    
	serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
	bzero(&(serv_addr.sin_zero), 8);     
	
    
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        printf("ERROR connecting\n");
    int seqn = 512;
    while(1){

        printf("Enter the message: ");
        bzero(buffer, 256);
        fgets(buffer, 256, stdin);
        buffer[256]='\0';
        PACKET packet = create_packet(DATA_PACKET, seqn++, strlen(buffer),(int)time(NULL), buffer ); 
        printf("Sending data header: type: %u, seq: %u, length %u and timestamp %u\n", ntohs(packet.type), ntohs(packet.seqn), ntohs(packet.length), packet.timestamp);
        deserialize_header(serialize_packet(packet), &packet) ;
        printf("\nReceiving data header: type: %u, seq: %u, length %u and timestamp %u\n", ntohs(packet.type), ntohs(packet.seqn), ntohs(packet.length), packet.timestamp);
    	/* write in the socket */
    	  n = write(sockfd, serialize_packet(packet), 256 + HEADER_SIZE);
        if (n < 0) 
    		  printf("ERROR writing to socket\n");
        printf("\nSend %d bytes", n);
        bzero(buffer,256);
    	
    	
    }
	close(sockfd);
    return 0;
}