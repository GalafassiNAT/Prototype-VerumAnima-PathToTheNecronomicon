#include <genesis.h>
#pragma once

typedef enum {
	ENEMY_TYPE_BAT,
	ENEMY_TYPE_CROW,
	ENEMY_TYPE_MINIEYE,
	ENEMY_TYPE_MINIBOSS,
	ENEMY_TYPE_BOSS
} EnemyType;

typedef enum {
	AI_BEHAVIOR_VERTICAL_BOUNCE,
	AI_BEHAVIOR_MIRROR_BOUNCE,
	AI_BEHAVIOR_FLY_STRAIGHT,
	AI_BEHAVIOR_STATIC,
} AIBehaviorType;


typedef struct {
	f32 x;
	f32 y;

	u8 wave;
	u8 type;
	u8 behavior;
} WaveObjectData;

typedef struct {
	const WaveObjectData* enemies;
	u16 num_enemies;
} WaveDef;

typedef struct {
	WaveDef* waves;
	u16 num_waves;
} StageDef;