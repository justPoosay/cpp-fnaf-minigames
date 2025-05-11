#include "raylib.h"
#include "raymath.h"

#include "GameSettings.h"

#include "MainMenuResources.h"
#include "MotoristResources.h"
#include "RainbowLandResources.h"

// --- Game states ---
typedef enum GameScreen {
    LOADING,
    MAIN_MENU,
    SETTINGS,
    PLAYING_MOTORIST,
    PLAYING_RAINBOW,
    EXITING
} GameScreen;

int runMidnightMotorist(GraphicsQuality quality, Shader postProcessingShader, bool applyShader);
int runMagicRainbowLand(GraphicsQuality quality);
void StopAndUnloadMusic(Music& music, bool& loadedFlag);


// --- UI Data  ---
const char* resolutionOptions[] = { "1280x720", "1920x1080" };
const ScreenResolution resolutionValues[] = { RES_1280x720, RES_1920x1080 };
const int numResolutionOptions = sizeof(resolutionOptions) / sizeof(resolutionOptions[0]);
int currentResolutionIndex = 2; // Default - 1280x720

const char* qualityOptions[] = { "Low", "Medium", "High" };
const GraphicsQuality qualityValues[] = { QUALITY_LOW, QUALITY_MEDIUM, QUALITY_HIGH };
const int numQualityOptions = sizeof(qualityOptions) / sizeof(qualityOptions[0]);
int currentQualityIndex = 1; // Default - Medium

static bool settingsAreInitialized = false;

// --- UI Functions ---
static bool GuiButton(Rectangle bounds, const char* text, Font font, int fontSize, Color textColor, Color bgColor, Color hoverColor, Color pressedColor, Vector2 scaledMousePos) {
    bool clicked = false;
    Color currentBgColor = bgColor;

    if (CheckCollisionPointRec(scaledMousePos, bounds)) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            currentBgColor = pressedColor;
        else
            currentBgColor = hoverColor;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            clicked = true;
    }
    DrawRectangleRec(bounds, currentBgColor);

    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1);
    DrawTextEx(font, text,
        { bounds.x + (bounds.width / 2) - (textSize.x / 2), bounds.y + (bounds.height / 2) - (textSize.y / 2) },
        fontSize, 1, textColor);
    return clicked;
}

static int GuiSelector(Rectangle bounds, const char* currentOptionText, Font font, int optionFontSize,
    Color textColor, Color boxColor, Color arrowColor, Color arrowHoverColor, Color arrowPressedColor,
    Vector2 scaledMousePos)
{
    int clicked_direction = 0;
    float arrowPaddingFromBox = 4;
    float arrowHotspotExtraPadding = 6;

    DrawRectangleRec(bounds, boxColor);
    DrawRectangleLinesEx(bounds, 1, Fade(BLACK, 0.3));

    Vector2 optionTextSize = MeasureTextEx(font, currentOptionText, optionFontSize, 1);
    DrawTextEx(font, currentOptionText,
        { bounds.x + (bounds.width / 2) - (optionTextSize.x / 2),
          bounds.y + (bounds.height / 2) - (optionTextSize.y / 2) },
        optionFontSize, 1, textColor);

    int arrowFontSize = optionFontSize * 2;
    if (arrowFontSize < 12) arrowFontSize = 12;

    const char* leftArrowStr = "<";
    Vector2 leftArrowTextSize = MeasureTextEx(font, leftArrowStr, arrowFontSize, 1);

    float leftArrowHotspotWidth = leftArrowTextSize.x + 2 * arrowHotspotExtraPadding;
    float leftArrowHotspotHeight = fmaxf(bounds.height, leftArrowTextSize.y + 2 * arrowHotspotExtraPadding);

    Rectangle leftArrowHotspot = {
        bounds.x - leftArrowHotspotWidth - arrowPaddingFromBox,
        bounds.y + (bounds.height / 2) - (leftArrowHotspotHeight / 2),
        leftArrowHotspotWidth,
        leftArrowHotspotHeight
    };
    Color currentLeftArrowColor = arrowColor;

    if (CheckCollisionPointRec(scaledMousePos, leftArrowHotspot)) {
        currentLeftArrowColor = arrowHoverColor;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            currentLeftArrowColor = arrowPressedColor;

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            clicked_direction = -1;
    }

    DrawTextEx(font, leftArrowStr,
        { leftArrowHotspot.x + (leftArrowHotspot.width / 2) - (leftArrowTextSize.x / 2),
          leftArrowHotspot.y + (leftArrowHotspot.height / 2) - (leftArrowTextSize.y / 2) },
        arrowFontSize, 1, currentLeftArrowColor);

    const char* rightArrowStr = ">";
    Vector2 rightArrowTextSize = MeasureTextEx(font, rightArrowStr, arrowFontSize, 1);

    float rightArrowHotspotWidth = rightArrowTextSize.x + 2 * arrowHotspotExtraPadding;
    float rightArrowHotspotHeight = fmaxf(bounds.height, rightArrowTextSize.y + 2 * arrowHotspotExtraPadding);

    Rectangle rightArrowHotspot = {
        bounds.x + bounds.width + arrowPaddingFromBox,
        bounds.y + (bounds.height / 2) - (rightArrowHotspotHeight / 2),
        rightArrowHotspotWidth,
        rightArrowHotspotHeight
    };
    Color currentRightArrowColor = arrowColor;

    if (CheckCollisionPointRec(scaledMousePos, rightArrowHotspot)) {
        currentRightArrowColor = arrowHoverColor;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            currentRightArrowColor = arrowPressedColor;

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && clicked_direction == 0)
            clicked_direction = 1;
    }

    DrawTextEx(font, rightArrowStr, { rightArrowHotspot.x + (rightArrowHotspot.width / 2) - (rightArrowTextSize.x / 2), rightArrowHotspot.y + (rightArrowHotspot.height / 2) - (rightArrowTextSize.y / 2) }, arrowFontSize, 1, currentRightArrowColor);

    return clicked_direction;
}

