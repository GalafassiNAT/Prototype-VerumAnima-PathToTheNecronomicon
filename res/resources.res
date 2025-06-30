IMAGE   img_background    "backgrounds/level1_bg.png" BEST
IMAGE   img_hud           "hud/hud.png" BEST

PALETTE level1_pal        "levels/level1/level1_pal.pal"
MAP     level1_map        "levels/level1/level1_map.tmx" map_layer BEST 0

SPRITE  spr_plat          "sprites/player.png"       4 5 FAST 20
SPRITE 	spr_bat        	  "sprites/enemy_bat.png"    2 2 FAST 5
SPRITE  spr_player_shot   "sprites/player_shots.png" 2 2 FAST 0

OBJECTS waves_1  	 "levels/level1/level1_waves.tmx" obj_layer "wave:u8;type:EnemyType;start_x:s16;start_y:s16" "sortby:wave" 