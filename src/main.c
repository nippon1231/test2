// main.c - Moteur de plateforme avec sols solides et traversants pour SGDK 2.11

#include <genesis.h>
#include "config.h"

// Small LCG RNG for environments where <stdlib.h> is not available
static unsigned int _rng_state = 0x12345678;
int my_rand(void) {
    _rng_state = _rng_state * 1664525u + 1013904223u;
    return (int)((_rng_state >> 16) & 0x7FFF);
}
#include "resources.h"
#include "game.h"
#include "bullets.h"
#include "boss.h"
#include "enemies.h"



// Variables globales
GameState game_state;
char info[10];
Player player;
Boss boss;
Enemy enemies[MAX_ENEMIES];
s16 cameraX = 0;
s16 cameraY = 0;
static u16 fire_cooldown = 0;
Map* bgMap = NULL; // Map pour BG_A
// Tile map étendue


const u8 levelMap2[MAP_HEIGHT*MAP_WIDTH] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 2, 2, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 2, 2, 2, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 

};
// Récupère le type de tile à une position donnée
u8 getTileAt(s16 x, s16 y) {
    s16 tileX = x / 8;
    s16 tileY = y / 8;
    u16 index= tileY * MAP_WIDTH + tileX;
    if(index >= sizeof(levelMap2) || index < 0)
        return TILE_EMPTY;
    else
        return levelMap2[index];

        /*

    if (tileX < 0 || tileX >= MAP_WIDTH || tileY < 0 || tileY >= MAP_HEIGHT)
        return TILE_EMPTY;
    
      //          char hud[32];
            // position x en colonnes de caractères : 28 (à droite pour 320px/8=40 cols, 28 est à droite)
        //    sprintf(hud, "Tile:%d x=%d y=%d", levelMap[tileY][tileX], tileX, tileY);
          //  VDP_drawText(hud, 10, 3);

    return levelMap2[tileY][tileX];
    */
}

// Vérifie la collision avec les sols solides
bool checkSolidCollision(fix32 x, fix32 y, s16 width, s16 height) {
    s16 px = F32_toInt(x);
    s16 py = F32_toInt(y);
    
    if (getTileAt(px, py) == TILE_SOLID) return TRUE;
    if (getTileAt(px + width - 1, py) == TILE_SOLID) return TRUE;
    if (getTileAt(px, py + height - 1) == TILE_SOLID) return TRUE;
    if (getTileAt(px + width - 1, py + height - 1) == TILE_SOLID) return TRUE;
    if (getTileAt(px + width/2, py) == TILE_SOLID) return TRUE;
    if (getTileAt(px + width/2, py + height - 1) == TILE_SOLID) return TRUE;
    
    return FALSE;
}

// Vérifie la collision avec les plateformes
bool checkPlatformCollision(fix32 x, fix32 y, s16 width, s16 height) {
    s16 px = F32_toInt(x);
    s16 py = F32_toInt(y);
    
    if (getTileAt(px, py + height - 1) == TILE_PLATFORM) return TRUE;
    if (getTileAt(px + width - 1, py + height - 1) == TILE_PLATFORM) return TRUE;
    if (getTileAt(px + width/2, py + height - 1) == TILE_PLATFORM) return TRUE;
    
    return FALSE;
}

// Initialise le joueur
void initPlayer() {
    player.x = FIX32(40);
    player.y = FIX32(0);
    player.vx = FIX32(0);
    player.vy = FIX32(0);
    player.onGround = FALSE;
    player.jumpPressed = FALSE;
    player.downPressed = FALSE;
    player.jump_count = 0;
    player.onWall = FALSE;
    player.wallDir = 0;
    player.health = PLAYER_MAX_HEALTH;
    player.anim_timer = 0;
    
    player.sprite = SPR_addSprite(&sprite_player, 
                                  F32_toInt(player.x), 
                                  F32_toInt(player.y), 
                                  TILE_ATTR(PAL0, 0, FALSE, FALSE));
    player.sprite_bullet = SPR_addSprite(&sprite_bullet, 0, 10, TILE_ATTR(PAL0, 0, FALSE, FALSE));
    player.sprite_playershoot = SPR_addSprite(&sprite_playershoot, 0, 0, TILE_ATTR(PAL0, 0, FALSE, FALSE));
}

