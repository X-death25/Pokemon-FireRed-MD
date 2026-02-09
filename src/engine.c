#include "genesis.h"

#include "res/sprite.h"
#include "res/gfx.h"

#define MAP_WIDTH_PIXELS   480
#define MAP_HEIGHT_PIXELS  320

#define SCREEN_WIDTH       320
#define SCREEN_HEIGHT      224

typedef struct 
{
    const TileSet *tilesetA;
    const TileSet *tilesetB;
    const TileMap *tilemapA; 
    const TileMap *tilemapB;
    const u8  *collisionMap;
    const u8  *npcMap;
    const Palette *paletteA;  
    const Palette *paletteB;
    const Palette *paletteNPC;
} LevelDefinition;

void Engine_LoadLevel(const LevelDefinition *lvl)
{
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
    
    VDP_setTileMapEx(BG_B, lvl->tilemapB, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, idxA), 0, 0, 0, 0, lvl->tilemapB->w, lvl->tilemapB->h, CPU);

    VDP_setTileMapEx(BG_A, lvl->tilemapA, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, idxB),0, 0, 0, 0, lvl->tilemapA->w, lvl->tilemapA->h, CPU);
    
    SYS_enableInts();
}
