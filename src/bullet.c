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
            game_state.enemy_bullet[i].targetX = (targetX);
            game_state.enemy_bullet[i].targetY = (targetY);
            game_state.enemy_bullet[i].speed = 0.1;
            game_state.enemy_bullet[i].active = TRUE;
            game_state.enemy_bullet[i].isMoving = TRUE;
            
            if (game_state.enemy_bullet[i].sprite == NULL) {
                game_state.enemy_bullet[i].sprite = SPR_addSprite(&sprite_bullet,
                                                              x, y ,
                                                              TILE_ATTR(PAL0, TRUE, FALSE, FALSE));                                              
            } else {
                SPR_setPosition(game_state.enemy_bullet[i].sprite, x, y);
                SPR_setVisibility(game_state.enemy_bullet[i].sprite, VISIBLE);
            }
          
            break;
        }
    }
}
void enemy_bullets_update() {


        char info[10];
            sprintf(info,"%10li",game_state.enemy_bullet[0].targetX);
            VDP_drawTextBG(BG_A, info, 4, 17);
        //     sprintf(info,"%10li",game_state.enemy_bullet[0].targetY);           
        //    VDP_drawTextBG(BG_A, info, 4, 18);
            sprintf(info,"%10li",game_state.enemy_bullet[0].currentX);
            VDP_drawTextBG(BG_A, info, 4, 19);
                sprintf(info,"%10li",game_state.enemy_bullet[0].currentY);           
                VDP_drawTextBG(BG_A, info, 4, 20); 
    for (u16 i = 0; i < MAX_ENEMY_BULLETS; i++) {
         if (game_state.enemy_bullet[i].active){
            // Calculer la distance restante
            s16 deltaX = game_state.enemy_bullet[i].targetX - game_state.enemy_bullet[i].currentX;
            s16 deltaY = game_state.enemy_bullet[i].targetY - game_state.enemy_bullet[i].currentY;
            
            // Calculer la distance totale
            s16 distance = F16_sqrt((deltaX* deltaX)+(deltaY* deltaY));
            
            // Vérifier si on est arrivé (seuil de 0.5 pixel)
            if (distance < FIX16(0.5)) {
                game_state.enemy_bullet[i].currentX = 0;
                game_state.enemy_bullet[i].currentY = 0; 
                game_state.enemy_bullet[i].isMoving = FALSE;
                game_state.enemy_bullet[i].active = FALSE;
                SPR_setVisibility(game_state.enemy_bullet[i].sprite, HIDDEN);
            } else {
                // Normaliser le vecteur de direction et appliquer la vitesse
                fix16 moveX = (deltaX* game_state.enemy_bullet[i].speed)+ distance;
                fix16 moveY = (deltaY* game_state.enemy_bullet[i].speed)+ distance;             
                game_state.enemy_bullet[i].currentX = (game_state.enemy_bullet[i].currentX+ F16_toInt(moveX));
                game_state.enemy_bullet[i].currentY = (game_state.enemy_bullet[i].currentY+ F16_toInt(moveY));
            }   

                                        // Désactiver si hors écran
            
            if (game_state.enemy_bullet[i].currentX > 320 || game_state.enemy_bullet[i].currentX < 0 ||
                game_state.enemy_bullet[i].currentY > 224 || game_state.enemy_bullet[i].currentY < 0) {
                game_state.enemy_bullet[i].active = FALSE;
                game_state.enemy_bullet[i].currentX = 0;
                game_state.enemy_bullet[i].currentY = 0;    
                SPR_setVisibility(game_state.enemy_bullet[i].sprite, HIDDEN);
            } else {
             
                            // Mettre à jour la position du sprite
            SPR_setPosition(game_state.enemy_bullet[i].sprite, 
                           game_state.enemy_bullet[i].currentX,                           
                            game_state.enemy_bullet[i].currentY
                        );
         
                }
        }
    }   
}