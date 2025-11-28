#include <genesis.h>

#ifndef _RES_player_H_
#define _RES_player_H_

#define ANIM_IDLE  0
#define ANIM_WALK  1
#define ANIM_CROUCH  2
#define ANIM_FIRE  3
#define ANIM_JUMP  4

// États du joueur
typedef struct {
    fix32 x, y;
    fix32 vx, vy;
    bool onGround;
    bool jumpPressed;
    bool downPressed;
    bool mirroir;
    u8 action;
    Sprite* sprite;
    Sprite* sprite_bullet;
    Sprite* sprite_playershoot;
} Player;




#endif // _RES_player_H_