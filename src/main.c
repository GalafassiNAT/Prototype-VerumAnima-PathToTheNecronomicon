/**
 * Para compilar & rodar:
 * ---------------------
 *   CTRL + SHIFT + B   (gera out/rom.bin )			<< compilar
 *   [F1], Run Task, Run Gens						<< executar
 *
 * Para compilar & rodar (com extensão Genesis Code):
 * -------------------------------------------------
 *  OBS: instalar extensao Genesis Code e configurar "Gens Path"
 *
 *  Executar: $ cmd									<< pelo terminal do VSCode (extensão não funciona em PowerShell)
 *  F1 -> Genesis Code Compile Project				<< compilar
 *  F1 -> Genesis Code Compiler & Run Project		<< compilar & executar
 * 
 * LINUX
 * ==================================================================
 * Considerando que o caminho seja ~/sgdk200, para fazer build:
 * 
 * $ make GDK=~/sgdk200 -f ~/sgdk200/makefile_wine.gen
 * 
 * TILED -> customizar grade em preferências
 * GENS -> habilitar recursos avançados de debug
 *
 * PROBLEMAS
 * ------------------------------
 * > Mudar imagem de tileset fez com que o Rescomp nao encotrasse mais os tiles (problema no TMX/TSX).
 *   SOLUCAO: criar novo arquivo tsx com a mesma PNG dos tiles e usar a opção "replace tilset" do Tiled.
 * > Mensagens com KLog não aparecem no emulador
 *   SOLUCAO: Option -> Debug -> Active Development Features [x]
 * 
 * IDEIAS
 * ------------------------------
 * Bola que quebra blocos (mas vc não). Pode quebrar na primeia, passando através do bloco.
 * Bolas passam pelas paredes de choque.
 * 
 * TODO
 * ------------------------------
 * [  ] Efeitos para rastro do movimento das bolas
 * [  ] Efeitos para coleta de itens
 * [  ] Efeitos para ricochetes
 * [  ] Tiles dos itens animados
 * [  ] Inimigos bolas que ricocheteiam
 * [NO] Implementar solução para o Mapa de Itens simplemente usando TileMap descompactado em RAM
 * [OK] Testar consumo de RAM com MAP e sem MAP
 * [OK] Contabilizar item coletado
 * [OK] Modificar tiles de itens no mapa de forma permanente (restaura quando faz scroll)
 * [OK] Aumentar mapa de colisão para 3 tiles a mais de cada lado, permitindo que o player saia da tela
 * [OK] Gerar mapa de colisão para cada troca de tela
 */
#include <genesis.h>
#include <sprite_eng.h>

#include "globals.h"
#include "resources.h"

#include "engine/gameobject.h"
#include "engine/utils.h"
#include "engine/background.h"
#include "engine/level.h"
#include "player.h"
#include "hud.h"

// index for tiles in VRAM (first tile reserved for SGDK)
// u16 ind = 1;
u16 ind = TILE_USER_INDEX;

// glow color effect
u8 bg_colors_delay = 5;
const u16 const bg_color_glow[] = {0x0, 0x222, 0x444, 0x666, 0x888};

#define MAX_OBJ 78
GameObject balls_list[MAX_OBJ];

////////////////////////////////////////////////////////////////////////////
// GAME INIT

u16** ball_indexes;

static void frame_changed(Sprite* sprite) {
    // get VRAM tile index for current animation of this sprite
    u16 tileIndex = ball_indexes[sprite->animInd][sprite->frameInd];
	
    // manually set tile index for the current frame (preloaded in VRAM)
    SPR_setVRAMTileIndex(sprite, tileIndex);
}


void game_init() {
	VDP_setScreenWidth320();
	SPR_init();
	
	// init BACKGROUND, LEVEL AND HUD ///////////////////////////////

	#ifdef DEBUG
	VDP_setTextPlane(BG_BACKGROUND);
	#else	
	ind += BACKGROUND_init(ind, FIX16(-1.06), FIX16(-0.05));
	#endif

	ind += LEVEL_init(ind);
	
	#ifdef DEBUG
	LEVEL_draw_map();
	#endif
	
	#ifndef DEBUG
	ind += HUD_init(ind);
	#endif
	
	// init GAME OBJECTS ////////////////////////////////////////////

	PLAYER_init(ind);

	// init_balls();
}

////////////////////////////////////////////////////////////////////////////
// GAME LOGIC

// static inline void color_effects() {
// 	--bg_colors_delay;
// 	if (bg_colors_delay == 0) {
// 		// rotate_colors_left(PAL_BACKGROUND*16, PAL_BACKGROUND*16+15);
// 		glow_color(PAL_BACKGROUND*16+8, bg_color_glow, 5);

// 		bg_colors_delay = 15;
// 	}
// }

// inline void update_enemies() {
// 	GameObject* ball = &balls_list[0];
// 	for (u8 i = 0; i < MAX_OBJ; ++i, ++ball) {
// 		ball->x += ball->speed_x;
// 		ball->y += ball->speed_y;

// 		GAMEOBJECT_update_boundbox(ball->x, ball->y, ball);
// 		GAMEOBJECT_bounce_off_screen(ball);
// 		SPR_setPosition(ball->sprite, ball->box.left, ball->box.top);
// 	}
// }

static inline void game_update() {
	update_input();

	PLAYER_update();
	// update_enemies();

	#ifndef DEBUG
	BACKGROUND_update();
	#endif

	#if MAP_SOLUTION == MAP_BY_COMPACT_MAP
	LEVEL_update_camera(&player);
	#endif
	// color_effects();
}

////////////////////////////////////////////////////////////////////////////
// MAIN

int main(bool resetType) {
	// Soft reset doesn't clear RAM. Can lead to bugs.
	if (!resetType) {
		SYS_hardReset();
	}
	SYS_showFrameLoad(TRUE);
	game_init();

	SYS_doVBlankProcess();
	
	kprintf("Free RAM after Game Init: %d", MEM_getFree());

	while (TRUE) {
		game_update();

		SPR_update();
		SYS_doVBlankProcess();
	}

	return 0;
}
