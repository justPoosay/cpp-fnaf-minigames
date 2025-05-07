#include "raylib.h"
#include "raymath.h"
#include "GameSettings.h"
#include "MotoristResources.h"
#include "RainbowLandResources.h"
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

int runMidnightMotorist(GraphicsQuality quality, Shader postProcessingShader, bool applyShader);
int runMagicRainbowLand(GraphicsQuality quality);
void StopAndUnloadMusic(Music& music, bool& loadedFlag);

// --- UI Functions ---
static bool GuiButton(Rectangle bounds, const char* text) {
    bool clicked = false;
    Vector2 mousePoint = GetMousePosition();
    const int logicalWidth = 1280;
    const int logicalHeight = 720;
    float renderTextureScaleX = (float)logicalWidth / GetScreenWidth();
    float renderTextureScaleY = (float)logicalHeight / GetScreenHeight();
    Vector2 scaledMousePoint = { mousePoint.x * renderTextureScaleX, mousePoint.y * renderTextureScaleY };

    if (CheckCollisionPointRec(scaledMousePoint, bounds)) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) 
            DrawRectangleRec(bounds, GRAY);
        else 
            DrawRectangleRec(bounds, LIGHTGRAY);
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) 
            clicked = true;
    } else 
        DrawRectangleRec(bounds, WHITE);

    float textWidth = MeasureText(text, 20);
    DrawText(text, bounds.x + (bounds.width / 2) - (textWidth / 2), bounds.y + (bounds.height / 2) - 10, 20, BLACK);
    return clicked;
}

// --- Helper Function to Stop and Unload Music ---
void StopAndUnloadMusic(Music& music, bool& loadedFlag) {
    if (loadedFlag && music.stream.buffer) {
        if (IsMusicStreamPlaying(music)) {
            StopMusicStream(music);
        }

        UnloadMusicStream(music);
        music = { 0 };
        loadedFlag = false;
        TraceLog(LOG_INFO, "AUDIO: Music stream stopped and unloaded.");
    } else {
        loadedFlag = false;
        music = { 0 };
    }
}

