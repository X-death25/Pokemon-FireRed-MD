#include "genesis.h"
#include "engine.h"
#include "player.h"
#include "res/sprite.h"
#include "res/gfx.h"

#define MAP_WIDTH_PIXELS   480
#define MAP_HEIGHT_PIXELS  320

#define SCREEN_WIDTH       320
#define SCREEN_HEIGHT      224

#define ANIM_DOWN   0
#define ANIM_LEFT   1
#define ANIM_RIGHT  2
#define ANIM_UP     3

#define PLAYER_SPEED_WALK  1
#define PLAYER_SPEED_RUN   2

#define ANIM_TIMER_WALK    6
#define ANIM_TIMER_RUN     3

#define TILE_PIXEL_SIZE     8   // unité TileMap C (rescomp découpe en 8×8)
#define FOOT_OFFSET_Y  4   // offset descendant pour la collision des pieds
// Hitbox "physique" du sprite : on garde le concept de pieds mais on ajoute de la hauteur
#define HITBOX_OFFSET_X     8   // pixels rognés à gauche/droite
#define HITBOX_OFFSET_TOP   16  // pixels rognés en haut (au-dessus de la taille de Red)
#define HITBOX_OFFSET_BOT   4   // pixels en dessous des pieds (l'offset existant)

// Sprite de Red : 32×32, donc occupe 4×4 tiles 8×8
// La collision se fait sur les 2 cases du bas (les "pieds")
#define SPRITE_WIDTH        32
#define SPRITE_HEIGHT       32


extern int scrollX;
extern int scrollY;

static bool isRunning = FALSE;

static Sprite* player;
static s16 posX;
static s16 posY;
static s16 xOrder;
static s16 yOrder;
static s16 facingDir;
static u16 animFrame = 0;
static u16 animTimer = 0;

// --- Récupère le type d'une tile par ses coords TileMap (8×8) ---
static TileType getTileType(u16 tileX, u16 tileY)
{
    const LevelDefinition *lvl = Engine_GetCurrentLevel();
    if (!lvl || !lvl->collisionMap) return TILE_UNKNOWN;
    
    const TileMap *cl = lvl->collisionMap;
    
    if (tileX >= cl->w || tileY >= cl->h)
        return TILE_UNKNOWN;
    
    u16 id = cl->tilemap[tileY * cl->w + tileX] & 0x07FF;
    return (TileType)id;
}


// --- Renvoie TRUE si la tile bloque physiquement le mouvement ---
static bool isTileSolid(TileType type)
{
    switch (type)
    {
        case TILE_BLOCKED:
        case TILE_WATER:        // Eau bloque tant qu'on n'a pas Surf
        case TILE_UNKNOWN:      // Hors map
            return TRUE;
        
        case TILE_PASSABLE:
        case TILE_GRASS:        // On peut marcher dans l'herbe haute
        case TILE_WARP:         // On peut marcher sur un warp
        case TILE_SIGN:         // On peut s'approcher d'un panneau
        default:
            return FALSE;
    }
}


static bool canMoveTo(s16 newPosX, s16 newPosY)
{
    if (newPosX < 0 || newPosY < 0) return FALSE;
    if (newPosX + SPRITE_WIDTH  > MAP_WIDTH_PIXELS)  return FALSE;
    if (newPosY + SPRITE_HEIGHT > MAP_HEIGHT_PIXELS) return FALSE;
    
    // Hitbox = rectangle du corps de Red (sans la tête)
    s16 hitboxLeft   = newPosX + HITBOX_OFFSET_X;
    s16 hitboxRight  = newPosX + SPRITE_WIDTH - HITBOX_OFFSET_X - 1;
    s16 hitboxTop    = newPosY + HITBOX_OFFSET_TOP;
    s16 hitboxBottom = newPosY + SPRITE_HEIGHT - 1 - FOOT_OFFSET_Y;
    
    // Tester les 4 coins de la hitbox
    if (isTileSolid(getTileType(hitboxLeft  / 8, hitboxTop    / 8))) return FALSE;
    if (isTileSolid(getTileType(hitboxRight / 8, hitboxTop    / 8))) return FALSE;
    if (isTileSolid(getTileType(hitboxLeft  / 8, hitboxBottom / 8))) return FALSE;
    if (isTileSolid(getTileType(hitboxRight / 8, hitboxBottom / 8))) return FALSE;
    
    return TRUE;
}
    
