#include <genesis.h>
#include "level.h"
#include "utils.h"

Map* map;
u8 collision_map[SCREEN_METATILES_W + OFFSCREEN_TILES*2][SCREEN_METATILES_H + OFFSCREEN_TILES*2] = {0}; // screen collision map
u16 tilemap_buff[SCREEN_TILES_W * SCREEN_TILES_H];

u8 collision_result;
char text[10];

// Defines the amount of bits needed to map every tile in the screen/room
// 320/16 x 224/16 = 20 x 14 = 280 bits are necessary to map all screen tiles.
// 280/32 bits (long) = 8.75 longs = 9 longs needed (9x4 bytes = 36 bytes per room.)
#define NUMBER_OF_32BIT_BITMAPS 9 

////////////////////////////////////////////////////////////////////////////
// PRIVATE MEMBERS

// Top-Left screen position in map
static u16 screen_x = 0;
static u16 screen_y = 0;

static u32 items_table[NUMBER_OF_ROOMS][NUMBER_OF_32BIT_BITMAPS] = {0}; 

static void LEVEL_remove_tile_from_buffer(u16 x, u16 y, u8 new_index);
static void LEVEL_generate_screen_collision_map(u8 empty, u8 first_wall, u8 last_wall);
static void LEVEL_register_tiles_in_room(u8 room);
static void LEVEL_restore_tiles_in_room(u8 room);

static void LEVEL_scroll_map(s16 x, s16 y);
static void LEVEL_scroll_and_update_collision(s16 offset_x, s16 offset_y);

////////////////////////////////////////////////////////////////////////////
// INIT

u16 LEVEL_init(u16 ind) {
	PAL_setPalette(PAL_MAP, level1_pal.data, DMA);
	// VDP_loadTileSet(&level1_tiles, ind, DMA);
	map = MAP_create(&level1_map, BG_MAP, TILE_ATTR_FULL(PAL_MAP, FALSE, FALSE, FALSE, ind));
	
	LEVEL_scroll_map(0, 0);
	LEVEL_generate_screen_collision_map(IDX_EMPTY, IDX_WALL_FIRST, IDX_WALL_LAST);
	
	// ind += level1_tiles.numTile;

	// start tiles BIT MAP with 1's
	memsetU32((u32*)items_table, 0xFFFFFFFF, NUMBER_OF_ROOMS * NUMBER_OF_32BIT_BITMAPS);

	return ind;
}

////////////////////////////////////////////////////////////////////////////
// LOGIC & UPDATE

void LEVEL_remove_tileXY(s16 x, s16 y, u8 new_index) {
	// use 8x8 position in 16x16 collision vector
	LEVEL_set_tileXY(x, y, new_index);

	// find the position of the first 8x8 tile corresponding to the 16x16 tile
	x = x / METATILE_W * 2;
	y = y / METATILE_W * 2;

	VDP_clearTileMapRect(BG_MAP, x, y, 2, 2);// put zeros in 2x2 tiles

	#ifdef DEBUG
	LEVEL_draw_map();
	#endif
}

/**
 * Verifica limites do mapa e reposiciona GameObject informado.
 * @param GameObject em que será verificada a posição. Sua posição será alterada, caso ultrapasse os limites do mapa.
 * OBS: Definir corretamente MAP_W e MAP_H em <globais.h>.
 */
void LEVEL_check_map_boundaries(GameObject* obj) {
	if (fix16ToInt(obj->x) + screen_x > MAP_W - obj->w) {
		obj->x = FIX16(SCREEN_W - obj->w);
	} else
	if (fix16ToInt(obj->x) + screen_x < 0) {
		obj->x = 0;
	}

	if (fix16ToInt(obj->y) + screen_y > MAP_H - obj->h) {
		obj->y = FIX16(SCREEN_H - obj->h);
	} else
	if (fix16ToInt(obj->y) + screen_y < 0) {
		obj->y = 0;
	}
}

void LEVEL_update_camera(GameObject* obj) {
	if (obj->x > (FIX16(SCREEN_W - obj->w/2))) {
		obj->x = 0;
		LEVEL_scroll_and_update_collision(SCREEN_W, 0);
	} else
	if (obj->x < (FIX16(-obj->w/2))) {
		obj->x = FIX16(SCREEN_W - obj->w);
		LEVEL_scroll_and_update_collision(-SCREEN_W, 0);
	}
	
	if (obj->y > (FIX16(SCREEN_H - obj->h/2))) {
		obj->y = 0;
		LEVEL_scroll_and_update_collision(0, SCREEN_H);
	} else
	if (obj->y < (FIX16(-obj->h/2))) {
		obj->y = FIX16(SCREEN_H - obj->h);
		LEVEL_scroll_and_update_collision(0, -SCREEN_H);
	}
}

