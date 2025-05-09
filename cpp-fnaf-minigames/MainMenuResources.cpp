#include "MainMenuResources.h"
#include "raylib.h"
#include <string> // Required for std::string
#include <iostream> // For TraceLog, though raylib handles it

// Helper to initialize only gif on loading screen
static void InitializeLoadingScreenResourceStates(MainMenuResources& res) {
    res.loadingGifImage = { 0 };
    res.loadingGifTexture = { 0 };
    res.loadingGifAnimFrames = 0;
    res.loadingGifLoaded = false;
}

bool LoadInitialLoadingScreenResources(MainMenuResources& res) {
    InitializeLoadingScreenResourceStates(res);
    TraceLog(LOG_INFO, "RESOURCES: Attempting to load initial loading screen GIF from '%s'", res.loadingGifPath);
    if (FileExists(res.loadingGifPath)) {
        res.loadingGifImage = LoadImageAnim(res.loadingGifPath, &res.loadingGifAnimFrames);
        if (res.loadingGifImage.data != NULL && res.loadingGifAnimFrames > 0) {
            res.loadingGifTexture = LoadTextureFromImage(res.loadingGifImage);
            if (res.loadingGifTexture.id > 0) {
                res.loadingGifLoaded = true;
                SetTextureFilter(res.loadingGifTexture, TEXTURE_FILTER_BILINEAR);
                TraceLog(LOG_INFO, "RESOURCES: Initial loading screen GIF loaded ('%s', %d frames).", res.loadingGifPath, res.loadingGifAnimFrames);
                return true;
            }
            else {
                UnloadImage(res.loadingGifImage);
                res.loadingGifImage = { 0 };
                TraceLog(LOG_WARNING, "RESOURCES: Failed to create texture from initial loading screen GIF '%s'.", res.loadingGifPath);
            }
        }
        else {
            if (res.loadingGifImage.data) UnloadImage(res.loadingGifImage);
            res.loadingGifImage = { 0 };
            TraceLog(LOG_WARNING, "RESOURCES: Failed to load or process initial loading screen GIF data from '%s'. Frames: %d", res.loadingGifPath, res.loadingGifAnimFrames);
        }
    }
    else {
        TraceLog(LOG_WARNING, "RESOURCES: Initial loading screen GIF file not found: '%s'", res.loadingGifPath);
    }
    return false;
}


// Helper to initialize resource flags and structs to a known state
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

    // Inicjalizacja nowych pól dla Helpy'ego
    res.helpyGifImage = { 0 };
    res.helpyTexture = { 0 };
    res.helpyAnimFrames = 0;
    res.helpyGifLoaded = false;

    // Inicjalizacja czcionek
    res.defaultGuiFont = GetFontDefault();
    res.arcadeClassicFont = { 0 };
    res.bytesFont = { 0 };

    res.menuMusic = { 0 };
    res.settingsMusic = { 0 };
    res.menuMusicLoaded = false;
    res.settingsMusicLoaded = false;

    res.targetRenderTexture = { 0 };
}

