#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <genesis.h>
#include "sprite_eng.h"
#include "globals.h"
#include "level_data.h"

typedef struct {
	s16 left;
	s16 right;
	s16 top;
	s16 bottom;
} BoundBox;

typedef struct {
	Sprite* sprite;
	f16 x;
	f16 y;
	u8 anim;

	f16 next_x;
	f16 next_y;
	f16 speed_x;
	f16 speed_y;
	u8 w;
	u8 h;
	BoundBox box;
	s8 w_offset;
	s8 h_offset;

	s8 health;
	u8 mana;
	EnemyType type;

	u8 ai_state;

	u16 ai_timer;
	f16 target_y;
	f16 offset_x_from_anchor;
	f16 offset_y_from_anchor;
} GameObject;


typedef struct GameObject_node {
	GameObject* g_object;
	struct GameObject_node* next;
} GameObject_node;

typedef struct {
	GameObject_node* free_h;
	GameObject_node* active_h;
} GameObject_Pool;
////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 GAMEOBJECT_init(GameObject* const obj, const SpriteDefinition* const sprite, s16 x, s16 y, s8 w_offset, s8 h_offset, u8 pal, u16 ind);


////////////////////////////////////////////////////////////////////////////
// POOL MANAGEMENT
/**
 * @brief Intializes a GameObject pool.
 * @param pool Pointer to the GameObject_Pool structure to initialize.
 * @param object_array Pointer to the array of GameObject structures to use in the pool.
 * @param node_array Pointer to the array of GameObject_node structures to use in the pool.
 * @param size The size of the arrays (number of GameObjects in the pool).
 */
void GAMEOBJECT_pool_init(GameObject_Pool* pool, GameObject* object_array, GameObject_node* node_array,
u16 size);

/**
 * @brief Allocates a GameObject from the pool, moving it from the free list to the active list.
 * @param pool Pointer to the GameObject_Pool structure.
 * @return Pointer to the allocated GameObject, or NULL if the pool is empty.
 */
GameObject* GAMEOBJECT_pool_alloc(GameObject_Pool* pool);

/**
 * @brief Frees a GameObject from the pool, moving it from the active list back to the free list.
 * @param pool Pointer to the GameObject_Pool structure.
 * @param obj_to_free Pointer to the GameObject to free.
 * @note The GameObject must have been allocated from the same pool.
 */
void GAMEOBJECT_pool_free(GameObject_Pool* pool, GameObject* obj_to_free);



////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

void GAMEOBJECT_update_boundbox(f16 x, f16 y, GameObject* obj);
void GAMEOBJECT_clamp_screen(GameObject* obj);
void GAMEOBJECT_wrap_screen(GameObject* obj);
void GAMEOBJECT_bounce_off_screen(GameObject* obj);
bool GAMEOBJECT_check_collision(GameObject* obj1, GameObject* obj2);

#endif // _STRUCTS_H_