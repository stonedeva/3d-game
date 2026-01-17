#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include "./crab.h"

int g_client_fd;
struct sockaddr_in g_server_addr;


void crab_send_game_packet(GamePacket* packet)
{
    size_t sent = sendto(g_client_fd, 
			 packet, 
			 sizeof(packet), 
			 0,
			 (struct sockaddr*)&g_server_addr,
			 sizeof(g_server_addr));
    if (sent < 0) {
	fprintf(stderr, "ERROR: sent(): %s\n", strerror(errno));
	exit(1);
    }
}

void crab_setup_client(char* server_ip, int server_port)
{
    g_client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (g_client_fd < 0) {
	fprintf(stderr, "ERROR: socket(): %s\n", strerror(errno));
	exit(1);
    }

    memset(&g_server_addr, 0, sizeof(g_server_addr));
    g_server_addr.sin_family = AF_INET;
    g_server_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &g_server_addr.sin_addr) <= 0) {
	fprintf(stderr, "ERROR: inet_pton(): %s\n", strerror(errno));
	close(g_client_fd);
	exit(1);
    }

    GamePacket packet = {0};
    memcpy(packet.magic, CRAB_MAGIC, 4);
    packet.version_maj = 1;
    packet.version_min = 0;
    packet.type = GAME_PACKET_JOIN;
    crab_send_game_packet(&packet);
}

int main(void)
{
    crab_setup_client("127.0.0.1", 25565);
    return 0;
}
