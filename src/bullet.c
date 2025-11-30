#include <genesis.h>
#include "game.h"
#include "resources.h"
#include "bullets.h"
#include "player.h"

void bullets_init() {
    for (u16 i = 0; i < MAX_BULLETS; i++) {
        game_state.player_bullet[i].active = FALSE;
        game_state.player_bullet[i].sprite = NULL;
    }
    
}

void bullets_spawn(s16 x, s16 y,bool mirrored ) {
    for (u16 i = 0; i < MAX_BULLETS; i++) {
        if (!game_state.player_bullet[i].active) {
            player.action = ANIM_FIRE;
            if(  player.mirroir )
            {
                // on ffiche ele sprite player.shoot a la position du player
                SPR_setPosition(player.sprite_playershoot, x-38, y);
            }
            else {
                // on ffiche ele sprite player.shoot a la position du player
                SPR_setPosition(player.sprite_playershoot, x+10,y);
            }

            // on affiche le sprite
            SPR_setHFlip(player.sprite_playershoot, player.mirroir);
            SPR_setVisibility(player.sprite_playershoot, VISIBLE);  

            game_state.player_bullet[i].x = x;
            game_state.player_bullet[i].y = y;
            game_state.player_bullet[i].active = TRUE;
            
            if (game_state.player_bullet[i].sprite == NULL) {
                game_state.player_bullet[i].sprite = SPR_addSprite(&sprite_bullet,
                                                              x, y,
                                                              TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
            } else {
                SPR_setPosition(game_state.player_bullet[i].sprite, game_state.player_bullet[i].x, game_state.player_bullet[i].y);
                SPR_setVisibility(game_state.player_bullet[i].sprite, VISIBLE);
                game_state.player_bullet[i].mirrored = mirrored;
            }
            SPR_setHFlip(game_state.player_bullet[i].sprite, game_state.player_bullet[i].mirrored);
            break;
        }
    }
}

void bullets_update() {
    for (u16 i = 0; i < MAX_BULLETS; i++) {
        if (game_state.player_bullet[i].active) {
            if(game_state.player_bullet[i].mirrored){
                game_state.player_bullet[i].x -= BULLET_SPEED;
                if (game_state.player_bullet[i].x <0) {
                    game_state.player_bullet[i].active = FALSE;
                    SPR_setVisibility(game_state.player_bullet[i].sprite, HIDDEN);
                } else {
                    SPR_setPosition(game_state.player_bullet[i].sprite,
                                game_state.player_bullet[i].x,
                                game_state.player_bullet[i].y);
                }
            }                
            else {
                game_state.player_bullet[i].x += BULLET_SPEED;
                if (game_state.player_bullet[i].x > 320) {
                    game_state.player_bullet[i].active = FALSE;
                    SPR_setVisibility(game_state.player_bullet[i].sprite, HIDDEN);
                } else {
                    SPR_setPosition(game_state.player_bullet[i].sprite,
                                game_state.player_bullet[i].x,
                                game_state.player_bullet[i].y);
                }
            }
            
            // Désactiver si hors écran
            if (game_state.player_bullet[i].x > 320) {
                game_state.player_bullet[i].active = FALSE;
                SPR_setVisibility(game_state.player_bullet[i].sprite, HIDDEN);
            } else {
                SPR_setPosition(game_state.player_bullet[i].sprite,
                              game_state.player_bullet[i].x,
                              game_state.player_bullet[i].y);
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
            
            // Calculer le vecteur directionnel
            s16 dx = targetX - x;
            s16 dy = targetY - y;
  
                 char buffer[10];
            sprintf(buffer, "%s: %d", "Cur X", targetX);
            VDP_drawText(buffer, 20, 10);
             sprintf(buffer, "%s: %d", "Cur Y", targetY);   
            VDP_drawText(buffer, 20, 11);           

            // Calculer la distance (approximation rapide pour Genesis)
            // Utilise fix16 pour plus de précision
            fix16 fdx = FIX16(dx);
            fix16 fdy = FIX16(dy);
            fix16 distance = F16_sqrt((fdx * fdx)+(fdy * fdy));
            
            sprintf(buffer, "%s: %d", "Cur Y", distance);   
            VDP_drawText(buffer, 20, 13);  

            // Normaliser et appliquer la vitesse
            if (distance > FIX16(0)) {
                fix16 speed = FIX16(game_state.enemy_bullet[i].speed);
                 game_state.enemy_bullet[i].vx = (fdx* speed)/ distance;
                 game_state.enemy_bullet[i].vy =(fdy* speed)/ distance;

             sprintf(buffer, "%s: %d", "v X", (fdx* speed)/ distance);
            VDP_drawText(buffer, 20, 14);
             sprintf(buffer, "%s: %d", "v Y", game_state.enemy_bullet[i].vy);   
            VDP_drawText(buffer, 20, 15);                    
            } else {
                // Si cible = position de départ, tirer vers la droite par défaut
                 game_state.enemy_bullet[i].vx = game_state.enemy_bullet[i].speed;
                 game_state.enemy_bullet[i].vy = 0;
            }
            
            game_state.enemy_bullet[i].sprite = SPR_addSprite(&sprite_bullet, 
                                             game_state.enemy_bullet[i].currentX, 
                                             game_state.enemy_bullet[i].currentY, 
                                            TILE_ATTR(PAL0, 0, FALSE, FALSE));
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
            
            // Vérifier les limites de l'écran
            if (game_state.enemy_bullet[i].currentX < 0 || game_state.enemy_bullet[i].currentX > 320 || 
                game_state.enemy_bullet[i].currentY < 0 || game_state.enemy_bullet[i].currentY > 224) {
                game_state.enemy_bullet[i].active = FALSE;
                SPR_releaseSprite(game_state.enemy_bullet[i].sprite);
                game_state.enemy_bullet[i].sprite = NULL;
            } else {
                // Mettre à jour la position du sprite
                SPR_setPosition(game_state.enemy_bullet[i].sprite, game_state.enemy_bullet[i].currentX, game_state.enemy_bullet[i].currentY );
            }
        }
    } 
    SPR_update();  
}