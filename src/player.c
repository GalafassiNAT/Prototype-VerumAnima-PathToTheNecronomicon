#include <genesis.h>
#include <maths.h>

#include "player.h"
#include "engine/level.h"
#include "engine/utils.h"
#include "hud.h"

GameObject player;
GameObject player_bullets[MAX_PLAYER_BULLETS];
#define BULLET_SPEED FIX16(4)

#define ANIM_PLAYER_IDLE 	0
#define ANIM_PLAYER_UP 		1
#define ANIM_PLAYER_DOWN 	2
#define ANIM_PLAYER_LEFT 	3
#define ANIM_PLAYER_RIGHT 	4


////////////////////////////////////////////////////////////////////////////
// PRIVATE MEMBERS

static inline void PLAYER_get_input_platformer();
static inline void PLAYER_get_input_dir4();
static inline void PLAYER_get_input_dir8();
static inline bool on_ground();

////////////////////////////////////////////////////////////////////////////
// INIT

u16 PLAYER_init(u16 ind) {
	ind += GAMEOBJECT_init(&player, &spr_plat, SCREEN_W/2-12, SCREEN_H/2-12, -16, -16, PAL_PLAYER, ind);
	player.health = PLAYER_MAX_HEALTH;

	u16 bullet_color = RGB24_TO_VDPCOLOR(0xE00000); // red

	PAL_setColor(PAL_MAP * 16 + 1, bullet_color);

	for (u8 i = 0; i < MAX_PLAYER_BULLETS; i++) {
		player_bullets[i].health = 0; // not active

		u16 bullet_sprite_tiles = GAMEOBJECT_init(&player_bullets[i], &spr_player_shot, -64,-64, 0,0, PAL_MAP, ind);
		ind += bullet_sprite_tiles;
		SPR_setVisibility(player_bullets[i].sprite, HIDDEN);

	}

	return ind;
}

////////////////////////////////////////////////////////////////////////////
// UPDATE

void PLAYER_update() {
	// input
	// PLAYER_get_input_dir4();
	PLAYER_get_input_dir8();
	PLAYER_shoot();
	PLAYER_update_bullets();
	// PLAYER_get_input_platformer();
	
	// project next position
	player.next_x = player.x + player.speed_x;
	player.next_y = player.y + player.speed_y;

	// check and resolve walls
	LEVEL_move_and_slide(&player);
	
	// update current position
	player.x = player.next_x;
	player.y = player.next_y;
	
	// limit do map boundaries
	LEVEL_check_map_boundaries(&player);
	
	// item check
	GAMEOBJECT_update_boundbox(player.x, player.y, &player);
	if (LEVEL_tileXY(player.box.left + player.w/2, player.box.top + player.h/2) == IDX_ITEM) {
		HUD_gem_collected(1);
		LEVEL_remove_tileXY(player.box.left + player.w/2, player.box.top + player.h/2, 0);
	}

	// GAMEOBJECT_wrap_screen(&player);
	GAMEOBJECT_clamp_screen(&player);
	
	// update VDP/SGDK
	GAMEOBJECT_update_boundbox(player.x, player.y, &player);
	SPR_setPosition(player.sprite, player.box.left + player.w_offset, player.box.top + player.h_offset);
	SPR_setAnim(player.sprite, player.anim);

}

////////////////////////////////////////////////////////////////////////////
// SHOOT
void PLAYER_shoot() {
	if (key_pressed(JOY_1, BUTTON_A)) {

		for (u8 i = 0; i < MAX_PLAYER_BULLETS; i++) {
			if (player_bullets[i].health == 0) {
				GameObject *bullet = &player_bullets[i];
				bullet->health = 1; // active
				
				bullet->y = player.y + FIX16(player.h/2) - FIX16(bullet->h/2);
				bullet->speed_y = 0;
	
				bullet->x = player.x + FIX16(player.w);
	
				bullet->speed_x = BULLET_SPEED;
				bullet->speed_y = 0;
	
				SPR_setHFlip(bullet->sprite, FALSE); // default right
				GAMEOBJECT_update_boundbox(bullet->x, bullet->y, bullet);
				SPR_setPosition(bullet->sprite, bullet->box.left + bullet->w_offset, bullet->box.top + bullet->h_offset);
				SPR_setVisibility(bullet->sprite, VISIBLE);
				SPR_setAnimAndFrame(bullet->sprite, 0, 0); // default animation
	
				return;
			}
		}
	}
}


void PLAYER_update_bullets() {
	for (u8 i = 0; i < MAX_PLAYER_BULLETS; i++) {
		GameObject *bullet = &player_bullets[i];
		if (bullet->health > 0) {
			bullet->x += bullet->speed_x;

			GAMEOBJECT_update_boundbox(bullet->x, bullet->y, bullet);
			SPR_setPosition(bullet->sprite, bullet->box.left + bullet->w_offset, bullet->box.top + bullet->h_offset);

			if (bullet->box.right < 0 || bullet->box.left > SCREEN_W ||
				bullet->box.bottom < 0 || bullet->box.top > SCREEN_H) {
				// out of screen
				bullet->health = 0;
				SPR_setVisibility(bullet->sprite, HIDDEN);
			} else {
				// check collision with level map
				u8 collision = LEVEL_collision_result();
				if (collision & COLLISION_LEFT || collision & COLLISION_RIGHT ||
					collision & COLLISION_TOP || collision & COLLISION_BOTTOM) {
					// hit a wall
					bullet->health = 0;
					SPR_setVisibility(bullet->sprite, HIDDEN);
				}
			}
		}
	}
}



