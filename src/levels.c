#include "Engine.h"
#include "res/gfx.h"
#include "res/sprite.h"

const LevelDefinition LEVEL_BOURG_PALETTE_DAY = 
{
    .tilesetA = &PaletteTown_0_tileset,
    .tilesetB = &PaletteTown_1_tileset,

    .tilemapA = &PaletteTown_L0,
    .tilemapB = &PaletteTown_L1,

    .collisionMap = NULL,
    .npcMap = NULL,

    .paletteA = &PaletteTown_0_pal,
    .paletteB = &PaletteTown_1_pal,
    .paletteNPC = NULL
};

