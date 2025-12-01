// main.c - Moteur de plateforme avec sols solides et traversants pour SGDK 2.11

#include <genesis.h>
#include "resources.h"
#include "game.h"
#include "bullets.h"
#include "boss.h"
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
#define  TOP_COLLISION_OFFSET 42


// Variables globales
GameState game_state;
char info[10];
Player player;
Boss boss;
Map* bgMap;
s16 cameraX = 0;
s16 cameraY = 0;
static u16 fire_cooldown = 0; 
// Tile map étendue
const u8 levelMap[MAP_HEIGHT][MAP_WIDTH] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,1,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// Récupère le type de tile à une position donnée
u8 getTileAt(s16 x, s16 y) {
    s16 tileX = x / 8;
    s16 tileY = y / 8;
    
    if (tileX < 0 || tileX >= MAP_WIDTH || tileY < 0 || tileY >= MAP_HEIGHT)
        return TILE_EMPTY;
    
    return levelMap[tileY][tileX];
}

// Vérifie la collision avec les sols solides
bool checkSolidCollision(fix32 x, fix32 y, s16 width, s16 height) {
    s16 px = F32_toInt(x);
    s16 py = F32_toInt(y);
    
    if (getTileAt(px, py) == TILE_SOLID) return TRUE;
    if (getTileAt(px + width - 1, py) == TILE_SOLID) return TRUE;
    if (getTileAt(px, py + height - 1) == TILE_SOLID) return TRUE;
    if (getTileAt(px + width - 1, py + height - 1) == TILE_SOLID) return TRUE;
    if (getTileAt(px + width/2, py) == TILE_SOLID) return TRUE;
    if (getTileAt(px + width/2, py + height - 1) == TILE_SOLID) return TRUE;
    
    return FALSE;
}

// Vérifie la collision avec les plateformes
bool checkPlatformCollision(fix32 x, fix32 y, s16 width, s16 height) {
    s16 px = F32_toInt(x);
    s16 py = F32_toInt(y);
    
    if (getTileAt(px, py + height - 1) == TILE_PLATFORM) return TRUE;
    if (getTileAt(px + width - 1, py + height - 1) == TILE_PLATFORM) return TRUE;
    if (getTileAt(px + width/2, py + height - 1) == TILE_PLATFORM) return TRUE;
    
    return FALSE;
}

// Initialise le joueur
void initPlayer() {
    player.x = FIX32(40);
    player.y = FIX32(0);
    player.vx = FIX32(0);
    player.vy = FIX32(0);
    player.onGround = FALSE;
    player.jumpPressed = FALSE;
    player.downPressed = FALSE;
    
    player.sprite = SPR_addSprite(&sprite_player, 
                                  F32_toInt(player.x), 
                                  F32_toInt(player.y), 
                                  TILE_ATTR(PAL0, 0, FALSE, FALSE));
    player.sprite_bullet = SPR_addSprite(&sprite_bullet, 0, 10, TILE_ATTR(PAL0, 0, FALSE, FALSE));
    player.sprite_playershoot = SPR_addSprite(&sprite_playershoot, 0, 0, TILE_ATTR(PAL0, 0, FALSE, FALSE));
}

// Initialise le joueur
void initBoss() {
    boss.x = FIX32(200);
    boss.y = FIX32(80);
    
    boss.sprite = SPR_addSprite(&sprite_boss, 
                                  F32_toInt(boss.x), 
                                  F32_toInt(boss.y), 
                                  TILE_ATTR(PAL0, 0, FALSE, FALSE));
}

