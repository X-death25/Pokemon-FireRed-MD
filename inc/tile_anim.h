#ifndef TILE_ANIM_H
#define TILE_ANIM_H

#include "genesis.h"
#include "engine.h"

void TileAnim_LoadAll(const TileAnimDefinition *anims, u8 numAnims);
void TileAnim_Update(void);
void TileAnim_Clear(void);

#endif