// Initialise le joueur
void initBoss() {
    boss.x = FIX32(324);
    boss.y = FIX32(120);
    
    boss.sprite = SPR_addSprite(&sprite_boss, 
                                  F32_toInt(boss.x), 
                                  F32_toInt(boss.y), 
                                  TILE_ATTR(PAL3, 0, FALSE, FALSE));
    boss.shoot_cooldown = 0;
    boss.health = BOSS_MAX_HEALTH;
    boss.alive = TRUE;
}

// Initialiser les ennemis
void initEnemies() {
    // Initialize all enemies as inactive
    for (u8 i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = FALSE;
        enemies[i].alive = FALSE;
        enemies[i].sprite = NULL;
    }
    
    // Scan the map for TILE_ENEMY_SPAWN (type 3) tiles
    u8 enemyCount = 0;
    for (u16 y = 0; y < MAP_HEIGHT && enemyCount < MAX_ENEMIES; y++) {
        for (u16 x = 0; x < MAP_WIDTH && enemyCount < MAX_ENEMIES; x++) {
            u16 index = y * MAP_WIDTH + x;
            if (levelMap2[index] == TILE_ENEMY_SPAWN) {
                // Found a spawn point - create an enemy here
                s16 spawnX = x * 8; // tile width in pixels
                s16 spawnY = (y * 8)-40; // tile height in pixels
                
                enemies[enemyCount].x = FIX32(spawnX);
                enemies[enemyCount].y = FIX32(spawnY);
                enemies[enemyCount].vx = FIX32(0);
                enemies[enemyCount].vy = FIX32(0);
                enemies[enemyCount].active = TRUE;
                enemies[enemyCount].alive = TRUE;
                enemies[enemyCount].onGround = FALSE;
                enemies[enemyCount].facingLeft = (enemyCount % 2) == 0;
                enemies[enemyCount].health = ENEMY_MAX_HEALTH;
                enemies[enemyCount].action = EANIM_IDLE;
                // Set patrol zone around spawn point
                enemies[enemyCount].patrol_left = spawnX - 50;
                enemies[enemyCount].patrol_right = spawnX + 50;
                enemies[enemyCount].aggro_range = 100;
                enemies[enemyCount].respawn_timer = 0;
                // Set respawn zone
                enemies[enemyCount].spawn_min_x = spawnX - 50;
                enemies[enemyCount].spawn_max_x = spawnX + 50;
                enemies[enemyCount].spawn_y = spawnY;
                
                // Create sprite with camera offset
                s16 screenX = spawnX - cameraX;
                s16 screenY = spawnY - cameraY;
                enemies[enemyCount].sprite = SPR_addSprite(&sprite_soldier, 
                                                           screenX, 
                                                           screenY, 
                                                           TILE_ATTR(PAL2, 0, FALSE, FALSE));
                if (enemies[enemyCount].sprite != NULL) {
                    SPR_setAnim(enemies[enemyCount].sprite, EANIM_IDLE);
                }
                
                enemyCount++;
            }
        }
    }
    
    // If no spawn tiles found, create default enemies
    if (enemyCount == 0) {
        // First enemy
        enemies[0].x = FIX32(200);
        enemies[0].y = FIX32(200);
        enemies[0].vx = FIX32(0);
        enemies[0].vy = FIX32(0);
        enemies[0].active = TRUE;
        enemies[0].alive = TRUE;
        enemies[0].onGround = FALSE;
        enemies[0].facingLeft = TRUE;
        enemies[0].health = ENEMY_MAX_HEALTH;
        enemies[0].action = EANIM_IDLE;
        enemies[0].patrol_left = 150;
        enemies[0].patrol_right = 250;
        enemies[0].aggro_range = 100;
        enemies[0].respawn_timer = 0;
        enemies[0].spawn_min_x = 150;
        enemies[0].spawn_max_x = 300;
        enemies[0].spawn_y = 50;
        enemies[0].sprite = SPR_addSprite(&sprite_soldier, 
                                          F32_toInt(enemies[0].x), 
                                          F32_toInt(enemies[0].y), 
                                          TILE_ATTR(PAL2, 0, FALSE, FALSE));
        if (enemies[0].sprite != NULL) {
            SPR_setAnim(enemies[0].sprite, EANIM_IDLE);
        }
        
        // Second enemy
        enemies[1].x = FIX32(400);
        enemies[1].y = FIX32(200);
        enemies[1].vx = FIX32(0);
        enemies[1].vy = FIX32(0);
        enemies[1].active = TRUE;
        enemies[1].alive = TRUE;
        enemies[1].onGround = FALSE;
        enemies[1].facingLeft = FALSE;
        enemies[1].health = ENEMY_MAX_HEALTH;
        enemies[1].action = EANIM_IDLE;
        enemies[1].patrol_left = 350;
        enemies[1].patrol_right = 450;
        enemies[1].aggro_range = 80;
        enemies[1].respawn_timer = 0;
        enemies[1].spawn_min_x = 350;
        enemies[1].spawn_max_x = 500;
        enemies[1].spawn_y = 50;
        enemies[1].sprite = SPR_addSprite(&sprite_soldier, 
                                          F32_toInt(enemies[1].x), 
                                          F32_toInt(enemies[1].y), 
                                          TILE_ATTR(PAL2, 0, FALSE, FALSE));
        if (enemies[1].sprite != NULL) {
            SPR_setAnim(enemies[1].sprite, EANIM_IDLE);
        }
    }
}

