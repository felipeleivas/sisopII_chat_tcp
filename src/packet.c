#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "../lib/packet.h"

void sitoca(uint16_t value, char *bytes)
{

  bytes[1] = (value >> 8) & 0xFF;
  bytes[0] = value & 0xFF;
}

uint16_t catosi(char *bytes)
{
  char hi = bytes[1];
  char lo = bytes[0];

  uint16_t value = ((hi & 0xFF) << 8) | (lo & 0xFF);
  return value;
}


PACKET create_packet(uint16_t type, uint16_t seqn, uint16_t length, uint16_t timestamp, const char *_payload)
{
  PACKET new_packet;
  new_packet.type = htons(type);
  new_packet.seqn = htons(seqn);
  new_packet.length = htons(length);
  new_packet.timestamp = htons(timestamp);
  new_packet._payload = _payload;

  return new_packet;
}

char *serialize_packet(PACKET packet, char *serialized_packet)
{
  int data_load_size = strlen(packet._payload);
  // char *serialized_packet = realloc(NULL, sizeof(char) * (data_load_size + 4 * sizeof(uint16_t)));
  for (int i = 0; i < sizeof(char) * (data_load_size + 4 * sizeof(uint16_t)); i++)
  {
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

  serialized_packet[6] = buffer[0];
  serialized_packet[7] = buffer[1];
  strcpy(serialized_packet + 8, packet._payload);

  return serialized_packet;
}

void deserialize_header(char *serialize_header, PACKET *new_packet)
{
  new_packet->type = ntohs(catosi(serialize_header));
  new_packet->seqn = ntohs(catosi(serialize_header + 2));      //sizeof(uint16_t));
  new_packet->length = ntohs(catosi(serialize_header + 4));    //sizeof(uint16_t) * 2);
  new_packet->timestamp = ntohs(catosi(serialize_header + 6)); //sizeof(uint16_t) * 3);
  new_packet->_payload = serialize_packet + 8;
}