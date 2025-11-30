#ifndef _RES_GAME_H_
#define _RES_GAME_H_
#include "player.h"
#include "bullets.h"
#define ANIM_IDLE  0
#define ANIM_WALK  1
#define ANIM_CROUCH  2
#define ANIM_FIRE  3
#define ANIM_JUMP  4
#define MAX_BULLETS 10
#define MAX_ENEMY_BULLETS 2

typedef struct {
 // tableau de bullets
    bullets player_bullet[MAX_BULLETS];
    Enemy_bullet enemy_bullet[MAX_ENEMY_BULLETS];       
} GameState;

extern GameState game_state;
extern Player player;
extern u16 VDPTilesFilled;

#endif // _RES_GAME_H_