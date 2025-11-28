#ifndef _RES_BULLETS_H_
#define _RES_BULLETS_H_
#include <genesis.h>
#include "player.h"
#define BULLET_SPEED 4
#define ENEMY_BULLET_SPEED 2    

extern GameState game_state;
extern Player player;
void bullets_init();
void bullets_spawn(s16 x, s16 y,bool mirrored );
void bullets_update();
#endif // _RES_BULLETS_H_