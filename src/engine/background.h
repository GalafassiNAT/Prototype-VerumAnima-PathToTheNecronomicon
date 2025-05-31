#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

#include <genesis.h>
#include "globals.h"
#include "gameobject.h"
#include "resources.h"

u16 BACKGROUND_init(u16 ind, f16 start_speed, f16 speed_increase);
void BACKGROUND_update();

#endif