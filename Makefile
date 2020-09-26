CC=gcc
LIB_DIR=./lib
BIN_DIR=./bin
SRC_DIR=./src
CFLAGS = -Wall

all: directory clean lib
directory:
	mkdir lib -p

server: packet.o
	$(CC) $(SRC_DIR)/server.c -o $(BIN_DIR)/server $(BIN_DIR)/packet.o -g -Wall

client: packet.o
	$(CC) $(SRC_DIR)/client.c -o $(BIN_DIR)/client $(BIN_DIR)/packet.o -g -Wall

packet.o:
	$(CC) -c $(SRC_DIR)/packet.c -o $(BIN_DIR)/packet.o -Wall 

start_server: server
	$(BIN_DIR)/server

start_client: client
	$(BIN_DIR)/client localhost

clean:
	rm -rf $(BIN_DIR)/*