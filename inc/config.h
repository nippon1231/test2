#ifndef TEST2_CONFIG_H
#define TEST2_CONFIG_H

// Screen
#define SCREEN_W 320
#define SCREEN_H 224

// Sprite / rendering
#define SPRITE_MARGIN 8

// Player
#define PLAYER_HITBOX_W 24
#define PLAYER_HITBOX_H 28
#define PLAYER_HITBOX_H_CROUCH 16
#define PLAYER_HITBOX_OFFSET_X 4
#define PLAYER_HITBOX_OFFSET_Y 0
#define PLAYER_MAX_HEALTH 3
// Width used for ground/platform collision (original game value)
#define PLAYER_GROUND_W 26

// Bullets
#define BULLET_SPEED 4
#define ENEMY_BULLET_SPEED 1
// Scale/divisor to reduce enemy bullet velocity (higher => slower)
#define ENEMY_BULLET_SPEED_SCALE 80

// Boss
#define BOSS_HITBOX_W 48
#define BOSS_HITBOX_H 48
#define BOSS_MAX_HEALTH 20

// Constantes
#define GRAVITY 0.4
#define JUMP_FORCE -6.5
#define MOVE_SPEED 2
#define MAX_FALL_SPEED 15

// Dimensions de la map
#define MAP_WIDTH 64
#define MAP_HEIGHT 36

// Types de tiles
#define TILE_EMPTY 0
#define TILE_SOLID 1
#define TILE_PLATFORM 2
#define TOP_COLLISION_OFFSET 42
#define PLAYER_SPRITE_OFFSET_Y 10

#endif // TEST2_CONFIG_H