// Met à jour la physique du joueur
void updatePlayer() {
    u16 joy = JOY_readJoypad(JOY_1);
    SPR_setVisibility(player.sprite_playershoot, HIDDEN);   
    player.vx = FIX32(0);
    if(player.onGround && !player.is_shooting){
        player.action = ANIM_IDLE;
    }

//si on appuie sur A (tir depuis le boss plutôt qu'une position fixe)
if (joy & BUTTON_A)
{
    enemy_bullet_shoot(boss.x, boss.y, player.x, player.y, ENEMY_BULLET_SPEED);
}

    if (joy & BUTTON_LEFT) {
        player.vx = FIX32(-MOVE_SPEED);
        player.mirroir = TRUE;
       if(!player.jumpPressed && player.onGround)
            player.action = ANIM_WALK;
    }
    if (joy & BUTTON_RIGHT) {
        player.vx = FIX32(MOVE_SPEED);
        player.mirroir = FALSE;
        if(!player.jumpPressed && player.onGround)
            player.action = ANIM_WALK;        
    }
    if (joy & BUTTON_DOWN) {
        player.vx = FIX32(0);
        player.action = ANIM_CROUCH;
    }
   
    if (joy & BUTTON_C) {
        if (fire_cooldown > 0) fire_cooldown--;
        if (fire_cooldown == 0) {
            player.is_shooting = TRUE;
            fire_cooldown = 10; // Délai entre les tirs
            bullets_spawn(F32_toInt(player.x) - cameraX + 21, F32_toInt(player.y) - cameraY + 10, player.mirroir);
        }
    } else {
        if (fire_cooldown == 5) SPR_setVisibility(player.sprite_playershoot, HIDDEN);
        player.is_shooting = FALSE;
        fire_cooldown = 0;
    }

    bool jump = (joy & BUTTON_B);
    bool downDown = ((joy & BUTTON_DOWN) && (joy & BUTTON_B));
    bool prevJump = player.jumpPressed;

    if (jump && !prevJump && player.onGround) {
        if (player.action != ANIM_CROUCH) {
            player.vy = FIX32(JUMP_FORCE);
            player.onGround = FALSE;
            player.action = ANIM_JUMP;
            player.jumpPressed = TRUE;
        }
    }

    
    if (downDown && !player.downPressed && player.onGround) {
        player.y = player.y + FIX32(1);
        player.onGround = FALSE;
        player.jumpPressed = FALSE;
        player.vy = player.vy + FIX32(GRAVITY);
    }
    player.downPressed = downDown;
    
    if (!player.onGround) {
        player.vy = player.vy + FIX32(GRAVITY);
        if (F32_toInt(player.vy) > MAX_FALL_SPEED) {
            player.vy = FIX32(MAX_FALL_SPEED);
        }
    }
    
    fix32 newX = player.x + player.vx;
    if (!checkSolidCollision(newX, player.y, 42, TOP_COLLISION_OFFSET)) {
        player.x = newX;
    }
    
    fix32 newY = player.y + player.vy;
    
    if (player.vy > FIX32(0)) {
        if (checkSolidCollision(player.x, newY, 42, TOP_COLLISION_OFFSET)) {
            player.y = FIX32((F32_toInt(player.y) / 8) * 8);
            player.vy = FIX32(0);
            player.onGround = TRUE;
            // conserver l'état du bouton jump (si maintenu, rester true jusqu'au relâchement)
            player.jumpPressed = jump ? TRUE : FALSE;
        } else if (checkPlatformCollision(player.x, newY, 42, TOP_COLLISION_OFFSET) && 
                   !checkPlatformCollision(player.x, player.y, 42, TOP_COLLISION_OFFSET)) {
            player.y = FIX32((F32_toInt(newY) / 8) * 8);
            player.vy = FIX32(0);
            player.onGround = TRUE;
            player.jumpPressed = jump ? TRUE : FALSE;
        } else {
            player.y = newY;
            player.onGround = FALSE;
        }
    } else if (player.vy < FIX32(0)) {
        if (checkSolidCollision(player.x, newY, 42, TOP_COLLISION_OFFSET)) {
            player.vy = FIX32(0);
        } else {
            player.y = newY;
            player.onGround = FALSE;
        }
    }

    // Clear jumpPressed when button released (edge detection)
    if (!jump) player.jumpPressed = FALSE;
    
    if (player.onGround) {
        if (!checkSolidCollision(player.x, player.y, 42, TOP_COLLISION_OFFSET) && 
            !checkPlatformCollision(player.x, player.y, 42, TOP_COLLISION_OFFSET)) {
            player.onGround = FALSE;
        }
    }
    
    if (F32_toInt(player.x) < 0) player.x = FIX32(0);
    if (F32_toInt(player.x) > (MAP_WIDTH * 8) - 42) player.x = FIX32((MAP_WIDTH * 8) - 42);
    if (F32_toInt(player.y) > (MAP_HEIGHT * 8)) player.y = FIX32(0);
    
    s16 playerScreenX = F32_toInt(player.x);
    s16 playerScreenY = F32_toInt(player.y);
    s16 targetX = playerScreenX - 160 + 21;
    s16 targetY = playerScreenY - 112 + 21;
    
    if (targetX < 0) targetX = 0;
    if (targetX > (MAP_WIDTH * 8) - 320) targetX = (MAP_WIDTH * 8) - 320;
    if (targetY < 0) targetY = 0;
    if (targetY > (MAP_HEIGHT * 8) - 224) targetY = (MAP_HEIGHT * 8) - 224;
    
    cameraX += (targetX - cameraX) / 8;
    cameraY += (targetY - cameraY) / 8;
    
    VDP_setHorizontalScroll(BG_B, -cameraX);
    VDP_setVerticalScroll(BG_B, cameraY);
    
    SPR_setPosition(player.sprite, F32_toInt(player.x) - cameraX, F32_toInt(player.y) - cameraY);
    SPR_setHFlip(player.sprite, player.mirroir); 
    SPR_setAnim(player.sprite,player.action); 
}

// Dessine la map
void drawMap() {
    for (u16 y = 0; y < MAP_HEIGHT; y++) {
        for (u16 x = 0; x < MAP_WIDTH; x++) {
            u16 tile;
            switch (levelMap[y][x]) {
                case TILE_SOLID:
                    tile = TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 2);
                    break;
                case TILE_PLATFORM:
                    tile = TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, 3);
                    break;
                default:
                    tile = TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, 1);
                    break;
            }
            VDP_setTileMapXY(BG_B, tile, x, y);
        }
    }
}

int main() {
    VDP_setScreenWidth320();
    PAL_setPalette(PAL0,palette_main.data,DMA);
    SPR_init();

    u16 palette[16];
    

    
    palette[0] = RGB24_TO_VDPCOLOR(0x000000);
    palette[1] = RGB24_TO_VDPCOLOR(0x800000);
    palette[2] = RGB24_TO_VDPCOLOR(0xFF0000);
    palette[3] = RGB24_TO_VDPCOLOR(0xFF8080);
    PAL_setPalette(PAL1, palette, CPU);
    
    palette[0] = RGB24_TO_VDPCOLOR(0x000000);
    palette[1] = RGB24_TO_VDPCOLOR(0x008000);
    palette[2] = RGB24_TO_VDPCOLOR(0x00FF00);
    palette[3] = RGB24_TO_VDPCOLOR(0x80FF80);
    PAL_setPalette(PAL2, palette, CPU);
    
    drawMap();
    initPlayer();
    initBoss(); 
    enemy_bullets_init();
    SPR_setAnim(boss.sprite,BANIM_IDLE);
    while (1) {
        updatePlayer();
        bullets_update();
        enemy_bullets_update();
        bullets_debug_draw();

        SPR_update();
        SYS_doVBlankProcess();
    }
    
    return 0;
}