#include "genesis.h"
#include "engine.h"
#include "res/sprite.h"
#include "res/gfx.h"

// Le niveau actuellement chargé
static const LevelDefinition *currentLevel = NULL;

void Engine_LoadLevel(const LevelDefinition *lvl)
{
    currentLevel = lvl;
    
    // Charger palettes
    PAL_setPalette(PAL0, lvl->paletteA->data, DMA);
    PAL_setPalette(PAL1, lvl->paletteB->data, DMA);
    
    if (lvl->paletteNPC) {
        PAL_setPalette(PAL3, lvl->paletteNPC->data, DMA);
    }
    
    PAL_setColor(63, 0xFFFF);
    VDP_setTextPalette(PAL3);
    
    // Charger tilesets
    u16 ind = TILE_USER_INDEX;
    u16 idxA = ind;
    VDP_loadTileSet(lvl->tilesetA, ind, DMA);
    ind += lvl->tilesetA->numTile;
    
    u16 idxB = ind;
    VDP_loadTileSet(lvl->tilesetB, ind, DMA);
    ind += lvl->tilesetB->numTile;
    
    // Charger les tilemaps
    VDP_setTileMapEx(BG_B, lvl->tilemapB, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, idxA),
                     0, 0, 0, 0, lvl->tilemapB->w, lvl->tilemapB->h, CPU);
    VDP_setTileMapEx(BG_A, lvl->tilemapA, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, idxB),
                     0, 0, 0, 0, lvl->tilemapA->w, lvl->tilemapA->h, CPU);
}

const LevelDefinition* Engine_GetCurrentLevel(void)
{
    return currentLevel;
}

void Engine_HardReset(void)
{
    asm("move.l (4),%a0\n");
    asm("jmp (%a0)\n");
}