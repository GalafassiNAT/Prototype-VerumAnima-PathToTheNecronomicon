#include "enemy.h"
#include "globals.h"
#include "resources.h"
#include "player.h"

GameObject enemies_bat[MAX_BATS];
u8 active_bats_count = 0;

void ENEMY_init_system(){
	active_bats_count = 0;
}

void ENEMY_spawn_bat(s16 x, s16 y, u16* current_vram_tile_index) {
	if (active_bats_count > MAX_BATS) {
		return;
	}

	GameObject* new_bat = &enemies_bat[active_bats_count];

	u16 tiles_for_enemy = GAMEOBJECT_init(new_bat, &spr_bat, x, y, -16, -16, PAL_ENEMY, *current_vram_tile_index);
	*current_vram_tile_index += tiles_for_enemy;

	new_bat->speed_x = FIX16(-1.5);
	new_bat->speed_y = 0;
	new_bat->health = 15;

	SPR_setAnim(new_bat->sprite, 0);

	active_bats_count++;
}

void ENEMY_update_all() {
	for (s8 i = MAX_BATS - 1; i >= 0; i--) {
        GameObject* enemy = &enemies_bat[i]; 

        if (enemy->health > 0) {
            enemy->x += enemy->speed_x;

            s16 current_x_px = fix16ToInt(enemy->x);
            if (current_x_px < 0) {
                enemy->x = FIX16(0);
                enemy->speed_x = -enemy->speed_x;
                SPR_setHFlip(enemy->sprite, FALSE); 
            } else if (current_x_px + enemy->w > SCREEN_W) {
                enemy->x = FIX16(SCREEN_W - enemy->w);
                enemy->speed_x = -enemy->speed_x;
                SPR_setHFlip(enemy->sprite, TRUE); 
            }
            // ...

            GAMEOBJECT_update_boundbox(enemy->x, enemy->y, enemy); 
            


            for (u8 j = 0; j < MAX_PLAYER_BULLETS; j++) {
                GameObject *bullet = &player_bullets[j];

                if (bullet->health > 0) {
                    
                    if (GAMEOBJECT_check_collision(enemy, bullet)) {
                        enemy->health -= PLAYER_bullet_dmg;
                        bullet->health = 0; // deactivate bullet
                        SPR_setVisibility(bullet->sprite, HIDDEN);


                        if (enemy->health <= 0) {

                            // if (i < active_bats_count - 1) {
                            //     // Move the last active bat to the current position
                            //     enemies_bat[i] = enemies_bat[active_bats_count - 1];
                            // }
                            enemy->health = 0;
                            SPR_setVisibility(enemy->sprite, HIDDEN);
                            active_bats_count--;
                            break;
                        }
                    }
                }
            }
            if (enemy->health > 0) {
                SPR_setVisibility(enemy->sprite, VISIBLE);
                SPR_setPosition(enemy->sprite, enemy->box.left + enemy->w_offset, enemy->box.top + enemy->h_offset);
            }
        }
    }
}