#ifndef _ENEMY_H_
#define _ENEMY_H_
#pragma once

#include "engine/gameobject.h"
#include "types.h"
#include "level_data.h"


#define MAX_ENEMIES 15
#define MAX_ENEMIES_PER_FORMATION 8

typedef struct {
	AIBehaviorType behavior;
	u8 ai_state;
	f16 speed_y;
	
	u8 num_members;
	GameObject* members[MAX_ENEMIES_PER_FORMATION];

	BoundBox box;
} Formation;



void ENEMY_init_system();
GameObject* ENEMY_spawn_bat(s16 x, s16 y, u16 *current_vram_tile_index);
GameObject* ENEMY_spawn_crow(s16 x, s16 y, u16 *current_vram_tile_index);
GameObject* ENEMY_spawn_minieye(s16 x, s16 y, u16 *current_vram_tile_index);
GameObject* ENEMY_spawn(EnemyType type, s16 x, s16 y, u16* vram_index);
u8 ENEMY_get_active_count();
void ENEMY_update_all();

#endif