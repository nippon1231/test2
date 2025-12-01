#include <genesis.h>
#include "game.h"
#include "resources.h"
#include "bullets.h"
#include "player.h"
#include "boss.h"
// Camera position (defined in main.c)
extern s16 cameraX;
extern s16 cameraY;
// Boss instance (defined in main.c)
extern Boss boss;
// Screen dimensions and sprite margin used for off-screen checks
#define SCREEN_W 320
#define SCREEN_H 224
#define SPRITE_MARGIN 8
#define ENEMY_BULLET_W 4
#define ENEMY_BULLET_H 4
// Player bullet size (approx)
#define PLAYER_BULLET_W 4
#define PLAYER_BULLET_H 4

// Integer square-root for non-negative 64-bit values. Returns floor(sqrt(x)).
static s32 isqrt32(long long x) {
    if (x <= 0) return 0;
    long long lo = 0;
    long long hi = 1LL << 16; // sqrt(2^32) ~ 65536, safe upper bound
    long long res = 0;
    while (lo <= hi) {
        long long mid = (lo + hi) >> 1;
        long long sq = mid * mid;
        if (sq == x) return (s32)mid;
        if (sq < x) {
            res = mid;
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }
    return (s32)res;
}

void bullets_init() {
    for (u16 i = 0; i < MAX_BULLETS; i++) {
        game_state.player_bullet[i].active = FALSE;
        game_state.player_bullet[i].sprite = NULL;
        game_state.player_bullet[i].mirrored = FALSE;
        game_state.player_bullet[i].x = 0;
        game_state.player_bullet[i].y = 0;
    }
    
}

void bullets_spawn(s16 x, s16 y,bool mirrored ) {
    for (u16 i = 0; i < MAX_BULLETS; i++) {
        if (!game_state.player_bullet[i].active) {
            player.action = ANIM_FIRE;
            // Utiliser le paramètre `mirrored` fourni pour la direction
            if (mirrored) {
                SPR_setPosition(player.sprite_playershoot, x - 38, y);
            } else {
                SPR_setPosition(player.sprite_playershoot, x + 10, y);
            }

            SPR_setHFlip(player.sprite_playershoot, mirrored);
            SPR_setVisibility(player.sprite_playershoot, VISIBLE);

            // convert incoming screen coordinates to world coordinates
            s32 worldX = x + cameraX;
            s32 worldY = y + cameraY;
            game_state.player_bullet[i].x = FIX32(worldX);
            game_state.player_bullet[i].y = FIX32(worldY);
            game_state.player_bullet[i].active = TRUE;
            game_state.player_bullet[i].mirrored = mirrored;

            // create sprite at screen coordinates (world - camera)
            s32 sx = F32_toInt(game_state.player_bullet[i].x) - cameraX;
            s32 sy = F32_toInt(game_state.player_bullet[i].y) - cameraY;
            if (game_state.player_bullet[i].sprite == NULL) {
                game_state.player_bullet[i].sprite = SPR_addSprite(&sprite_bullet,
                                                              sx, sy,
                                                              TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
            } else {
                SPR_setPosition(game_state.player_bullet[i].sprite, sx, sy);
                SPR_setVisibility(game_state.player_bullet[i].sprite, VISIBLE);
            }
            SPR_setHFlip(game_state.player_bullet[i].sprite, game_state.player_bullet[i].mirrored);
            break;
        }
    }
}

void bullets_update() {
    for (u16 i = 0; i < MAX_BULLETS; i++) {
        if (game_state.player_bullet[i].active) {
            // Mettre à jour la position monde (fix32) selon la direction
            if (game_state.player_bullet[i].mirrored) {
                game_state.player_bullet[i].x = game_state.player_bullet[i].x - FIX32(BULLET_SPEED);
            } else {
                game_state.player_bullet[i].x = game_state.player_bullet[i].x + FIX32(BULLET_SPEED);
            }

            // Calculer position écran
            s32 sx = F32_toInt(game_state.player_bullet[i].x) - cameraX;
            s32 sy = F32_toInt(game_state.player_bullet[i].y) - cameraY;

            // Check collision with boss in world coordinates
            if (boss.alive && boss.sprite != NULL) {
                s32 bulletWorldX = F32_toInt(game_state.player_bullet[i].x);
                s32 bulletWorldY = F32_toInt(game_state.player_bullet[i].y);
                s32 bossWorldX = F32_toInt(boss.x);
                s32 bossWorldY = F32_toInt(boss.y);
                if (bulletWorldX < bossWorldX + BOSS_HITBOX_W && bulletWorldX + PLAYER_BULLET_W > bossWorldX
                    && bulletWorldY < bossWorldY + BOSS_HITBOX_H && bulletWorldY + PLAYER_BULLET_H > bossWorldY) {
                    // bullet hits boss
                    game_state.player_bullet[i].active = FALSE;
                    if (game_state.player_bullet[i].sprite) {
                        SPR_releaseSprite(game_state.player_bullet[i].sprite);
                        game_state.player_bullet[i].sprite = NULL;
                    }
                    // apply damage
                    if (boss.health > 0) boss.health--;
                    if (boss.health > 0) {
                        if (boss.sprite) {
                            SPR_setAnim(boss.sprite, BANIM_CROUCH);
                            boss.anim_timer = 30;
                        }
                        boss.shake_timer = 12; // shake for 12 frames
                    } else {
                        // boss defeated
                        boss.alive = FALSE;
                        if (boss.sprite) {
                            SPR_releaseSprite(boss.sprite);
                            boss.sprite = NULL;
                        }
                    }
                    continue; // next bullet
                }
            }
            // Désactiver si hors écran (comparer avec marge pour tenir compte de la taille du sprite)
            if (sx < -SPRITE_MARGIN || sx >= SCREEN_W + SPRITE_MARGIN) {
                game_state.player_bullet[i].active = FALSE;
                SPR_setVisibility(game_state.player_bullet[i].sprite, HIDDEN);
            } else {
                SPR_setPosition(game_state.player_bullet[i].sprite, sx, sy);
            }
        }
    }
}


// enemy bullets
void enemy_bullets_init() {
    for (u16 i = 0; i < MAX_ENEMY_BULLETS; i++) {
        game_state.enemy_bullet[i].active = FALSE;
        game_state.enemy_bullet[i].sprite = NULL;
    }
}

void enemy_bullet_shoot(s32 x,s32 y, s32 targetX, s32 targetY, s16 speed) {



    for (u16 i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (!game_state.enemy_bullet[i].active) {

            game_state.enemy_bullet[i].currentX = x;
            game_state.enemy_bullet[i].currentY = y;
            game_state.enemy_bullet[i].active = TRUE;
            
                // Calculer le vecteur directionnel (en pixels)
                s32 dx_pixels = F32_toInt(targetX) - F32_toInt(x);
                s32 dy_pixels = F32_toInt(targetY) - F32_toInt(y);
                bool mirrored = (dx_pixels < 0);

                /* Distance en pixels computed as integer sqrt of dx^2+dy^2 */
                long long dist_sq = (long long)dx_pixels * (long long)dx_pixels + (long long)dy_pixels * (long long)dy_pixels;
                s32 dist = isqrt32(dist_sq);

            // Normaliser et appliquer la vitesse (calcul en pixels puis conversion en fix32)
            if (dist > 0) {
                // Appliquer un facteur de réduction configuré via ENEMY_BULLET_SPEED_SCALE
                long long denom = (long long)dist * (long long)ENEMY_BULLET_SPEED_SCALE;
                s32 vx_fixed = (s32)((((long long)dx_pixels * (long long)speed) << 16) / denom);
                s32 vy_fixed = (s32)((((long long)dy_pixels * (long long)speed) << 16) / denom);
                game_state.enemy_bullet[i].vx = vx_fixed;
                game_state.enemy_bullet[i].vy = vy_fixed;
            } else {
                /* cible au même pixel -> tirer à droite */
                game_state.enemy_bullet[i].vx = ((s32)speed) << 16;
                game_state.enemy_bullet[i].vy = 0;
            }
            
            game_state.enemy_bullet[i].mirrored = mirrored;
            game_state.enemy_bullet[i].sprite = SPR_addSprite(&sprite_bullet,
                                             F32_toInt(game_state.enemy_bullet[i].currentX) - cameraX,
                                             F32_toInt(game_state.enemy_bullet[i].currentY) - cameraY,
                                            TILE_ATTR(PAL0, 0, FALSE, FALSE));
            SPR_setHFlip(game_state.enemy_bullet[i].sprite, mirrored);
            break;
        }      
    }
}
void enemy_bullets_update() {
    for (u16 i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (game_state.enemy_bullet[i].active) {
            // Déplacer le projectile


            game_state.enemy_bullet[i].currentX += game_state.enemy_bullet[i].vx;
            game_state.enemy_bullet[i].currentY += game_state.enemy_bullet[i].vy;
            
            // Vérifier les limites de l'écran (comparer en pixels, pas en fix32)
            s32 ex = F32_toInt(game_state.enemy_bullet[i].currentX) - cameraX;
            s32 ey = F32_toInt(game_state.enemy_bullet[i].currentY) - cameraY;
            // Collision simple AABB avec le joueur (positions écran)
            s32 px = F32_toInt(player.x) - cameraX;
            s32 py = F32_toInt(player.y) - cameraY;
            if (ex < px + PLAYER_HITBOX_W && ex + ENEMY_BULLET_W > px && ey < py + PLAYER_HITBOX_H && ey + ENEMY_BULLET_H > py) {
                // Collision détectée : désactiver la balle
                game_state.enemy_bullet[i].active = FALSE;
                if (game_state.enemy_bullet[i].sprite) {
                    SPR_releaseSprite(game_state.enemy_bullet[i].sprite);
                    game_state.enemy_bullet[i].sprite = NULL;
                }

                // Appliquer dégâts au joueur
                if (player.health > 0) player.health--;

                // Si le joueur a encore de la vie, appliquer un knockback visuel
                if (player.health > 0) {
                    s32 knock = (ex < px) ? 2 : -2; // pousser vers la droite si l'impact vient de la gauche
                    player.vx = FIX32(knock);
                    player.vy = FIX32(-4); // petit recul vertical
                    player.action = ANIM_CROUCH;
                } else {
                    // Mort : respawn au point de départ et réinitialiser la vie
                    player.x = FIX32(40);
                    player.y = FIX32(0);
                    player.vx = FIX32(0);
                    player.vy = FIX32(0);
                    player.onGround = FALSE;
                    player.health = PLAYER_MAX_HEALTH;
                    player.action = ANIM_CROUCH;
                }

                continue;
            }
            if (ex < -SPRITE_MARGIN || ex >= SCREEN_W + SPRITE_MARGIN || ey < -SPRITE_MARGIN || ey >= SCREEN_H + SPRITE_MARGIN) {
                game_state.enemy_bullet[i].active = FALSE;
                SPR_releaseSprite(game_state.enemy_bullet[i].sprite);
                game_state.enemy_bullet[i].sprite = NULL;
            } else {
                // Mettre à jour la position du sprite et appliquer flip
                SPR_setPosition(game_state.enemy_bullet[i].sprite, ex, ey );
                SPR_setHFlip(game_state.enemy_bullet[i].sprite, game_state.enemy_bullet[i].mirrored);
            }
        }
    } 
    SPR_update();  
}

// Debug drawing removed.

// Camera position (defined in main.c) declared at top of file