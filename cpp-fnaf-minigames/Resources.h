#pragma once

#include "raylib.h"
#include <vector>
#include <string>

struct GameResources { // Store game resources

// Chica (player)
    Texture2D idleRight;
    Texture2D idleLeft;
    Texture2D walkRight;
    Texture2D walkLeft;
    Texture2D jumpRight;
    Texture2D jumpLeft;

// Rainbow
    Texture2D npcBodyTexture;
    Texture2D npcDialogueBodyTexture;

// Rainbow (eyes)
    std::vector<Texture2D> npcEyeTextures;
    const int numEyeSprites = 32;

// Background, Tiles, UI
    Texture2D bg;
    Texture2D tile;
    Texture2D platformTexture;
    Texture2D buttonVoicesOn;
    Texture2D buttonVoicesOff;

// Music & FX
    Music backgroundMusic;
    Sound jumpSound;
    Sound npcDialogueSound;
    //Sound laserShootSound;

    bool musicLoaded = false;
    bool jumpSoundLoaded = false;
    bool npcDialogueSoundLoaded = false;
    //bool laserSoundLoaded = false;
};

// Loading Resources
GameResources LoadGameResources();

// Unloadind Resources
void UnloadGameResources(GameResources& resources);