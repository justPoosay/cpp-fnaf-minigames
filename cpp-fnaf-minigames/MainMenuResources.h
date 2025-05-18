#pragma once

#include "raylib.h"

#include <iostream>
#include <string>

struct MainMenuResources {
    Texture2D helpyLoadingScreenTexture;
    bool helpyLoadingTextureLoaded;

    Texture2D cursor;

// --- Rendering Variables ---
    Shader crtShader;
    int crtResolutionLoc; 
    int crtTimeLoc;
    bool shaderLoadedSuccessfully;

// --- Asset Variables ---
    Image bgGifImage; 
    Texture2D bgTexture;
    Texture2D settingsBgTexture;
    int animFrames;
    bool gifLoaded;

    // Helpy gif
    Image helpyGifImage;
    Texture2D helpyTexture;
    int helpyAnimFrames;
    bool helpyGifLoaded;

    // Cracked Helpy gif
    Image helpyCrackedGifImage;
    Texture2D helpyCrackedTexture;
    int helpyCrackedAnimFrames;
    bool helpyCrackedGifLoaded;

    int helpyCrackedCurrentPlayingFrame;
    float helpyCrackedFramePlayTimer;
    bool helpyCrackAnimationHasPlayed;

    // Fonts;
    Font defaultGuiFont;
    Font arcadeClassicFont; 
    Font bytesFont;

    // Button sounds 
    Sound buttonPoof;
    Sound buttonClick;
    Sound buttonSelect;

    Music menuMusic;
    Music settingsMusic;
    bool menuMusicLoaded;
    bool settingsMusicLoaded;

    RenderTexture2D targetRenderTexture;
};

bool LoadInitialLoadingScreenResources(MainMenuResources& res);
bool LoadMainMenuResources(MainMenuResources& res, int logicalWidth, int logicalHeight);
void UnloadMainMenuResources(MainMenuResources& res);