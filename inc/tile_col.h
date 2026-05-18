#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "genesis.h"

void PLAYER_init(u16 vramIndex);
void PLAYER_update(void);
void PLAYER_updateScreenPosition(void);
void PLAYER_handleInput(u16 value);

#endif