// GameObject box must be updated before calling this function
// OBS: this function only checks for objects that are multiple of 16 pixels
u8 LEVEL_check_wall(GameObject* obj) {
	for (u16 x = obj->box.left; x <= obj->box.right ; x += METATILE_W) {
		for (u16 y = obj->box.top; y <= obj->box.bottom; y += METATILE_W) {
			if (LEVEL_wallXY(x, y) != 0)
				return TRUE;
		}
	}
	return FALSE;
}

/**
 * Verifica e resolve colisões com tiles marcados como paredes no Mapa de Colisão (collision_map).
 * A posição do GameObject passado como parâmetro é modificada durante esta função. Para acessar o
 * resultado da colisão, utilize LEVEL_collision_result().
 * @param Endereço do GameObject com o qual se deseja verificar paredes.
 *
 * OBS: Antes da chamada desta função, os campos GameObject::x e GameObject::next_x deve estar preenchidos, 
 * respectivamente, com a posição atual e a próxima posição que o objeto ocupará.
 * \code{.c}
 * // project next position
 * player.next_x = player.x + player.speed_x;
 * player.next_y = player.y + player.speed_y;
 *
 * // check and resolve walls
 * LEVEL_move_and_slide(&player);
 *  
 * // update current position
 * player.x = player.next_x;
 * player.y = player.next_y;
 * \endcode
 */ 
