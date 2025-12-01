#include <genesis.h>

#ifndef _RES_boss_H_
#define _RES_boss_H_

#define BANIM_INTRO  0
#define BANIM_IDLE  1
#define BANIM_CROUCH  2
#define BANIM_FIRE_C  3
#define BANIM_FIRE_U  4
#define BANIM_GRIFFE 5  
// États du joueur
typedef struct {
    fix32 x, y;
    fix32 vx, vy;
    Sprite* sprite;
    Sprite* sprite_bullet;
    Sprite* sprite_playershoot;
    u16 shoot_cooldown;
    u16 anim_timer;
} Boss;




#endif // _RES_player_H_