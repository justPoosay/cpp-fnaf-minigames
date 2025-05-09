#include "raylib.h"
#include "raymath.h"
#include "GameSettings.h"
#include "MotoristResources.h"
#include "RainbowLandResources.h"
#include "MainMenuResources.h" // Upewnij siê, ¿e ten plik jest poprawny

#include <string>
#include <vector>
#include <iostream>
#include <cmath>

// --- States ---
typedef enum GameScreen {
    LOADING,
    MAIN_MENU,
    SETTINGS,
    PLAYING_MOTORIST,
    PLAYING_RAINBOW,
    EXITING
} GameScreen;

// --- Forward declarations ---
int runMidnightMotorist(GraphicsQuality quality, Shader postProcessingShader, bool applyShader);
int runMagicRainbowLand(GraphicsQuality quality);
void StopAndUnloadMusic(Music& music, bool& loadedFlag);

// --- UI Functions ---
static bool GuiButton(Rectangle bounds, const char* text, Font font, int fontSize, Color textColor, Color bgColor, Color hoverColor, Color pressedColor) {
    bool clicked = false;
    Vector2 mousePoint = GetMousePosition();
    const int logicalWidth_const = 1280; 
    const int logicalHeight_const = 720;
    float renderTextureScaleX = (float)logicalWidth_const / GetScreenWidth();
    float renderTextureScaleY = (float)logicalHeight_const / GetScreenHeight();
    Vector2 scaledMousePoint = { mousePoint.x * renderTextureScaleX, mousePoint.y * renderTextureScaleY };

    Color currentBgColor = bgColor;
    if (CheckCollisionPointRec(scaledMousePoint, bounds)) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            currentBgColor = pressedColor;
        else
            currentBgColor = hoverColor;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            clicked = true;
    }
    DrawRectangleRec(bounds, currentBgColor);

    float textWidth = MeasureTextEx(font, text, (float)fontSize, 1).x;
    DrawTextEx(font, text,
        { bounds.x + (bounds.width / 2) - (textWidth / 2), bounds.y + (bounds.height / 2) - (float)fontSize / 2 },
        (float)fontSize, 1, textColor);
    return clicked;
}

void StopAndUnloadMusic(Music& music, bool& loadedFlag) {
    if (loadedFlag && music.stream.buffer) {
        if (IsMusicStreamPlaying(music)) {
            StopMusicStream(music);
        }
        UnloadMusicStream(music);
        music = { 0 };
        loadedFlag = false;
        TraceLog(LOG_INFO, "AUDIO: Music stream stopped and unloaded.");
    }
    else {
        loadedFlag = false;
        music = { 0 };
    }
}