void PLAYER_init(u16 vramIndex)
{
    posX = 120;
    posY = 112;
    xOrder = 0;
    yOrder = 0;
    facingDir = ANIM_DOWN;
    animFrame = 0;
    animTimer = 0;

    PAL_setPalette(PAL2, player_sprite.palette->data, DMA);
		
    player = SPR_addSprite(&player_sprite, posX, posY, TILE_ATTR(PAL2, 0, FALSE, FALSE));
    SPR_setAnim(player, ANIM_DOWN);
	
	KLog("=== Dump tilemap ===");
{
    const TileMap *cl = Engine_GetCurrentLevel()->collisionMap;
    u16 w = cl->w;
    u16 h = cl->h;
    char buf[256];
    char tmp[8];
    
    for (u16 y = 0; y < h; y += 2) {  // toutes les 2 lignes (= 1 case Tiled)
        buf[0] = '\0';
        for (u16 x = 0; x < w; x += 2) {  // toutes les 2 colonnes
            u16 id = cl->tilemap[y * w + x] & 0x07FF;
            sprintf(tmp, "%d ", id);
            strcat(buf, tmp);
        }
        KLog(buf);
    }
}

}

void PLAYER_update(void)
{
    // --- Vitesse selon mode ---
    s16 speed = isRunning ? PLAYER_SPEED_RUN : PLAYER_SPEED_WALK;
   
    // --- Déplacement avec collisions, axe par axe ---
    // Axe X
    if (xOrder != 0)
    {
        s16 newX = posX + (xOrder * speed);
        if (canMoveTo(newX, posY))
            posX = newX;
    }
    
    // Axe Y
    if (yOrder != 0)
    {
        s16 newY = posY + (yOrder * speed);
        if (canMoveTo(posX, newY))
            posY = newY;
    }
    
    // --- Détection mouvement ---
    bool isMoving = (xOrder != 0 || yOrder != 0);
    
    // --- Direction (basée sur l'input, pas sur le mouvement réel) ---
    // → important : si on appuie contre un mur, on tourne quand même la tête
    if (yOrder > 0)      facingDir = ANIM_DOWN;
    else if (yOrder < 0) facingDir = ANIM_UP;
    else if (xOrder > 0) facingDir = ANIM_RIGHT;
    else if (xOrder < 0) facingDir = ANIM_LEFT;
    
    // --- Animation ---
    if (isMoving)
    {
        animTimer++;
        u16 animSpeed = isRunning ? ANIM_TIMER_RUN : ANIM_TIMER_WALK;
        
        if (animTimer >= animSpeed)
        {
            animTimer = 0;
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
        animFrame = 0;
        animTimer = 0;
    }
	
	// --- Détection de déclencheurs sur la case sous les pieds ---
    s16 footY = posY + SPRITE_HEIGHT - 1 - FOOT_OFFSET_Y;
    s16 footX = posX + (SPRITE_WIDTH / 2);
    u16 tileX = footX / TILE_PIXEL_SIZE;
    u16 tileY = footY / TILE_PIXEL_SIZE;
	
	TileType under = getTileType(tileX, tileY);
	if (under == TILE_WARP)
    {
        Engine_HardReset();
    }

    
    SPR_setAnim(player, facingDir);
    SPR_setFrame(player, animFrame);
}



void PLAYER_handleInput(u16 value)
{
    // Course : B maintenu
    isRunning = (value & BUTTON_B) ? TRUE : FALSE;

    if (value & BUTTON_UP)         yOrder = -1;
    else if (value & BUTTON_DOWN)  yOrder = +1;
    else                           yOrder = 0;

    if (value & BUTTON_LEFT)       xOrder = -1;
    else if (value & BUTTON_RIGHT) xOrder = +1;
    else                           xOrder = 0;
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
