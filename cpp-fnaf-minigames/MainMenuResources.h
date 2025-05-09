#pragma once

#include "raylib.h"
#include "GameSettings.h"
#include <string>

struct MainMenuResources {
    Texture2D helpyLoadingScreenTexture;
    //int loadingGifAnimFrames;
    bool helpyLoadingTextureLoaded;

    // --- Rendering Variables ---
    Shader crtShader;
    int crtResolutionLoc; // For CRT shader
    int crtTimeLoc;       // For CRT shader
    bool shaderLoadedSuccessfully;

    // --- Asset Variables ---
    Image bgGifImage;         // Raw GIF data for animation
    Texture2D bgTexture;      // Texture for current GIF frame (main menu)
    Texture2D settingsBgTexture; // Background for settings screen
    int animFrames;           // Total frames in bgGifImage
    bool gifLoaded;

    // Helpy gif
    Image helpyGifImage;
    Texture2D helpyTexture;
    int helpyAnimFrames;
    bool helpyGifLoaded;

    //Font guiFont;
    Font defaultGuiFont; // Bêdzie to GetFontDefault()
    Font arcadeClassicFont;    // Dla np. ARCADECLASSIC.TTF
    Font bytesFont;

    Music menuMusic;
    Music settingsMusic;
    bool menuMusicLoaded;
    bool settingsMusicLoaded;

    // --- Paths (centralized for clarity) ---
    const char* helpyLoadingScreenPath = "resources/helpy/helpyLoadingScreen.png";
    const char* iconPath = "resources/favicon.png";
    const char* crtShaderPathFS = "crt.fs";
    const char* gifPath = "resources/menuBg.gif";
    const char* helpyGifPath = "resources/helpy/helpyGrooves.gif";
    const char* settingsBgPath = "resources/settingsBg.png";
    const char* arcadeClassicFontPath = "resources/ARCADECLASSIC.TTF";
    const char* bytesFontPath = "resources/Bytes.otf";
    const char* menuMusicPath = "resources/mainMenu.mp3";
    const char* settingsMusicPath = "resources/SETTINGS.mp3";

    RenderTexture2D targetRenderTexture; // Main render target for UI
};

bool LoadInitialLoadingScreenResources(MainMenuResources& res);

bool LoadMainMenuResources(MainMenuResources& res, int logicalWidth, int logicalHeight);


// Function to unload all main menu resources
void UnloadMainMenuResources(MainMenuResources& res);