int main(void) {
    InitializeGameSettings();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(g_settings.screenWidth, g_settings.screenHeight, "FNaF Minigames - Loading...");
    InitAudioDevice();

    const int logicalWidth = 1280;
    const int logicalHeight = 720;

    MainMenuResources mainMenuRes;

    bool initialLoadingScreenReady = LoadInitialLoadingScreenResources(mainMenuRes);
    if (initialLoadingScreenReady) {
        SetWindowTitle("FNaF Minigames - Loading...");
    }
    else {
        TraceLog(LOG_ERROR, "MAIN: Failed to load initial loading screen resources! Proceeding with text only.");
    }
    int loadingScreenCurrentAnimFrame = 0;
    float loadingScreenFrameDelay = 0.5f;
    float loadingScreenFrameTimer = 0.0f;

    int monitorWidth = GetMonitorWidth(GetCurrentMonitor());
    int monitorHeight = GetMonitorHeight(GetCurrentMonitor());
    SetWindowPosition((monitorWidth - g_settings.screenWidth) / 2, (monitorHeight - g_settings.screenHeight) / 2);
    SetMasterVolume(g_settings.masterVolume);
    SetTargetFPS(60);

    GameScreen currentScreen = LOADING;
    GameScreen previousScreen = currentScreen;
    bool shouldExit = false;

    int currentAnimFrame = 0; // Dla g³ównego GIF-a menu
    float frameDelay = 1.0f / 15.0f;
    float frameTimer = 0.0f;

    int helpyCurrentAnimFrame = 0;
    float helpyFrameDelay = 0.72f;
    float helpyFrameTimer = 0.0f;

    std::string loadingStatus = "Initializing...";

    float fadeAlpha = 0.0f;
    float fadeSpeed = 1.5f;
    bool isFadingOut = false;
    bool isFadingIn = false;
    GameScreen fadeNextScreen = MAIN_MENU;

    while (!shouldExit && !WindowShouldClose()) {
        if (currentScreen == LOADING) {

			// --- Loading Screen Update ---
            if (mainMenuRes.loadingGifLoaded && mainMenuRes.loadingGifAnimFrames > 1) { // Dodano warunek > 1 dla animFrames
                loadingScreenFrameTimer += GetFrameTime();

                bool frameUpdated = false;
                // Pêtla while, aby obs³u¿yæ przypadki, gdy minê³o wystarczaj¹co du¿o czasu na wiêcej ni¿ jedn¹ klatkê
                while (loadingScreenFrameTimer >= loadingScreenFrameDelay) {
                    loadingScreenFrameTimer -= loadingScreenFrameDelay;
                    loadingScreenCurrentAnimFrame = (loadingScreenCurrentAnimFrame + 1) % mainMenuRes.loadingGifAnimFrames;
                    frameUpdated = true;
                }

                if (frameUpdated) {
                    // Obliczanie rozmiaru danych jednej klatki
                    // Szerokoœæ i wysokoœæ obrazu GIF (jednej klatki)
                    int frameWidth = mainMenuRes.loadingGifImage.width;
                    int frameHeight = mainMenuRes.loadingGifImage.height;
                    // Format pikseli obrazu GIF
                    int frameFormat = mainMenuRes.loadingGifImage.format;

                    // Rozmiar danych jednej klatki w bajtach
                    int frameDataSize = GetPixelDataSize(frameWidth, frameHeight, frameFormat);

                    // WskaŸnik na dane bie¿¹cej klatki animacji
                    // mainMenuRes.loadingGifImage.data zawiera wszystkie klatki po kolei
                    unsigned char* frameDataOffset = (unsigned char*)mainMenuRes.loadingGifImage.data + (loadingScreenCurrentAnimFrame * frameDataSize);

                    // Zaktualizuj teksturê GPU nowymi danymi klatki
                    UpdateTexture(mainMenuRes.loadingGifTexture, frameDataOffset);
                }
            }

			// --- Loading Screen Drawing ---
            BeginDrawing();
            ClearBackground(BLACK);

            if (mainMenuRes.loadingGifLoaded && mainMenuRes.loadingGifTexture.id > 0) {
                float gifDrawWidth = (float)mainMenuRes.loadingGifTexture.width;
                float gifDrawHeight = (float)mainMenuRes.loadingGifTexture.height;
                // Mo¿esz dodaæ logikê skalowania tutaj, jeœli potrzebujesz
                DrawTexture(mainMenuRes.loadingGifTexture,
                    (GetScreenWidth() - gifDrawWidth) / 2,
                    (GetScreenHeight() - gifDrawHeight) / 2 - 60,
                    WHITE);
            }

            Font loadingFont = GetFontDefault();
            int textWidth = MeasureTextEx(loadingFont, loadingStatus.c_str(), 30, 1).x;
            DrawTextEx(loadingFont, loadingStatus.c_str(),
                { (float)(GetScreenWidth() - textWidth) / 2, (float)GetScreenHeight() / 2 + (mainMenuRes.loadingGifLoaded ? 50.0f : 0.0f) },
                30, 1, WHITE);
            EndDrawing();

            static bool loadAttempted = false;
            if (!loadAttempted) {
                loadAttempted = true;
                bool resourcesLoaded = LoadMainMenuResources(mainMenuRes, loadingStatus, logicalWidth, logicalHeight);

                if (!resourcesLoaded) {
                    TraceLog(LOG_ERROR, "MAIN: Critical main resources failed to load.");
                }
                if (!mainMenuRes.shaderLoadedSuccessfully) {
                    g_settings.useCRTShader = false;
                    TraceLog(LOG_WARNING, "MAIN: CRT Shader failed to load or not found. Disabling shader option.");
                }

                loadingStatus = "Finalizing...";
                frameTimer = 0.0f; currentAnimFrame = 0;

                if (mainMenuRes.helpyGifLoaded) {
                    helpyFrameTimer = 0.0f;
                    helpyCurrentAnimFrame = 0;
                }

                previousScreen = currentScreen;
                currentScreen = MAIN_MENU;
                fadeNextScreen = MAIN_MENU;
                SetWindowTitle("FNaF Minigames Port");
                isFadingIn = true;
                fadeAlpha = 1.0f;

                if (mainMenuRes.menuMusicLoaded) {
                    PlayMusicStream(mainMenuRes.menuMusic);
                }
            }
        }
        else { // Not LOADING state
            if (isFadingOut) {
                fadeAlpha += fadeSpeed * GetFrameTime();
                if (fadeAlpha >= 1.0f) {
                    fadeAlpha = 1.0f;
                    isFadingOut = false;

                    StopAndUnloadMusic(mainMenuRes.menuMusic, mainMenuRes.menuMusicLoaded);
                    StopAndUnloadMusic(mainMenuRes.settingsMusic, mainMenuRes.settingsMusicLoaded);

                    int gameResult = 0;
                    if (fadeNextScreen == PLAYING_MOTORIST) {
                        MotoristGameResources motoristRes = LoadMotoristResources(g_settings.quality);
                        gameResult = runMidnightMotorist(g_settings.quality, mainMenuRes.crtShader, g_settings.useCRTShader && mainMenuRes.shaderLoadedSuccessfully);
                        UnloadMotoristResources(motoristRes);
                    }
                    else if (fadeNextScreen == PLAYING_RAINBOW) {
                        RainbowLandGameResources rainbowRes = LoadRainbowLandResources(g_settings.quality);
                        gameResult = runMagicRainbowLand(g_settings.quality);
                        UnloadRainbowLandResources(rainbowRes);
                    }

                    previousScreen = currentScreen;
                    currentScreen = MAIN_MENU;
                    fadeNextScreen = MAIN_MENU;
                    if (IsWindowMinimized()) RestoreWindow();

                    SetMasterVolume(g_settings.masterVolume);
                    SetTargetFPS(60);

                    if (!mainMenuRes.menuMusicLoaded && FileExists(mainMenuRes.menuMusicPath)) {
                        mainMenuRes.menuMusic = LoadMusicStream(mainMenuRes.menuMusicPath);
                        if (mainMenuRes.menuMusic.stream.buffer) {
                            mainMenuRes.menuMusicLoaded = true;
                            SetMusicVolume(mainMenuRes.menuMusic, 1.0f);
                        }
                    }
                    if (mainMenuRes.menuMusicLoaded) PlayMusicStream(mainMenuRes.menuMusic);
                    isFadingIn = true;
                }
            }
            else if (isFadingIn) {
                fadeAlpha -= fadeSpeed * GetFrameTime();
                if (fadeAlpha <= 0.0f) {
                    fadeAlpha = 0.0f;
                    isFadingIn = false;
                    currentScreen = fadeNextScreen;
                }
            }
            else { // Normal Update (Not Fading) - G³ówna logika aktualizacji
                // --- Aktualizacje ogólne ---
                if (mainMenuRes.menuMusicLoaded && IsMusicStreamPlaying(mainMenuRes.menuMusic)) {
                    UpdateMusicStream(mainMenuRes.menuMusic);
                    if (GetMusicTimePlayed(mainMenuRes.menuMusic) >= GetMusicTimeLength(mainMenuRes.menuMusic) - 0.1f)
                        SeekMusicStream(mainMenuRes.menuMusic, 0.0f);
                }
                if (mainMenuRes.settingsMusicLoaded && IsMusicStreamPlaying(mainMenuRes.settingsMusic)) {
                    UpdateMusicStream(mainMenuRes.settingsMusic);
                    if (GetMusicTimePlayed(mainMenuRes.settingsMusic) >= GetMusicTimeLength(mainMenuRes.settingsMusic) - 0.1f)
                        SeekMusicStream(mainMenuRes.settingsMusic, 0.0f);
                }

                if (mainMenuRes.gifLoaded && currentScreen == MAIN_MENU) {
                    frameTimer += GetFrameTime();
                    if (frameTimer >= frameDelay) {
                        frameTimer -= frameDelay;
                        currentAnimFrame = (currentAnimFrame + 1) % mainMenuRes.animFrames;
                        int frameDataSize = GetPixelDataSize(mainMenuRes.bgGifImage.width, mainMenuRes.bgGifImage.height, mainMenuRes.bgGifImage.format);
                        unsigned char* frameDataOffset = (unsigned char*)mainMenuRes.bgGifImage.data + (currentAnimFrame * frameDataSize);
                        UpdateTexture(mainMenuRes.bgTexture, frameDataOffset);
                        if (frameTimer >= frameDelay) frameTimer = frameDelay - 0.001f;
                    }
                }

                if (IsWindowResized() && !IsWindowFullscreen()) {
                    SetWindowPosition((GetMonitorWidth(GetCurrentMonitor()) - GetScreenWidth()) / 2,
                        (GetMonitorHeight(GetCurrentMonitor()) - GetScreenHeight()) / 2);
                }

                // Aktualizacja animacji Helpy'ego
                if (mainMenuRes.helpyGifLoaded && currentScreen == SETTINGS) {
                    helpyFrameTimer += GetFrameTime();
                    if (helpyFrameTimer >= helpyFrameDelay) {
                        helpyFrameTimer -= helpyFrameDelay;
                        helpyCurrentAnimFrame = (helpyCurrentAnimFrame + 1) % mainMenuRes.helpyAnimFrames;
                        int frameDataSize = GetPixelDataSize(mainMenuRes.helpyGifImage.width, mainMenuRes.helpyGifImage.height, mainMenuRes.helpyGifImage.format);
                        unsigned char* frameDataOffset = (unsigned char*)mainMenuRes.helpyGifImage.data + (helpyCurrentAnimFrame * frameDataSize);
                        UpdateTexture(mainMenuRes.helpyTexture, frameDataOffset);
                        if (helpyFrameTimer >= helpyFrameDelay) helpyFrameTimer = helpyFrameDelay - 0.001f;
                    }
                }

                // --- Logika Inputu i Zmiany Stanu ---
                GameScreen nextScreenCandidate = currentScreen;
                Font fontForMainMenuButtons = mainMenuRes.bytesFont;
                Font fontForGeneralUI = mainMenuRes.defaultGuiFont;

                switch (currentScreen) {
                case MAIN_MENU: {
                    float buttonX = logicalWidth / 2.0f - 150;
                    float startY = 160.0f + 30;
                    float buttonSpacing = 70;
                    int buttonFontSize = 22;

                    if (GuiButton({ buttonX, startY + 0 * buttonSpacing, 300, 50 }, "Midnight Motorist", fontForMainMenuButtons, buttonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY)) {
                        isFadingOut = true; fadeAlpha = 0.0f; fadeNextScreen = PLAYING_MOTORIST;
                    }
                    if (GuiButton({ buttonX, startY + 1 * buttonSpacing, 300, 50 }, "Chica's Magic Rainbow Land", fontForMainMenuButtons, buttonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY)) {
                        isFadingOut = true; fadeAlpha = 0.0f; fadeNextScreen = PLAYING_RAINBOW;
                    }
                    if (GuiButton({ buttonX, startY + 2 * buttonSpacing, 300, 50 }, "Settings", fontForMainMenuButtons, buttonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY))
                        nextScreenCandidate = SETTINGS;
                    if (GuiButton({ buttonX, startY + 3 * buttonSpacing, 300, 50 }, "Exit", fontForMainMenuButtons, buttonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY))
                        nextScreenCandidate = EXITING;
                } break;

                case SETTINGS: {
                    Vector2 mousePos = GetMousePosition();
                    float scaleX = (float)logicalWidth / GetScreenWidth();
                    float scaleY = (float)logicalHeight / GetScreenHeight();
                    Vector2 scaledMousePos = { mousePos.x * scaleX, mousePos.y * scaleY };
                    Rectangle volumeSliderRect = { 250, 175, 300, 30 };
                    if (CheckCollisionPointRec(scaledMousePos, volumeSliderRect) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                        float mouseXRelative = scaledMousePos.x - volumeSliderRect.x;
                        g_settings.masterVolume = Clamp(mouseXRelative / volumeSliderRect.width, 0.0f, 1.0f);
                        SetMasterVolume(g_settings.masterVolume);
                    }
                    ScreenResolution tempRes = g_settings.currentResolution;
                    float resButtonY = 235; float resButtonXStart = 250; float resButtonSpacing = 110;
                    int settingsButtonFontSize = 18;

                    if (GuiButton({ resButtonXStart, resButtonY, 100, 30 }, "640x360", fontForGeneralUI, settingsButtonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY)) tempRes = RES_640x360;
                    if (GuiButton({ resButtonXStart + resButtonSpacing, resButtonY, 100, 30 }, "854x480", fontForGeneralUI, settingsButtonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY)) tempRes = RES_854x480;
                    if (GuiButton({ resButtonXStart + 2 * resButtonSpacing, resButtonY, 120, 30 }, "1280x720", fontForGeneralUI, settingsButtonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY)) tempRes = RES_1280x720;
                    if (GuiButton({ resButtonXStart + 2 * resButtonSpacing + 130, resButtonY, 120, 30 }, "1920x1080", fontForGeneralUI, settingsButtonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY)) tempRes = RES_1920x1080;

                    if (tempRes != g_settings.currentResolution) {
                        g_settings.currentResolution = tempRes; UpdateScreenDimensionsFromSettings();
                        SetWindowSize(g_settings.screenWidth, g_settings.screenHeight);
                        if (!IsWindowFullscreen()) {
                            SetWindowPosition((GetMonitorWidth(GetCurrentMonitor()) - g_settings.screenWidth) / 2, (GetMonitorHeight(GetCurrentMonitor()) - g_settings.screenHeight) / 2);
                        }
                    }

                    float qualityButtonY = 295; float qualityButtonXStart = 250; float qualityButtonSpacing = 110;
                    if (GuiButton({ qualityButtonXStart, qualityButtonY, 100, 30 }, "Low", fontForGeneralUI, settingsButtonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY)) g_settings.quality = QUALITY_LOW;
                    if (GuiButton({ qualityButtonXStart + qualityButtonSpacing, qualityButtonY, 100, 30 }, "Medium", fontForGeneralUI, settingsButtonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY)) g_settings.quality = QUALITY_MEDIUM;
                    if (GuiButton({ qualityButtonXStart + 2 * qualityButtonSpacing, qualityButtonY, 100, 30 }, "High", fontForGeneralUI, settingsButtonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY)) g_settings.quality = QUALITY_HIGH;

                    Rectangle crtToggleButton = { qualityButtonXStart, 355, 100, 30 };
                    if (GuiButton(crtToggleButton, g_settings.useCRTShader ? "ON" : "OFF", fontForGeneralUI, settingsButtonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY)) {
                        if (mainMenuRes.shaderLoadedSuccessfully) g_settings.useCRTShader = !g_settings.useCRTShader;
                    }
                    Rectangle backButton = { logicalWidth / 2.0f - 100, logicalHeight - 100, 200, 50 };
                    if (GuiButton(backButton, "Back", fontForGeneralUI, 20, BLACK, WHITE, LIGHTGRAY, GRAY) || IsKeyPressed(KEY_ESCAPE))
                        nextScreenCandidate = MAIN_MENU;
                } break;
                case EXITING: { shouldExit = true; } break;
                default: break;
                } // Koniec switch (currentScreen)

                // Logika zmiany ekranu
                if (nextScreenCandidate != currentScreen && !isFadingOut && !isFadingIn) {
                    if (currentScreen == MAIN_MENU && mainMenuRes.menuMusicLoaded) StopMusicStream(mainMenuRes.menuMusic);
                    if (currentScreen == SETTINGS && mainMenuRes.settingsMusicLoaded) StopMusicStream(mainMenuRes.settingsMusic);

                    previousScreen = currentScreen;
                    currentScreen = nextScreenCandidate;

                    if (currentScreen == MAIN_MENU) {
                        if (!mainMenuRes.menuMusicLoaded && FileExists(mainMenuRes.menuMusicPath)) {
                            mainMenuRes.menuMusic = LoadMusicStream(mainMenuRes.menuMusicPath);
                            if (mainMenuRes.menuMusic.stream.buffer) { mainMenuRes.menuMusicLoaded = true; SetMusicVolume(mainMenuRes.menuMusic, 1.0f); }
                        }
                        if (mainMenuRes.menuMusicLoaded) PlayMusicStream(mainMenuRes.menuMusic);
                    }
                    else if (currentScreen == SETTINGS) {
                        if (!mainMenuRes.settingsMusicLoaded && FileExists(mainMenuRes.settingsMusicPath)) {
                            mainMenuRes.settingsMusic = LoadMusicStream(mainMenuRes.settingsMusicPath);
                            if (mainMenuRes.settingsMusic.stream.buffer) { mainMenuRes.settingsMusicLoaded = true; SetMusicVolume(mainMenuRes.settingsMusic, 1.0f); }
                        }
                        if (mainMenuRes.settingsMusicLoaded) PlayMusicStream(mainMenuRes.settingsMusic);
                    }
                } // Koniec if (nextScreenCandidate != currentScreen ...)
            } // Koniec else dla "Normal Update (Not Fading)"
        } // Koniec else dla "Not LOADING state"

        // --- SEKCJA RYSOWANIA ---
        if (currentScreen == LOADING && !isFadingIn) {
            BeginDrawing();
            ClearBackground(BLACK);
            int textWidth = MeasureTextEx(mainMenuRes.defaultGuiFont, loadingStatus.c_str(), 30, 1).x;
            DrawTextEx(mainMenuRes.defaultGuiFont, loadingStatus.c_str(), { (float)(GetScreenWidth() - textWidth) / 2, (float)GetScreenHeight() / 2 - 15 }, 30, 1, WHITE);
            EndDrawing();
        }
        else if (mainMenuRes.targetRenderTexture.id > 0) {
            BeginTextureMode(mainMenuRes.targetRenderTexture);
            ClearBackground(RAYWHITE);

            GameScreen screenToDraw = currentScreen;
            if (isFadingOut) 
                screenToDraw = currentScreen;
            else if (isFadingIn) 
                screenToDraw = fadeNextScreen;

            Font titleDrawFont = mainMenuRes.bytesFont;
            Font uiDrawFont = mainMenuRes.defaultGuiFont;
            Font mainMenuButtonDrawFont = mainMenuRes.arcadeClassicFont;

            switch (screenToDraw) {
            case MAIN_MENU: {
                if (mainMenuRes.gifLoaded && mainMenuRes.bgTexture.id > 0)
                    DrawTexturePro(mainMenuRes.bgTexture, { 0.0f, 0.0f, (float)mainMenuRes.bgTexture.width, (float)mainMenuRes.bgTexture.height }, { 0.0f, 0.0f, (float)logicalWidth, (float)logicalHeight }, { 0, 0 }, 0.0f, WHITE);
                else
                    ClearBackground(DARKBLUE);

                float overlayWidth = 400.0f; float overlayY = 160.0f;
                float overlayX = (logicalWidth - overlayWidth) / 2.0f;
                float buttonStartY = overlayY + 30; float buttonSpacing = 70;
                float lastButtonY = buttonStartY + 3 * buttonSpacing + 50;
                float overlayHeight = (lastButtonY - overlayY) + 20;
                DrawRectangleRec({ overlayX, overlayY, overlayWidth, overlayHeight }, Fade(BLACK, 0.70f));

                const char* titleText = "FNaF Minigames Port";
                float titleFontSize = 75;
                Vector2 titleSize = MeasureTextEx(titleDrawFont, titleText, titleFontSize, 1);
                DrawTextEx(titleDrawFont, titleText, { (float)logicalWidth / 2 - titleSize.x / 2, 40 }, titleFontSize, 1, WHITE);

                float buttonX = logicalWidth / 2.0f - 150;
                int buttonFontSize = 22;

                GuiButton({ buttonX, buttonStartY + 0 * buttonSpacing, 300, 50 }, "Midnight Motorist", mainMenuButtonDrawFont, buttonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY);
                GuiButton({ buttonX, buttonStartY + 1 * buttonSpacing, 300, 50 }, "Chica's Magic Rainbow Land", mainMenuButtonDrawFont, buttonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY);
                GuiButton({ buttonX, buttonStartY + 2 * buttonSpacing, 300, 50 }, "Settings", mainMenuButtonDrawFont, buttonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY);
                GuiButton({ buttonX, buttonStartY + 3 * buttonSpacing, 300, 50 }, "Exit", mainMenuButtonDrawFont, buttonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY);
            } break;

            case SETTINGS: {
                if (mainMenuRes.settingsBgTexture.id > 0)
                    DrawTexturePro(mainMenuRes.settingsBgTexture, { 0.0f, 0.0f, (float)mainMenuRes.settingsBgTexture.width, (float)mainMenuRes.settingsBgTexture.height },
                        { 0.0f, 0.0f, (float)logicalWidth, (float)logicalHeight }, { 0, 0 }, 0.0f, WHITE);
                else ClearBackground(DARKGRAY);

                const char* settingsTitle = "SETTINGS"; 
                float settingsTitleSize = 65;
                Vector2 stSize = MeasureTextEx(mainMenuButtonDrawFont, settingsTitle, settingsTitleSize, 1);
                DrawTextEx(titleDrawFont, settingsTitle, { (float)logicalWidth / 2 - stSize.x / 2, 80 }, settingsTitleSize, 1, WHITE);

                int settingsTextFontSize = 25;
                int settingsButtonFontSize = 18;

                DrawTextEx(uiDrawFont, "Volume:", { 100, 180 }, settingsTextFontSize, 1, WHITE);
                Rectangle volumeSliderRect = { 250, 175, 300, 30 };
                DrawRectangleRec(volumeSliderRect, WHITE);
                DrawRectangleRec({ volumeSliderRect.x, volumeSliderRect.y, volumeSliderRect.width * g_settings.masterVolume, volumeSliderRect.height }, PURPLE);
                DrawRectangleLinesEx(volumeSliderRect, 2, Fade(BLACK, 0.5f));
                DrawTextEx(uiDrawFont, TextFormat("%.0f%%", g_settings.masterVolume * 100), { volumeSliderRect.x + volumeSliderRect.width + 15, 180 }, settingsTextFontSize, 1, WHITE);

                DrawTextEx(uiDrawFont, "Resolution:", { 100, 240 }, settingsTextFontSize, 1, WHITE);
                float resButtonY = 235; float resButtonXStart = 250; float resButtonSpacing = 110;
                GuiButton({ resButtonXStart, resButtonY, 100, 30 }, "640x360", uiDrawFont, settingsButtonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY);
                GuiButton({ resButtonXStart + resButtonSpacing, resButtonY, 100, 30 }, "854x480", uiDrawFont, settingsButtonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY);
                GuiButton({ resButtonXStart + 2 * resButtonSpacing, resButtonY, 120, 30 }, "1280x720", uiDrawFont, settingsButtonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY);
                GuiButton({ resButtonXStart + 2 * resButtonSpacing + 130, resButtonY, 120, 30 }, "1920x1080", uiDrawFont, settingsButtonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY);

                DrawTextEx(uiDrawFont, "Quality:", { 100, 300 }, settingsTextFontSize, 1, WHITE);
                float qualityButtonY = 295; float qualityButtonXStart = 250; float qualityButtonSpacing = 110;
                GuiButton({ qualityButtonXStart, qualityButtonY, 100, 30 }, "Low", uiDrawFont, settingsButtonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY);
                GuiButton({ qualityButtonXStart + qualityButtonSpacing, qualityButtonY, 100, 30 }, "Medium", uiDrawFont, settingsButtonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY);
                GuiButton({ qualityButtonXStart + 2 * qualityButtonSpacing, qualityButtonY, 100, 30 }, "High", uiDrawFont, settingsButtonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY);

                DrawTextEx(uiDrawFont, "Shaders:", { 100, 360 }, settingsTextFontSize, 1, WHITE);
                Rectangle crtToggleButton = { qualityButtonXStart, 355, 100, 30 };
                GuiButton(crtToggleButton, g_settings.useCRTShader ? "ON" : "OFF", uiDrawFont, settingsButtonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY);
                if (!mainMenuRes.shaderLoadedSuccessfully)
                    DrawTextEx(uiDrawFont, "(Shader N/A)", { crtToggleButton.x + 110, 360 }, settingsTextFontSize, 1, LIGHTGRAY);

                if (mainMenuRes.helpyGifLoaded && mainMenuRes.helpyTexture.id > 0) {
                    float helpyDrawWidthOnScreen = 200.0f;
                    float helpyDrawHeightOnScreen = 200.0f;
                    float helpyOriginalAspectRatio = (float)mainMenuRes.helpyTexture.width / mainMenuRes.helpyTexture.height;
                    float targetAspectRatio = helpyDrawWidthOnScreen / helpyDrawHeightOnScreen;
                    float finalDrawWidth = helpyDrawWidthOnScreen;
                    float finalDrawHeight = helpyDrawHeightOnScreen;

                    if (helpyOriginalAspectRatio > targetAspectRatio) {
                        finalDrawHeight = helpyDrawWidthOnScreen / helpyOriginalAspectRatio;
                    }
                    else {
                        finalDrawWidth = helpyDrawHeightOnScreen * helpyOriginalAspectRatio;
                    }
                    float paddingX = -10.0f; // Odstêp od prawej krawêdzi
                    float paddingY = 15.0f; // Odstêp od dolnej krawêdzi

                    Rectangle helpyDestinationRect = {
                        logicalWidth - finalDrawWidth - paddingX,
                        logicalHeight - finalDrawHeight - paddingY,
                        finalDrawWidth,
                        finalDrawHeight
                    };
                    Rectangle helpySourceRect = { 0.0f, 0.0f, (float)mainMenuRes.helpyTexture.width, (float)mainMenuRes.helpyTexture.height };
                    DrawTexturePro(mainMenuRes.helpyTexture, helpySourceRect, helpyDestinationRect, { 0,0 }, 0.0f, WHITE);
                }

                Rectangle backButton = { logicalWidth / 2.0f - 100, logicalHeight - 100, 200, 50 };
                GuiButton(backButton, "Back", uiDrawFont, 20, BLACK, WHITE, LIGHTGRAY, GRAY);
            } break;
            case PLAYING_MOTORIST:
            case PLAYING_RAINBOW:
                ClearBackground(BLACK);
                break;
            case EXITING: {
                ClearBackground(BLACK);
                const char* exitMsg = "Exiting..."; float exitMsgSize = 20;
                Vector2 emSize = MeasureTextEx(uiDrawFont, exitMsg, exitMsgSize, 1);
                DrawTextEx(uiDrawFont, exitMsg, { (float)logicalWidth / 2 - emSize.x / 2, (float)logicalHeight / 2 - emSize.y / 2 }, exitMsgSize, 1, LIGHTGRAY);
            } break;
            default: break;
            }
            EndTextureMode();

            BeginDrawing();
            ClearBackground(BLACK);

            float scale = fmin((float)GetScreenWidth() / logicalWidth, (float)GetScreenHeight() / logicalHeight);
            float scaledWidth = logicalWidth * scale;
            float scaledHeight = logicalHeight * scale;
            Vector2 drawPos = { (GetScreenWidth() - scaledWidth) / 2.0f, (GetScreenHeight() - scaledHeight) / 2.0f };
            Rectangle sourceRect = { 0.0f, 0.0f, (float)mainMenuRes.targetRenderTexture.texture.width, -(float)mainMenuRes.targetRenderTexture.texture.height };
            Rectangle destRect = { drawPos.x, drawPos.y, scaledWidth, scaledHeight };

            DrawTexturePro(mainMenuRes.targetRenderTexture.texture, sourceRect, destRect, { 0,0 }, 0.0f, WHITE);

            if (isFadingOut || isFadingIn)
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, fadeAlpha));

            DrawFPS(10, 10);
            EndDrawing();
        } // Koniec else if (mainMenuRes.targetRenderTexture.id > 0)
    } // Koniec pêtli while (!shouldExit && !WindowShouldClose())

    TraceLog(LOG_INFO, "CLEANUP: Starting main cleanup...");
    UnloadMainMenuResources(mainMenuRes);

    CloseAudioDevice();
    CloseWindow();
    TraceLog(LOG_INFO, "CLEANUP: Finished. Exiting application.");

    return 0;
}