#pragma once

#include "raylib.h"

// Structure to hold all game textures
struct GameTextures {
    // Player Textures
    Texture2D playerIdleRight;
    Texture2D playerIdleLeft;
    Texture2D playerWalkRight;
    Texture2D playerWalkLeft;
    Texture2D playerJumpRight;
    Texture2D playerJumpLeft;

    // Level Textures
    Texture2D levelTile;
    Texture2D levelBackground;

    // UI Textures
    Texture2D buttonVoicesOff;
    Texture2D buttonVoicesOn;

    // Platform Texture
    Texture2D platformTexture; // Tekstura dla platform

    // --- Methods ---
    bool LoadAll();    // Returns true on success, false on failure
    void UnloadAll();
};