// Met à jour un ennemi
void updateEnemy(Enemy* enemy) {
    if (!enemy->active) return;
    
    // Handle respawn timer if enemy is dead
    if (!enemy->alive) {
        if (enemy->respawn_timer > 0) {
            enemy->respawn_timer--;
        } else {
            // Time to respawn - pick random position
            s16 spawn_range = enemy->spawn_max_x - enemy->spawn_min_x;
            if (spawn_range > 0) {
                s16 random_offset = (my_rand() % spawn_range);
                s16 spawn_x = enemy->spawn_min_x + random_offset;
                
                enemy->x = FIX32(spawn_x);
                enemy->y = FIX32(enemy->spawn_y);
                enemy->vx = FIX32(0);
                enemy->vy = FIX32(0);
                enemy->alive = TRUE;
                enemy->onGround = FALSE;
                enemy->health = ENEMY_MAX_HEALTH;
                enemy->action = EANIM_IDLE;
                enemy->facingLeft = (my_rand() % 2) == 0;
                
                // Recreate sprite
                if (enemy->sprite == NULL) {
                    s16 screenX = F32_toInt(enemy->x) - cameraX;
                    s16 screenY = F32_toInt(enemy->y) - cameraY;
                    enemy->sprite = SPR_addSprite(&sprite_soldier, 
                                                  screenX, 
                                                  screenY, 
                                                  TILE_ATTR(PAL2, 0, FALSE, FALSE));
                    if (enemy->sprite != NULL) {
                        SPR_setAnim(enemy->sprite, EANIM_IDLE);
                    }
                }
            }
        }
        return;
    }
    
    // Apply gravity
    if (!enemy->onGround) {
        enemy->vy = enemy->vy + FIX32(GRAVITY);
        if (enemy->vy > FIX32(MAX_FALL_SPEED)) {
            enemy->vy = FIX32(MAX_FALL_SPEED);
        }
    }
    
    // Calculate distance to player
    s16 distToPlayer = F32_toInt(player.x - enemy->x);
    s16 absDistToPlayer = (distToPlayer < 0) ? -distToPlayer : distToPlayer;
    
    // AI behavior
    if (absDistToPlayer < enemy->aggro_range) {
        // Aggro: move toward player
        if (distToPlayer < -10) {
            enemy->vx = FIX32(-ENEMY_WALK_SPEED);
            enemy->facingLeft = FALSE;
            if (enemy->onGround) enemy->action = EANIM_WALK;
        } else if (distToPlayer > 10) {
            enemy->vx = FIX32(ENEMY_WALK_SPEED);
            enemy->facingLeft = TRUE;
            if (enemy->onGround) enemy->action = EANIM_WALK;
        } else {
            enemy->vx = FIX32(0);
            if (enemy->onGround) enemy->action = EANIM_IDLE;
        }
    } else {
        // Patrol behavior
        s16 enemyX = F32_toInt(enemy->x);
        if (enemyX <= enemy->patrol_left) {
            enemy->vx = FIX32(ENEMY_WALK_SPEED);
            enemy->facingLeft = true;
        } else if (enemyX >= enemy->patrol_right) {
            enemy->vx = FIX32(-ENEMY_WALK_SPEED);
            enemy->facingLeft = false;
        }
        if (enemy->onGround && enemy->vx != FIX32(0)) {
            enemy->action = EANIM_WALK;
        } else if (enemy->onGround) {
            enemy->action = EANIM_IDLE;
        }
    }
    
    // Horizontal movement with collision
    fix32 newX = enemy->x + enemy->vx;
    fix32 collisionY = enemy->y;
    
    if (!checkSolidCollision(newX, collisionY, ENEMY_HITBOX_W, ENEMY_HITBOX_H)) {
        enemy->x = newX;
    }
    
    // Vertical movement with collision
    fix32 newY = enemy->y + enemy->vy;
    
    if (enemy->vy > FIX32(0)) {
        // Falling
        enemy->onGround = FALSE;
        if (!checkSolidCollision(enemy->x, newY, ENEMY_HITBOX_W, ENEMY_HITBOX_H)) {
            if (!checkPlatformCollision(enemy->x, newY, ENEMY_HITBOX_W, ENEMY_HITBOX_H)) {
                enemy->y = newY;
            } else {
                enemy->onGround = TRUE;
                enemy->vy = FIX32(0);
            }
        } else {
            enemy->onGround = TRUE;
            enemy->vy = FIX32(0);
        }
    } else if (enemy->vy < FIX32(0)) {
        // Jumping (not used but kept for completeness)
        if (!checkSolidCollision(enemy->x, newY, ENEMY_HITBOX_W, ENEMY_HITBOX_H)) {
            enemy->y = newY;
        } else {
            enemy->vy = FIX32(0);
        }
    }
    
    // Check if still on ground (to handle walking off platforms)
    if (enemy->onGround) {
        fix32 checkY = enemy->y + FIX32(ENEMY_HITBOX_H + 2);
        if (!checkSolidCollision(enemy->x, checkY, ENEMY_HITBOX_W, 1) && 
            !checkPlatformCollision(enemy->x, checkY, ENEMY_HITBOX_W, 1)) {
            // No ground below - start falling
            enemy->onGround = FALSE;
        }
    }
    
    // Check collision with player for damage
    if (absDistToPlayer < enemy->aggro_range && enemy->alive) {
        s16 enemyX = F32_toInt(enemy->x);
        s16 enemyY = F32_toInt(enemy->y);
        s16 playerX = F32_toInt(player.x);
        s16 playerY = F32_toInt(player.y);
        
        if (enemyX < playerX + PLAYER_HITBOX_W && enemyX + ENEMY_HITBOX_W > playerX &&
            enemyY < playerY + PLAYER_HITBOX_H && enemyY + ENEMY_HITBOX_H > playerY) {
            // Enemy touches player - deal damage
            if (player.health > 0) {
                player.health--;
                // Push player back slightly
                if (distToPlayer > 0) {
                    player.vx = FIX32(3); // push right
                } else {
                    player.vx = FIX32(-3); // push left
                }
                // Small upward bump
                player.vy = FIX32(-2);
                player.onGround = FALSE;
            }
        }
    }
    
    // Update sprite position (relative to camera)
    if (enemy->sprite != NULL) {
        s16 screenX = F32_toInt(enemy->x) - cameraX;
        s16 screenY = F32_toInt(enemy->y) - cameraY;
        
        // Hide sprite if outside screen bounds (with some margin)
        if (screenX < -32 || screenX > 320 + 32 || screenY < -32 || screenY > 224 + 32) {
            SPR_setVisibility(enemy->sprite, HIDDEN);
        } else {
            SPR_setVisibility(enemy->sprite, VISIBLE);
            SPR_setPosition(enemy->sprite, screenX, screenY);
            SPR_setHFlip(enemy->sprite, enemy->facingLeft);
            SPR_setAnim(enemy->sprite, enemy->action);
        }
    }
}

