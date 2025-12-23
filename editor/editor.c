#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define TILE_SIZE 40

#define WINDOW_WIDTH (MAP_WIDTH*TILE_SIZE)
#define WINDOW_HEIGHT (MAP_HEIGHT*TILE_SIZE)

bool is_saved = 0;
int map[MAP_WIDTH][MAP_HEIGHT];


void draw_map()
{
    for (int x = 0; x < MAP_WIDTH; x++) {
	for (int y = 0; y < MAP_HEIGHT; y++) {
	    Color color;
	    switch (map[x][y]) {
	    case 0:
		color = BLUE;
		break;
	    case 1:
		color = RED;
		break;
	    }
	    DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, color);
	    DrawRectangleLines(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLACK);
	}
    }
}

void save_map(char* out_path)
{
    FILE* fp = fopen(out_path, "w");
    if (!fp) {
	fprintf(stderr, "ERROR: fopen(): Failed to open output file: %s\n",
		strerror(errno));
	exit(1);
    }

    size_t n = fwrite(map, sizeof(int), MAP_WIDTH*MAP_HEIGHT, fp);
    if (n != MAP_WIDTH*MAP_HEIGHT) {
	fprintf(stderr, "ERROR: fwrite(): Expected %d bytes written but got %d bytes: %s\n",
		n, MAP_WIDTH*MAP_HEIGHT, strerror(errno));
	exit(1);
    }
    fclose(fp);
}

void handle_input()
{
    int mouse_x = GetMouseX();
    int mouse_y = GetMouseY();
    int tile_x = mouse_x / TILE_SIZE;
    int tile_y = mouse_y / TILE_SIZE;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SPACE)) {
	map[tile_x][tile_y] = 1; 
    }
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) || IsKeyPressed(KEY_BACKSPACE)) {
	map[tile_x][tile_y] = 0; 
    }
}

bool alert_popup(char* title, char* content, char* options)
{
    return GuiMessageBox((Rectangle){WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 500, 300},
			 title, content, options);
}

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Map Editor");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
	BeginDrawing();
	handle_input();
	draw_map();
	EndDrawing();
    }
    printf("Save map!\n");
    save_map("map.txt");

    CloseWindow();
}