// --- Main Application Entry Point ---
int main(void) {
    InitializeGameSettings();

// --- Raylib Initialization (Window First) ---
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(g_settings.screenWidth, g_settings.screenHeight, "FNaF Minigames - Loading...");
    InitAudioDevice();

// --- Load and Set Window Icon ---
    const char* iconPath = "resources/favicon.png";
    if (FileExists(iconPath)) {
        Image icon = LoadImage(iconPath);
        if (icon.data) {
            SetWindowIcon(icon);
            UnloadImage(icon);
            TraceLog(LOG_INFO, "ICON: Window icon loaded successfully from '%s'", iconPath);
        } else
            TraceLog(LOG_WARNING, "ICON: Failed to load window icon data from '%s'", iconPath);

    int monitorWidth = GetMonitorWidth(GetCurrentMonitor());
    int monitorHeight = GetMonitorHeight(GetCurrentMonitor());
    SetWindowPosition((monitorWidth - g_settings.screenWidth) / 2, (monitorHeight - g_settings.screenHeight) / 2);
    SetMasterVolume(g_settings.masterVolume);
    SetTargetFPS(60);

// --- Game State Variables ---
    GameScreen currentScreen = LOADING;
    GameScreen previousScreen = currentScreen;
    bool shouldExit = false;

// --- Rendering Variables ---
    const int logicalWidth = 1280;
    const int logicalHeight = 720;
    RenderTexture2D target = { 0 };
    Shader crtShader = { 0 };
    int crtResolutionLoc = -1;
    int crtTimeLoc = -1;
    bool shaderLoadedSuccessfully = false;
    bool shaderLoadAttempted = false;

// --- Asset Variables ---
    Image bgGifImage = { 0 };
    Texture2D bgTexture = { 0 };
    Image settingsBgImage = { 0 };
    Texture2D settingsBgTexture = { 0 };
    int animFrames = 0;
    int currentAnimFrame = 0;
    float frameDelay = 1.0f / 15.0f;
    float frameTimer = 0.0f;
    bool gifLoaded = false;
    bool gifLoadAttempted = false;
    const char* gifPath = "resources/menuBg.gif";
    const char* settingsBgPath = "resources/settingsBg.png";

    Font guiFont = LoadFont("resources/ARCADECLASSIC.TTF");

    Music menuMusic = { 0 };
    Music settingsMusic = { 0 };
    bool menuMusicLoaded = false;
    bool settingsMusicLoaded = false;
    bool menuMusicLoadAttempted = false;
    bool settingsMusicLoadAttempted = false;
    const char* menuMusicPath = "resources/mainMenu.mp3";
    const char* settingsMusicPath = "resources/SETTINGS.mp3";

// --- Loading Screen Variables ---
    std::string loadingStatus = "Initializing...";

// --- Fade Transition Variables ---
    float fadeAlpha = 0.0f;
    float fadeSpeed = 1.5f;
    bool isFadingOut = false;
    bool isFadingIn = false;
    GameScreen fadeNextScreen = MAIN_MENU; 

// --- MAIN GAME LOOP ---
    while (!shouldExit && !WindowShouldClose()) {
    // --- Asset Loading Logic (LOADING State) ---
        if (currentScreen == LOADING) {
        // Load assets sequentially
            if (!shaderLoadAttempted) {
                loadingStatus = "Loading resources..."; 
                shaderLoadAttempted = true;
                if (FileExists("crt.fs")) {
                    crtShader = LoadShader(0, "crt.fs");
                    if (crtShader.id > 0) {
                        shaderLoadedSuccessfully = true;
                        crtResolutionLoc = GetShaderLocation(crtShader, "resolution"); 
                        crtTimeLoc = GetShaderLocation(crtShader, "time"); 
                    }
                } else { 
                    g_settings.useCRTShader = false; 
                    shaderLoadedSuccessfully = false;
                }
            } else if (!gifLoadAttempted) {
                loadingStatus = "Loading background...";
                gifLoadAttempted = true;

                if (FileExists(gifPath)) {
                    bgGifImage = LoadImageAnim(gifPath, &animFrames);
                    if (bgGifImage.data != NULL && animFrames > 0) {
                        bgTexture = LoadTextureFromImage(bgGifImage);
                        settingsBgTexture = LoadTexture(settingsBgPath);
                        if (bgTexture.id > 0) {
                            gifLoaded = true;
                            SetTextureFilter(bgTexture, TEXTURE_FILTER_BILINEAR); 
                        }
                    }
                } else {
                    UnloadImage(bgGifImage);
                    bgGifImage = { 0 };
                    gifLoaded = false;
                }
            } else if (!menuMusicLoadAttempted) {
                loadingStatus = "Loading menu audio..."; 
                menuMusicLoadAttempted = true;
                if (FileExists(menuMusicPath)) {
                    menuMusic = LoadMusicStream(menuMusicPath);
                    if (menuMusic.stream.buffer) {
                        menuMusicLoaded = true; 
                        SetMusicVolume(menuMusic, 1.0f); 
                    }
                } else
                    menuMusicLoaded = false;

            } else if (!settingsMusicLoadAttempted) {
                loadingStatus = "Loading settings audio...";
                settingsMusicLoadAttempted = true;
                if (FileExists(settingsMusicPath)) {
                    settingsMusic = LoadMusicStream(settingsMusicPath);
                    if (settingsMusic.stream.buffer) {
                        settingsMusicLoaded = true; 
                        SetMusicVolume(settingsMusic, 1.0f);
                    }
                } else settingsMusicLoaded = false;

            } else {
                loadingStatus = "Finalizing...";

                if (target.id == 0) 
                    target = LoadRenderTexture(logicalWidth, logicalHeight);
                if (target.id > 0) 
                    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
                else 
                    TraceLog(LOG_ERROR, "Failed to create render texture!");

                frameTimer = 0.0f; currentAnimFrame = 0; 
                if (gifLoaded && bgGifImage.data) 
                    UpdateTexture(bgTexture, bgGifImage.data); 

                previousScreen = currentScreen; 
                currentScreen = MAIN_MENU; 
                fadeNextScreen = MAIN_MENU;
                SetWindowTitle("FNaF Minigames in C++");
                isFadingIn = true; 
                fadeAlpha = 1.0f; 

                if (menuMusicLoaded) 
                    PlayMusicStream(menuMusic);
            }
        } else {
            // --- Fade Logic ---
            if (isFadingOut) {
                fadeAlpha += fadeSpeed * GetFrameTime();
                if (fadeAlpha >= 1.0f) {
                    fadeAlpha = 1.0f;
                    isFadingOut = false; // Fade out complete

                    // Actions after fading OUT: Stop music, start game, prepare for fade IN
                    StopAndUnloadMusic(menuMusic, menuMusicLoaded);
                    StopAndUnloadMusic(settingsMusic, settingsMusicLoaded);

                    int gameResult = 0; // Placeholder for potential game return values

                // --- Launch the actual game (BLOCKING CALL) ---
                    if (fadeNextScreen == PLAYING_MOTORIST) {
                        MotoristGameResources motoristRes = LoadMotoristResources(g_settings.quality);
                        gameResult = runMidnightMotorist(g_settings.quality, crtShader, g_settings.useCRTShader && shaderLoadedSuccessfully);
                        UnloadMotoristResources(motoristRes);
                    } else if (fadeNextScreen == PLAYING_RAINBOW) {
                        RainbowLandGameResources rainbowRes = LoadRainbowLandResources(g_settings.quality);
                        gameResult = runMagicRainbowLand(g_settings.quality);
                        UnloadRainbowLandResources(rainbowRes);
                      }

                    // --- Actions after game returns ---
                    previousScreen = currentScreen; 
                    currentScreen = MAIN_MENU;
                    fadeNextScreen = MAIN_MENU;     
                    if (IsWindowMinimized()) 
                        RestoreWindow();

                    SetMasterVolume(g_settings.masterVolume);
                    SetTargetFPS(60);

                    // Reload and start menu music for fade-in
                    if (!menuMusicLoaded && menuMusicLoadAttempted && FileExists(menuMusicPath)) {
                        menuMusic = LoadMusicStream(menuMusicPath);
                        if (menuMusic.stream.buffer) { 
                            menuMusicLoaded = true; 
                            SetMusicVolume(menuMusic, 1.0f); 
                        } else menuMusicLoaded = false;
                    }

                    if (menuMusicLoaded) 
                        PlayMusicStream(menuMusic);

                    isFadingIn = true;
 
                }
            } else if (isFadingIn) {
                fadeAlpha -= fadeSpeed * GetFrameTime();
                if (fadeAlpha <= 0.0f) {
                    fadeAlpha = 0.0f;
                    isFadingIn = false;
                    currentScreen = fadeNextScreen;
                }
            } else { 
            // --- Normal Update (Not Fading) ---
                if (menuMusicLoaded && IsMusicStreamPlaying(menuMusic)) {
                    UpdateMusicStream(menuMusic);
                    if (GetMusicTimePlayed(menuMusic) >= GetMusicTimeLength(menuMusic) - 0.1f) 
                        SeekMusicStream(menuMusic, 0.0f);
                }
                if (settingsMusicLoaded && IsMusicStreamPlaying(settingsMusic)) {
                    UpdateMusicStream(settingsMusic);
                    if (GetMusicTimePlayed(settingsMusic) >= GetMusicTimeLength(settingsMusic) - 0.1f) 
                        SeekMusicStream(settingsMusic, 0.0f);
                }

                // Update GIF Animation
                if (gifLoaded && currentScreen == MAIN_MENU) {
                    frameTimer += GetFrameTime();
                    if (frameTimer >= frameDelay) {
                        if (frameTimer > frameDelay * 5)
                            frameTimer = frameDelay;
                        while (frameTimer >= frameDelay) {
                            frameTimer -= frameDelay;
                            currentAnimFrame = (currentAnimFrame + 1) % animFrames;
                            if (frameTimer < frameDelay) {
                                int frameDataSize = GetPixelDataSize(bgGifImage.width, bgGifImage.height, bgGifImage.format);
                                unsigned char* frameDataOffset = (unsigned char*)bgGifImage.data + (currentAnimFrame * frameDataSize);
                                UpdateTexture(bgTexture, frameDataOffset);
                            }
                        }
                    }
                }

            // Window Resize Handling
                if (IsWindowResized() && !IsWindowFullscreen()) {
                    int monW = GetMonitorWidth(GetCurrentMonitor());
                    int monH = GetMonitorHeight(GetCurrentMonitor());

                    SetWindowPosition((monW - GetScreenWidth()) / 2, (monH - GetScreenHeight()) / 2);
                }

            // --- Input & State Transitions (when not fading) ---
                GameScreen nextScreen = currentScreen;
                switch (currentScreen) {
                    case MAIN_MENU: {
                        float buttonX = logicalWidth / 2.0f - 150; 
                        float startY = 160.0f + 30; 
                        float buttonSpacing = 70;

                        if (GuiButton({ buttonX, startY + 0 * buttonSpacing, 300, 50 }, "Midnight Motorist")) {
                            isFadingOut = true;
                            fadeAlpha = 0.0f;
                            fadeNextScreen = PLAYING_MOTORIST;
                        }
                        if (GuiButton({ buttonX, startY + 1 * buttonSpacing, 300, 50 }, "Chica's Magic Rainbow Land")) {
                            isFadingOut = true; 
                            fadeAlpha = 0.0f; 
                            fadeNextScreen = PLAYING_RAINBOW;
                        }
                        if (GuiButton({ buttonX, startY + 2 * buttonSpacing, 300, 50 }, "Settings"))
                            nextScreen = SETTINGS;
                        if (GuiButton({ buttonX, startY + 3 * buttonSpacing, 300, 50 }, "Exit")) 
                            nextScreen = EXITING;
                    } break;

                    case SETTINGS: {
                        Vector2 mousePos = GetMousePosition(); float scaleX = (float)logicalWidth / GetScreenWidth(); float scaleY = (float)logicalHeight / GetScreenHeight();
                        Vector2 scaledMousePos = { mousePos.x * scaleX, mousePos.y * scaleY }; Rectangle volumeSliderRect = { 250, 175, 300, 30 };
                        if (CheckCollisionPointRec(scaledMousePos, volumeSliderRect) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                            float mouseXRelative = scaledMousePos.x - volumeSliderRect.x; 
                            g_settings.masterVolume = Clamp(mouseXRelative / volumeSliderRect.width, 0.0f, 1.0f);
                            SetMasterVolume(g_settings.masterVolume); 
                        }
                        ScreenResolution tempRes = g_settings.currentResolution; float resButtonY = 235; float resButtonXStart = 250; float resButtonSpacing = 110;
                        if (GuiButton({ resButtonXStart, resButtonY, 100, 30 }, "640x360")) 
                            tempRes = RES_640x360;
                        if (GuiButton({ resButtonXStart + resButtonSpacing, resButtonY, 100, 30 }, "854x480"))
                            tempRes = RES_854x480; 
                        if (GuiButton({ resButtonXStart + 2 * resButtonSpacing, resButtonY, 120, 30 }, "1280x720")) 
                            tempRes = RES_1280x720;
                        if (GuiButton({ resButtonXStart + 2 * resButtonSpacing + 130, resButtonY, 120, 30 }, "1920x1080")) 
                            tempRes = RES_1920x1080;
                        if (tempRes != g_settings.currentResolution) {
                            g_settings.currentResolution = tempRes; 
                            UpdateScreenDimensionsFromSettings(); 
                            SetWindowSize(g_settings.screenWidth, g_settings.screenHeight);
                            if (!IsWindowFullscreen()) { 
                                int monW = GetMonitorWidth(GetCurrentMonitor());
                                int monH = GetMonitorHeight(GetCurrentMonitor());
                                SetWindowPosition((monW - g_settings.screenWidth) / 2, (monH - g_settings.screenHeight) / 2); 
                            } 
                        }

                        float qualityButtonY = 295;
                        float qualityButtonXStart = 250; 
                        float qualityButtonSpacing = 110; 
                        if (GuiButton({ qualityButtonXStart, qualityButtonY, 100, 30 }, "Low")) 
                            g_settings.quality = QUALITY_LOW;
                        if (GuiButton({ qualityButtonXStart + qualityButtonSpacing, qualityButtonY, 100, 30 }, "Medium"))
                            g_settings.quality = QUALITY_MEDIUM; 
                        if (GuiButton({ qualityButtonXStart + 2 * qualityButtonSpacing, qualityButtonY, 100, 30 }, "High"))
                            g_settings.quality = QUALITY_HIGH;
                        Rectangle crtToggleButton = { qualityButtonXStart, 355, 100, 30 }; 
                        if (GuiButton(crtToggleButton, g_settings.useCRTShader ? "ON" : "OFF")) { 
                            if (shaderLoadedSuccessfully)
                                g_settings.useCRTShader = !g_settings.useCRTShader; 
                        }
                        Rectangle backButton = { logicalWidth / 2.0f - 100, logicalHeight - 100, 200, 50 };
                        if (GuiButton(backButton, "Back") || IsKeyPressed(KEY_ESCAPE))
                            nextScreen = MAIN_MENU;
                    } break;

                    case EXITING: { 
                        shouldExit = true;
                        break; 
                    }
                    default: break;
                }

                // --- Music Management for Non-Fade Transitions (e.g., Menu -> Settings) ---
                if (nextScreen != currentScreen && !isFadingOut && !isFadingIn) {
                    // Stop music of the screen we are leaving
                    if (currentScreen == MAIN_MENU && menuMusicLoaded) 
                        StopMusicStream(menuMusic);
                    if (currentScreen == SETTINGS && settingsMusicLoaded) 
                        StopMusicStream(settingsMusic);

                    // Start music for the screen we are entering
                    if (nextScreen == MAIN_MENU) {
                        if (!menuMusicLoaded && menuMusicLoadAttempted && FileExists(menuMusicPath)) {
                            menuMusic = LoadMusicStream(menuMusicPath); 
                            if (menuMusic.stream.buffer) {
                                menuMusicLoaded = true; 
                                SetMusicVolume(menuMusic, 1.0f);
                            } else 
                                menuMusicLoaded = false; 
                        }
                        if (menuMusicLoaded) 
                            PlayMusicStream(menuMusic);

                    } else if (nextScreen == SETTINGS) {
                        if (!settingsMusicLoaded && settingsMusicLoadAttempted && FileExists(settingsMusicPath)) { 
                            settingsMusic = LoadMusicStream(settingsMusicPath); 
                            if (settingsMusic.stream.buffer) { 
                                settingsMusicLoaded = true; 
                                SetMusicVolume(settingsMusic, 1.0f); 
                            } else
                                settingsMusicLoaded = false; 
                        }
                        if (settingsMusicLoaded) 
                            PlayMusicStream(settingsMusic);
                    }

                    previousScreen = currentScreen;
                    currentScreen = nextScreen;
                }
            } 
        } 


        // --- DRAWING ---
        if (currentScreen == LOADING) {
            // Draw loading screen directly to window buffer
            BeginDrawing();
            ClearBackground(BLACK);
            int textWidth = MeasureText(loadingStatus.c_str(), 30);
            DrawText(loadingStatus.c_str(), (GetScreenWidth() - textWidth) / 2, GetScreenHeight() / 2 - 15, 30, WHITE);
            EndDrawing();
        } else if (target.id > 0) { // Only draw if render texture is valid
            BeginTextureMode(target);
            ClearBackground(RAYWHITE);

            GameScreen screenToDraw = currentScreen;
            if (isFadingOut)
                screenToDraw = currentScreen;
            else if (isFadingIn) 
                screenToDraw = fadeNextScreen;

            switch (screenToDraw) {
                case MAIN_MENU: {
                    if (gifLoaded && bgTexture.id > 0) 
                        DrawTexturePro(bgTexture, { 0.0f, 0.0f, (float)bgTexture.width, (float)bgTexture.height }, { 0.0f, 0.0f, (float)logicalWidth, (float)logicalHeight }, { 0, 0 }, 0.0f, WHITE);
                    else 
                        ClearBackground(DARKBLUE);

                    float overlayWidth = 400.0f;
                    float overlayY = 160.0f; 
                    float overlayX = (logicalWidth - overlayWidth) / 2.0f; 
                    float buttonStartY = overlayY + 30; 
                    float buttonSpacing = 70; 
                    float lastButtonY = buttonStartY + 3 * buttonSpacing + 50; 
                    float overlayHeight = (lastButtonY - overlayY) + 20;
                    DrawRectangleRec({ overlayX, overlayY, overlayWidth, overlayHeight }, Fade(BLACK, 0.70f)); 
                    DrawText("FNaF Minigames in C++", logicalWidth / 2 - MeasureText("FNaF Minigames in C++", 40) / 2, 80, 40, WHITE);
                    float buttonX = logicalWidth / 2.0f - 150;
                    GuiButton({ buttonX, buttonStartY + 0 * buttonSpacing, 300, 50 }, "Midnight Motorist"); 
                    GuiButton({ buttonX, buttonStartY + 1 * buttonSpacing, 300, 50 }, "Chica's Magic Rainbow Land");
                    GuiButton({ buttonX, buttonStartY + 2 * buttonSpacing, 300, 50 }, "Settings");
                    GuiButton({ buttonX, buttonStartY + 3 * buttonSpacing, 300, 50 }, "Exit");
                } break;

                case SETTINGS: {
                    DrawTexturePro(settingsBgTexture, { 0.0f, 0.0f, (float)settingsBgTexture.width, (float)settingsBgTexture.height }, { 0.0f, 0.0f, (float)logicalWidth, (float)logicalHeight }, { 0, 0 }, 0.0f, WHITE);

                    DrawText("SETTINGS", logicalWidth / 2 - MeasureText("SETTINGS", 40) / 2, 80, 40, WHITE);
                    DrawText("Volume:", 100, 180, 23, WHITE);
                    Rectangle volumeSliderRect = { 250, 175, 300, 30 };
                    DrawRectangleRec(volumeSliderRect, WHITE); 
                    DrawRectangleRec({ volumeSliderRect.x, volumeSliderRect.y, volumeSliderRect.width * g_settings.masterVolume, volumeSliderRect.height }, PURPLE);
                    DrawRectangleLinesEx(volumeSliderRect, 5, WHITE);
                    DrawText(TextFormat("%.0f%%", g_settings.masterVolume * 100), volumeSliderRect.x + volumeSliderRect.width + 15, 180, 23, WHITE);
                    DrawText("Resolution:", 100, 240, 23, WHITE);
                    float resButtonY = 235;
                    float resButtonXStart = 250;
                    float resButtonSpacing = 110; GuiButton({ resButtonXStart, resButtonY, 100, 30 }, "640x360"); 
                    GuiButton({ resButtonXStart + resButtonSpacing, resButtonY, 100, 30 }, "854x480");
                    GuiButton({ resButtonXStart + 2 * resButtonSpacing, resButtonY, 120, 30 }, "1280x720"); 
                    GuiButton({ resButtonXStart + 2 * resButtonSpacing + 130, resButtonY, 120, 30 }, "1920x1080");
                    DrawText("Quality:", 100, 300, 23, WHITE); 
                    float qualityButtonY = 295; 
                    float qualityButtonXStart = 250; 
                    float qualityButtonSpacing = 110; 
                    GuiButton({ qualityButtonXStart, qualityButtonY, 100, 30 }, "Low"); 
                    GuiButton({ qualityButtonXStart + qualityButtonSpacing, qualityButtonY, 100, 30 }, "Medium"); 
                    GuiButton({ qualityButtonXStart + 2 * qualityButtonSpacing, qualityButtonY, 100, 30 }, "High");
                    DrawText("Shaders:", 100, 360, 23, WHITE);
                    Rectangle crtToggleButton = { qualityButtonXStart, 355, 100, 30 }; 
                    GuiButton(crtToggleButton, g_settings.useCRTShader ? "ON" : "OFF"); 
                    if (!shaderLoadedSuccessfully) 
                        DrawText("(Shader N/A)", crtToggleButton.x + 110, 360, 23, WHITE);
                    Rectangle backButton = { logicalWidth / 2.0f - 100, logicalHeight - 100, 200, 50 }; 
                    GuiButton(backButton, "Back");
                } break;

                case PLAYING_RAINBOW: {
                    // Draw a black screen or placeholder if desired while the game *should* be running (covered by fade now)
                    ClearBackground(BLACK);
                    break;
                }

                case EXITING: { 
                    ClearBackground(BLACK);
                    DrawText("Exiting...", logicalWidth / 2 - MeasureText("Exiting...", 20) / 2, logicalHeight / 2 - 10, 20, LIGHTGRAY); 
                    break; 
                } 
                default: break;
            }
            EndTextureMode();

        // --- Draw Final Output to Screen ---
            BeginDrawing();
            ClearBackground(BLACK);

            // Draw the render texture scaled and centered
            float scale = fmin((float)GetScreenWidth() / logicalWidth, (float)GetScreenHeight() / logicalHeight);
            float scaledWidth = logicalWidth * scale; 
            float scaledHeight = logicalHeight * scale;
            Vector2 drawPos = { (GetScreenWidth() - scaledWidth) / 2.0f, (GetScreenHeight() - scaledHeight) / 2.0f };
            Rectangle sourceRect = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
            Rectangle destRect = { drawPos.x, drawPos.y, scaledWidth, scaledHeight };
            DrawTexturePro(target.texture, sourceRect, destRect, { 0,0 }, 0.0f, WHITE);

            // --- Draw Fade Overlay ---
            if (isFadingOut || isFadingIn)
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, fadeAlpha));

            DrawFPS(10, 10);
            EndDrawing();
        } 
    }

    // --- CLEANUP ---
    TraceLog(LOG_INFO, "CLEANUP: Starting cleanup...");
    StopAndUnloadMusic(menuMusic, menuMusicLoaded);
    StopAndUnloadMusic(settingsMusic, settingsMusicLoaded);

    if (target.id > 0)
        UnloadRenderTexture(target);
    if (gifLoaded) {
        UnloadImage(bgGifImage);
        UnloadTexture(bgTexture); 
    }
    if (shaderLoadedSuccessfully) 
        UnloadShader(crtShader);

    CloseAudioDevice();
    CloseWindow();
    TraceLog(LOG_INFO, "CLEANUP: Finished. Exiting application.");

    return 0;
}}