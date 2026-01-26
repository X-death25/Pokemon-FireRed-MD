#include "genesis.h"

#include "player.h"
#include "res/sprite.h"

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
    // position de départ (au milieu de la map par exemple)
    posX = 120;
    posY = 112;

    xOrder = 0;
    yOrder = 0;
    facingDir = 0; // DOWN

    // ajout du sprite joueur
    player = SPR_addSprite(&player_sprite, posX, posY, TILE_ATTR(PAL2, 0, FALSE, FALSE));

    return vramIndex;
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

void PLAYER_update(void)
{
    // déplacement simple
    if (xOrder > 0)  posX += PLAYER_SPEED;
    if (xOrder < 0)  posX -= PLAYER_SPEED;
    if (yOrder > 0)  posY += PLAYER_SPEED;
    if (yOrder < 0)  posY -= PLAYER_SPEED;

    // mise à jour du facing
    if (yOrder > 0)      facingDir = 0; // DOWN
    else if (yOrder < 0) facingDir = 3; // UP
    else if (xOrder < 0) facingDir = 1; // LEFT
    else if (xOrder > 0) facingDir = 2; // RIGHT

    // animation en code
    if ((xOrder != 0) || (yOrder != 0))
    {
        // on bouge → animation
        animTimer++;
        if (animTimer >= 10)   // vitesse d'anim
        {
            animTimer = 0;
            animFrame++;
            if (animFrame >= 3) animFrame = 0;
        }
    }
    else
    {
        // idle → frame 0
        animFrame = 0;
        animTimer = 0;
    }

    // calcul de la frame finale
    // chaque direction = 3 frames
    u16 frame = facingDir * 3 + animFrame;

    SPR_setFrame(player, frame);
}

void PLAYER_updateScreenPosition(void)
{
    // on affiche le joueur en fonction du scroll
    s16 screenX = posX - scrollX;
    s16 screenY = posY - scrollY;

    SPR_setPosition(player, screenX, screenY);
}
