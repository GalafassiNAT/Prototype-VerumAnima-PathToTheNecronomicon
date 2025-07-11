#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <genesis.h>
#include "engine/gameobject.h"
#include "resources.h"

#define ANIM_VERTICAL    0
#define ANIM_HORIZONTAL  1

#define MAX_PLAYER_BULLETS 8

extern u8 PLAYER_bullet_dmg;


// #define PLAYER_SPEED45 FIX16(0.707)

extern GameObject player;

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 PLAYER_init(u16 ind);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC
/**
 * @brief Retorna um ponteiro para o início da lista de tiros ativos do jogador.
 * * @return GameObject_node* O primeiro nó da lista, ou NULL se não houver tiros ativos.
 */
GameObject_node* PLAYER_get_active_bullets_list();
void PLAYER_update();
void PLAYER_shoot();
void PLAYER_update_bullets();

#endif // _PLAYER_H_