// Met à jour tous les ennemis
void updateEnemies() {
    for (u8 i = 0; i < MAX_ENEMIES; i++) {
        updateEnemy(&enemies[i]);
    }
}

// Met à jour la physique du joueur
void updatePlayer() {
    u16 joy = JOY_readJoypad(JOY_1);
    SPR_setVisibility(player.sprite_playershoot, HIDDEN);   
    
    // Gérer le timer d'animation
    if (player.anim_timer > 0) {
        player.anim_timer--;
        if (player.anim_timer == 0) {
            // Si c'était l'animation de mort, faire le respawn maintenant
            if (player.action == ANIM_DEATH) {
                player.x = FIX32(40);
                player.y = FIX32(0);
                player.vx = FIX32(0);
                player.vy = FIX32(0);
                player.onGround = FALSE;
                player.jump_count = 0;
                player.onWall = FALSE;
                player.wallDir = 0;
                player.health = PLAYER_MAX_HEALTH;
            }
            player.action = ANIM_IDLE;
        }
        // Bloquer les contrôles pendant l'animation
        return;
    }
    
    player.vx = FIX32(0);
    if(player.onGround && !player.is_shooting){
        player.action = ANIM_IDLE;
    }

//si on appuie sur A (tir depuis le boss plutôt qu'une position fixe)
if (joy & BUTTON_A)
{
    enemy_bullet_shoot(boss.x+FIX32(80), boss.y+30, player.x, player.y, ENEMY_BULLET_SPEED);
    if (boss.sprite) {
        SPR_setAnim(boss.sprite, BANIM_FIRE_U);
        boss.anim_timer = 12; // durée de l'animation en frames
    }
    boss.shoot_cooldown = 60;
}

    if (joy & BUTTON_LEFT) {
        player.vx = FIX32(-MOVE_SPEED);
        player.mirroir = TRUE;
       if(!player.jumpPressed && player.onGround)
            player.action = ANIM_WALK;
    }
    if (joy & BUTTON_RIGHT) {
        player.vx = FIX32(MOVE_SPEED);
        player.mirroir = FALSE;
        if(!player.jumpPressed && player.onGround)
            player.action = ANIM_WALK;        
    }
    if (joy & BUTTON_DOWN) {
        player.vx = FIX32(0);
        player.action = ANIM_CROUCH;
    }
   
    if (joy & BUTTON_C) {
        if (fire_cooldown > 0) fire_cooldown--;
        if (fire_cooldown == 0) {
            player.is_shooting = TRUE;
            fire_cooldown = 10; // Délai entre les tirs
            bullets_spawn(F32_toInt(player.x) - cameraX + 21, F32_toInt(player.y) - cameraY + 10, player.mirroir);
        }
    } else {
        if (fire_cooldown == 5) SPR_setVisibility(player.sprite_playershoot, HIDDEN);
        player.is_shooting = FALSE;
        fire_cooldown = 0;
    }

    bool jump = (joy & BUTTON_B);
    bool downDown = ((joy & BUTTON_DOWN) && (joy & BUTTON_B));
    bool prevJump = player.jumpPressed;

    // Saut normal, double saut ou wall jump
    if (jump && !prevJump && !downDown) {
        if (player.onGround) {
            // Saut normal depuis le sol
            if (player.action != ANIM_CROUCH) {
                player.vy = FIX32(JUMP_FORCE);
                player.onGround = FALSE;
                player.action = ANIM_JUMP;
                player.jumpPressed = TRUE;
                player.jump_count = 1;
            }
        } else if (player.onWall) {
            // Wall jump - sauter en s'éloignant du mur
            player.vy = FIX32(JUMP_FORCE);
            player.vx = FIX32(MOVE_SPEED * 2.5 * -player.wallDir); // pousser dans la direction opposée au mur
            player.mirroir = (player.wallDir > 0); // regarder dans la direction du saut
            player.onWall = FALSE;
            player.wallDir = 0;
            player.action = ANIM_JUMP;
            player.jumpPressed = TRUE;
            player.jump_count = 1; // wall jump compte comme premier saut
        } else if (player.jump_count < 2) {
            // Double saut en l'air (si pas encore utilisé)
            player.vy = FIX32(JUMP_FORCE);
            player.action = ANIM_JUMP;
            player.jumpPressed = TRUE;
            player.jump_count++;
        }
    }

    
    if (downDown && !player.downPressed && player.onGround) {
        player.y = player.y + FIX32(1);
        player.onGround = FALSE;
        player.jumpPressed = FALSE;
        player.vy = player.vy + FIX32(GRAVITY);
    }
    player.downPressed = downDown;
    
    if (!player.onGround) {
        player.vy = player.vy + FIX32(GRAVITY);
        if (F32_toInt(player.vy) > MAX_FALL_SPEED) {
            player.vy = FIX32(MAX_FALL_SPEED);
        }
    }
    
    fix32 newX = player.x + player.vx;
    fix32 collisionY = player.y + FIX32(PLAYER_SPRITE_OFFSET_Y);
    
    // Vérifier collision horizontale
    bool hitWall = checkSolidCollision(newX, collisionY, PLAYER_GROUND_W, PLAYER_HITBOX_H);
    
    if (!hitWall) {
        player.x = newX;
        // Pas de mur détecté
        if (player.onWall) {
            player.onWall = FALSE;
            player.wallDir = 0;
        }
    } else {
        // Collision avec un mur
        if (!player.onGround && player.vy > FIX32(0)) {
            player.action=ANIM_SLIDE;
            // En l'air et en train de tomber - activer le wall slide
            player.onWall = TRUE;
            // Déterminer de quel côté est le mur
            if (player.vx < FIX32(0)) {
                player.wallDir = -1; // mur à gauche
                player.mirroir=true;
            } else if (player.vx > FIX32(0)) {
                player.wallDir = 1; // mur à droite
                 player.mirroir=false;
            }
            // Ralentir la chute sur le mur
            player.vy = FIX32(1.0); // glissement lent sur le mur
        }
    }
    
    fix32 newY = player.y + player.vy;
    
    if (player.vy > FIX32(0)) {
        // check using adjusted collision Y (sprite offset) and hitbox height
        if (checkSolidCollision(player.x, newY + FIX32(PLAYER_SPRITE_OFFSET_Y), PLAYER_GROUND_W, PLAYER_HITBOX_H)) {
            player.y = FIX32((F32_toInt(player.y) / 8) * 8);
            player.vy = FIX32(0);
            player.onGround = TRUE;
            player.jump_count = 0;
            player.onWall = FALSE;
            player.wallDir = 0;
            // conserver l'état du bouton jump (si maintenu, rester true jusqu'au relâchement)
            player.jumpPressed = jump ? TRUE : FALSE;
        } else if (checkPlatformCollision(player.x, newY, PLAYER_GROUND_W, TOP_COLLISION_OFFSET) && 
               !checkPlatformCollision(player.x, player.y, PLAYER_GROUND_W, TOP_COLLISION_OFFSET)) {
            // platform collisions should also use adjusted Y
            player.y = FIX32((F32_toInt(newY) / 8) * 8);
            player.vy = FIX32(0);
            player.onGround = TRUE;
            player.jump_count = 0;
            player.onWall = FALSE;
            player.wallDir = 0;
            player.jumpPressed = jump ? TRUE : FALSE;
        } else {
            player.y = newY;
            player.onGround = FALSE;
        }
    } else if (player.vy < FIX32(0)) {
        if (checkSolidCollision(player.x, newY + FIX32(PLAYER_SPRITE_OFFSET_Y), PLAYER_GROUND_W, PLAYER_HITBOX_H)) {
            player.vy = FIX32(0);
        } else {
            player.y = newY;
            player.onGround = FALSE;
        }
    }

    // Clear jumpPressed when button released (edge detection)
    if (!jump) player.jumpPressed = FALSE;
    
    if (player.onGround) {
        if (!checkSolidCollision(player.x, player.y + FIX32(PLAYER_SPRITE_OFFSET_Y), PLAYER_GROUND_W, PLAYER_HITBOX_H) && 
            !checkPlatformCollision(player.x, player.y + FIX32(PLAYER_SPRITE_OFFSET_Y), PLAYER_GROUND_W, PLAYER_HITBOX_H)) {
            player.onGround = FALSE;
        }
    }
    
    if (F32_toInt(player.x) < 0) player.x = FIX32(0);
    if (F32_toInt(player.x) > (MAP_WIDTH * 8) - PLAYER_GROUND_W) player.x = FIX32((MAP_WIDTH * 8) - PLAYER_GROUND_W);
    if (F32_toInt(player.y) > (MAP_HEIGHT * 8)) player.y = FIX32(0);
    
    s16 playerScreenX = F32_toInt(player.x);
    s16 playerScreenY = F32_toInt(player.y);
    s16 targetX = playerScreenX - 160 + 21;
    s16 targetY = playerScreenY - 112 + 21;
    
    if (targetX < 0) targetX = 0;
    if (targetX > (MAP_WIDTH * 8) - 320) targetX = (MAP_WIDTH * 8) - 320;
    if (targetY < 0) targetY = 0;
    if (targetY > (MAP_HEIGHT * 8) - 224) targetY = (MAP_HEIGHT * 8) - 224;
    
    cameraX += (targetX - cameraX) / 8;
    cameraY += (targetY - cameraY) / 8;
    
    // Mettre à jour le scroll de la map BG_A
    if (bgMap != NULL) {
        MAP_scrollTo(bgMap, cameraX, cameraY);
    }
    
    VDP_setHorizontalScroll(BG_B, -cameraX);
    VDP_setVerticalScroll(BG_B, cameraY);
    
    SPR_setPosition(player.sprite, F32_toInt(player.x) - cameraX, F32_toInt(player.y) - cameraY + PLAYER_SPRITE_OFFSET_Y);
    SPR_setHFlip(player.sprite, player.mirroir); 
    SPR_setAnim(player.sprite,player.action); 
    // Mettre à jour la position du boss en coordonnées écran (monde - caméra)
    if (boss.sprite != NULL) {
        s32 bx = F32_toInt(boss.x) - cameraX;
        s32 by = F32_toInt(boss.y) - cameraY;
        // apply shake if active
        if (boss.shake_timer > 0) {
            s32 shake = (my_rand() % 7) - 3; // -3..+3 pixels
            bx += shake;
            boss.shake_timer--;
        }
        SPR_setPosition(boss.sprite, bx, by);
    }
}

