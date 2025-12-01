#ifndef _RES_BULLETS_H_
#define _RES_BULLETS_H_
#include <genesis.h>
#include "player.h"

#define BULLET_SPEED 4
#define ENEMY_BULLET_SPEED 1    
// Diviseur pour réduire la vitesse effective des projectiles ennemis
#define ENEMY_BULLET_SPEED_SCALE 6

typedef struct {
    fix32 x, y;
    bool active;
    bool mirrored;
    Sprite* sprite;    
} bullets;

typedef struct {
    Sprite* sprite;
    fix32 currentX;
    fix32  currentY;
    fix32 targetX;
    fix32 targetY;
    fix32 vx;
    fix32 vy;   
    fix32 speed;
    bool isMoving;
    bool active;
    bool mirrored;
    
} Enemy_bullet;

void bullets_init();
void bullets_spawn(s16 x, s16 y,bool mirrored );
void bullets_update();
void bullets_debug_draw();
void enemy_bullets_spawn(s16 x, s16 y,s16 s);
void enemy_bullets_update();    
void enemy_bullets_init();
void enemy_bullet_shoot(s32 x,s32 y, s32 targetX, s32 targetY, s16 speed);

#endif // _RES_BULLETS_H_