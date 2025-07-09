#include <genesis.h>

typedef enum {
	ENEMY_TYPE_BAT,
	ENEMY_TYPE_CROW,
	ENEMY_TYPE_MINIEYE,
	ENEMY_TYPE_MINIBOSS,
	ENEMY_TYPE_BOSS
} EnemyType;

typedef struct {
	s16 x;
	s16 y;
	u16 w;
	u16 h;
	u8 wave;
	u8 type;
} WaveObjectData;

typedef struct {
	const WaveObjectData* enemies;
	u16 num_enemies;
} WaveDef;

typedef struct {
	WaveDef* waves;
	u16 num_waves;
} StageDef;