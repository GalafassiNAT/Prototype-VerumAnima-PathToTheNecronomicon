#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "engine/gameobject.h"
#include "types.h"

#define MAX_BATS 5

extern GameObject enemies_bat[MAX_BATS];
extern u8 active_bats_count;

void ENEMY_init_system();
void ENEMY_spawn_bat(s16 x, s16 y, u16* current_vram_tile_index);
void ENEMY_update_all();

#endif