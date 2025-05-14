#include "MainMenuResources.h"

bool LoadInitialLoadingScreenResources(MainMenuResources& res) {
    res.helpyLoadingScreenTexture = LoadTexture("resources/helpy/helpyLoadingScreen.png");
    if (res.helpyLoadingScreenTexture.id > 0) {
        res.helpyLoadingTextureLoaded = true;
        SetTextureFilter(res.helpyLoadingScreenTexture, TEXTURE_FILTER_BILINEAR);

        return true;
    }

    return false;
}


static void InitializeResourceStates(MainMenuResources& res) {
    res.crtShader = { 0 };
    res.crtResolutionLoc = -1;
    res.crtTimeLoc = -1;
    res.shaderLoadedSuccessfully = false;

    res.bgGifImage = { 0 };
    res.bgTexture = { 0 };
    res.settingsBgTexture = { 0 };
    res.animFrames = 0;
    res.gifLoaded = false;

    res.helpyGifImage = { 0 };
    res.helpyTexture = { 0 };
    res.helpyAnimFrames = 0;
    res.helpyGifLoaded = false;

    res.defaultGuiFont = GetFontDefault();
    res.arcadeClassicFont = { 0 };
    res.bytesFont = { 0 };

    res.menuMusic = { 0 };
    res.settingsMusic = { 0 };
    res.menuMusicLoaded = false;
    res.settingsMusicLoaded = false;

    res.targetRenderTexture = { 0 };
}

bool LoadMainMenuResources(MainMenuResources& res, int logicalWidth, int logicalHeight) {
    InitializeResourceStates(res);
    bool allCriticalLoaded = true;

    res.cursor = LoadTexture("resources/cursor.png");


    // --- Load Window Icon ---
    if (FileExists("resources/favicon.png")) {
        Image icon = LoadImage("resources/favicon.png");
        if (icon.data) {
            SetWindowIcon(icon);
            UnloadImage(icon);
        }
    }

    // --- Load Shader ---
    if (FileExists("crt.fs")) {
        res.crtShader = LoadShader(0, "crt.fs");
        if (res.crtShader.id > 0) {
            res.shaderLoadedSuccessfully = true;
            res.crtResolutionLoc = GetShaderLocation(res.crtShader, "resolution");
            res.crtTimeLoc = GetShaderLocation(res.crtShader, "time");
        }
    }

    // --- Load GIF Background for Main Menu ---
    if (FileExists("resources/menuBg.gif")) {
        res.bgGifImage = LoadImageAnim("resources/menuBg.gif", &res.animFrames);
        if (res.bgGifImage.data != NULL && res.animFrames > 0) {
            res.bgTexture = LoadTextureFromImage(res.bgGifImage);
            if (res.bgTexture.id > 0) {
                res.gifLoaded = true;
                SetTextureFilter(res.bgTexture, TEXTURE_FILTER_BILINEAR);
            }
            else {
                UnloadImage(res.bgGifImage); 
                res.bgGifImage = { 0 };
            }
        }
        else {
            if (res.bgGifImage.data) UnloadImage(res.bgGifImage);
            res.bgGifImage = { 0 };
        }
    }

    // --- Load Helpy GIF for Settings Screen ---
    if (FileExists("resources/helpy/helpyGrooves.gif")) {
        res.helpyGifImage = LoadImageAnim("resources/helpy/helpyGrooves.gif", &res.helpyAnimFrames);
        if (res.helpyGifImage.data != NULL && res.helpyAnimFrames > 0) {
            res.helpyTexture = LoadTextureFromImage(res.helpyGifImage);
            if (res.helpyTexture.id > 0) {
                res.helpyGifLoaded = true;
                SetTextureFilter(res.helpyTexture, TEXTURE_FILTER_BILINEAR);
            }
            else {
                UnloadImage(res.helpyGifImage); 
                res.helpyGifImage = { 0 };
            }
        }
        else {
            if (res.helpyGifImage.data) UnloadImage(res.helpyGifImage);
            res.helpyGifImage = { 0 };
        }
    }

    // --- Load Settings Background ---
    if (FileExists("resources/settingsBg.png")) {
        res.settingsBgTexture = LoadTexture("resources/settingsBg.png");
        if (res.settingsBgTexture.id > 0) {
            SetTextureFilter(res.settingsBgTexture, TEXTURE_FILTER_BILINEAR);
        }
    }

    // --- Load Buttons FX ---
    if (FileExists("resources/click.mp3") && FileExists("resources/poof.mp3") && FileExists("resources/select.mp3")) {
        res.buttonClick = LoadSound("resources/click.mp3");
        res.buttonPoof = LoadSound("resources/poof.mp3");
        res.buttonSelect = LoadSound("resources/select.mp3");
    }

    // --- Load Custom Fonts ---
    if (FileExists("resources/ARCADECLASSIC.TTF")) {
        res.arcadeClassicFont = LoadFont("resources/ARCADECLASSIC.TTF");
        res.bytesFont = LoadFont("resources/Bytes.otf");

        if (res.arcadeClassicFont.texture.id > 0) 
            SetTextureFilter(res.arcadeClassicFont.texture, TEXTURE_FILTER_POINT);
    }

    // --- Load Menu Music ---
    if (FileExists("resources/mainMenu.mp3")) {
        res.menuMusic = LoadMusicStream("resources/mainMenu.mp3");
        if (res.menuMusic.stream.buffer) {
            res.menuMusicLoaded = true;
            SetMusicVolume(res.menuMusic, 1);
        }
    }

    // --- Load Settings Music ---
    if (FileExists("resources/SETTINGS.mp3")) {
        res.settingsMusic = LoadMusicStream("resources/SETTINGS.mp3");
        if (res.settingsMusic.stream.buffer) {
            res.settingsMusicLoaded = true;
            SetMusicVolume(res.settingsMusic, 1);
        }
    }

    // --- Load Render Texture ---
    res.targetRenderTexture = LoadRenderTexture(logicalWidth, logicalHeight);
    if (res.targetRenderTexture.id > 0) {
        SetTextureFilter(res.targetRenderTexture.texture, TEXTURE_FILTER_BILINEAR);
    }
    else allCriticalLoaded = false;


    TraceLog(LOG_INFO, "RESOURCES: Main menu resource loading complete.");
    return allCriticalLoaded;
}

