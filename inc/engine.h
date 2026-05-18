#ifndef ENGINE_H
#define ENGINE_H

#include "genesis.h"

// --- Types de tiles dans la collisionMap ---
// Correspond aux IDs en C après rescomp (déterminés par scan)
typedef enum {
    TILE_PASSABLE = 0,    // Vert
    TILE_BLOCKED  = 1,    // Rouge
    TILE_WATER    = 3,    // Blanc — Surf
    TILE_GRASS    = 8,    // Violet — rencontres Pokémon
    TILE_WARP     = 6,    // Bleu foncé — téléportation
    TILE_SIGN     = 7,   // Jaune — déclencheur de dialogue
    
    TILE_UNKNOWN  = 0xFFFF  // Hors map
} TileType;

// --- Définition d'un panneau / déclencheur de dialogue ---
typedef struct {
    u16 tileX, tileY;       // Position en cases TileMap (8×8)
    u16 dialogueId;         // ID du dialogue (enum DialogueId à définir plus tard)
} SignDefinition;

// --- Définition d'un warp / téléportation ---
struct LevelDefinitionStruct;   // forward declaration pour le pointeur ci-dessous
typedef struct {
    u16 tileX, tileY;                           // Position du warp (TileMap 8×8)
    const struct LevelDefinitionStruct *target; // Niveau de destination
    u16 destTileX, destTileY;                   // Position d'arrivée
    u8 destFacingDir;                           // Direction au respawn
} WarpDefinition;

// --- Définition complète d'un niveau ---
typedef struct LevelDefinitionStruct {
    // Graphismes
    const TileSet *tilesetA;
    const TileSet *tilesetB;
    const TileMap *tilemapA;
    const TileMap *tilemapB;
    const Palette *paletteA;
    const Palette *paletteB;
    const Palette *paletteNPC;
    
    // Logique
    const TileMap *collisionMap;
    const u8      *npcMap;
    
    // Déclencheurs
    const SignDefinition *signs;
    u8 numSigns;
    
    const WarpDefinition *warps;
    u8 numWarps;
    
} LevelDefinition;

// --- API Engine ---
void Engine_LoadLevel(const LevelDefinition *lvl);
const LevelDefinition* Engine_GetCurrentLevel(void);
void Engine_HardReset(void);

#endif