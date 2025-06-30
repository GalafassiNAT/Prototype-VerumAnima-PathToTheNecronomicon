#include <genesis.h>

typedef enum {
	ENEMY_TYPE_BAT,
	ENEMY_TYPE_CROW,
	ENEMY_TYPE_MINIEYE,
	ENEMY_TYPE_MINIBOSS,
	ENEMY_TYPE_BOSS
} EnemyType;

typedef struct {
	u8 wave;
	EnemyType type;
	s16 start_x;
	s16 start_y;
} WaveEnemyDef;

typedef struct {
	WaveEnemyDef* enemies;
	u16 num_enemies;
} WaveDef;

typedef struct {
	WaveDef* waves;
	u16 num_waves;
} StageDef;