////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

/**
 * Implements a platformer controlled character.
 * Current implementation contains the following features:
 * - Variable height jump (by button hold)
 * - Head bump on level map (makes the character fall instantly)
 * - Level blocks breaking by head bumping them from bellow
 */
static inline bool on_ground() {
	return LEVEL_collision_result() & COLLISION_BOTTOM;
}

static inline void PLAYER_get_input_platformer() {
	if (key_down(JOY_1, BUTTON_RIGHT)) {
		player.speed_x = PLAYER_SPEED;
		player.anim = ANIM_PLAYER_RIGHT;
	}
	else 
	if (key_down(JOY_1, BUTTON_LEFT)) {
		player.speed_x = -PLAYER_SPEED;
		player.anim = ANIM_PLAYER_LEFT;
	} else {
		player.speed_x = 0;
	}

	// if is on ground
	if (on_ground()) {
		player.speed_y = FIX16(1);
	} 
	else
	// if head stomped a block above
	if (LEVEL_collision_result() & COLLISION_TOP) {
		LEVEL_remove_tileXY(player.box.left + player.w/2 - 4, player.box.top - 8, 0);
		LEVEL_remove_tileXY(player.box.left + player.w/2 + 4, player.box.top - 8, 0);
		player.speed_y = 0;
	}
	
	// falls when jump button is released
	if (key_released(JOY_1, BUTTON_A)) {
		// if isn't on ground and is going up
		if (!on_ground()) {
			if (player.speed_y < 0 && player.speed_y < FIX16(-2.4)) {
				player.speed_y = FIX16(-2.4);
			}
		}
	}

	// jump
	if (key_pressed(JOY_1, BUTTON_A)) {
		if (on_ground()) {
			player.speed_y = FIX16(-4);
		}
	}

	// limit gravity down speed
	player.speed_y += FIX16(0.15);
	if (player.speed_y > FIX16(4)) {
		player.speed_y = FIX16(4);
	}

// 	if (player.speed_y < 0) 
// 		player.anim = 2;
// 	else if (player.speed_y > 0) 
// 		player.anim = 6;
}

/**
 * Get player input and set ship speed with:
 * - Constant speed
 * - Always moving (doesn't stop when dpad released)
 * - Four directions, mutually exclusive
 */
static inline void PLAYER_get_input_dir4() {
	if (key_down(JOY_1, BUTTON_RIGHT)) {
		player.speed_x = PLAYER_SPEED;
		player.speed_y = 0;
		player.anim = 0;
	}
	else 
	if (key_down(JOY_1, BUTTON_LEFT)) {
		player.speed_x = -PLAYER_SPEED;
		player.speed_y = 0;
		player.anim = 4;
	}
	else
	if (key_down(JOY_1, BUTTON_UP)) {
		player.speed_x = 0;
		player.speed_y = -PLAYER_SPEED;
		player.anim = 2;
	}
	else
	if (key_down(JOY_1, BUTTON_DOWN)) {
		player.speed_x = 0;
		player.speed_y = PLAYER_SPEED;
		player.anim = 6;
	} 
}

/**
 * Get player input and set ship speed with:
 * - Constant speed
 * - Stop when dpad released
 * - Eight directions + fix for diagonals
 */
static inline void PLAYER_get_input_dir8() {
	s16 final_speed_X = 0;
	s16 final_speed_Y = 0;
	u8 anim = player.anim;

	/* ANIM DIRECTIONS
		      2
		   3     1
		4           0
		   5     7
		      6
	*/

	if (key_down(JOY_1, BUTTON_UP)) final_speed_Y = -PLAYER_SPEED;
    else if (key_down(JOY_1, BUTTON_DOWN)) final_speed_Y = PLAYER_SPEED;

    if (key_down(JOY_1, BUTTON_LEFT)) final_speed_X = -PLAYER_SPEED;
    else if (key_down(JOY_1, BUTTON_RIGHT)) final_speed_X = PLAYER_SPEED;

    player.speed_x = final_speed_X;
    player.speed_y = final_speed_Y;


	if (player.speed_y < 0) {
		anim = ANIM_PLAYER_UP;
	} else if (player.speed_y > 0) {
		anim = ANIM_PLAYER_DOWN;
	} else if (player.speed_x < 0) {
		if (player.speed_y < 0) {
			anim = ANIM_PLAYER_UP; // up-left
		} else if (player.speed_y > 0) {
			anim = ANIM_PLAYER_DOWN; // down-left
		} else {
			anim = ANIM_PLAYER_LEFT; // left
		}
	} else if (player.speed_x > 0) {
		if (player.speed_y < 0) {
			anim = ANIM_PLAYER_UP; // up-right
		} else if (player.speed_y > 0) {
			anim = ANIM_PLAYER_DOWN; // down-right
		} else {
			anim = ANIM_PLAYER_RIGHT; // right
		}
	} else {
		anim = ANIM_PLAYER_IDLE;
	}
	player.anim = anim;

	if (player.speed_x != 0 && player.speed_y != 0) {
        player.speed_x = (player.speed_x > 0) ? PLAYER_SPEED45 : -PLAYER_SPEED45;
        player.speed_y = (player.speed_y > 0) ? PLAYER_SPEED45 : -PLAYER_SPEED45;
    }
}
