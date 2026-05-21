#include "Engine.h"
#include "res/gfx.h"
#include "res/sprite.h"

// --- Animations de décor de Bourg Palette ---

static const TileAnimPos bourgWaterPositions[] = {
	{9, 16},{10, 16}, {11, 16},{12, 16},
    {9, 17},{10, 17}, {11, 17},{12, 17},
    {9, 18},{10, 18}, {11, 18},{12, 18},
    {9, 19},{10, 19}, {11, 19},{12, 19}
};

static const TileAnimPos bourgFlowerPositions[] = {
    {7, 11}, {8, 11}, {9, 11}, {10, 11},
    {7, 12}, {8, 12}, {9, 12}, {10, 12},
};

static const TileAnimDefinition bourgAnims[] = {
    {
        .sprite       = &water_anim_sprite,
        .positions    = bourgWaterPositions,
        .numPositions = sizeof(bourgWaterPositions) / sizeof(TileAnimPos),
        .animSpeed    = 8,
		.priority     = FALSE,   	
        .palette      = PAL1,
    },
	
	{
        .sprite       = &flower_anim_sprite,
        .positions    = bourgFlowerPositions,
        .numPositions = sizeof(bourgFlowerPositions) / sizeof(TileAnimPos),
        .animSpeed    = 15,    
        .priority     = FALSE,
        .palette      = PAL1,   
    },
};

const LevelDefinition LEVEL_BOURG_PALETTE_DAY = 
{
    .tilesetA = &PaletteTown_0_tileset,
    .tilesetB = &PaletteTown_1_tileset,
    .tilemapA = &PaletteTown_L0,
    .tilemapB = &PaletteTown_L1,
    .collisionMap = &PaletteTown_CL,
    .npcMap = NULL,
    .paletteA = &PaletteTown_0_pal,
    .paletteB = &PaletteTown_1_pal,
    .paletteNPC = NULL,
    
    .signs = NULL,
    .numSigns = 0,
    .warps = NULL,
    .numWarps = 0,
    
    .tileAnims = bourgAnims,
    .numTileAnims = sizeof(bourgAnims) / sizeof(TileAnimDefinition),
};

