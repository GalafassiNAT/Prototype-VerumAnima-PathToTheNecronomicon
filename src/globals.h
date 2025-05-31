#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <genesis.h>
#include <sprite_eng.h>

// #define DEBUG

/*** YOU MUST MODIFY *************************************************
    
    The following configs are specific for this example project 
    and must be taylored for your project needs.
    
*** YOU MUST MODIFY **************************************************/

// MAP CONFIG ///////////////////////////////////////////

#define MAP_METATILES_W 60      // width of map in 16x16 tiles
#define MAP_METATILES_H 42      // height og map in 16x16 tiles

#define HUD_TILES 1             // hud height in tiles

// To check what are the tile indexes in your map, just
// put all tiles you need to find the indexes in
// the first row of the map and use this function to print 
// the indexes in the MESSAGES panel of GensKMod
// void LEVEL_print_tilemap_buff();

#define IDX_EMPTY 	   10       
#define IDX_WALL_FIRST 0
#define IDX_WALL_LAST  5
#define IDX_ITEM  	   8

// GENERAL VDP CONFIG ///////////////////////////////////

#define PAL_PLAYER 		PAL0
#define PAL_ENEMY 		PAL1
#define PAL_MAP 		PAL2
#define PAL_BACKGROUND 	PAL3

#define BG_BACKGROUND BG_B
#define BG_MAP        BG_A

#define NUMBER_OF_JOYPADS 2

// GENERAL PLAYER CONFIG ////////////////////////////////

#define PLAYER_SPEED   FIX16(2)
#define PLAYER_SPEED45 FIX16(0.707 * 2)

#define PLAYER_MAX_HEALTH 10


/*** ATENTION: DO NOT YOU MUST MODIFYT *********************************
 
    These configs are project independent and needed for the game engine to work properly.
    It's advisable to not modify theses configs, except if you know what you're doing.

**** ATENTION: DO NOT YOU MUST MODIFYT *********************************/

// GENERAL MAP CONFIG ///////////////////////////////////

#define SCREEN_W 320
#define SCREEN_H 224

#define MAP_W MAP_METATILES_W * METATILE_W
#define MAP_H MAP_METATILES_H * METATILE_W

// maximum number of screens (rooms) in a map
#define NUMBER_OF_ROOMS MAP_W/SCREEN_W * MAP_H/SCREEN_H
#define ROOMS_PER_ROW   MAP_H/SCREEN_H

// GENERAL SCREEN CONFIG ////////////////////////////////

#define SCREEN_W_F16 FIX16(320)
#define SCREEN_H_F16 FIX16(224)

#define SCREEN_TILES_W SCREEN_W/8
#define SCREEN_TILES_H SCREEN_H/8

// Map is made of 16x16 metatiles
#define METATILE_W 16
#define SCREEN_METATILES_W SCREEN_W/METATILE_W
#define SCREEN_METATILES_H SCREEN_H/METATILE_W

#endif