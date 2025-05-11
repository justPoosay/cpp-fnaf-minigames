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

    //Font guiFont;
    Font defaultGuiFont;
    Font arcadeClassicFont; 
    Font bytesFont;

    Music menuMusic;
    Music settingsMusic;
    bool menuMusicLoaded;
    bool settingsMusicLoaded;

    // --- Paths  ---
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

    RenderTexture2D targetRenderTexture;
};

bool LoadInitialLoadingScreenResources(MainMenuResources& res);
bool LoadMainMenuResources(MainMenuResources& res, int logicalWidth, int logicalHeight);
void UnloadMainMenuResources(MainMenuResources& res);