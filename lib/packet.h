#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define DATA_PACKET 1
#define CMD_PACKET 2

#define HEADER_SIZE (sizeof(uint16_t) * 4)
typedef struct __packet{
 uint16_t type;
 uint16_t seqn; 
 uint16_t length; 
 uint16_t timestamp; 
 const char* _payload;
 } PACKET; 


PACKET create_packet(uint16_t type, uint16_t seqn, uint16_t length, uint16_t timestamp, const char* _payload);
char* serialize_packet(PACKET packet);
void deserialize_header(char* serialize_header, PACKET *new_packet);
