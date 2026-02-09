#include "genesis.h"

#include "player.h"
#include "res/sprite.h"
#include "res/gfx.h"

#define MAP_WIDTH_PIXELS   480
#define MAP_HEIGHT_PIXELS  320

#define SCREEN_WIDTH       320
#define SCREEN_HEIGHT      224

extern int scrollX;
extern int scrollY;

#define ANIM_DOWN   0
#define ANIM_LEFT   1
#define ANIM_RIGHT  2
#define ANIM_UP     3

Sprite* player;

s16 posX;
s16 posY;

s16 xOrder;
s16 yOrder;

s16 facingDir;

#define PLAYER_SPEED  1

u16 animFrame = 0;
u16 animTimer = 0;

u16 getCollisionTile(u16 x, u16 y)
{
    if (x >= PaletteTown_CL.w || y >= PaletteTown_CL.h)
        return 3; // Hors map = bloqué
    
    u16 index = y * PaletteTown_CL.w + x;
    return PaletteTown_CL.tilemap[index];
}

bool canWalkTo(u16 tileX, u16 tileY)
{
    u16 tile = getCollisionTile(tileX, tileY);
    
    // Tiles walkables
    return (tile == 0);
}

u16 PLAYER_init(u16 vramIndex)
{
    posX = 120;
    posY = 112;
    xOrder = 0;
    yOrder = 0;
    facingDir = ANIM_DOWN;
    animFrame = 0;
    animTimer = 0;

    PAL_setPalette(PAL2, player_sprite.palette->data, DMA);
    
   player = SPR_addSprite(&player_sprite, posX, posY, 
        TILE_ATTR(PAL2, 0, FALSE, FALSE));

    // CHANGEMENT MINIMAL : charger la palette
   // VDP_setPalette(PAL2, player_sprite.palette->data);

    // CHANGEMENT MINIMAL : sélectionner une animation
    SPR_setAnim(player, ANIM_DOWN);

    return vramIndex;
}

void PLAYER_update(void)
{
    // --- Déplacement ---
    if (xOrder > 0)  posX += PLAYER_SPEED;
    if (xOrder < 0)  posX -= PLAYER_SPEED;
    if (yOrder > 0)  posY += PLAYER_SPEED;
    if (yOrder < 0)  posY -= PLAYER_SPEED;

    // --- Détection mouvement ---
    bool isMoving = (xOrder != 0 || yOrder != 0);

    // --- Direction ---
    if (yOrder > 0)      facingDir = ANIM_DOWN;
    else if (yOrder < 0) facingDir = ANIM_UP;
    else if (xOrder > 0) facingDir = ANIM_RIGHT;
    else if (xOrder < 0) facingDir = ANIM_LEFT;

    // --- Animation ---
    if (isMoving)
    {
        animTimer++;

        // vitesse d'anim Pokémon (lent mais vivant)
        if (animTimer >= 6)
        {
            animTimer = 0;

            // cycle Pokémon : 0 → 1 → 2 → 1 → 0
            switch(animFrame)
            {
                case 0: animFrame = 1; break;
                case 1: animFrame = 2; break;
                case 2: animFrame = 1; break;
            }
        }
    }
    else
    {
        // idle Pokémon : frame 0
        animFrame = 0;
        animTimer = 0;
    }


    // --- Appliquer anim ---
    SPR_setAnim(player, facingDir);
    SPR_setFrame(player, animFrame);
}



void PLAYER_handleInput(u16 value)
{
    if (value & BUTTON_UP)      yOrder = -1;
    else if (value & BUTTON_DOWN) yOrder = +1;
    else                        yOrder = 0;

    if (value & BUTTON_LEFT)    xOrder = -1;
    else if (value & BUTTON_RIGHT) xOrder = +1;
    else                        xOrder = 0;
}

void PLAYER_updateScreenPosition(void)
{
    const s16 screenCenterX = SCREEN_WIDTH / 2;
    const s16 screenCenterY = SCREEN_HEIGHT / 2;
    const s16 deadZone = 4;

    s16 screenX = posX - scrollX;
    s16 screenY = posY - scrollY;

    if (screenX > screenCenterX + deadZone)
        scrollX += (screenX - (screenCenterX + deadZone));

    if (screenX < screenCenterX - deadZone)
        scrollX += (screenX - (screenCenterX - deadZone));

    if (screenY > screenCenterY + deadZone)
        scrollY += (screenY - (screenCenterY + deadZone));

    if (screenY < screenCenterY - deadZone)
        scrollY += (screenY - (screenCenterY - deadZone));

    if (scrollX < 0) scrollX = 0;
    if (scrollY < 0) scrollY = 0;

    if (scrollX > MAP_WIDTH_PIXELS - SCREEN_WIDTH)
        scrollX = MAP_WIDTH_PIXELS - SCREEN_WIDTH;

    if (scrollY > MAP_HEIGHT_PIXELS - SCREEN_HEIGHT)
        scrollY = MAP_HEIGHT_PIXELS - SCREEN_HEIGHT;

    SPR_setPosition(player, posX - scrollX, posY - scrollY);
}