bool LoadMainMenuResources(MainMenuResources& res, std::string& loadingStatus, int logicalWidth, int logicalHeight) {
    InitializeResourceStates(res);
    bool allCriticalLoaded = true;

    // --- Load Window Icon ---
    loadingStatus = "Loading icon...";
    TraceLog(LOG_INFO, "RESOURCES: Attempting to load icon from '%s'", res.iconPath);
    if (FileExists(res.iconPath)) {
        Image icon = LoadImage(res.iconPath);
        if (icon.data) {
            SetWindowIcon(icon);
            UnloadImage(icon);
            TraceLog(LOG_INFO, "RESOURCES: Window icon loaded successfully.");
        }
        else {
            TraceLog(LOG_WARNING, "RESOURCES: Failed to load window icon data from '%s'", res.iconPath);
        }
    }
    else {
        TraceLog(LOG_WARNING, "RESOURCES: Icon file not found: '%s'", res.iconPath);
    }

    // --- Load Shader ---
    loadingStatus = "Loading shaders...";
    TraceLog(LOG_INFO, "RESOURCES: Attempting to load CRT shader from '%s'", res.crtShaderPathFS);
    if (FileExists(res.crtShaderPathFS)) {
        res.crtShader = LoadShader(0, res.crtShaderPathFS);
        if (res.crtShader.id > 0) {
            res.shaderLoadedSuccessfully = true;
            res.crtResolutionLoc = GetShaderLocation(res.crtShader, "resolution");
            res.crtTimeLoc = GetShaderLocation(res.crtShader, "time");
            TraceLog(LOG_INFO, "RESOURCES: CRT Shader loaded successfully.");
        }
        else {
            TraceLog(LOG_WARNING, "RESOURCES: Failed to load CRT shader. Shader ID: %d", res.crtShader.id);
            // g_settings.useCRTShader might be set to false by main if this fails
        }
    }
    else {
        TraceLog(LOG_WARNING, "RESOURCES: CRT Shader file not found: '%s'. CRT Shader will be unavailable.", res.crtShaderPathFS);
        // g_settings.useCRTShader might be set to false by main
    }

    // --- Load GIF Background for Main Menu ---
    loadingStatus = "Loading menu background...";
    TraceLog(LOG_INFO, "RESOURCES: Attempting to load menu GIF from '%s'", res.gifPath);
    if (FileExists(res.gifPath)) {
        res.bgGifImage = LoadImageAnim(res.gifPath, &res.animFrames);
        if (res.bgGifImage.data != NULL && res.animFrames > 0) {
            res.bgTexture = LoadTextureFromImage(res.bgGifImage); // Load first frame into texture
            if (res.bgTexture.id > 0) {
                res.gifLoaded = true;
                SetTextureFilter(res.bgTexture, TEXTURE_FILTER_BILINEAR);
                TraceLog(LOG_INFO, "RESOURCES: Menu GIF background loaded ('%s', %d frames).", res.gifPath, res.animFrames);
            }
            else {
                UnloadImage(res.bgGifImage); // Unload image if texture creation failed
                res.bgGifImage = { 0 }; // Reset image struct
                TraceLog(LOG_WARNING, "RESOURCES: Failed to create texture from menu GIF image '%s'.", res.gifPath);
            }
        }
        else {
            if (res.bgGifImage.data) UnloadImage(res.bgGifImage); // If data was allocated but animFrames is 0 or error
            res.bgGifImage = { 0 };
            TraceLog(LOG_WARNING, "RESOURCES: Failed to load or process menu GIF image data from '%s'. Frames: %d", res.gifPath, res.animFrames);
        }
    }
    else {
        TraceLog(LOG_WARNING, "RESOURCES: Menu GIF file not found: '%s'", res.gifPath);
    }


    // --- Load Helpy GIF for Settings Screen ---
    loadingStatus = "Loading Helpy animation...";
    TraceLog(LOG_INFO, "RESOURCES: Attempting to load Helpy GIF from '%s'", res.helpyGifPath);
    if (FileExists(res.helpyGifPath)) {
        res.helpyGifImage = LoadImageAnim(res.helpyGifPath, &res.helpyAnimFrames);
        if (res.helpyGifImage.data != NULL && res.helpyAnimFrames > 0) {
            res.helpyTexture = LoadTextureFromImage(res.helpyGifImage); // Za³aduj pierwsz¹ klatkê
            if (res.helpyTexture.id > 0) {
                res.helpyGifLoaded = true;
                SetTextureFilter(res.helpyTexture, TEXTURE_FILTER_BILINEAR); // Lub TEXTURE_FILTER_POINT, jeœli preferujesz pixel art
                TraceLog(LOG_INFO, "RESOURCES: Helpy GIF loaded ('%s', %d frames).", res.helpyGifPath, res.helpyAnimFrames);
            }
            else {
                UnloadImage(res.helpyGifImage); // Zwolnij obraz, jeœli tekstura siê nie uda³a
                res.helpyGifImage = { 0 };        // Zresetuj strukturê obrazu
                TraceLog(LOG_WARNING, "RESOURCES: Failed to create texture from Helpy GIF image '%s'.", res.helpyGifPath);
            }
        }
        else {
            if (res.helpyGifImage.data) UnloadImage(res.helpyGifImage); // Jeœli dane zosta³y zaalokowane, ale wyst¹pi³ b³¹d
            res.helpyGifImage = { 0 };
            TraceLog(LOG_WARNING, "RESOURCES: Failed to load or process Helpy GIF image data from '%s'. Frames: %d", res.helpyGifPath, res.helpyAnimFrames);
        }
    }
    else {
        TraceLog(LOG_WARNING, "RESOURCES: Helpy GIF file not found: '%s'", res.helpyGifPath);
    }

    // --- Load Settings Background ---
    loadingStatus = "Loading settings background...";
    TraceLog(LOG_INFO, "RESOURCES: Attempting to load settings background from '%s'", res.settingsBgPath);
    if (FileExists(res.settingsBgPath)) {
        res.settingsBgTexture = LoadTexture(res.settingsBgPath);
        if (res.settingsBgTexture.id > 0) {
            SetTextureFilter(res.settingsBgTexture, TEXTURE_FILTER_BILINEAR);
            TraceLog(LOG_INFO, "RESOURCES: Settings background loaded ('%s').", res.settingsBgPath);
        }
        else {
            TraceLog(LOG_WARNING, "RESOURCES: Failed to load settings background texture from '%s'.", res.settingsBgPath);
        }
    }
    else {
        TraceLog(LOG_WARNING, "RESOURCES: Settings background file not found: '%s'", res.settingsBgPath);
    }

    // --- Load Custom Fonts ---
    loadingStatus = "Loading special font...";
    TraceLog(LOG_INFO, "RESOURCES: Attempting to load special font from '%s'", res.arcadeClassicFontPath);
    if (FileExists(res.arcadeClassicFontPath)) {
        res.arcadeClassicFont = LoadFont(res.arcadeClassicFontPath);
        res.bytesFont = LoadFont(res.bytesFontPath);
        if (res.arcadeClassicFont.texture.id > 0) {
            SetTextureFilter(res.arcadeClassicFont.texture, TEXTURE_FILTER_POINT); // Dla stylu pixel art
            TraceLog(LOG_INFO, "RESOURCES: Special Font loaded ('%s').", res.arcadeClassicFontPath);
        }
        else {
            TraceLog(LOG_WARNING, "RESOURCES: Failed to load Special Font from '%s'. Special font will be unavailable.", res.arcadeClassicFontPath);
            // Nie ustawiamy GetFontDefault() tutaj, bo defaultGuiFont ju¿ j¹ ma.
        }
    }
    else {
        TraceLog(LOG_WARNING, "RESOURCES: Special Font file not found: '%s'. Special font will be unavailable.", res.arcadeClassicFontPath);
    }

    // --- Load Menu Music ---
    loadingStatus = "Loading menu audio...";
    TraceLog(LOG_INFO, "RESOURCES: Attempting to load menu music from '%s'", res.menuMusicPath);
    if (FileExists(res.menuMusicPath)) {
        res.menuMusic = LoadMusicStream(res.menuMusicPath);
        if (res.menuMusic.stream.buffer) {
            res.menuMusicLoaded = true;
            SetMusicVolume(res.menuMusic, 1.0f);
            TraceLog(LOG_INFO, "RESOURCES: Menu music loaded ('%s').", res.menuMusicPath);
        }
        else {
            TraceLog(LOG_WARNING, "RESOURCES: Failed to load menu music stream from '%s'.", res.menuMusicPath);
        }
    }
    else {
        TraceLog(LOG_WARNING, "RESOURCES: Menu music file not found: '%s'.", res.menuMusicPath);
    }

    // --- Load Settings Music ---
    loadingStatus = "Loading settings audio...";
    TraceLog(LOG_INFO, "RESOURCES: Attempting to load settings music from '%s'", res.settingsMusicPath);
    if (FileExists(res.settingsMusicPath)) {
        res.settingsMusic = LoadMusicStream(res.settingsMusicPath);
        if (res.settingsMusic.stream.buffer) {
            res.settingsMusicLoaded = true;
            SetMusicVolume(res.settingsMusic, 1.0f);
            TraceLog(LOG_INFO, "RESOURCES: Settings music loaded ('%s').", res.settingsMusicPath);
        }
        else {
            TraceLog(LOG_WARNING, "RESOURCES: Failed to load settings music stream from '%s'.", res.settingsMusicPath);
        }
    }
    else {
        TraceLog(LOG_WARNING, "RESOURCES: Settings music file not found: '%s'.", res.settingsMusicPath);
    }

    // --- Load Render Texture ---
    loadingStatus = "Initializing render target...";
    TraceLog(LOG_INFO, "RESOURCES: Creating render texture (%dx%d).", logicalWidth, logicalHeight);
    res.targetRenderTexture = LoadRenderTexture(logicalWidth, logicalHeight);
    if (res.targetRenderTexture.id > 0) {
        SetTextureFilter(res.targetRenderTexture.texture, TEXTURE_FILTER_BILINEAR);
        TraceLog(LOG_INFO, "RESOURCES: Render texture created successfully.");
    }
    else {
        TraceLog(LOG_ERROR, "RESOURCES: Failed to create render texture! This is critical.");
        allCriticalLoaded = false; // Render target is critical
    }

    loadingStatus = "Finalizing...";
    TraceLog(LOG_INFO, "RESOURCES: Main menu resource loading complete.");
    return allCriticalLoaded;
}