void LEVEL_move_and_slide(GameObject* obj) {
	collision_result = 0;
	GAMEOBJECT_update_boundbox(obj->next_x, obj->y, obj);

	/*
	+---------+  <- right,top
	|         |     				} up to 16 px
	|         |  <- right,top+h/2
	|         |      				} up to 16 px
	+---------+  <- right, bottom
	*/
	if (obj->speed_x > 0) {				// moving right
		if (LEVEL_wallXY(obj->box.right, obj->box.top) || 
	    	LEVEL_wallXY(obj->box.right, obj->box.top + obj->h/2) || 
			LEVEL_wallXY(obj->box.right, obj->box.bottom-1)) {
				obj->next_x = FIX16(obj->box.right/METATILE_W * METATILE_W - obj->w);
				collision_result |= COLLISION_RIGHT;
		}
	}
	/*
	    left, top ->  +---------+
	                  |         |    } up to 16 px
	left, top+h/2 ->  |         |
	                  |         |    } up to 16 px
	 left, bottom ->  +---------+
	*/
	else 
	if (obj->speed_x < 0) {			// moving left
		if (LEVEL_wallXY(obj->box.left, obj->box.top) || 
			LEVEL_wallXY(obj->box.left, obj->box.top + obj->h/2) || 
			LEVEL_wallXY(obj->box.left, obj->box.bottom-1)) {
				obj->next_x = FIX16((obj->box.left/METATILE_W + 1) * METATILE_W);
				collision_result |= COLLISION_LEFT;
		}
	}

	GAMEOBJECT_update_boundbox(obj->next_x, obj->next_y, obj);
	/*
	         left+w/2,
	           top
    left,top    |    right,top
	       |    |    |
 	       +---------+ 
	       |         | 
	       |         | 
	       |         | 
	       +---------+ 
	*/
	if (obj->speed_y < 0) {        // moving up
		if (LEVEL_wallXY(obj->box.left,  obj->box.top) || 
			LEVEL_wallXY(obj->box.left + obj->w/2, obj->box.top) || 
			LEVEL_wallXY(obj->box.right-1, obj->box.top)) {
				obj->next_y = FIX16((obj->box.top/METATILE_W + 1) * METATILE_W);
				collision_result |= COLLISION_TOP;
		}
    }
	/*
	         left+w/2,
	           top
    left,top    |    right,top
	       |    |    |
 	       +---------+ 
	       |         | 
	       |         | 
	       |         | 
	       +---------+ 
	*/
	else
	if (obj->speed_y > 0) {   // moving down
		if (LEVEL_wallXY(obj->box.left,  obj->box.bottom) || 
			LEVEL_wallXY(obj->box.left + obj->w/2, obj->box.bottom) || 
			LEVEL_wallXY(obj->box.right-1, obj->box.bottom)) {
				obj->next_y = FIX16((obj->box.bottom/METATILE_W) * METATILE_W - obj->h);
				collision_result |= COLLISION_BOTTOM;
		}
    }
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE MEMBERS

// COLLISION AND SCREEN MAPS PRIVATE GETTERS + SETTERS

/**
 * Remove tile do MAPA DE COLISAO e do BUFFER DO MAPA.
 * @param x Posição X em tiles 8x8.
 * @param y Posição Y em tiles 8x8.
 */
static void LEVEL_remove_tile_from_buffer(u16 x, u16 y, u8 new_index) {
	// remove metatile from collision vector
	LEVEL_set_tileIDX16(x/2, y/2, new_index);

	// remove a 2x2 tiles square from tilemap buffer
	LEVEL_set_mapbuffIDX8(x,   y,   IDX_EMPTY);
	LEVEL_set_mapbuffIDX8(x+1, y,   IDX_EMPTY);
	LEVEL_set_mapbuffIDX8(x,   y+1, IDX_EMPTY);
	LEVEL_set_mapbuffIDX8(x+1, y+1, IDX_EMPTY);
}

/**
 * Varre a estrutura do MAPA TMX e monta a matriz de colisão do espaço da tela.
 *   Paredes -> idx 1
 * 		Para otimizar os tiles de parede devem estar de forma contigua, na mesma "linha" do tileset
 *   Vazio   -> idx 0
 *   Demais  -> idx original do TMX
 */
static void LEVEL_generate_screen_collision_map(u8 empty, u8 first_wall, u8 last_wall) {
/*
    IMPORTANTE
	Os indices dos TILES sao os mesmos do tileset, na VRAM, exceto que, neste mapa,
	utiliza uma matriz 16x16. Logo, este código considera apenas o tile no canto esquero 
	superior de cada metatile de 4x tiles 8x8.
*/
	#ifdef DEBUG
	kprintf("Generating collision map for screen in Meta Tiles 16x16 + 3 offscreen tiles at each side.");
	#endif

	u8 col_x = 0;
	u8 col_y = 0;
	for (u16 y = 0; y < SCREEN_TILES_H; y += 2) {
		for (u16 x = 0; x < SCREEN_TILES_W; x += 2) {
			u16 tile_index = LEVEL_mapbuffIDX8(x, y);
			
			// text_add_int(tile_index);

			if (tile_index == empty) {											// empty -> 0
				LEVEL_set_tileIDX16(col_x, col_y, 0);
			} else if (tile_index >= first_wall && tile_index <= last_wall) {	// wall  -> 1
				LEVEL_set_tileIDX16(col_x, col_y, 1);
			} else {															// others-> tmx idx
				LEVEL_set_tileIDX16(col_x, col_y, tile_index);
			}
			col_x++;
		}
		// text_print_and_clear();
		col_x = 0;
		col_y++;
	}	
}

/**
 * Register the item in ITEMS TABLE.
 * Scans the ROOM map by 16x16 tiles, until it finds the corresponding bit in the table.
 * @param x The item x position.
 * @param y The item y position.
 */
static void LEVEL_register_tiles_in_room(u8 room) {
	/*
	When an item is colected, its tiles are removed from MAP and the collision_map
	is set as 80 (item collected mark).
	Then, when exiting a room, the code below search for 8 (IDX_ITEM) or 80 (IDX_ITEM_DONE) in the collision_map,
	scanning through every item in the room (from left to right, top to bottom) and building a BIT MAP (64 bits total) 
	of every item status in the room (1 = item, 0 = collected item).
	*/
	u32 mask = 0x80000000; // 2147483648
	u8 offset = 0;
	u16 count = 0;

	#ifdef DEBUG
	kprintf("BIT MAP: Registering tiles in room %d", room);	
	#endif
	
	// loop through COLLISION MAP (16x16 metatiles)
	for (u8 tile_y = 0; tile_y < SCREEN_METATILES_H; ++tile_y) {
		for (u8 tile_x = 0; tile_x < SCREEN_METATILES_W; ++tile_x) {
			
			// does it have any item here?
			u8 map_index = LEVEL_tileIDX16(tile_x, tile_y);
			++count;
			
			if (map_index == 0) { 
				items_table[room][offset] &= ~mask;	// tile is empty -> clear flag
			} else {
				items_table[room][offset] |= mask;	// tile is not empty -> set flag
			}

			// if we already shifted all bits for the first byte, prepare for the second
			if (mask == 1) { 
				mask = 0x80000000;
				++offset;
				if (offset > 9) 
					kprintf("ERROR: too many non tiles for the tiles_table in room %d", room);
			} else {
				mask = mask >> 1;
			}
		}
	}
	#ifdef DEBUG
	kprintf("Total entries: %d", count);
	for (u8 i = 0; i < 9; i++)
		print_bits(items_table[room][i]);
	#endif
}

static void LEVEL_restore_tiles_in_room(u8 room) {
	#ifdef DEBUG
	kprintf("Entering room %d", room);
	for (u8 i = 0; i < 9; i++)
		print_bits(items_table[room][i]);
	#endif

	/*
	When the player enters a room, the screen is scrolled and the SGDK retores all item tiles in the room.
	Then, the code below search for 8 (IDX_ITEM) in the SGDK map,
	scanning through every item in the room (from left to right, top to bottom). 
	When a item tile is found (IDX_ITEM), its checked against the items BIT MAP (64 bits in total) to
	find if the item is there or is collected.
	*/
	u32 mask = 0x80000000; // 2147483648
	u8 offset = 0;
	// u8 count = 0;

	#ifdef DEBUG
	kprintf("Restoring tiles in room %d", room);
	#endif
	
	// loop through TILED MAP (8x8 tiles)
	for (u8 tile_y = 0; tile_y < SCREEN_TILES_H; tile_y += 2) {
		for (u8 tile_x = 0; tile_x < SCREEN_TILES_W; tile_x += 2) {
			
			if (LEVEL_mapbuffIDX8(tile_x, tile_y) != 10) { // non-empty tile in SGDK map

				// Is BIT MAP 0 (empty)? Remove item from map and collision map
				if (!(items_table[room][offset] & mask)) {
					#ifdef DEBUG
					kprintf("Tile removed %d, %d", tile_x/2, tile_y/2);
					#endif
					LEVEL_remove_tile_from_buffer(tile_x, tile_y, 0);
				}			
			}
			// if we already shifted all bits for the first byte, prepare for the second
			if (mask == 1) { 
				mask = 0x80000000;
				++offset;
				if (offset > 9) 
					kprintf("ERROR: too many non tiles for the tiles_table in room %d", room);
			} else {
				mask = mask >> 1;
			}
		}
	}
	#ifdef DEBUG
	// kprintf("Room restored by tiles BIT MAP: %d", count);
	// for (u8 i = 0; i < 9; i++)
	// 	print_bits(items_table[room][i]);
	LEVEL_print_tilemap_buff();
	#endif
}

static void LEVEL_scroll_map(s16 x, s16 y) {
	MAP_getTilemapRect(map, x/16, y/16, SCREEN_TILES_W/2, SCREEN_TILES_H/2, FALSE, tilemap_buff);
	
	// LEVEL_print_tilemap_buff();
		
	VDP_setTileMapDataRect(VDP_BG_A, tilemap_buff, 0, 0, SCREEN_TILES_W, SCREEN_TILES_H, SCREEN_TILES_W, DMA_QUEUE);
}

static void LEVEL_scroll_and_update_collision(s16 offset_x, s16 offset_y) {
	// >> COLLISION MAP (16x16)
	// << ROOM BIT MAP (16x16)
	LEVEL_register_tiles_in_room(screen_y/SCREEN_H * ROOMS_PER_ROW + screen_x/SCREEN_W);
	
	// move to next room and generate collision map
	screen_x += offset_x;
	screen_y += offset_y;
	
	#ifdef DEBUG
	kprintf("Obtaing map region 40x28 from SGDK Map for room %d", screen_y/SCREEN_H * NUMBER_OF_ROOM_ROWS + screen_x/SCREEN_W);
	#endif
	// >> SGDP COMPRESSED MAP
	// << MAP RECT (8x8)
	MAP_getTilemapRect(map, screen_x/16, screen_y/16, SCREEN_TILES_W/2, SCREEN_TILES_H/2, FALSE, tilemap_buff);
	
	// >> MAP RECT (8x8) + ROOM BIT MAP (16x16)
	// << MAP RECT (8x8)
	LEVEL_restore_tiles_in_room(screen_y/SCREEN_H * ROOMS_PER_ROW + screen_x/SCREEN_W);

	// >> SGDK COMPRESSED MAP
	// << COLLISION MAP (16x16)
	LEVEL_generate_screen_collision_map(IDX_EMPTY, IDX_WALL_FIRST, IDX_WALL_LAST);	

	#ifdef DEBUG
	kprintf("Setting VRAM with obtained map region, 40x28 tiles.");
	#endif
	// >> MAP RECT (8x8)
	// << VDP
	VDP_setTileMapDataRect(VDP_BG_A, tilemap_buff, 0, 0, SCREEN_TILES_W, SCREEN_TILES_H, SCREEN_TILES_W, DMA_QUEUE);

	#ifdef DEBUG
	LEVEL_draw_map();
	#endif
}

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

void LEVEL_print_tilemap_buff() {
	kprintf("Screen Tilemap Buffer");
	// u16* pbuff = tilemap_buff;
	u8 count = SCREEN_TILES_W/2;
	for (u16 x = 0; x < SCREEN_TILES_W*SCREEN_TILES_H; x += 2) {
		u16 index = (tilemap_buff[x] - map->baseTile) & TILE_INDEX_MASK;
		text_add_int(index);

		// text_add_int((tilemap_buff[x]) & TILE_INDEX_MASK);
		// pbuff++;
	
		--count;
		if (!count) {
				text_print_and_clear();
				count = SCREEN_TILES_W/2;
				x += SCREEN_TILES_W;
		}
	}
}
