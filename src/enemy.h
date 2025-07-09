#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "engine/gameobject.h"
#include "types.h"
#include "level_data.h"

#define MAX_BATS 5

extern GameObject enemies_bat[MAX_BATS];
extern u8 active_bats_count;

void ENEMY_init_system();
u16 ENEMY_spawn_bat(s16 x, s16 y, u16 current_vram_tile_index);
void ENEMY_spawn(EnemyType type, s16 x, s16 y, u16* vram_index);
u8 ENEMY_get_active_count();
void ENEMY_update_all();

#endif