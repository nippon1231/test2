#include <genesis.h>
#include "inc/config.h"

#ifndef _RES_player_H_
#define _RES_player_H_

#define ANIM_IDLE  0
#define ANIM_WALK  1
#define ANIM_CROUCH  2
#define ANIM_FIRE  3
#define ANIM_JUMP  4
#define ANIM_HIT  6
// États du joueur
typedef struct {
    fix32 x, y;
    fix32 vx, vy;
    bool onGround;
    bool jumpPressed;
    bool downPressed;
    u8 jump_count; // number of jumps used (supports double-jump)
    bool mirroir;
    bool is_shooting;
    u8 action;
    u16 anim_timer;
    Sprite* sprite;
    Sprite* sprite_bullet;
    Sprite* sprite_playershoot;
    s16 health;
} Player;

#endif // _RES_player_H_