void UnloadMainMenuResources(MainMenuResources& res) {
    TraceLog(LOG_INFO, "RESOURCES: Unloading main menu resources...");

    if (res.loadingGifLoaded) {
        UnloadTexture(res.loadingGifTexture);
        UnloadImage(res.loadingGifImage);
        res.loadingGifLoaded = false;
    }

    if (res.menuMusicLoaded && res.menuMusic.stream.buffer) UnloadMusicStream(res.menuMusic);
    if (res.settingsMusicLoaded && res.settingsMusic.stream.buffer) UnloadMusicStream(res.settingsMusic);

    if (res.targetRenderTexture.id > 0) UnloadRenderTexture(res.targetRenderTexture);

    if (res.gifLoaded) {
        if (res.bgTexture.id > 0) UnloadTexture(res.bgTexture);
        if (res.bgGifImage.data) UnloadImage(res.bgGifImage); // Unload the raw GIF image data
    }

    if (res.helpyGifLoaded) {
        if (res.helpyTexture.id > 0) UnloadTexture(res.helpyTexture);
        if (res.helpyGifImage.data) UnloadImage(res.helpyGifImage); // Zwolnij surowe dane GIF-a
    }

    if (res.settingsBgTexture.id > 0) UnloadTexture(res.settingsBgTexture);
    
// Unload custom font only if it was loaded
    if (res.arcadeClassicFont.texture.id > 0) {
        UnloadFont(res.arcadeClassicFont);
        UnloadFont(res.bytesFont);
    }

    if (res.shaderLoadedSuccessfully && res.crtShader.id > 0) UnloadShader(res.crtShader);

    InitializeResourceStates(res); // Reset struct members

    TraceLog(LOG_INFO, "RESOURCES: Main menu resources unloaded.");
}