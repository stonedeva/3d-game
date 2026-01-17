#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>
#include <errno.h>
#include "./crab.h"

#define CRAB_PORT 25565
#define BUF_CAP 1024


typedef unsigned char Byte;

struct sockaddr_in g_server_addr, g_client_addr;
socklen_t g_client_len;
int g_server_fd;


void crab_dump_game_packet(FILE* stream, GamePacket* packet)
{
    fprintf(stream, "GamePacket\n===========\n");
    fprintf(stream, "	Magic: 0x%02x 0x%02x 0x%02x 0x%02x",
		    packet->magic[0], packet->magic[1],
		    packet->magic[2], packet->magic[3], packet->magic[4]);
    fprintf(stream, "	Version: %d.%d\n", 
		    packet->version_maj, packet->version_min);
    fprintf(stream, "	Type: %s\n", 
		    gamepacket_type_cstr[packet->type]);
    fprintf(stream, "	Data: ");
    for (size_t i = 0; i < CRAB_GP_DATA_SIZE; i++) {
	if (i % 16 == 0) {
	    fprintf(stream, "\n");
	}
	fprintf(stream, " 0x%02x", packet->data[i]);
    }
}

void crab_handle_game_packet(GamePacket* packet)
{
    switch (packet->type) {
    case GAME_PACKET_JOIN:
	break;
    case GAME_PACKET_QUIT:
	break;
    case GAME_PACKET_MOVE:
	break;
    }
}

void crab_main_loop(void)
{
    while (1) {
	GamePacket packet;
	ssize_t received = recvfrom(g_server_fd, &packet, sizeof(GamePacket), 0,
				    (struct sockaddr*)&g_client_addr, &g_client_len);
	crab_handle_game_packet(&packet);
	crab_dump_game_packet(stdout, &packet);
    }
}

int crab_init_server(void)
{
    g_client_len = sizeof(g_client_addr);
    g_server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (g_server_fd < 0) {
	fprintf(stderr, "ERROR: socket(): %s\n", strerror(errno));
	return 1;
    }

    g_server_addr.sin_family = AF_INET;
    g_server_addr.sin_addr.s_addr = INADDR_ANY;
    g_server_addr.sin_port = htons(CRAB_PORT);

    if (bind(g_server_fd, (struct sockaddr*)&g_server_addr, sizeof(g_server_addr)) < 0) { 
	fprintf(stderr, "ERROR: bind(): %s\n", strerror(errno)); 
	return 1; 
    }

    printf("Crab Server is listening on port %d\n", CRAB_PORT);

    return 0;
}

int main(void)
{
    crab_init_server();
    crab_main_loop();
}

