/*
 * MD Pokemon Fire Red
 *
 */
 
#include "genesis.h"
#include "gfx.h"
#include "sprite.h"
#include "player.h"

// Dans player.h ou en haut de player.c
static bool wasMoving = FALSE;
static u8 lastFacingDir = 0;

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

    // palettes
    PAL_setPalette(PAL0, PaletteTown_0_pal.data, DMA);
    PAL_setPalette(PAL1, PaletteTown_1_pal.data, DMA);
	PAL_setPalette(PAL2, player_sprite.palette->data, DMA);
    PAL_setColor(63, 0xFFFF);
    VDP_setTextPalette(PAL3);

    // tilesets
    ind = TILE_USER_INDEX;

    int idx1 = ind;
    VDP_loadTileSet(&PaletteTown_0_tileset, ind, DMA);
    ind += PaletteTown_0_tileset.numTile;

    int idx2 = ind;
    VDP_loadTileSet(&PaletteTown_1_tileset, ind, DMA);
    ind += PaletteTown_1_tileset.numTile;

  //  player = SPR_addSprite(&player_sprite, 0, 10, TILE_ATTR(PAL2, 0, FALSE, FALSE));
    PLAYER_init(0);

    // tilemaps
    const TileMap *mapL0 = &PaletteTown_L0;
    const TileMap *mapL1 = &PaletteTown_L1;

    VDP_setTileMapEx(BG_A, mapL0, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, idx2),
                     0, 0, 0, 0, mapL0->w, mapL0->h, CPU);

    VDP_setTileMapEx(BG_B, mapL1, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, idx1),
                     0, 0, 0, 0, mapL1->w, mapL1->h, CPU);

       // VDP process done, we can re enable interrupts
    SYS_enableInts();

    while(1)
    {
		// Update Sprite
		 u16 value = JOY_readJoypad(JOY_1);
		 PLAYER_handleInput(value); 
		 PLAYER_update(); 
		 PLAYER_updateScreenPosition();
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
        asm("move.l (4),%a0\n");
        asm("jmp (%a0)\n");
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