void StopAndUnloadMusic(Music& music, bool& loadedFlag) {
    if (loadedFlag && music.stream.buffer) {
        if (IsMusicStreamPlaying(music))
            StopMusicStream(music);

        UnloadMusicStream(music);
        music = { 0 };
        loadedFlag = false;
        TraceLog(LOG_INFO, "AUDIO: Music stream stopped and unloaded.");
    }
    else {
        music = { 0 };
        loadedFlag = false;
    }
}

int main(void) {
    InitializeGameSettings();

    for (int i = 0; i < numResolutionOptions; i++) {
        if (g_settings.currentResolution == resolutionValues[i]) {
            currentResolutionIndex = i;
            break;
        }
    }
    for (int i = 0; i < numQualityOptions; i++) {
        if (g_settings.quality == qualityValues[i]) {
            currentQualityIndex = i;
            break;
        }
    }

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(g_settings.screenWidth, g_settings.screenHeight, "FNaF Minigames - Loading...");
    InitAudioDevice();

    MainMenuResources mainMenuRes;

    bool initialStaticLoadScreenReady = LoadInitialLoadingScreenResources(mainMenuRes);
    if (initialStaticLoadScreenReady && mainMenuRes.helpyLoadingTextureLoaded) {
        SetWindowTitle("FNaF Minigames - Loading...");
    }
    else {
        TraceLog(LOG_ERROR, "MAIN: Failed to load initial STATIC loading screen texture!");
    }

    float loadingScreenMinVisibleTime = 2;
    float loadingScreenVisibleTimer = 0;

    int monitorWidth = GetMonitorWidth(GetCurrentMonitor());
    int monitorHeight = GetMonitorHeight(GetCurrentMonitor());
    SetWindowPosition((monitorWidth - g_settings.screenWidth) / 2, (monitorHeight - g_settings.screenHeight) / 2);
    SetMasterVolume(g_settings.masterVolume);
    SetTargetFPS(60);

    GameScreen currentScreen = LOADING;
    GameScreen previousScreen = currentScreen;
    bool shouldExit = false;

    int mainMenuBgCurrentFrame = 0;
    float mainMenuBgFrameDelay = 0.067;
    float mainMenuBgFrameTimer = 0;

    int helpyCurrentAnimFrame = 0;
    float helpyFrameDelay = 0.725;
    float helpyFrameTimer = 0;

    float fadeAlpha = 0;
    float fadeSpeed = 1.5;
    bool isFadingOut = false;
    bool isFadingIn = false;
    GameScreen fadeNextScreen = MAIN_MENU;

    bool mainResourcesLoadAttempted = false;
    bool mainResourcesActuallyLoaded = false;

    while (!shouldExit && !WindowShouldClose()) {
        Vector2 mousePointWindow = GetMousePosition();
        float mouseScaleX = (float)logicalWidth / GetScreenWidth();
        float mouseScaleY = (float)logicalHeight / GetScreenHeight();
        Vector2 scaledMousePos = { mousePointWindow.x * mouseScaleX, mousePointWindow.y * mouseScaleY };

        if (isFadingOut) {
            fadeAlpha += fadeSpeed * GetFrameTime();
            if (fadeAlpha >= 1) {
                fadeAlpha = 1;
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
                currentScreen = fadeNextScreen;

                fadeNextScreen = MAIN_MENU;

                if (IsWindowMinimized())
                    RestoreWindow();
                SetMasterVolume(g_settings.masterVolume);
                SetTargetFPS(60);

                if (!mainMenuRes.menuMusicLoaded && mainMenuRes.menuMusicPath && FileExists(mainMenuRes.menuMusicPath)) {
                    mainMenuRes.menuMusic = LoadMusicStream(mainMenuRes.menuMusicPath);
                    if (mainMenuRes.menuMusic.stream.buffer) {
                        mainMenuRes.menuMusicLoaded = true;
                        SetMusicVolume(mainMenuRes.menuMusic, 1);
                    }
                }
                if (mainMenuRes.menuMusicLoaded)
                    PlayMusicStream(mainMenuRes.menuMusic);

                isFadingIn = true;
            }
        }
        else if (isFadingIn) {
            fadeAlpha -= fadeSpeed * GetFrameTime();
            if (fadeAlpha <= 0) {
                fadeAlpha = 0;
                isFadingIn = false;
                currentScreen = fadeNextScreen;

                if (previousScreen == LOADING && currentScreen == MAIN_MENU)
                    if (mainMenuRes.menuMusicLoaded)
                        PlayMusicStream(mainMenuRes.menuMusic);
            }
        }

        if (!isFadingOut && !isFadingIn) {
            switch (currentScreen) {
            case LOADING: {
                loadingScreenVisibleTimer += GetFrameTime();

                if (!mainResourcesLoadAttempted && loadingScreenVisibleTimer > 0) {
                    mainResourcesLoadAttempted = true;

                    TraceLog(LOG_INFO, "MAIN: Starting LoadMainMenuResources...");
                    mainResourcesActuallyLoaded = LoadMainMenuResources(mainMenuRes, logicalWidth, logicalHeight);
                    TraceLog(LOG_INFO, "MAIN: Finished LoadMainMenuResources. Success: %s", mainResourcesActuallyLoaded ? "true" : "false");

                    if (!mainResourcesActuallyLoaded) {
                        TraceLog(LOG_ERROR, "MAIN: Critical main resources failed to load.");
                    }
                    if (!mainMenuRes.shaderLoadedSuccessfully) {
                        g_settings.useCRTShader = false;
                        TraceLog(LOG_WARNING, "MAIN: CRT Shader failed to load or not found. Disabling shader option.");
                    }

                    mainMenuBgFrameTimer = 0;
                    mainMenuBgCurrentFrame = 0;
                    if (mainMenuRes.helpyGifLoaded) {
                        helpyFrameTimer = 0;
                        helpyCurrentAnimFrame = 0;
                    }
                    SetWindowTitle("FNaF Minigames Port");
                }

                if (mainResourcesLoadAttempted && loadingScreenVisibleTimer >= loadingScreenMinVisibleTime) {
                    if (!isFadingIn) {
                        previousScreen = LOADING;
                        fadeNextScreen = MAIN_MENU;
                        isFadingIn = true;
                        fadeAlpha = 1;
                    }
                }
            } break;

            case MAIN_MENU: {
                Font fontForMainMenuButtons = mainMenuRes.arcadeClassicFont;
                if (mainMenuRes.menuMusicLoaded && IsMusicStreamPlaying(mainMenuRes.menuMusic)) {
                    UpdateMusicStream(mainMenuRes.menuMusic);
                    if (GetMusicTimePlayed(mainMenuRes.menuMusic) >= GetMusicTimeLength(mainMenuRes.menuMusic))
                        SeekMusicStream(mainMenuRes.menuMusic, 0);
                }
                if (mainMenuRes.gifLoaded && mainMenuRes.bgTexture.id > 0 && mainMenuRes.bgGifImage.data) {
                    mainMenuBgFrameTimer += GetFrameTime();
                    bool bgFrameUpdated = false;
                    while (mainMenuBgFrameTimer >= mainMenuBgFrameDelay) {
                        mainMenuBgFrameTimer -= mainMenuBgFrameDelay;
                        mainMenuBgCurrentFrame = (mainMenuBgCurrentFrame + 1) % mainMenuRes.animFrames;
                        bgFrameUpdated = true;
                    }

                    if (bgFrameUpdated) {
                        int frameDataSize = GetPixelDataSize(mainMenuRes.bgGifImage.width, mainMenuRes.bgGifImage.height, mainMenuRes.bgGifImage.format);
                        unsigned char* frameDataOffset = (unsigned char*)mainMenuRes.bgGifImage.data + (mainMenuBgCurrentFrame * frameDataSize);
                        UpdateTexture(mainMenuRes.bgTexture, frameDataOffset);
                    }
                }

                if (IsWindowResized() && !IsWindowFullscreen()) {
                    SetWindowPosition((GetMonitorWidth(GetCurrentMonitor()) - GetScreenWidth()) / 2,
                        (GetMonitorHeight(GetCurrentMonitor()) - GetScreenHeight()) / 2);
                }

                float buttonX = logicalWidth / 2 - 150;
                float startY = 160 + 30;
                float buttonSpacing = 70;
                int buttonFontSize = 22;

                if (GuiButton({ buttonX, startY + 0 * buttonSpacing, 300, 50 }, "Midnight Motorist", fontForMainMenuButtons, buttonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY, scaledMousePos)) {
                    previousScreen = currentScreen;
                    fadeNextScreen = PLAYING_MOTORIST;
                    isFadingOut = true; fadeAlpha = 0;
                }

                if (GuiButton({ buttonX, startY + 1 * buttonSpacing, 300, 50 }, "Chica's Magic Rainbow Land", fontForMainMenuButtons, buttonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY, scaledMousePos)) {
                    previousScreen = currentScreen;
                    fadeNextScreen = PLAYING_RAINBOW;
                    isFadingOut = true; fadeAlpha = 0;
                }

                if (GuiButton({ buttonX, startY + 2 * buttonSpacing, 300, 50 }, "Settings", fontForMainMenuButtons, buttonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY, scaledMousePos)) {
                    settingsAreInitialized = false;
                    if (mainMenuRes.menuMusicLoaded)
                        StopMusicStream(mainMenuRes.menuMusic);

                    previousScreen = currentScreen;
                    currentScreen = SETTINGS;
                    if (!mainMenuRes.settingsMusicLoaded && mainMenuRes.settingsMusicPath && FileExists(mainMenuRes.settingsMusicPath)) {
                        mainMenuRes.settingsMusic = LoadMusicStream(mainMenuRes.settingsMusicPath);

                        if (mainMenuRes.settingsMusic.stream.buffer) {
                            mainMenuRes.settingsMusicLoaded = true;
                            SetMusicVolume(mainMenuRes.settingsMusic, 1);
                        }
                    }
                    if (mainMenuRes.settingsMusicLoaded)
                        PlayMusicStream(mainMenuRes.settingsMusic);
                }

                if (GuiButton({ buttonX, startY + 3 * buttonSpacing, 300, 50 }, "Exit", fontForMainMenuButtons, buttonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY, scaledMousePos)) {
                    previousScreen = currentScreen;
                    currentScreen = EXITING;
                }
            } break;

            case SETTINGS: {
                Font fontForGeneralUI = mainMenuRes.defaultGuiFont;
                if (!settingsAreInitialized) {
                    for (int i = 0; i < numResolutionOptions; i++) {
                        if (g_settings.currentResolution == resolutionValues[i]) {
                            currentResolutionIndex = i;
                            break;
                        }
                    }
                    for (int i = 0; i < numQualityOptions; i++) {
                        if (g_settings.quality == qualityValues[i]) {
                            currentQualityIndex = i;
                            break;
                        }
                    }
                    settingsAreInitialized = true;
                }

                if (mainMenuRes.settingsMusicLoaded && IsMusicStreamPlaying(mainMenuRes.settingsMusic)) {
                    UpdateMusicStream(mainMenuRes.settingsMusic);

                    if (GetMusicTimePlayed(mainMenuRes.settingsMusic) >= GetMusicTimeLength(mainMenuRes.settingsMusic))
                        SeekMusicStream(mainMenuRes.settingsMusic, 0);
                }

                if (mainMenuRes.helpyGifLoaded && mainMenuRes.helpyTexture.id > 0 && mainMenuRes.helpyGifImage.data) {
                    helpyFrameTimer += GetFrameTime();
                    bool helpyFrameUpdated = false;

                    while (helpyFrameTimer >= helpyFrameDelay) {
                        helpyFrameTimer -= helpyFrameDelay;
                        helpyCurrentAnimFrame = (helpyCurrentAnimFrame + 1) % mainMenuRes.helpyAnimFrames;
                        helpyFrameUpdated = true;
                    }

                    if (helpyFrameUpdated) {
                        int frameDataSize = GetPixelDataSize(mainMenuRes.helpyGifImage.width, mainMenuRes.helpyGifImage.height, mainMenuRes.helpyGifImage.format);
                        unsigned char* frameDataOffset = (unsigned char*)mainMenuRes.helpyGifImage.data + (helpyCurrentAnimFrame * frameDataSize);
                        UpdateTexture(mainMenuRes.helpyTexture, frameDataOffset);
                    }
                }

                Rectangle volumeSliderRect = { 250, 175, 300, 30 };

                if (CheckCollisionPointRec(scaledMousePos, volumeSliderRect) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    float mouseXRelative = scaledMousePos.x - volumeSliderRect.x;
                    g_settings.masterVolume = Clamp(mouseXRelative / volumeSliderRect.width, 0, 1);
                    SetMasterVolume(g_settings.masterVolume);
                }

                Rectangle resolutionSelectorBounds = { selectorX, 235, selectorWidth, selectorHeight };
                int resolutionChange = GuiSelector(resolutionSelectorBounds, resolutionOptions[currentResolutionIndex], fontForGeneralUI, settingsSelectorFontSize, BLACK, WHITE, DARKGRAY, GRAY, BLACK, scaledMousePos);

                if (resolutionChange != 0) {
                    currentResolutionIndex += resolutionChange;

                    if (currentResolutionIndex < 0)
                        currentResolutionIndex = numResolutionOptions - 1;

                    if (currentResolutionIndex >= numResolutionOptions)
                        currentResolutionIndex = 0;

                    g_settings.currentResolution = resolutionValues[currentResolutionIndex];
                    UpdateScreenDimensionsFromSettings();
                    SetWindowSize(g_settings.screenWidth, g_settings.screenHeight);

                    if (!IsWindowFullscreen())
                        SetWindowPosition((GetMonitorWidth(GetCurrentMonitor()) - g_settings.screenWidth) / 2, (GetMonitorHeight(GetCurrentMonitor()) - g_settings.screenHeight) / 2);
                }

                Rectangle qualitySelectorBounds = { selectorX, 295, selectorWidth, selectorHeight };
                int qualityChange = GuiSelector(qualitySelectorBounds, qualityOptions[currentQualityIndex], fontForGeneralUI, settingsSelectorFontSize, BLACK, WHITE, DARKGRAY, GRAY, BLACK, scaledMousePos);

                if (qualityChange != 0) {
                    currentQualityIndex += qualityChange;

                    if (currentQualityIndex < 0)
                        currentQualityIndex = numQualityOptions - 1;

                    if (currentQualityIndex >= numQualityOptions)
                        currentQualityIndex = 0;

                    g_settings.quality = qualityValues[currentQualityIndex];
                }

                Rectangle crtToggleButton = { selectorX, 355, 100, 30 };
                if (GuiButton(crtToggleButton, g_settings.useCRTShader ? "ON" : "OFF", fontForGeneralUI, settingsSelectorFontSize, BLACK, WHITE, LIGHTGRAY, GRAY, scaledMousePos))
                    if (mainMenuRes.shaderLoadedSuccessfully)
                        g_settings.useCRTShader = !g_settings.useCRTShader;


                Rectangle backButton = { logicalWidth / 2 - 100, logicalHeight - 100, 200, 50 };
                if (GuiButton(backButton, "Back", fontForGeneralUI, 20, BLACK, WHITE, LIGHTGRAY, GRAY, scaledMousePos) || IsKeyPressed(KEY_ESCAPE)) {
                    settingsAreInitialized = false;

                    if (mainMenuRes.settingsMusicLoaded)
                        StopMusicStream(mainMenuRes.settingsMusic);

                    previousScreen = currentScreen;
                    currentScreen = MAIN_MENU;
                    if (!mainMenuRes.menuMusicLoaded && mainMenuRes.menuMusicPath && FileExists(mainMenuRes.menuMusicPath)) {
                        mainMenuRes.menuMusic = LoadMusicStream(mainMenuRes.menuMusicPath);

                        if (mainMenuRes.menuMusic.stream.buffer) {
                            mainMenuRes.menuMusicLoaded = true;
                            SetMusicVolume(mainMenuRes.menuMusic, 1);
                        }
                    }

                    if (mainMenuRes.menuMusicLoaded)
                        PlayMusicStream(mainMenuRes.menuMusic);

                    helpyCurrentAnimFrame = 0;
                    helpyFrameTimer = 0.0;
                }
            } break;

            case EXITING: {
                shouldExit = true;
            } break;
            default: break;
            }
        }


        // --- DRAWING SECTION ---
        BeginDrawing();
        ClearBackground(BLACK);

        if (currentScreen == LOADING) {
            if (mainMenuRes.helpyLoadingTextureLoaded && mainMenuRes.helpyLoadingScreenTexture.id > 0) {
                float imgDrawWidth = mainMenuRes.helpyLoadingScreenTexture.width;
                float imgDrawHeight = mainMenuRes.helpyLoadingScreenTexture.height;
                DrawTexture(mainMenuRes.helpyLoadingScreenTexture, (GetScreenWidth() - imgDrawWidth) / 2, (GetScreenHeight() - imgDrawHeight) / 2 - 60, WHITE);
            }
            else {
                const char* errMsg = "Loading...";
                DrawText(errMsg, GetScreenWidth() / 2 - MeasureText(errMsg, 40) / 2, GetScreenHeight() / 2 - 20, 40, LIGHTGRAY);
            }
        }
        else if (mainMenuRes.targetRenderTexture.id > 0) {
            BeginTextureMode(mainMenuRes.targetRenderTexture);
            ClearBackground(RAYWHITE);

            GameScreen screenContentToDraw = currentScreen;
            if (isFadingOut) screenContentToDraw = previousScreen;
            else if (isFadingIn) screenContentToDraw = fadeNextScreen;

            Font titleDrawFont = mainMenuRes.bytesFont;
            Font uiDrawFont = mainMenuRes.defaultGuiFont;
            Font mainMenuButtonDrawFont = mainMenuRes.arcadeClassicFont;

            switch (screenContentToDraw) {
            case MAIN_MENU: {
                if (mainMenuRes.gifLoaded && mainMenuRes.bgTexture.id > 0)
                    DrawTexturePro(mainMenuRes.bgTexture, { 0, 0, (float)mainMenuRes.bgTexture.width, (float)mainMenuRes.bgTexture.height }, { 0, 0, (float)logicalWidth, (float)logicalHeight }, { 0, 0 }, 0, WHITE);
                else
                    ClearBackground(DARKBLUE);

                float overlayWidth = 400; float overlayY = 160;
                float overlayX = (logicalWidth - overlayWidth) / 2;
                float buttonStartY = overlayY + 30; float buttonSpacing = 70;
                float lastButtonY = buttonStartY + 3 * buttonSpacing + 50;
                float overlayHeight = (lastButtonY - overlayY) + 20;
                DrawRectangleRec({ overlayX, overlayY, overlayWidth, overlayHeight }, Fade(BLACK, 0.70f));

                const char* titleText = "FNaF Minigames Port";
                float titleFontSize = 75;
                Vector2 titleSize = MeasureTextEx(titleDrawFont, titleText, titleFontSize, 1);
                DrawTextEx(titleDrawFont, titleText, { logicalWidth / 2 - titleSize.x / 2, 40 }, titleFontSize, 1, WHITE);

                float buttonX = logicalWidth / 2 - 150;
                int buttonFontSize = 22;

                GuiButton({ buttonX, buttonStartY + 0 * buttonSpacing, 300, 50 }, "Midnight Motorist", mainMenuButtonDrawFont, buttonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY, scaledMousePos);
                GuiButton({ buttonX, buttonStartY + 1 * buttonSpacing, 300, 50 }, "Chica's Magic Rainbow Land", mainMenuButtonDrawFont, buttonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY, scaledMousePos);
                GuiButton({ buttonX, buttonStartY + 2 * buttonSpacing, 300, 50 }, "Settings", mainMenuButtonDrawFont, buttonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY, scaledMousePos);
                GuiButton({ buttonX, buttonStartY + 3 * buttonSpacing, 300, 50 }, "Exit", mainMenuButtonDrawFont, buttonFontSize, BLACK, WHITE, LIGHTGRAY, GRAY, scaledMousePos);
            } break;
            case SETTINGS: {
                if (mainMenuRes.settingsBgTexture.id > 0)
                    DrawTexturePro(mainMenuRes.settingsBgTexture, { 0, 0, (float)mainMenuRes.settingsBgTexture.width, (float)mainMenuRes.settingsBgTexture.height },
                        { 0, 0, logicalWidth, logicalHeight }, { 0, 0 }, 0, WHITE);
                else ClearBackground(DARKGRAY);

                const char* settingsTitle = "SETTINGS";
                float settingsTitleSize = 65;
                Vector2 stSize = MeasureTextEx(titleDrawFont, settingsTitle, settingsTitleSize, 1);
                DrawTextEx(titleDrawFont, settingsTitle, { logicalWidth / 2 - stSize.x / 2, 80 }, settingsTitleSize, 1, WHITE);

                DrawTextEx(uiDrawFont, "Volume:", { 100, 180 }, settingsTextFontSize, 1, WHITE);
                Rectangle volumeSliderRect = { 250, 175, 300, 30 };
                DrawRectangleRec(volumeSliderRect, WHITE);
                DrawRectangleRec({ volumeSliderRect.x, volumeSliderRect.y, volumeSliderRect.width * g_settings.masterVolume, volumeSliderRect.height }, PURPLE);
                DrawRectangleLinesEx(volumeSliderRect, 2, Fade(BLACK, 0.5));
                DrawTextEx(uiDrawFont, TextFormat("%%%", g_settings.masterVolume * 100), { volumeSliderRect.x + volumeSliderRect.width + 15, 180 }, settingsTextFontSize, 1, WHITE);

                DrawTextEx(uiDrawFont, "Resolution:", { 100, 240 }, settingsTextFontSize, 1, WHITE);
                float selectorWidth = 125;
                float selectorHeight = 30;
                float selectorX = 250;
                Rectangle resolutionSelectorBounds = { selectorX, 235, selectorWidth, selectorHeight };
                GuiSelector(resolutionSelectorBounds, resolutionOptions[currentResolutionIndex],
                    uiDrawFont, settingsSelectorFontSize, BLACK, WHITE, DARKGRAY, GRAY, BLACK, scaledMousePos);

                DrawTextEx(uiDrawFont, "Quality:", { 100, 300 }, settingsTextFontSize, 1, WHITE);
                Rectangle qualitySelectorBounds = { selectorX, 295, selectorWidth, selectorHeight };
                GuiSelector(qualitySelectorBounds, qualityOptions[currentQualityIndex],
                    uiDrawFont, settingsSelectorFontSize, BLACK, WHITE, DARKGRAY, GRAY, BLACK, scaledMousePos);

                DrawTextEx(uiDrawFont, "Shaders:", { 100, 360 }, settingsTextFontSize, 1, WHITE);
                Rectangle crtToggleButton = { selectorX, 355, 100, 30 };
                GuiButton(crtToggleButton, g_settings.useCRTShader ? "ON" : "OFF", uiDrawFont, settingsSelectorFontSize, BLACK, WHITE, LIGHTGRAY, GRAY, scaledMousePos);
                if (!mainMenuRes.shaderLoadedSuccessfully)
                    DrawTextEx(uiDrawFont, "(Shader N/A)", { crtToggleButton.x + crtToggleButton.width + 10, 360 }, 18, 1, LIGHTGRAY);


                if (mainMenuRes.helpyGifLoaded && mainMenuRes.helpyTexture.id > 0) {
                    float helpyDrawWidthOnScreen = 200;
                    float helpyDrawHeightOnScreen = 200;
                    float helpyOriginalAspectRatio = (float)mainMenuRes.helpyTexture.width / mainMenuRes.helpyTexture.height;

                    float finalDrawWidth, finalDrawHeight;
                    if (helpyOriginalAspectRatio > 0) {
                        if (mainMenuRes.helpyTexture.width > mainMenuRes.helpyTexture.height) {
                            finalDrawWidth = helpyDrawWidthOnScreen;
                            finalDrawHeight = helpyDrawWidthOnScreen / helpyOriginalAspectRatio;
                        }
                        else {
                            finalDrawHeight = helpyDrawHeightOnScreen;
                            finalDrawWidth = helpyDrawHeightOnScreen * helpyOriginalAspectRatio;
                        }
                        if (finalDrawHeight > helpyDrawHeightOnScreen) {
                            finalDrawHeight = helpyDrawHeightOnScreen;
                            finalDrawWidth = finalDrawHeight * helpyOriginalAspectRatio;
                        }
                        if (finalDrawWidth > helpyDrawWidthOnScreen) {
                            finalDrawWidth = helpyDrawWidthOnScreen;
                            finalDrawHeight = finalDrawWidth / helpyOriginalAspectRatio;
                        }
                    }
                    else {
                        finalDrawWidth = helpyDrawWidthOnScreen;
                        finalDrawHeight = helpyDrawHeightOnScreen;
                    }

                    float paddingX = -10;
                    float paddingY = 15;

                    Rectangle helpyDestinationRect = {
                        logicalWidth - finalDrawWidth - paddingX,
                        logicalHeight - finalDrawHeight - paddingY,
                        finalDrawWidth,
                        finalDrawHeight
                    };
                    Rectangle helpySourceRect = { 0, 0, mainMenuRes.helpyTexture.width, mainMenuRes.helpyTexture.height };
                    DrawTexturePro(mainMenuRes.helpyTexture, helpySourceRect, helpyDestinationRect, { 0,0 }, 0, WHITE);
                }

                Rectangle backButton = { logicalWidth / 2 - 100, logicalHeight - 100, 200, 50 };
                GuiButton(backButton, "Back", uiDrawFont, 20, BLACK, WHITE, LIGHTGRAY, GRAY, scaledMousePos);
            } break;
            case PLAYING_MOTORIST:
            case PLAYING_RAINBOW:
                ClearBackground(BLACK);
                break;
            case EXITING: {
                ClearBackground(BLACK);
                const char* exitMsg = "Exiting...";
                float exitMsgSize = 20;
                Vector2 emSize = MeasureTextEx(uiDrawFont, exitMsg, exitMsgSize, 1);
                DrawTextEx(uiDrawFont, exitMsg, { logicalWidth / 2 - emSize.x / 2, logicalHeight / 2 - emSize.y / 2 }, exitMsgSize, 1, LIGHTGRAY);
            } break;
            default: break;
            }
            EndTextureMode();

            float scale = min(GetScreenWidth() / logicalWidth, GetScreenHeight() / logicalHeight);
            float scaledWidth = logicalWidth * scale;
            float scaledHeight = logicalHeight * scale;
            Vector2 drawPos = { (GetScreenWidth() - scaledWidth) / 2, (GetScreenHeight() - scaledHeight) / 2 };
            Rectangle sourceRect = { 0, 0, mainMenuRes.targetRenderTexture.texture.width, -mainMenuRes.targetRenderTexture.texture.height };
            Rectangle destRect = { drawPos.x, drawPos.y, scaledWidth, scaledHeight };
            DrawTexturePro(mainMenuRes.targetRenderTexture.texture, sourceRect, destRect, { 0,0 }, 0, WHITE);

        }

        if ((isFadingOut || isFadingIn) && fadeAlpha > 0) {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, fadeAlpha));
        }

        DrawFPS(10, 10);
        EndDrawing();
    }

    TraceLog(LOG_INFO, "CLEANUP: Starting main cleanup...");
    UnloadMainMenuResources(mainMenuRes);

    CloseAudioDevice();
    CloseWindow();
    TraceLog(LOG_INFO, "CLEANUP: Finished. Exiting application.");

    return 0;
}