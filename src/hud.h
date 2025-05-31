#ifndef _HUD_H
#define _HUD_H

#include <genesis.h>
#include "globals.h"
#include "engine/gameobject.h"
#include "resources.h"

extern u8 player_gems;

void HUD_update_health(u8 value);

void HUD_gem_collected(u8 value);
void HUD_update_gems();

u16 HUD_init(u16 ind);

#endif