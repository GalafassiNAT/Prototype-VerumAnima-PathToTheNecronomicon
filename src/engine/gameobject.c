#include "gameobject.h"
#include "utils.h"
#include "../globals.h"

////////////////////////////////////////////////////////////////////////////
// INIT

u16 GAMEOBJECT_init(GameObject* const obj, const SpriteDefinition* const sprite, s16 x, s16 y, s8 w_offset, s8 h_offset, u8 pal, u16 ind) {
	obj->x = FIX16(x);
	obj->y = FIX16(y);
	obj->next_x = obj->x;
	obj->next_y = obj->y;
	obj->speed_x = 0;
	obj->speed_y = 0;
	obj->anim = 0;
	PAL_setPalette(pal, sprite->palette->data, DMA);
	
	obj->sprite = SPR_addSprite(sprite, x, y, TILE_ATTR_FULL(pal, FALSE, FALSE, 0, ind));

	obj->w = obj->sprite->definition->w + w_offset;
	obj->h = obj->sprite->definition->h + h_offset;
	obj->w_offset = w_offset/2; // half offset for each side
	obj->h_offset = h_offset/2;
	
	return obj->sprite->definition->maxNumTile;
}


/////////////////////////////////////////////////////////////////////////////
// POOL MANAGEMENT
void GAMEOBJECT_pool_init(GameObject_Pool* pool, GameObject* object_array, GameObject_node* node_array, u16 size) {
	pool->active_h = NULL;
	pool->free_h = &node_array[0];

	for (u16 i = 0; i < size; i++) {
		node_array[i].g_object = &object_array[i];

		if (i < size - 1) {
			node_array[i].next = &node_array[i + 1];
		} else {
			node_array[i].next = NULL;
		}
	}
}

GameObject* GAMEOBJECT_pool_alloc(GameObject_Pool* pool) {
	GameObject_node* node_to_alloc = pool->free_h;

	if (node_to_alloc == NULL) return NULL;

	pool->free_h = node_to_alloc->next;

	node_to_alloc->next = pool->active_h;
	pool->active_h = node_to_alloc;

	return node_to_alloc->g_object;
}

void GAMEOBJECT_pool_free(GameObject_Pool* pool, GameObject* obj_to_free) {
	GameObject_node* current = pool->active_h;
	GameObject_node* prev = NULL;
}

////////////////////////////////////////////////////////////////////////////
// UPDATE

/**
 * Updates GameObject's bound box (integer values) from positions and size (fix16).
 */
void GAMEOBJECT_update_boundbox(f16 x, f16 y, GameObject* obj) {
	obj->box.left  = fix16ToInt(x);
	obj->box.top   = fix16ToInt(y);
	obj->box.right = fix16ToInt(x) + obj->w;// - 1;
	obj->box.bottom= fix16ToInt(y) + obj->h;// - 1;
}

/**
 *	Prevents object from going outside screen by repositing it at screen bounds.
*/
void GAMEOBJECT_clamp_screen(GameObject* obj) {
	obj->x = clamp(obj->x, 0, FIX16(SCREEN_W - obj->w));
	obj->y = clamp(obj->y, 0, FIX16(SCREEN_H - obj->h));
}

/**
 * Wraps object around screen bounds.
 */
void GAMEOBJECT_wrap_screen(GameObject* obj) {
	WRAP(obj->x, -fix16Div(obj->w, 2), FIX16(SCREEN_W) - obj->w/2)
	WRAP(obj->y, -fix16Div(obj->h, 2), FIX16(SCREEN_H) - obj->h/2)
}

/**
 * Prevents object from going outside screen by reversing the speed sign (x or y)at screen bounds.
 * OBS: The GameObject's Box must be updated before calling this function.
 */
void GAMEOBJECT_bounce_off_screen(GameObject* obj) {
	// bounce off screen bounds
	if (obj->box.left < 0 || obj->box.right > SCREEN_W) {
		obj->speed_x = -obj->speed_x;
	}	

	if (obj->box.top < 0 || obj->box.bottom > SCREEN_H) {
		obj->speed_y = -obj->speed_y;
	}	
}

bool GAMEOBJECT_check_collision(GameObject *obj1, GameObject *obj2)
{	if (
		obj1->box.right > obj2->box.left &&
		obj1->box.left < obj2->box.right &&
		obj1->box.bottom > obj2->box.top &&
		obj1->box.top < obj2->box.bottom
	) {
		return TRUE;
	}
	return FALSE;
}
