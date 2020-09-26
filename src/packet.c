#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "../lib/packet.h"

 void sitoca(uint16_t value, char* bytes){
    
    bytes[0] = (value >> 8) & 0xFF;
    bytes[1] = value & 0xFF;
    
 }

 void litoca(uint32_t value, char* bytes){
   bytes[0] = (value >> 24) & 0xFF;
   bytes[1] = (value >> 16) & 0xFF;
   bytes[2] = (value >> 8) & 0xFF;
   bytes[3] = value & 0xFF;
 }
  uint16_t catosi(char* bytes){
    char hi = bytes[0];
    char lo = bytes[1];

    // printf("\n hi << 8%x ",  (hi << 8));
    // printf("\n hi %x \n",  );
    uint16_t value = ((hi & 0xFF) << 8) | (lo & 0xFF) ;

    // printf(" catosi : %x %x generated value: %x \n",bytes[0] & 0xFF, bytes[1] & 0xFF, value );
    return value;
 }

//  uint32_t catoli(char* )

 PACKET create_packet(uint16_t type, uint16_t seqn, uint16_t length, uint16_t timestamp, const char* _payload){
   PACKET new_packet;
   new_packet.type = htons(type);
   new_packet.seqn = htons(seqn);
   new_packet.length = htons(length);
   new_packet.timestamp = htons(timestamp);
   new_packet._payload = _payload;

   return new_packet;
 }

  char* serialize_packet(PACKET packet){
    int data_load_size = strlen(packet._payload);
    char* serialized_packet = realloc(NULL, sizeof(char)*(data_load_size + 4 * sizeof(uint16_t)));
    for(int i=0; i< sizeof(char)*(data_load_size + 4 * sizeof(uint16_t)); i++){
      serialized_packet[i] = 0;
    }
    char buffer[2];
    sitoca(packet.type, buffer);
    serialized_packet[0] = buffer[0];
    serialized_packet[1] = buffer[1];
    sitoca(packet.seqn, buffer);
    serialized_packet[2] = buffer[0];
    serialized_packet[3] = buffer[1];
    sitoca(packet.length, buffer);
    serialized_packet[4] = buffer[0];
    serialized_packet[5] = buffer[1];
    sitoca(packet.timestamp, buffer);
    // printf("timestamp: %u ", packet.timestamp);
    // printf("timestamp: %u ", catosi(buffer));
    serialized_packet[6] = buffer[0];
    serialized_packet[7] = buffer[1];
    // printf(serialized_packet);
    // printf( "\nserialize_packet: ");
    // for (int i = 0 ; i <10; i++){
    //   printf("%u ", serialized_packet[i]);
    // }

    //  strcpy(serialized_packet, sitoca(packet.type));
    //  strcpy(serialized_packet + sizeof(uint16_t), sitoca(packet.seqn));
    //  strcpy(serialized_packet + sizeof(uint16_t) * 2, sitoca(packet.length));
    //  strcpy(serialized_packet + sizeof(uint16_t) * 3, sitoca(packet.timestamp));
    //  strcpy(serialized_packet + sizeof(uint16_t) * 4, packet._payload);
     printf(" \ndata_load_size %d\n", data_load_size);
     for(int i=0; i < data_load_size; i++ ){
       printf(" %u ", serialized_packet[i] & 0xFF);
     }
    return serialized_packet;
 }

void deserialize_header(char* serialize_header, PACKET *new_packet){
   
   printf( "\ndeserialize_header:");
   for (int i = 0 ; i <10; i++){
     printf("%u ", serialize_header[i] & 0xFF);
   }
   new_packet->type = catosi(serialize_header);
   new_packet->seqn = catosi(serialize_header + 2);//sizeof(uint16_t));
   new_packet->length = catosi(serialize_header +4); //sizeof(uint16_t) * 2);
   new_packet->timestamp = catosi(serialize_header + 6);//sizeof(uint16_t) * 3);
 }