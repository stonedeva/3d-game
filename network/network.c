#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>

typedef unsigned char Byte;
typedef uint32_t packet_size_t;

typedef struct {
    pthread_t thread;
    char ip[INET_ADDRSTRLEN];
    int port;
} Host;

bool network_validate_packet(void* packet, packet_size_t packet_sz)
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
	fprintf(stderr, "INFO: network_validate_packet(): Expected %d bytes but got %d bytes",
		packet_sz, extract_sz);
	return 0;
    }
    return 1;
}

void network_send_gamepacket(void* packet, packet_size_t packet_sz)
{
    Byte* packet_bytes = (Byte*)packet;
}

void* network_run_room(void* args)
{
    Host* host = (Host*)args;
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_CAP];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0) < 0)) {
	fprintf(stderr, "ERROR: socket(): Failed to setup socket: %s\n",
		strerror(errno));
	pthread_exit(0);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(host->port);
    if (inet_pton(AF_INET, host->ip, &server_addr.sin_addr) <= 0) {
	fprintf(stderr, "ERROR: inet_pton(): Invalid ip-address\n");
	close(sockfd);
	pthread_exit(0);
    }
    if (bind(sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
	fprintf(stderr, "ERROR: bind(): Failed to bind: %s\n", strerror(errno));
	close(sockfd);
	pthread_exit(0);
    }

    printf("Host is listening: port = %d, ip = %s, thread = %lu\n", host.port, host.ip,
	   host.thread);

    socklen_t clientaddr_len = sizeof(client_addr);
    while (1) {
	int n = recvfrom(sockfd, buffer, BUFFER_CAP-1, 0, (struct sockaddr*)&client_addr,
			 &clientaddr_len);
	if (n < 0) {
	    fprintf(stderr, "ERROR: recvfrom(): Failed to receive: %s\n", strerror(errno));
	    break;
	}
	buffer[n] = '\0';
    }

    close(sockfd);
    pthread_exit(0);
}

void network_setup_host(int port)
{
    Host host = {0};
    strcpy(host.ip, "0.0.0.0");
    host.port = port;

    if (phtread(&host.thread, 0, network_run_room, &host) != 0) {
	fprintf(stderr, "ERROR: pthread(): Failed to setup host thread: %s\n",
		strerror(errno));
	exit(1);
    }
    printf("Created host server: port = %d, ip = %s, thread = %lu\n", host.port, host.ip,
	   host.thread);
}
