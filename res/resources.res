IMAGE   img_background    "backgrounds/level1_bg.png" BEST
IMAGE   img_hud           "hud/hud.png" BEST

PALETTE level1_pal        "levels/level1/level1_pal.pal"


SPRITE  spr_plat          "sprites/player.png"       4 5 FAST 20

SPRITE 	spr_bat        	  "sprites/enemy_bat.png"    2 2 FAST 5
SPRITE  spr_crow		  "sprites/enemy_crow.png"   2 2 FAST 5
SPRITE  spr_eye 		  "sprites/enemy_eye.png"    2 2 FAST 5

SPRITE  spr_player_shot   "sprites/player_shots.png" 2 2 FAST 0

OBJECTS waves1 "levels/level1/level1_waves.tmx" obj_layer "wave:u8;type:u8;start_x:s16;start_y:s16" "sortby:wave"

