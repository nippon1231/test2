#ifndef _RES_BULLETS_H_
#define _RES_BULLETS_H_
#include <genesis.h>
#include "player.h"
#include "game.h"

#define BULLET_SPEED 4
#define ENEMY_BULLET_SPEED 0.2    

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
    fix32 speed;
    bool isMoving;
    bool active;
    
} Enemy_bullet;

void bullets_init();
void bullets_spawn(s16 x, s16 y,bool mirrored );
void bullets_update();
void enemy_bullets_spawn(s16 x, s16 y,s16 s);
void enemy_bullets_update();    
void enemy_bullets_init();
void enemy_bullet_shoot(fix16 x,fix16 y, fix16 targetX, fix16 targetY, fix16 speed);

#endif // _RES_BULLETS_H_