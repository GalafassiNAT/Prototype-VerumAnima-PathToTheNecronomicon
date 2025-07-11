#include "enemy.h"
#include "globals.h"
#include "resources.h"
#include "player.h"

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

GameObject* ENEMY_spawn_bat(s16 x, s16 y, u16* current_vram_tile_index) {

    // Use KLog_U2 para dois inteiros sem sinal
    KLog_U2("ENEMY_spawn_bat: x=", x, "y=", y);
    // E uma segunda chamada para o ponteiro se necessário
    KLog("current_vram_tile_index pointer address");

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

    if (&spr_bat == NULL || spr_bat.maxNumTile == 0) {
        KLog("ERRO CRITICO: O recurso 'spr_bat' parece invalido ou nao tem tiles!");
        GAMEOBJECT_pool_free(&enemy_pool, new_bat);
        return NULL;
    }

 
    u16 tiles_for_enemy = GAMEOBJECT_init(new_bat, &spr_bat, x, y, -16, -16, PAL_ENEMY, *current_vram_tile_index);
    *current_vram_tile_index += tiles_for_enemy;

    new_bat->speed_x = 0;
    new_bat->speed_y = FIX16(1.5);
    new_bat->health = 15;
    new_bat->type = ENEMY_TYPE_BAT;
    new_bat->ai_state = 0;

    SPR_setAnim(new_bat->sprite, 0);
    SPR_setVisibility(new_bat->sprite, VISIBLE);

    return new_bat;
}

GameObject *ENEMY_spawn_crow(s16 x, s16 y, u16* current_vram_tile_index) {
	if (enemy_pool.free_h == NULL) {
        KLog("ERRO CRITICO: A pool de inimigos esta vazia ou nao foi inicializada!");
        return NULL;
    }

    GameObject* new_crow = GAMEOBJECT_pool_alloc(&enemy_pool);
    if (new_crow == NULL) {
        KLog("AVISO: Nao foi possivel alocar inimigo da pool (provavelmente cheia).");
        return NULL;
    }

    if (&spr_crow == NULL || spr_crow.maxNumTile == 0) {
        KLog("ERRO CRITICO: O recurso 'spr_crow' parece invalido ou nao tem tiles!");
        GAMEOBJECT_pool_free(&enemy_pool, new_crow);
        return NULL;
    }

    u16 tiles_for_enemy = GAMEOBJECT_init(new_crow, &spr_crow, x, y, -16, -16, PAL_ENEMY, *current_vram_tile_index);
    *current_vram_tile_index += tiles_for_enemy;

    new_crow->speed_x = FIX16(-2.0);
    new_crow->speed_y = 0;
    new_crow->health = 10;
    new_crow->type = ENEMY_TYPE_CROW;

    SPR_setAnim(new_crow->sprite, 0);
    SPR_setVisibility(new_crow->sprite, VISIBLE);

    return new_crow;
}

GameObject *ENEMY_spawn_minieye(s16 x, s16 y, u16 *current_vram_tile_index){
	if (enemy_pool.free_h == NULL) {
        KLog("ERRO CRITICO: A pool de inimigos esta vazia ou nao foi inicializada!");
        return NULL;
    }

    GameObject* new_minieye = GAMEOBJECT_pool_alloc(&enemy_pool);
    if (new_minieye == NULL) {
        KLog("AVISO: Nao foi possivel alocar inimigo da pool (provavelmente cheia).");
        return NULL;
    }

    if (&spr_eye == NULL || spr_eye.maxNumTile == 0) {
        KLog("ERRO CRITICO: O recurso 'spr_eye' parece invalido ou nao tem tiles!");
        GAMEOBJECT_pool_free(&enemy_pool, new_minieye);
        return NULL;
    }

    u16 tiles_for_enemy = GAMEOBJECT_init(new_minieye, &spr_eye, x, y, -16, -16, PAL_ENEMY, *current_vram_tile_index);
    *current_vram_tile_index += tiles_for_enemy;

    new_minieye->speed_x = FIX16(-1.5);
    new_minieye->speed_y = 0;
    new_minieye->health = 30;
    new_minieye->type = ENEMY_TYPE_MINIEYE;

    SPR_setAnim(new_minieye->sprite, 0);
    SPR_setVisibility(new_minieye->sprite, VISIBLE);

    return new_minieye;
}

