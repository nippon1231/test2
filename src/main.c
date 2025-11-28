// main.c - Moteur de plateforme avec sols solides et traversants pour SGDK 2.11

#include <genesis.h>
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
    Sprite* sprite;
} Player;

// Variables globales
Player player;
Map* bgMap;
s16 cameraX = 0;
s16 cameraY = 0;

// Tile map étendue (64x32 tiles pour permettre le scroll)
const u8 levelMap[32][64] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
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
    
    if (tileX < 0 || tileX >= 64 || tileY < 0 || tileY >= 32)
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

// Met à jour la caméra pour suivre le joueur
void updateCamera() {
    s16 playerScreenX = F32_toInt(player.x);
    s16 playerScreenY = F32_toInt(player.y);
    
    // Centre horizontal (avec deadzone)
    s16 targetX = playerScreenX - 160 + 21; // Centre du sprite (42/2)
    
    // Limite la caméra à la zone de jeu (64 tiles * 8 = 512 pixels)
    if (targetX < 0) targetX = 0;
    if (targetX > (64 * 8) - 320) targetX = (64 * 8) - 320; // 512 - 320 = 192
    
    // Centre vertical (avec deadzone)
    s16 targetY = playerScreenY - 112 + 21; // Centre vertical
    
    if (targetY < 0) targetY = 0;
    if (targetY > (32 * 8) - 224) targetY = (32 * 8) - 224; // 256 - 224 = 32
    
    // Interpolation douce de la caméra
    cameraX += (targetX - cameraX) / 8;
    cameraY += (targetY - cameraY) / 8;
    
    // Met à jour le scroll du background
    VDP_setHorizontalScroll(BG_B, -cameraX);
    VDP_setVerticalScroll(BG_B, cameraY);

    u16 joy = JOY_readJoypad(JOY_1);
    
    // Déplacement horizontal
    player.vx = FIX32(0);
    if (joy & BUTTON_LEFT) {
        player.vx = FIX32(-MOVE_SPEED);
    }
    if (joy & BUTTON_RIGHT) {
        player.vx = FIX32(MOVE_SPEED);
    }
    
    // Détection des touches pour le saut et descente
    bool jumpDown = (joy & BUTTON_A) || (joy & BUTTON_B) || (joy & BUTTON_C);
    bool downDown = (joy & BUTTON_DOWN);
    
    // Saut
    if (jumpDown && !player.jumpPressed && player.onGround) {
        player.vy = FIX32(JUMP_FORCE);
        player.onGround = FALSE;
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
    fix32 newY = player.y + player.vy;
    
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
            player.onGround = FALSE;
        }
    }
    
    // Vérifie si le joueur est toujours sur le sol après le déplacement horizontal
    if (player.onGround) {
        if (!checkSolidCollision(player.x, player.y, 42, 42) && 
            !checkPlatformCollision(player.x, player.y, 42, 42)) {
            player.onGround = FALSE;
        }
    }
    
    // Limites de l'écran
    if (F32_toInt(player.x) < 0) player.x = FIX32(0);
    if (F32_toInt(player.x) > 470) player.x = FIX32(470); // 512 - 42
    if (F32_toInt(player.y) > 256) player.y = FIX32(100); // Respawn
    
    // Met à jour le sprite (position relative à la caméra pour les sprites)
    SPR_setPosition(player.sprite, 
                    F32_toInt(player.x) - cameraX, 
                    F32_toInt(player.y) - cameraY);
}

// Dessine la map
void drawMap() {
    for (u16 y = 0; y < 32; y++) {
        for (u16 x = 0; x < 64; x++) {
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
    
    // Palette simple (couleurs basiques)
    u16 palette[16];
    
    // PAL0 - Gris pour le joueur
    palette[0] = RGB24_TO_VDPCOLOR(0x000000);
    palette[1] = RGB24_TO_VDPCOLOR(0x808080);
    palette[2] = RGB24_TO_VDPCOLOR(0xC0C0C0);
    palette[3] = RGB24_TO_VDPCOLOR(0xFFFFFF);
    PAL_setPalette(PAL0, palette, CPU);
    
    // PAL1 - Rouge pour les sols solides
    palette[0] = RGB24_TO_VDPCOLOR(0x000000);
    palette[1] = RGB24_TO_VDPCOLOR(0x800000);
    palette[2] = RGB24_TO_VDPCOLOR(0xFF0000);
    palette[3] = RGB24_TO_VDPCOLOR(0xFF8080);
    PAL_setPalette(PAL1, palette, CPU);
    
    // PAL2 - Vert pour les plateformes
    palette[0] = RGB24_TO_VDPCOLOR(0x000000);
    palette[1] = RGB24_TO_VDPCOLOR(0x008000);
    palette[2] = RGB24_TO_VDPCOLOR(0x00FF00);
    palette[3] = RGB24_TO_VDPCOLOR(0x80FF80);
    PAL_setPalette(PAL2, palette, CPU);
    
    // Dessine la map
    drawMap();
    
    // Initialise le joueur
    initPlayer();
    
    // Boucle principale
    while (1) {
        updateCamera();
        updateCamera();
        SPR_update();
        SYS_doVBlankProcess();
    }
    
    return 0;
}