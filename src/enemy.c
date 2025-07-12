#include "enemy.h"
#include "globals.h"
#include "resources.h"
#include "player.h"
#include "level_data.h"

static GameObject enemy_object_array[MAX_ENEMIES];
static GameObject_node enemy_node_array[MAX_ENEMIES];
static GameObject_Pool enemy_pool;

#define MAX_ACTIVE_FORMATIONS 4
static Formation formation_storage[MAX_ACTIVE_FORMATIONS];
static u8 active_formations_count = 0;


void ENEMY_init_system(){
	GAMEOBJECT_pool_init(&enemy_pool, enemy_object_array, enemy_node_array, MAX_ENEMIES);
    active_formations_count = 0;
}

GameObject* ENEMY_spawn_bat(s16 x, s16 y, AIBehaviorType behavior, u16* current_vram_tile_index) {

    // Use KLog_U2 para dois inteiros sem sinal
    kprintf("BAT - Position X: %d, Position Y: %d", x, y);

    if (enemy_pool.free_h == NULL) {
        KLog("ERRO CRITICO: A pool de inimigos esta vazia ou nao foi inicializada!");
        return NULL;
    }

    // Tenta alocar um inimigo da pool
    GameObject* new_bat = GAMEOBJECT_pool_alloc(&enemy_pool);
    if (new_bat == NULL) {
        KLog("AVISO: Nao foi possivel alocar inimigo da pool (provavelmente cheia).");
        return NULL;
    }

    if (spr_bat.maxNumTile == 0) {
        KLog("ERRO CRITICO: O recurso 'spr_bat' parece invalido ou nao tem tiles!");
        GAMEOBJECT_pool_free(&enemy_pool, new_bat);
        return NULL;
    }

 
    u16 tiles_for_enemy = GAMEOBJECT_init(new_bat, &spr_bat, x, y, -16, -16, PAL_ENEMY, *current_vram_tile_index);
    *current_vram_tile_index += tiles_for_enemy;

    new_bat->speed_x = 0;
    new_bat->speed_y = FIX16(1.5);
    new_bat->health = 15;
    new_bat->behavior = behavior;
    new_bat->type = ENEMY_TYPE_BAT;
    new_bat->ai_state = 0;

    SPR_setAnim(new_bat->sprite, 0);
    SPR_setVisibility(new_bat->sprite, VISIBLE);

    return new_bat;
}

GameObject *ENEMY_spawn_crow(s16 x, s16 y, AIBehaviorType behavior, u16* current_vram_tile_index) {
	if (enemy_pool.free_h == NULL) {
        KLog("ERRO CRITICO: A pool de inimigos esta vazia ou nao foi inicializada!");
        return NULL;
    }

    GameObject* new_crow = GAMEOBJECT_pool_alloc(&enemy_pool);
    if (new_crow == NULL) {
        KLog("AVISO: Nao foi possivel alocar inimigo da pool (provavelmente cheia).");
        return NULL;
    }

    if (spr_crow.maxNumTile == 0) {
        KLog("ERRO CRITICO: O recurso 'spr_crow' parece invalido ou nao tem tiles!");
        GAMEOBJECT_pool_free(&enemy_pool, new_crow);
        return NULL;
    }

    u16 tiles_for_enemy = GAMEOBJECT_init(new_crow, &spr_crow, x, y, -16, -16, PAL_ENEMY, *current_vram_tile_index);
    *current_vram_tile_index += tiles_for_enemy;

    new_crow->speed_x = FIX16(-2.0);
    new_crow->speed_y = 0;
    new_crow->health = 10;
    new_crow->behavior = behavior;
    new_crow->type = ENEMY_TYPE_CROW;
    new_crow->ai_state = 0;

    SPR_setAnim(new_crow->sprite, 0);
    SPR_setVisibility(new_crow->sprite, VISIBLE);

    return new_crow;
}

GameObject *ENEMY_spawn_minieye(s16 x, s16 y, AIBehaviorType behavior, u16 *current_vram_tile_index){
	if (enemy_pool.free_h == NULL) {
        KLog("ERRO CRITICO: A pool de inimigos esta vazia ou nao foi inicializada!");
        return NULL;
    }

    GameObject* new_minieye = GAMEOBJECT_pool_alloc(&enemy_pool);
    if (new_minieye == NULL) {
        KLog("AVISO: Nao foi possivel alocar inimigo da pool (provavelmente cheia).");
        return NULL;
    }

    if (spr_eye.maxNumTile == 0) {
        KLog("ERRO CRITICO: O recurso 'spr_eye' parece invalido ou nao tem tiles!");
        GAMEOBJECT_pool_free(&enemy_pool, new_minieye);
        return NULL;
    }

    u16 tiles_for_enemy = GAMEOBJECT_init(new_minieye, &spr_eye, x, y, -16, -16, PAL_ENEMY, *current_vram_tile_index);
    *current_vram_tile_index += tiles_for_enemy;

    new_minieye->speed_x = FIX16(-1.5);
    new_minieye->speed_y = 0;
    new_minieye->health = 30;
    new_minieye->behavior = behavior;
    new_minieye->type = ENEMY_TYPE_MINIEYE;
    new_minieye->ai_state = 0;

    SPR_setAnim(new_minieye->sprite, 0);
    SPR_setVisibility(new_minieye->sprite, VISIBLE);

    return new_minieye;
}

