// main.c - Moteur de plateforme avec sols solides et traversants pour SGDK 2.11

#include <genesis.h>
#include "game.h"
#include "resources.h"
// Constantes
#define GRAVITY 0.4
#define JUMP_FORCE -6.5
#define MOVE_SPEED 2
#define MAX_FALL_SPEED 8

// Types de tiles
#define TILE_EMPTY 0
#define TILE_SOLID 1
#define TILE_PLATFORM 2

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
} Player;

// Variables globales
Player player;
Map* bgMap;

// Tile map simple (32x28 tiles)
const u8 levelMap[28][32] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,2,2,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// Récupère le type de tile à une position donnée
u8 getTileAt(s16 x, s16 y) {
    s16 tileX = x / 8;
    s16 tileY = y / 8;
    
    if (tileX < 0 || tileX >= 32 || tileY < 0 || tileY >= 28)
        return TILE_EMPTY;
    
    return levelMap[tileY][tileX];
}

// Vérifie la collision avec les sols solides
bool checkSolidCollision(fix32 x, fix32 y, s16 width, s16 height) {
    s16 px = F32_toInt(x);
    s16 py = F32_toInt(y);
    
    // Vérifie les 4 coins et le centre pour un sprite 42x42
    if (getTileAt(px, py) == TILE_SOLID) return TRUE;
    if (getTileAt(px + width - 1, py) == TILE_SOLID) return TRUE;
    if (getTileAt(px, py + height - 1) == TILE_SOLID) return TRUE;
    if (getTileAt(px + width - 1, py + height - 1) == TILE_SOLID) return TRUE;
    if (getTileAt(px + width/2, py) == TILE_SOLID) return TRUE;
    if (getTileAt(px + width/2, py + height - 1) == TILE_SOLID) return TRUE;
    
    return FALSE;
}

// Vérifie la collision avec les plateformes (uniquement par le haut)
bool checkPlatformCollision(fix32 x, fix32 y, s16 width, s16 height) {
    s16 px = F32_toInt(x);
    s16 py = F32_toInt(y);
    
    // Vérifie uniquement les pieds du personnage (plusieurs points pour 42px de large)
    if (getTileAt(px, py + height - 1) == TILE_PLATFORM) return TRUE;
    if (getTileAt(px + width - 1, py + height - 1) == TILE_PLATFORM) return TRUE;
    if (getTileAt(px + width/2, py + height - 1) == TILE_PLATFORM) return TRUE;
    
    return FALSE;
}

// Initialise le joueur
void initPlayer() {
    player.x = FIX32(40);
    player.y = FIX32(100);
    player.vx = FIX32(0);
    player.vy = FIX32(0);
    player.onGround = FALSE;
    player.jumpPressed = FALSE;
    player.downPressed = FALSE;
    
    // Sprite 42x42
    player.sprite = SPR_addSprite(&sprite_player, 
                                  F32_toInt(player.x), 
                                  F32_toInt(player.y), 
                                  TILE_ATTR(PAL0, 0, FALSE, FALSE));
}

// Met à jour la physique du joueur
void updatePlayer() {
    u16 joy = JOY_readJoypad(JOY_1);
    player.action = ANIM_IDLE;
    // Déplacement horizontal
    player.vx = FIX32(0);
    if (joy & BUTTON_LEFT) {
        player.vx = FIX32(-MOVE_SPEED);
        player.mirroir = TRUE;
        player.action = ANIM_WALK;
    }
    if (joy & BUTTON_RIGHT) {
        player.vx = FIX32(MOVE_SPEED);
        player.mirroir = FALSE;
        player.action = ANIM_WALK;

    }
    
    // Détection des touches pour le saut et descente
    bool jumpDown = (joy & BUTTON_A) || (joy & BUTTON_B) || (joy & BUTTON_C);
    bool downDown = (joy & BUTTON_DOWN);
    
    // Saut
    if (jumpDown && !player.jumpPressed && player.onGround) {
        player.vy = FIX32(JUMP_FORCE);
        player.onGround = FALSE;
        player.action = ANIM_JUMP;
    }
    player.jumpPressed = jumpDown;
    
    // Descendre à travers les plateformes
    if (downDown && !player.downPressed && player.onGround) {
        // Déplace légèrement vers le bas pour traverser la plateforme
        player.y = player.y + FIX32(2);
        player.onGround = FALSE;
    }
    player.downPressed = downDown;
    
    // Gravité
    if (!player.onGround) {
        player.vy = player.vy + FIX32(GRAVITY);
        if (F32_toInt(player.vy) > MAX_FALL_SPEED) {
            player.vy = FIX32(MAX_FALL_SPEED);
        }
    }
    
    // Déplacement horizontal
    fix32 newX = player.x + player.vx;
    if (!checkSolidCollision(newX, player.y, 42, 42)) {
        player.x = newX;
    }
    
    // Déplacement vertical
    fix32 newY = player.y+ player.vy;
    
    // Collision avec le sol solide
    if (player.vy > FIX32(0)) { // Descend
        if (checkSolidCollision(player.x, newY, 42, 42)) {
            // Ajuste la position au-dessus du sol
            player.y = FIX32((F32_toInt(player.y) / 8) * 8);
            player.vy = FIX32(0);
            player.onGround = TRUE;
        } else if (checkPlatformCollision(player.x, newY, 42, 42) && 
                   !checkPlatformCollision(player.x, player.y, 42, 42)) {
            // Atterrit sur une plateforme
            player.y = FIX32((F32_toInt(newY) / 8) * 8);
            player.vy = FIX32(0);
            player.onGround = TRUE;
        } else {
            player.y = newY;
            player.onGround = FALSE;
        }
    } else if (player.vy < FIX32(0)) { // Monte
        if (checkSolidCollision(player.x, newY, 42, 42)) {
            // Touche le plafond solide
            player.vy = FIX32(0);
        } else {
            player.y = newY;
        }
    }
    
    // Limites de l'écran
    if (F32_toInt(player.x) < 0) player.x = FIX32(0);
    if (F32_toInt(player.x) > 278) player.x = FIX32(278); // 320 - 42
    if (F32_toInt(player.y) > 224) player.y = FIX32(100); // Respawn
    
    // Met à jour le sprite
    SPR_setPosition(player.sprite, F32_toInt(player.x), F32_toInt(player.y));
        SPR_setHFlip(player.sprite, player.mirroir); 
    SPR_setAnim(player.sprite,player.action);  
}

// Dessine la map
void drawMap() {
    for (u16 y = 0; y < 28; y++) {
        for (u16 x = 0; x < 32; x++) {
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
    // Initialisation
    VDP_setScreenWidth320();
    SPR_init();
    
    // Palette simple
    PAL_setColor(0, RGB24_TO_VDPCOLOR(0x000000));
    PAL_setColor(1, RGB24_TO_VDPCOLOR(0xFFFFFF));
    PAL_setColor(2, RGB24_TO_VDPCOLOR(0xAA0000)); // Couleur pour le joueur
    
    // Dessine la map
    drawMap();
    
    // Initialise le joueur
    initPlayer();
    
    // Boucle principale
    while (1) {
        updatePlayer();
        SPR_update();
        SYS_doVBlankProcess();
    }
    
    return 0;
}