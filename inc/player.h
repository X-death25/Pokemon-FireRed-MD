#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "genesis.h"

void PLAYER_init(u16 vramIndex);
void PLAYER_update(void);
void PLAYER_updateScreenPosition(void);
void PLAYER_handleInput(u16 value);

// getters
s16  PLAYER_getPosX(void);
s16  PLAYER_getPosY(void);
u8   PLAYER_getFacingDir(void);
bool PLAYER_isRunning(void);
bool PLAYER_isMoving(void);
#endif
