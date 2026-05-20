/*
 * MD Pokemon Fire Red
 *
 */
 
#include "genesis.h"

#include "gfx.h"
#include "music.h"
#include "sprite.h"

#include "player.h"
#include "follower.h"
#include "levels.h"


// globals 

int scrollX = 0; 
int scrollY = 0; 
int mapWidthPx = 480; 
int mapHeightPx = 320; 
bool paused = FALSE;

// Joy Events

bool rightPressed = FALSE;
bool leftPressed  = FALSE;
bool upPressed    = FALSE;
bool downPressed  = FALSE;

static void handleInput();
static void joyEvent(u16 joy, u16 changed, u16 state);

// Scroll Events

static void UpdateScrolling();

int main() 
{
    u16 ind;

    // disable interrupt when accessing VDP
    SYS_disableInts();

    VDP_setScreenWidth320();
    VDP_setPlaneSize(64, 64, TRUE);
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

    // init joypad 
	JOY_setEventHandler(joyEvent);

    // init sprite engine with default parameters
    SPR_init();
	
	// init music driver
	
	// XGM2_play(oak_lab_music);

    // Load first Level

    Engine_LoadLevel(&LEVEL_BOURG_PALETTE_DAY);

    //  player = SPR_addSprite(&player_sprite, 0, 10, TILE_ATTR(PAL2, 0, FALSE, FALSE));
    PLAYER_init(0);
	FOLLOWER_init(PLAYER_getPosX(), PLAYER_getPosY(), PLAYER_getFacingDir());


    while(1)
    {
		// Update Player Sprite
		 u16 value = JOY_readJoypad(JOY_1);
		 PLAYER_handleInput(value); 
		 PLAYER_update(); 
		 PLAYER_updateScreenPosition();
		 
		 // Update Pokemon Follower 
		 
		 FOLLOWER_update(PLAYER_getPosX(), PLAYER_getPosY(), PLAYER_getFacingDir(), PLAYER_isMoving(),PLAYER_isRunning());
		 FOLLOWER_updateScreenPosition(scrollX, scrollY);
				
		// Update Scrolling
		UpdateScrolling();
        SPR_update();
         // always call this method at the end of the frame
        SYS_doVBlankProcess();
    }
}   

static void handleInput()
{
    u16 value = JOY_readJoypad(JOY_1);
}


static void joyEvent(u16 joy, u16 changed, u16 state)
{
    if (changed & BUTTON_RIGHT)
        rightPressed = (state & BUTTON_RIGHT);

    if (changed & BUTTON_LEFT)
        leftPressed = (state & BUTTON_LEFT);

    if (changed & BUTTON_UP)
        upPressed = (state & BUTTON_UP);

    if (changed & BUTTON_DOWN)
        downPressed = (state & BUTTON_DOWN);

    if (changed & state & BUTTON_B)
    {

    }
	
	if (changed & state & BUTTON_C)
	{
		FOLLOWER_cycleStarter();
	}
}

static void UpdateScrolling()
{
    // Axe X : inversé
    VDP_setHorizontalScroll(BG_A, -scrollX);
    VDP_setHorizontalScroll(BG_B, -scrollX);

    // Axe Y : direct
    VDP_setVerticalScroll(BG_A, scrollY);
    VDP_setVerticalScroll(BG_B, scrollY);
}