void UnloadMainMenuResources(MainMenuResources& res) {
    if (res.helpyLoadingTextureLoaded) {
        UnloadTexture(res.helpyLoadingScreenTexture);
        res.helpyLoadingTextureLoaded = false;
    }

    if (res.cursor.id > 0) UnloadTexture(res.cursor);

    if (res.menuMusic.stream.buffer) UnloadMusicStream(res.menuMusic);
    if (res.settingsMusic.stream.buffer) UnloadMusicStream(res.settingsMusic);

    if (res.targetRenderTexture.id > 0) UnloadRenderTexture(res.targetRenderTexture);

    if (res.gifLoaded) {
        if (res.bgTexture.id > 0) UnloadTexture(res.bgTexture);
        if (res.bgGifImage.data) UnloadImage(res.bgGifImage);
    }

    if (res.helpyGifLoaded) {
        if (res.helpyTexture.id > 0) UnloadTexture(res.helpyTexture);
        if (res.helpyGifImage.data) UnloadImage(res.helpyGifImage);
    }

    if (res.settingsBgTexture.id > 0) UnloadTexture(res.settingsBgTexture);

    if (res.buttonClick.stream.buffer) {
        UnloadSound(res.buttonClick);
        UnloadSound(res.buttonPoof);
        UnloadSound(res.buttonSelect);
    }
    
    if (res.arcadeClassicFont.texture.id > 0) {
        UnloadFont(res.arcadeClassicFont);
        UnloadFont(res.bytesFont);
    }

    if (res.shaderLoadedSuccessfully && res.crtShader.id > 0) UnloadShader(res.crtShader);

    InitializeResourceStates(res);

    TraceLog(LOG_INFO, "RESOURCES: Main menu resources unloaded.");
}