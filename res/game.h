#ifndef _RES_GAME_H_
#define _RES_GAME_H_

#define ANIM_IDLE  0
#define ANIM_WALK  1
#define ANIM_CROUCH  2
#define ANIM_FIRE  3
#define ANIM_JUMP  4
#define MAX_BULLETS 10
typedef struct {
    fix32 x, y;
    bool active;
    bool mirrored;
    Sprite* sprite;    
} bullets;

typedef struct {
 // tableau de bullets
    bullets player_bullet[MAX_BULLETS];    
} GameState;

extern u16 VDPTilesFilled;

#endif // _RES_GAME_H_