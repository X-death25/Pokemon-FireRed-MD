#include "tile_anim.h"

extern int scrollX;
extern int scrollY;

#define MAX_ANIM_GROUPS    4
#define MAX_SPRITES_PER_GROUP  16

typedef struct {
    const TileAnimDefinition *def;
    Sprite *sprites[MAX_SPRITES_PER_GROUP];
    u8  numSprites;
    u16 timer;
    u8  frame;
} AnimGroup;

static AnimGroup groups[MAX_ANIM_GROUPS];
static u8 numGroups = 0;

void TileAnim_LoadAll(const TileAnimDefinition *anims, u8 numAnims)
{
    TileAnim_Clear();
    if (!anims || numAnims == 0) return;
    
    numGroups = (numAnims < MAX_ANIM_GROUPS) ? numAnims : MAX_ANIM_GROUPS;
    
    for (u8 g = 0; g < numGroups; g++) {
        AnimGroup *grp = &groups[g];
        const TileAnimDefinition *def = &anims[g];
        
        grp->def = def;
        grp->timer = 0;
        grp->frame = 0;
        grp->numSprites = (def->numPositions < MAX_SPRITES_PER_GROUP) 
                        ? def->numPositions : MAX_SPRITES_PER_GROUP;
        
        for (u8 i = 0; i < grp->numSprites; i++) {
            s16 px = def->positions[i].tileX * 16;
            s16 py = def->positions[i].tileY * 16;
            
            grp->sprites[i] = SPR_addSprite(def->sprite, 
                px - scrollX, py - scrollY,
                TILE_ATTR(def->palette, def->priority, FALSE, FALSE));
            
            if (grp->sprites[i]) {
                SPR_setFrame(grp->sprites[i], 0);
            }
        }
    }
}

void TileAnim_Update(void)
{
    for (u8 g = 0; g < numGroups; g++) {
        AnimGroup *grp = &groups[g];
        if (!grp->def) continue;
        
        // Avance frame
        grp->timer++;
        if (grp->timer >= grp->def->animSpeed) {
            grp->timer = 0;
            grp->frame = (grp->frame + 1) % grp->def->sprite->animations[0]->numFrame;
        }
        
        // Update position écran + frame
        for (u8 i = 0; i < grp->numSprites; i++) {
            if (!grp->sprites[i]) continue;
            
            s16 px = grp->def->positions[i].tileX * 16 - scrollX;
            s16 py = grp->def->positions[i].tileY * 16 - scrollY;
            
            SPR_setPosition(grp->sprites[i], px, py);
            SPR_setFrame(grp->sprites[i], grp->frame);
        }
    }
}

void TileAnim_Clear(void)
{
    for (u8 g = 0; g < MAX_ANIM_GROUPS; g++) {
        for (u8 i = 0; i < groups[g].numSprites; i++) {
            if (groups[g].sprites[i]) {
                SPR_releaseSprite(groups[g].sprites[i]);
                groups[g].sprites[i] = NULL;
            }
        }
        groups[g].numSprites = 0;
        groups[g].def = NULL;
        groups[g].timer = 0;
        groups[g].frame = 0;
    }
    numGroups = 0;
}