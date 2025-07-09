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

u16 ENEMY_spawn_bat(s16 x, s16 y, u16 current_vram_tile_index) {

    if (enemy_pool.free_h == NULL) {
        KLog("ERRO CRITICO: A pool de inimigos esta vazia ou nao foi inicializada!");
        return 0;
    }

    // Tenta alocar um inimigo da pool
    GameObject* new_bat = GAMEOBJECT_pool_alloc(&enemy_pool);
    if (new_bat == NULL) {
        KLog("AVISO: Nao foi possivel alocar inimigo da pool (provavelmente cheia).");
        return 0;
    }

    if (&spr_bat == NULL || spr_bat.maxNumTile == 0) {
        KLog("ERRO CRITICO: O recurso 'spr_bat' parece invalido ou nao tem tiles!");
        GAMEOBJECT_pool_free(&enemy_pool, new_bat);
        return 0;
    }

 
    u16 tiles_for_enemy = GAMEOBJECT_init(new_bat, &spr_bat, x, y, -16, -16, PAL_ENEMY, current_vram_tile_index);
    

    new_bat->speed_x = FIX16(-1.5);
    new_bat->speed_y = 0;
    new_bat->health = 15;

    SPR_setAnim(new_bat->sprite, 0);
    SPR_setVisibility(new_bat->sprite, VISIBLE);

    return tiles_for_enemy;
}


void ENEMY_spawn(EnemyType type, s16 tiled_x, s16 tiled_y, u16* vram_index) {

    s16 x = tiled_x % VIRTUAL_SCREEN_W;

    switch (type) {
        case ENEMY_TYPE_BAT:
            *vram_index += ENEMY_spawn_bat(x, tiled_y, *vram_index);
            break;
        case ENEMY_TYPE_CROW:
            break;
        case ENEMY_TYPE_MINIEYE:
            break;
        case ENEMY_TYPE_MINIBOSS:
            break;
        case ENEMY_TYPE_BOSS:
            break;
        default:
            break;
    }
}



void ENEMY_update_all() {
	GameObject_node* current = enemy_pool.active_h;

    while (current != NULL) {
        GameObject* enemy = current->g_object;

        GameObject_node* next_node = current->next;
		// TODO
    }
}