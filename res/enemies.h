#include <genesis.h>

#ifndef _RES_ENEMIES_H_
#define _RES_ENEMIES_H_

#define MAX_ENEMIES 8
#define ENEMY_HITBOX_W 24
#define ENEMY_HITBOX_H 28
#define ENEMY_WALK_SPEED 1
#define ENEMY_RUN_SPEED 2
#define ENEMY_MAX_HEALTH 3
#define ENEMY_CONTACT_DAMAGE 1

// Animation states for soldier enemy
#define EANIM_IDLE  1
#define EANIM_WALK  2
#define EANIM_ATTACK 1
#define EANIM_HIT 3
#define EANIM_DEATH 4

typedef struct {
    fix32 x, y;
    fix32 vx, vy;
    bool active;
    bool alive;
    bool onGround;
    bool facingLeft;
    s16 health;
    u16 anim_timer;
    u8 action;
    Sprite* sprite;
    // AI behavior
    s16 patrol_left;
    s16 patrol_right;
    s16 aggro_range;
    u16 attack_cooldown;
    u16 damage_cooldown;
    // Respawn system
    u16 respawn_timer;
    s16 spawn_min_x;
    s16 spawn_max_x;
    s16 spawn_y;
} Enemy;

#endif // _RES_ENEMIES_H_
