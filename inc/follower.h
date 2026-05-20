#ifndef FOLLOWER_H
#define FOLLOWER_H

#include "genesis.h"

typedef enum {
    STARTER_CHARMANDER = 0,
    STARTER_BULBASAUR,
    STARTER_SQUIRTLE,
    
    STARTER_COUNT  // = 3, pratique pour le cycle
} StarterType;

// Init : crée les 3 sprites (cachés sauf le starter actif)
void FOLLOWER_init(s16 redX, s16 redY, u8 redFacingDir);

// À appeler à chaque frame depuis main, après PLAYER_update
void FOLLOWER_update(s16 redX, s16 redY, u8 redFacingDir, bool redIsMoving, bool redIsRunning);

// Met à jour la position écran (à appeler depuis PLAYER_updateScreenPosition)
void FOLLOWER_updateScreenPosition(s16 scrollX, s16 scrollY);

// Change de starter (appelé quand C est pressé)
void FOLLOWER_cycleStarter(void);

#endif