#include "./crab.h"

NetPlayer g_room[CRAB_ROOM_CAP] = {0};
int g_room_count = 0;


bool crab_netplayer_is_in_room(char* ip)
{
    for (int i = 0; i < g_room_count; i++) {
	if (strcmp(ip, g_room[i].ip) == 0) {
	    return 1;
	}
    }
    return 0;
}

void crab_room_add_netplayer(NetPlayer netplayer)
{
    if (g_room_count >= CRAB_ROOM_CAP) {
	crab_log(CRAB_WARNING, "Room capacity has been reached!\n");
    }
    if (netplayer_is_in_room_by_ip(netplayer->ip)) {
	crab_log(CRAB_INFO, "Netplayer %s is already in room\n", netplayer->ip);
	return;
    }
    g_room[g_room_count++] = netplayer;
}