// Dessine la map
void drawMap() {
    
    PAL_setPalette(PAL1, palette_lvl.data, DMA);
    
    // Charger le tileset à l'index 1
    VDP_loadTileSet(&bga_tileset, 1, DMA);
    
    // Charger la map graphique dans BG_A en utilisant MAP
    bgMap = MAP_create(&bga_map, BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 1));

    u16 x, y;
    

    // Initialiser le défilement de la map
    //VDP_setHorizontalScroll(BG_A, 0);
    //VDP_setVerticalScroll(BG_A, 0);
    MAP_scrollTo(bgMap,0,242);
}




int main() {
    VDP_setScreenWidth320();
    PAL_setPalette(PAL0,palette_main.data,DMA);
    SPR_init();

    // Charger la palette dédiée du boss (déclarée dans resources.res)
    PAL_setPalette(PAL3, palette_boss.data, DMA);
    
    // Charger la palette pour les ennemis soldier
    PAL_setPalette(PAL2, palette_soldier.data, DMA);
    
    drawMap();
    initPlayer();
    initBoss();
    initEnemies();
    enemy_bullets_init();
    SPR_setAnim(boss.sprite,BANIM_IDLE);
    while (1) {
        updatePlayer();
        updateEnemies();
        bullets_update();
        enemy_bullets_update();
        // Boss automatic shooting when visible on screen
        {
            s32 bx = F32_toInt(boss.x) - cameraX;
            s32 by = F32_toInt(boss.y) - cameraY;
            // simple visibility test with margin
            s32 margin = 32;
            bool boss_visible = (bx >= -margin && bx < 320 + margin && by >= -margin && by < 224 + margin);
            if (boss_visible) {
                if (boss.alive) {
                    if (boss.shoot_cooldown > 0) boss.shoot_cooldown--;
                    else {
                        // small random chance to fire (e.g. ~1%)
                        if ((my_rand() % 1000) < 10) {
                            // fire toward player
                            enemy_bullet_shoot(boss.x+FIX32(80), boss.y+30, player.x, player.y, ENEMY_BULLET_SPEED);
                            if (boss.sprite) {
                                SPR_setAnim(boss.sprite, BANIM_FIRE_U);
                                boss.anim_timer = 12;
                            }
                            boss.shoot_cooldown = 60; // cooldown in frames (~1s at 60fps)
                        }
                    }
                    // small random chance to play a crouch animation when idle (non-interrupting)
                    if (boss.anim_timer == 0) {
                        // ~2% chance per frame
                        if ((my_rand() % 1000) < 20) {
                            if (boss.sprite) {
                                SPR_setAnim(boss.sprite, BANIM_CROUCH);
                                boss.anim_timer = 48; // duration of the crouch animation in frames (~0.8s)
                            }
                        }
                    }
                }
            } else {
                // if not visible, slowly clear cooldown
                if (boss.shoot_cooldown > 0) boss.shoot_cooldown--;
            }
        }
        // Afficher le HUD : points de vie en haut à droite
        {
            char hud[32];
            // position x en colonnes de caractères : 28 (à droite pour 320px/8=40 cols, 28 est à droite)
            sprintf(hud, "HP:%d", player.health);
            VDP_drawText(hud, 28, 1);
            // afficher la vie du boss sous celle du player
            if (boss.alive) {
                sprintf(hud, "BOSS:%d", boss.health);
            } else {
                sprintf(hud, "BOSS:0");
            }
            VDP_drawText(hud, 28, 2);
        }

        // Gestion du timer d'animation du boss : revenir à l'animation idle quand terminé
        if (boss.anim_timer > 0) {
            boss.anim_timer--;
            if (boss.anim_timer == 0) {
                if (boss.sprite) SPR_setAnim(boss.sprite, BANIM_IDLE);
            }
        }

        SPR_update();
        SYS_doVBlankProcess();
    }
    
    return 0;
}