GameObject *ENEMY_spawn(EnemyType type, s16 tiled_x, s16 tiled_y, u16 *vram_index)
{

	s16 x = tiled_x % VIRTUAL_SCREEN_W;

    switch (type) {
        case ENEMY_TYPE_BAT:
            return ENEMY_spawn_bat(x, tiled_y, vram_index);
        case ENEMY_TYPE_CROW:
            return ENEMY_spawn_crow(x, tiled_y, vram_index);
        case ENEMY_TYPE_MINIEYE:
            return ENEMY_spawn_minieye(x, tiled_y, vram_index);
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
// void ENEMY_update_all() {
//     // Itera sobre as FORMAÇÕES ativas
//     for (u8 f = 0; f < active_formations_count; f++) {
//         Formation* form = &formation_storage[f];

//         // --- 1. ATUALIZA A LÓGICA DA FORMAÇÃO ---
//         switch (form->behavior) {
//             case AI_BEHAVIOR_VERTICAL_BOUNCE: {
//                 // (Sua lógica aqui para calcular a hitbox geral da formação e
//                 // inverter a 'form->speed_y' quando ela atinge as bordas está ótima!)
//                 s16 min_y = 300;
//                 s16 max_y = -100;
//                 // ...
//                 for(u8 i = 0; i < form->num_members; i++) {
//                     GameObject* member = form->members[i];
//                     if(member->health > 0) {
//                         s16 top_y = F16_toInt(member->y);
//                         s16 bottom_y = top_y + member->h;
//                         if (top_y < min_y) min_y = top_y;
//                         if (bottom_y > max_y) max_y = bottom_y;
//                     }
//                 }
//                 form->box.top = min_y;
//                 form->box.bottom = max_y;

//                 if (form->ai_state == 0) {
//                     if (form->box.top >= 0) form->ai_state = 1; // Começa a patrulhar
//                 } else {
//                     if (form->box.top <= 0 && form->speed_y < 0) {
//                         form->speed_y = -form->speed_y;
//                     } else if (form->box.bottom >= SCREEN_H && form->speed_y > 0) {
//                         form->speed_y = -form->speed_y;
//                     }
//                 }
//                 break;
//             }
//             // ... outros comportamentos
//         }

//         // --- 2. ATUALIZA CADA MEMBRO DA FORMAÇÃO ---
//         for (u8 i = 0; i < form->num_members; i++) {
//             GameObject* enemy = form->members[i];

//             if (enemy->health > 0) {
//                 // A. Aplica o movimento da formação ao inimigo
//                 if (form->behavior == AI_BEHAVIOR_VERTICAL_BOUNCE) {
//                     enemy->y += form->speed_y;
//                 } else if (form->behavior == AI_BEHAVIOR_FLY_STRAIGHT) {
//                     enemy->x += enemy->speed_x;
//                 }

//                 // B. CORREÇÃO CRÍTICA: Atualiza a hitbox com a nova posição
//                 GAMEOBJECT_update_boundbox(enemy->x, enemy->y, enemy);

//                 // C. Checa colisão com os tiros do jogador
//                 GameObject_node* bullet_node = PLAYER_get_active_bullets_list();
//                 while (bullet_node != NULL) {
//                     GameObject* bullet = bullet_node->g_object;

//                     if (GAMEOBJECT_check_collision(enemy, bullet)) {
//                         enemy->health -= PLAYER_bullet_dmg;
//                         // O inimigo "mata" o tiro ao zerar a vida dele.
//                         // O sistema do jogador em PLAYER_update_bullets se encarregará de liberá-lo.
//                         bullet->health = 0;
//                         break; // Tiro foi consumido, sai do loop de balas
//                     }
//                     bullet_node = bullet_node->next;
//                 }

//                 // D. Checa se o inimigo morreu e atualiza o sprite
//                 if (enemy->health <= 0) {
//                     SPR_setVisibility(enemy->sprite, HIDDEN);
//                     // TODO: Drop de itens aqui
//                 } else {
//                     // Esta chamada agora usará a hitbox correta e moverá o sprite!
//                     SPR_setPosition(enemy->sprite, enemy->box.left + enemy->w_offset, enemy->box.top + enemy->h_offset);
//                 }
//             }
//         }
//         // TODO: Lógica para remover a formação quando todos os membros morrerem
//     }
// }


// Em src/enemy.c
void ENEMY_update_all() {
    // Pega o primeiro nó da lista de inimigos ativos da nossa pool
    GameObject_node* current_node = enemy_pool.active_h;
    u8 update_counter = 0; // Nosso contador para este quadro
    // KLog("Iniciando ENEMY_update_all..."); // Log para ver se a função é chamada

    // Percorre a lista encadeada de TODOS os GameObjects ativos
    while (current_node != NULL) {
        update_counter++;
        GameObject* enemy = current_node->g_object;

        // --- TESTE: MOVIMENTO INCONDICIONAL PARA BAIXO ---
        
        // Ignoramos formações, comportamentos e estados.
        // Apenas adicionamos uma velocidade vertical constante.
        enemy->y += FIX16(1.0); // Move 1 pixel para baixo por quadro

        // Atualizamos a hitbox com a nova posição lógica
        GAMEOBJECT_update_boundbox(enemy->x, enemy->y, enemy);

        // E atualizamos a posição do sprite na tela
        SPR_setPosition(enemy->sprite, enemy->box.left + enemy->w_offset, enemy->box.top + enemy->h_offset);
        
        // Avançamos para o próximo inimigo na lista
        current_node = current_node->next;
    }
    KLog_U1("Inimigos realmente atualizados neste quadro:", update_counter);
}


void FORMATION_create(const WaveDef* wave_def, u16* idx) {
    if (active_formations_count >= MAX_ACTIVE_FORMATIONS) {
        return;
    }

    Formation* new_formation = &formation_storage[active_formations_count];
    new_formation->num_members = 0;

    u16 enemies_to_spawn = wave_def->num_enemies;
    if (enemies_to_spawn > MAX_ENEMIES_PER_FORMATION) {
        enemies_to_spawn = MAX_ENEMIES_PER_FORMATION;
    }

    for (u16 i = 0; i < enemies_to_spawn; i++) {
        const WaveObjectData* enemy_data = &wave_def->enemies[i];

        if (new_formation->members[i]->y > FIX16(-50)) {
            new_formation->members[i]->y = FIX16(-50);
        }

        GameObject* new_enemy = ENEMY_spawn(enemy_data->type, enemy_data->x, enemy_data->y, idx);

        if (new_enemy != NULL) {
            new_formation->members[new_formation->num_members++] = new_enemy;
        }
    }

    if (new_formation->num_members > 0) {
        new_formation->behavior = (AIBehaviorType)wave_def->enemies[0].behavior;
        new_formation->ai_state = 0;
        new_formation->speed_y = FIX16(1.6);
        active_formations_count++;
    }
}