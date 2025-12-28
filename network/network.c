#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include "./player.h"

typedef unsigned char Byte;
typedef uint32_t packet_size_t;

#define ROOM_CAP 5
#define BUFFER_CAP 1024

typedef struct {
    Player game_player;
    char ip[INET_ADDRSTRLEN];
    pthread_t thread;
} NetworkPlayer;

typedef struct {
    NetworkPlayer* host;
    int port;
    NetworkPlayer netplayers[ROOM_CAP];
    size_t room_sz;
} Room;

typedef enum {
    GAMEPACKET_JOIN = 0,
    GAMEPACKET_QUIT,
    GAMEPACKET_MOVE,
    GAMEPACKET_BREAK_WALL,
    GAMEPACKET_COUNT
} GamePacketType;

_Static_assert(GAMEPACKET_COUNT < 255, "sizeof(GamePacketType) exceded 1 byte!");

/*
 * GamePacket (= 39 bytes)
 * ============
 * Header
 * ---------
 *  Magic number: 4 bytes
 *  Packet size: 4 bytes
 *  Packet Type: 1 byte
 *
 * Body
 * --------
 *  Packet specific content: 30 bytes
*/
typedef struct {
    Byte magic[4];
    packet_size_t packet_sz;
    GamePacketType type;
} GamePacketHeader;

typedef struct {
    GamePacketHeader header;
    char client_ip[INET_ADDRSTRLEN];
} JoinGamePacket;

typedef struct {
    GamePacketHeader header;
    int player_index;
    char client_ip[INET_ADDRSTRLEN];
} QuitGamePacket;

typedef struct {
    GamePacketHeader header;
    int player_index;
    double new_x;
    double new_y;
} MoveGamePacket;


bool network_validate_gamepacket_header(void* packet, packet_size_t packet_sz)
{
    Byte* packet_bytes = (Byte*)packet;
    if (!packet_bytes) {
	fprintf(stderr, "INFO: network_validate_packet(): Packet is empty\n");
	return 0;
    }
    int ptr = 0;

    // Magic number
    if (packet_bytes[ptr++] != '#') return 0;
    if (packet_bytes[ptr++] != 'M') return 0;
    if (packet_bytes[ptr++] != 'Z') return 0;
    if (packet_bytes[ptr++] != 'E') return 0;

    // Packet size
    packet_size_t extract_sz = ((packet_size_t)packet_bytes[ptr++]) |
		       ((packet_size_t)packet_bytes[ptr++] << 8) |
		       ((packet_size_t)packet_bytes[ptr++] << 16) |
		       ((packet_size_t)packet_bytes[ptr++] << 24);
    if (packet_sz != extract_sz) {
	fprintf(stderr, "INFO: network_validate_gamepacket_header(): Expected %d bytes but got %d bytes",
		packet_sz, extract_sz);
	return 0;
    }

    // Packet Type
    Byte type = packet_bytes[ptr++];
    if (type > GAMEPACKET_COUNT || type < 0) {
	fprintf(stderr, "INFO: network_validate_gamepacket_header(): Packet Type is invalid: %d\n",
		type);
    }

    return 1;
}

bool network_is_netplayer_in_room(NetworkPlayer* netplayer, Room* room)
{
    bool present = 0;
    for (size_t i = 0; i < room->room_sz; i++) {
	if (strcmp(netplayer->ip, room->netplayers[i].ip) == 0 &&
	    pthread_equal(netplayer->thread, room->netplayers[i].thread)) {
	    present = 1;
	}
    }
    return present;
}

void network_send_gamepacket(NetworkPlayer* netplayer, Room* room,
			     void* packet, packet_size_t packet_sz)
{
    Byte* packet_bytes = (Byte*)packet;
    int sockfd;
    struct sockaddr_in server_addr;
    Byte buffer[BUFFER_CAP];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	fprintf(stderr, "ERROR: socket(): Failed to create client socket: %s\n",
		strerror(errno));
	exit(1);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(room->port);

    if (inet_pton(AF_INET, room->host->ip, &server_addr.sin_addr) <= 0) {
	fprintf(stderr, "ERROR: inet_pton(): Failed to convert ip-address: %s\n",
		strerror(errno));
	close(sockfd);
	exit(1);
    }
    ssize_t bytes_sent = sendto(sockfd, packet, packet_sz, 0, (struct sockaddr*)&server_addr,
				sizeof(server_addr));
    if (bytes_sent != packet_sz) {
	fprintf(stderr, "WARNING: sendto(): Failed to send the entire packet: %s\n",
		strerror(errno));
    }
    close(sockfd);
}

void* network_run_room(void* args)
{
    Room* room = (Room*)args;
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    Byte buffer[BUFFER_CAP];

    strcpy(room->host->ip, "127.0.0.1");

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	fprintf(stderr, "ERROR: socket(): Failed to setup socket: %s\n",
		strerror(errno));
	pthread_exit(0);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(room->port);

    if (inet_pton(AF_INET, room->host->ip, &server_addr.sin_addr) <= 0) {
	fprintf(stderr, "ERROR: inet_pton(): Invalid ip-address\n");
	close(sockfd);
	pthread_exit(0);
    }
    if (bind(sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
	fprintf(stderr, "ERROR: bind(): Failed to bind: %s\n", strerror(errno));
	close(sockfd);
	pthread_exit(0);
    }

    printf("Host is listening: port = %d, ip = %s, thread = %lu\n", room->port, room->host->ip,
	   room->host->thread);

    socklen_t clientaddr_len = sizeof(client_addr);
    while (1) {
	int n = recvfrom(sockfd, buffer, BUFFER_CAP-1, 0, (struct sockaddr*)&client_addr,
			 &clientaddr_len);
	if (n < 0) {
	    fprintf(stderr, "ERROR: recvfrom(): Failed to receive: %s\n", strerror(errno));
	    break;
	}
	buffer[n] = '\0';

	printf("Host received data!\n");
    }

    close(sockfd);
    pthread_exit(0);
}

void network_setup_room(Room* room, int port)
{
    room->host = (NetworkPlayer*)malloc(sizeof(NetworkPlayer));
    if (!room->host) {
	fprintf(stderr, "ERROR: malloc(): Failed to allocate host: %s\n",
		strerror(errno));
	exit(1);
    }
    strcpy(room->host->ip, "127.0.0.1");

    if (pthread_create(&room->host->thread, 0, network_run_room, &room) != 0) {
	fprintf(stderr, "ERROR: pthread(): Failed to setup host thread: %s\n",
		strerror(errno));
	exit(1);
    }
    printf("Created host server: port = %d, ip = %s, thread = %lu\n", port, room->host->ip,
	   room->host->thread);

    pthread_join(room->host->thread, 0);
    free(room->host);
}

int main(void)
{
    Room room = {0};
    network_setup_room(&room, 8080);

    JoinGamePacket join_packet = {0};
    memcpy(join_packet.header.magic, "#MZE", 4);
    join_packet.header.type = GAMEPACKET_JOIN;
    join_packet.header.packet_sz = sizeof(join_packet);
    strcpy(join_packet.client_ip, "127.0.0.1");

    network_send_gamepacket(NULL, &room, (void*)&join_packet, sizeof(join_packet));
    return 0;
}
