#include "enemy.h"
#include "globals.h"
#include "resources.h"
#include "player.h"

static GameObject enemy_object_array[MAX_BATS];
static GameObject_node enemy_node_array[MAX_BATS];
static GameObject_Pool enemy_pool;


void ENEMY_init_system(){
	GAMEOBJECT_pool_init(&enemy_pool, enemy_object_array, enemy_node_array, MAX_BATS);
}

void ENEMY_spawn_bat(s16 x, s16 y, u16* current_vram_tile_index) {
    
    GameObject* new_bat = GAMEOBJECT_pool_alloc(&enemy_pool);
	if (new_bat == NULL) {
		return;
	}

	u16 tiles_for_enemy = GAMEOBJECT_init(new_bat, &spr_bat, x, y, -16, -16, PAL_ENEMY, *current_vram_tile_index);
	*current_vram_tile_index += tiles_for_enemy;

	new_bat->speed_x = FIX16(-1.5);
	new_bat->speed_y = 0;
	new_bat->health = 15;

	SPR_setAnim(new_bat->sprite, 0);

}

void ENEMY_update_all() {
	GameObject_node* current = enemy_pool.active_h;

    while (current != NULL) {
        GameObject* enemy = current->g_object;

        GameObject_node* next_node = current->next;
    }
}