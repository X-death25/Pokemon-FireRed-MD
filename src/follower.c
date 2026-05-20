#include "genesis.h"
#include "follower.h"
#include "res/sprite.h"

#define HISTORY_SIZE     20   // buffer un peu plus grand que le délai
#define FOLLOWER_DELAY   16   // frames de retard ≈ 1 case à vitesse marche

#define ANIM_DOWN   0
#define ANIM_LEFT   1
#define ANIM_RIGHT  2
#define ANIM_UP     3

// --- Buffer circulaire des positions passées de Red ---
static s16 historyX[HISTORY_SIZE];
static s16 historyY[HISTORY_SIZE];
static u8  historyDir[HISTORY_SIZE];
static u8  historyMoving[HISTORY_SIZE];
static u8  historyHead = 0;

// --- État du starter ---
static Sprite* sprites[STARTER_COUNT];   // les 3 sprites
static StarterType currentStarter = STARTER_CHARMANDER;
static s16 followerX, followerY;
static u8  followerDir;
static u8  followerAnimFrame;
static u8  followerAnimTimer;

// --- Helpers internes ---
static void updateVisibility(void)
{
    for (u8 i = 0; i < STARTER_COUNT; i++)
    {
        SPR_setVisibility(sprites[i], (i == currentStarter) ? VISIBLE : HIDDEN);
    }
}

// --- API publique ---
void FOLLOWER_init(s16 redX, s16 redY, u8 redFacingDir)
{
    // Position initiale : juste derrière Red
    followerX = redX;
    followerY = redY + 16;  // 16px derrière (1/2 case)
    followerDir = redFacingDir;
    followerAnimFrame = 0;
    followerAnimTimer = 0;
    
    // Pré-remplir l'historique avec la position de Red
    for (u8 i = 0; i < HISTORY_SIZE; i++)
    {
        historyX[i] = redX;
        historyY[i] = redY;
        historyDir[i] = redFacingDir;
        historyMoving[i] = FALSE;
    }
    historyHead = 0;
    
    // Créer les 3 sprites
    sprites[STARTER_CHARMANDER] = SPR_addSprite(&charmander_sprite, followerX, followerY,
        TILE_ATTR(PAL2, 0, FALSE, FALSE));
    sprites[STARTER_BULBASAUR] = SPR_addSprite(&bulbasaur_sprite, followerX, followerY,
        TILE_ATTR(PAL2, 0, FALSE, FALSE));
    sprites[STARTER_SQUIRTLE] = SPR_addSprite(&squirtle_sprite, followerX, followerY,
        TILE_ATTR(PAL2, 0, FALSE, FALSE));
    
    // Animation initiale (statique vers le bas)
    for (u8 i = 0; i < STARTER_COUNT; i++)
    {
        SPR_setAnim(sprites[i], ANIM_DOWN);
        SPR_setFrame(sprites[i], 0);
    }
    
    updateVisibility();
}

void FOLLOWER_update(s16 redX, s16 redY, u8 redFacingDir, bool redIsMoving, bool redIsRunning)
{
    // 1. Enregistrer la position actuelle de Red dans l'historique
    historyHead = (historyHead + 1) % HISTORY_SIZE;
    historyX[historyHead] = redX;
    historyY[historyHead] = redY;
    historyDir[historyHead] = redFacingDir;
    historyMoving[historyHead] = redIsMoving;
    
    // 2. Lire dans l'historique avec le délai
    u8 readIndex = (historyHead + HISTORY_SIZE - FOLLOWER_DELAY) % HISTORY_SIZE;
    
    // 3. Appliquer la position SEULEMENT si Red bouge
    if (redIsMoving)
    {
        followerX = historyX[readIndex];
        followerY = historyY[readIndex];
        followerDir = historyDir[readIndex];
    }
    
    // 4. Animation : suit l'état de Red (pas le mouvement du follower)
    if (redIsMoving)
    {
        followerAnimTimer++;
        u8 animSpeed = redIsRunning ? 4 : 6;
        
        if (followerAnimTimer >= animSpeed)
        {
            followerAnimTimer = 0;
            followerAnimFrame = (followerAnimFrame == 0) ? 1 : 0;
        }
    }
    else
    {
        followerAnimFrame = 0;
        followerAnimTimer = 0;
    }
    
    // 5. Appliquer animation au sprite actif
    Sprite* active = sprites[currentStarter];
    SPR_setAnim(active, followerDir);
    SPR_setFrame(active, followerAnimFrame);
}

void FOLLOWER_updateScreenPosition(s16 scrollX, s16 scrollY)
{
    Sprite* active = sprites[currentStarter];
    SPR_setPosition(active, followerX - scrollX, followerY - scrollY);
}

void FOLLOWER_cycleStarter(void)
{
    currentStarter = (currentStarter + 1) % STARTER_COUNT;
    updateVisibility();
    
    // Synchroniser anim/frame du nouveau sprite actif
    Sprite* active = sprites[currentStarter];
    SPR_setAnim(active, followerDir);
    SPR_setFrame(active, followerAnimFrame);
}