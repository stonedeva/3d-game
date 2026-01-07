#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include "./network.h"

static volatile int running = 1;


void* nethost_run_thread(void* arg)
{
    char buf[BUF_CAP];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
	fprintf(stderr, "ERROR: socket(): %s\n", strerror(errno));
	return NULL;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
	fprintf(stderr, "ERROR: bind(): %s\n", strerror(errno));
	return NULL;
    }

    printf("Host: UDP Server started on port: %d\n", PORT);

    while (running) {
	ssize_t len = recvfrom(sockfd, buf, BUF_CAP-1, 0, (struct sockaddr*)&client_addr,
			       &client_len);
	if (len < 0) {
	    fprintf(stderr, "ERROR: recvfrom(): %s\n", strerror(errno));
	    return -1;
	}
	buf[len] = '\0';
	printf("Host: Recevied packet: %s\n", buf);
    }

    close(sockfd);
    printf("Host: Closed UDP Server\n");
}

void nethost_setup_thread()
{
    printf("Host: Setup seperate network thread\n");

    pthread_t thread;
    if (pthread_create(&thread, 0, nethost_run_thread, 0) != 0) {
	fprintf(stderr, "ERROR: pthread_create(): %s\n", strerror(errno));
	return;
    }

    printf("Host: Thread is running...\n");
    getchar();

    running = 0;

    return;
}
