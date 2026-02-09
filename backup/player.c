#include "genesis.h"

#include "player.h"
#include "res/sprite.h"

#define MAP_WIDTH_PIXELS   480
#define MAP_HEIGHT_PIXELS  320

#define SCREEN_WIDTH       320
#define SCREEN_HEIGHT      224

// on récupère le scroll de ton moteur
extern int scrollX;
extern int scrollY;

// index d'anim dans player_sprite.animations
#define ANIM_DOWN   0
#define ANIM_LEFT   1
#define ANIM_RIGHT  2
#define ANIM_UP     3

// sprite du joueur
Sprite* player;

// position du joueur en coordonnées monde (pixels)
s16 posX;
s16 posY;

// direction d'entrée
s16 xOrder;
s16 yOrder;

// direction de "facing" quand on est à l'arrêt
s16 facingDir;   // 0=DOWN,1=LEFT,2=RIGHT,3=UP

// vitesse en pixels par frame
#define PLAYER_SPEED  1

u16 animFrame = 0;
u16 animTimer = 0;

u16 PLAYER_init(u16 vramIndex)
{
    posX = 120;
    posY = 112;
    xOrder = 0;
    yOrder = 0;
    facingDir = 0;
    animFrame = 0;
    animTimer = 0;
    
    player = SPR_addSprite(&player_sprite, posX, posY, 
        TILE_ATTR(PAL2, 0, FALSE, FALSE));
    
    // ✅ Désactiver TOUTE auto-animation
 //   SPR_setAutoTileUpload(player, FALSE);
 //   SPR_setAnimAndFrame(player, 0, 0); // Anim 0, frame 0
    
    return vramIndex;
}

void PLAYER_update(void)
{
    // Déplacement
    if (xOrder > 0)  posX ++;
    if (xOrder < 0)  posX --;
    if (yOrder > 0)  posY ++;
    if (yOrder < 0)  posY --;
    
    // ✅ NE RIEN FAIRE d'autre
    // Forcer frame 0 en permanence
    SPR_setFrame(player,0);


}

void PLAYER_handleInput(u16 value)
{
    // même logique que Sonic : ordres de déplacement
    if (value & BUTTON_UP)      yOrder = -1;
    else if (value & BUTTON_DOWN) yOrder = +1;
    else                        yOrder = 0;

    if (value & BUTTON_LEFT)    xOrder = -1;
    else if (value & BUTTON_RIGHT) xOrder = +1;
    else                        xOrder = 0;
}

void PLAYER_updateScreenPosition(void)
{
    // --- paramètres caméra ---
    const s16 screenCenterX = SCREEN_WIDTH / 2;   // 160
    const s16 screenCenterY = SCREEN_HEIGHT / 2;  // 112
    const s16 deadZone = 4;                      // zone morte Pokémon

    // --- position actuelle du joueur à l'écran ---
    s16 screenX = posX - scrollX;
    s16 screenY = posY - scrollY;

    // --- ajustement horizontal ---
    if (screenX > screenCenterX + deadZone)
        scrollX += (screenX - (screenCenterX + deadZone));

    if (screenX < screenCenterX - deadZone)
        scrollX += (screenX - (screenCenterX - deadZone));

    // --- ajustement vertical ---
    if (screenY > screenCenterY + deadZone)
        scrollY += (screenY - (screenCenterY + deadZone));

    if (screenY < screenCenterY - deadZone)
        scrollY += (screenY - (screenCenterY - deadZone));

    // --- limites de map ---
    if (scrollX < 0) scrollX = 0;
    if (scrollY < 0) scrollY = 0;

    if (scrollX > MAP_WIDTH_PIXELS - SCREEN_WIDTH)
        scrollX = MAP_WIDTH_PIXELS - SCREEN_WIDTH;

    if (scrollY > MAP_HEIGHT_PIXELS - SCREEN_HEIGHT)
        scrollY = MAP_HEIGHT_PIXELS - SCREEN_HEIGHT;

    // --- position finale du sprite ---
    SPR_setPosition(player, posX - scrollX, posY - scrollY);
}

