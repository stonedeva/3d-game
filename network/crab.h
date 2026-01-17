#ifndef _CRAB_H_
#define _CRAB_H_

#define CRAB_GP_DATA_SIZE 33
#define NETPLAYER_CAP 20

typedef unsigned char Byte;

typedef enum {
    GAME_PACKET_JOIN,
    GAME_PACKET_QUIT,
    GAME_PACKET_MOVE,
    GAME_PACKET_COUNT
} GamePacketType;

typedef enum {
    CRAB_INFO,
    CRAB_WARNING,
    CRAB_FATAL
} CrabLogLevel;

const char* gamepacket_type_cstr[GAME_PACKET_COUNT] = {
    [GAME_PACKET_JOIN] = "GAME_PACKET_JOIN",
    [GAME_PACKET_QUIT] = "GAME_PACKET_QUIT",
    [GAME_PACKET_MOVE] = "GAME_PACKET_MOVE",
};

const Byte CRAB_MAGIC[4] = {'C', 'R', 'A', 'B'};

/*
 * GamePacket (40 + 8 Byte Padding = 48 Byte)
 * ===========
 * Magic: 4 Bytes
 * Version Min: 1 Byte
 * Version Maj: 1 Byte
 * Type: 1 Byte
 * Data: 33 Byte
*/
typedef struct {
    Byte magic[4];
    uint8_t version_maj;
    uint8_t version_min;
    GamePacketType type;
    Byte data[CRAB_GP_DATA_SIZE];
} GamePacket;

_Static_assert(sizeof(GamePacket) == 48, "sizeof(GamePacket) != 48 bytes");

typedef struct {
    Player* gameplayer;
    bool is_host;
    int client_fd;
    char* ip;
} NetPlayer;

extern NetPlayer g_room[NETPLAYER_CAP];
extern int g_room_count;

void crab_log(CrabLogLevel lvl, const char* fmt, ...)
{
    printf("[CRAB] ");
    switch (lvl) {
    case CRAB_INFO:
	printf("INFO: ");
	break;
    case CRAB_WARNING:
	printf("WARNING: ");
	break;
    case CRAB_FATAL:
	printf("FATAL: ");
	break;
    default:
	assert(0 && "crab_log(): Unreachable");
    }
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

bool crab_netplayer_is_in_room(char* ip)
void crab_room_add_netplayer(NetPlayer netplayer)
void crab_send_game_packet(GamePacket* packet);
void crab_dump_game_packet(FILE* stream, GamePacket* packet);
void crab_handle_client(int client_fd);
void crab_main_loop(void);
int crab_init_server(void);

#endif // _CRAB_H_
