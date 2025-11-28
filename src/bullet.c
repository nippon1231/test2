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

