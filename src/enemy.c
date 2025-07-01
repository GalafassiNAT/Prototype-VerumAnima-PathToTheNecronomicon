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
    KLog("--- Entrando em ENEMY_spawn_bat ---");

    // Verificação 1: A pool de objetos foi inicializada corretamente?
    if (enemy_pool.free_h == NULL) {
        KLog("ERRO CRITICO: A pool de inimigos esta vazia ou nao foi inicializada!");
        // Se a pool não foi iniciada, não podemos alocar nada.
        return 0;
    }

    // Tenta alocar um inimigo da pool
    GameObject* new_bat = GAMEOBJECT_pool_alloc(&enemy_pool);
    if (new_bat == NULL) {
        KLog("AVISO: Nao foi possivel alocar inimigo da pool (provavelmente cheia).");
        return 0;
    }
    KLog("Pool alloc OK. Objeto alocado da pool.");

    // Verificação 2: O recurso do sprite do morcego (&spr_bat) parece válido?
    if (&spr_bat == NULL || spr_bat.maxNumTile == 0) {
        KLog("ERRO CRITICO: O recurso 'spr_bat' parece invalido ou nao tem tiles!");
        // Se o recurso é inválido, precisamos liberar o objeto que acabamos de alocar.
        GAMEOBJECT_pool_free(&enemy_pool, new_bat);
        return 0;
    }
    KLog_U1("Recurso 'spr_bat' OK. maxNumTile =", spr_bat.maxNumTile);

    // Verificação 3: O índice da VRAM que recebemos parece razoável?
    KLog_U1("Tentando inicializar o sprite do inimigo no indice de VRAM =", current_vram_tile_index);

    // --- Esta é a chamada mais provável de falhar ---
    u16 tiles_for_enemy = GAMEOBJECT_init(new_bat, &spr_bat, x, y, -16, -16, PAL_ENEMY, current_vram_tile_index);
    // -------------------------------------------------
    
    KLog("GAMEOBJECT_init para o morcego retornou OK.");

    // Configura as propriedades do morcego
    new_bat->speed_x = FIX16(-1.5);
    new_bat->speed_y = 0;
    new_bat->health = 15;
    KLog("Propriedades do morcego definidas.");

    SPR_setAnim(new_bat->sprite, 0);
    SPR_setVisibility(new_bat->sprite, VISIBLE);
    KLog("Animacao e visibilidade do morcego OK.");

    KLog("--- Saindo de ENEMY_spawn_bat com sucesso! ---");
    return tiles_for_enemy;
}

void ENEMY_update_all() {
	GameObject_node* current = enemy_pool.active_h;

    while (current != NULL) {
        GameObject* enemy = current->g_object;

        GameObject_node* next_node = current->next;
		// TODO
    }
}