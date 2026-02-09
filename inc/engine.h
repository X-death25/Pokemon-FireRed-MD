#ifndef ENGINE_H
#define ENGINE_H

#include "genesis.h"

typedef struct {
    const TileSet *tilesetA;
    const TileSet *tilesetB;

    const TileMap *tilemapA;
    const TileMap *tilemapB;

    const u8  *collisionMap;
    const u8  *npcMap;

    const Palette *paletteA; 
    const Palette *paletteB; 
    const Palette *paletteNPC;

    u16 tilemapA_w, tilemapA_h;
    u16 tilemapB_w, tilemapB_h;
} LevelDefinition;

void Engine_LoadLevel(const LevelDefinition *lvl);

#endif
