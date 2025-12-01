#ifndef TEST2_CONFIG_H
#define TEST2_CONFIG_H

// Screen
#define SCREEN_W 320
#define SCREEN_H 224

// Sprite / rendering
#define SPRITE_MARGIN 8

// Player
#define PLAYER_HITBOX_W 32
#define PLAYER_HITBOX_H 32
#define PLAYER_MAX_HEALTH 3
// Width used for ground/platform collision (original game value)
#define PLAYER_GROUND_W 42

// Bullets
#define BULLET_SPEED 4
#define ENEMY_BULLET_SPEED 1
// Scale/divisor to reduce enemy bullet velocity (higher => slower)
#define ENEMY_BULLET_SPEED_SCALE 80

// Boss
#define BOSS_HITBOX_W 48
#define BOSS_HITBOX_H 48
#define BOSS_MAX_HEALTH 5

#endif // TEST2_CONFIG_H