GameObject *ENEMY_spawn(EnemyType type, s16 tiled_x, s16 tiled_y, AIBehaviorType behavior, u16 *vram_index)
{

	s16 x = tiled_x % VIRTUAL_SCREEN_W;
    s16 y = tiled_y - 224;


    switch (type) {
        case ENEMY_TYPE_BAT:
            return ENEMY_spawn_bat(x, y, behavior, vram_index);
        case ENEMY_TYPE_CROW:
            return ENEMY_spawn_crow(x, y, behavior, vram_index);
        case ENEMY_TYPE_MINIEYE:
            return ENEMY_spawn_minieye(x, y, behavior, vram_index);
        case ENEMY_TYPE_MINIBOSS:
            break;
        case ENEMY_TYPE_BOSS:
            break;
        default:
            break;
    }

    return NULL;
}

u8 ENEMY_get_active_count(){
	u8 count = 0;
    GameObject_node* current = enemy_pool.active_h;
    while(current != NULL) {
        GameObject* enemy = current->g_object;
        if (enemy->health > 0) {
            count++;
        }
        current = current->next;
    }

    return count;
}

// Em src/enemy.c
void ENEMY_update_all() {
    // Itera sobre as FORMAÇÕES ativas
    for (u8 f = 0; f < active_formations_count; f++) {
        Formation* form = &formation_storage[f];
        u8 live_members = 0;

            // Calcula a hitbox geral da formação
        for (u8 i = 0; i < form->num_members; i++) {
            GameObject* enemy = form->members[i];
            if (enemy->health > 0) {
                live_members++;

                switch(enemy->behavior) {
                    case AI_BEHAVIOR_VERTICAL_BOUNCE:
                        // Lógica para o morcego que quica na tela
                        if (enemy->ai_state == 0) { // Estado 0: "Entrando na tela"
                            enemy->y += enemy->speed_y; // Move para baixo
                            // Se alcançou seu ponto de patrulha Y...
                            if (enemy->y >= enemy->target_y) {
                                enemy->y = enemy->target_y; // Trava na posição
                                enemy->ai_state = 1;      // ...muda para o estado "Patrulhando"
                            }
                        } else { // ai_state == 1: "Patrulhando"
                            enemy->y += enemy->speed_y;
                            s16 y_px = F16_toInt(enemy->y);

                            // Bate e volta nas bordas da tela
                            if ((y_px <= 0 && enemy->speed_y < 0) || (y_px >= (SCREEN_H - enemy->h) && enemy->speed_y > 0)) {
                                enemy->speed_y = -enemy->speed_y; // Inverte a velocidade
                            }
                        }
                    break;

                    case AI_BEHAVIOR_MIRROR_BOUNCE:
                        // TODO: Implementar a lógica para os morcegos que se movem em espelho
                        // Por enquanto, eles ficarão parados após entrarem.
                        if (enemy->ai_state == 0) {
                             enemy->y += FIX16(1.0);
                             if(enemy->y >= enemy->target_y) enemy->ai_state = 1;
                        }
                    break;
                        
                    case AI_BEHAVIOR_FLY_STRAIGHT:
                        // Lógica para o corvo: apenas se move para a esquerda
                        enemy->x += enemy->speed_x;
                        if (enemy->x <= VIRTUAL_SCREEN_W) {
                            enemy->ai_state = 1; // Muda para o estado "Patrulhando"
                        }
                        if(enemy->ai_state > 0) { // Necessário porque a hitbox não acompanha o sprite depois do warp forçado
                            GAMEOBJECT_wrap_screen(enemy);
                            GAMEOBJECT_update_boundbox(enemy->x, enemy->y, enemy);
                        }
                    break;
                }

                GAMEOBJECT_update_boundbox(enemy->x, enemy->y, enemy);

                // C. Checa colisão com os tiros do jogador
                GameObject_node* bullet_node = PLAYER_get_active_bullets_list();
                while (bullet_node != NULL) {
                    GameObject* bullet = bullet_node->g_object;

                    if (GAMEOBJECT_check_collision(enemy, bullet)) {
                        enemy->health -= PLAYER_bullet_dmg;
                        // O inimigo "mata" o tiro ao zerar a vida dele.
                        // O sistema do jogador em PLAYER_update_bullets se encarregará de liberá-lo.
                        bullet->health = 0;
                        break; // Tiro foi consumido, sai do loop de balas
                    }
                    bullet_node = bullet_node->next;
                }

                // D. Checa se o inimigo morreu e atualiza o sprite
                if (enemy->health <= 0) {
                    SPR_setVisibility(enemy->sprite, HIDDEN);
                    // TODO: Drop de itens aqui
                } else {
                    SPR_setPosition(enemy->sprite, enemy->box.left + enemy->w_offset, enemy->box.top + enemy->h_offset);
                }
            }
        }

    }    // TODO: Lógica para remover a formação quando todos os membros morrere
}
// Em src/enemy.c

// Esta função recebe um array de ponteiros para GameObjects e os agrupa.
void FORMATION_create(GameObject* members[], u16 num_members, const WaveObjectData* wave_data) {
    if (active_formations_count >= MAX_ACTIVE_FORMATIONS) return;

    Formation* new_formation = &formation_storage[active_formations_count];
    new_formation->num_members = num_members;

    // Copia os ponteiros dos membros para a struct da formação
    for (u16 i = 0; i < num_members; i++) {
        new_formation->members[i] = members[i];
    }
    
    active_formations_count++;
}