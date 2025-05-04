#include "raylib.h"
#include "raymath.h"
#include "GameSettings.h" // <--- WA¯NE: DO£¥CZAMY GameSettings.h
#include "MotoristResources.h"
#include "RainbowLandResources.h"
#include <string>
#include <vector>
#include <iostream>

// --- Globalne Ustawienia ---
//const float SCREEN_RATIO = 16 / 9;
const int DEFAULT_SCREEN_WIDTH = 1280;
const int DEFAULT_SCREEN_HEIGHT = 720;
int currentScreenWidth = DEFAULT_SCREEN_WIDTH;
int currentScreenHeight = DEFAULT_SCREEN_HEIGHT;
float masterVolume = 1.0f; // 0.0f to 1.0f

// --- Dodane ustawienia rozdzielczoœci i jakoœci ---
ScreenResolution currentResolution = RES_1280x720; // Domyœlna rozdzielczoœæ
GraphicsQuality currentQuality = QUALITY_MEDIUM;   // Domyœlna jakoœæ

// --- Stany Aplikacji ---
typedef enum GameScreen {
    MAIN_MENU,
    SETTINGS,
    PLAYING_MOTORIST,
    PLAYING_RAINBOW,
    EXITING
} GameScreen;

// --- Deklaracje funkcji gier (MUSZ¥ BYÆ ZDEFINIOWANE W INNYCH PLIKACH .cpp) ---
int runMidnightMotorist(GraphicsQuality quality);
int runMagicRainbowLand(GraphicsQuality quality);

// --- Funkcje Pomocnicze UI ---
bool GuiButton(Rectangle bounds, const char* text) {
    bool clicked = false;
    Vector2 mousePoint = GetMousePosition();

    // Przeskaluj pozycjê myszy do przestrzeni RenderTexture
    float scaleX = (float)DEFAULT_SCREEN_WIDTH / GetScreenWidth();
    float scaleY = (float)DEFAULT_SCREEN_HEIGHT / GetScreenHeight();
    Vector2 scaledMousePoint = { mousePoint.x * scaleX, mousePoint.y * scaleY };

    if (CheckCollisionPointRec(scaledMousePoint, bounds)) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) DrawRectangleRec(bounds, LIGHTGRAY);
        else DrawRectangleRec(bounds, GRAY);

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) clicked = true;
    }
    else {
        DrawRectangleRec(bounds, DARKGRAY);
    }

    int textWidth = MeasureText(text, 20);
    DrawText(text, bounds.x + (bounds.width / 2) - (textWidth / 2), bounds.y + (bounds.height / 2) - 10, 20, WHITE);

    return clicked;
}

int main(void) {
    // --- Inicjalizacja ---
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    switch (currentResolution) {
        case RES_640x360: {
                currentScreenWidth = 640; 
                currentScreenHeight = 360; 
        } break;

        case RES_854x480: {
                currentScreenWidth = 854; 
                currentScreenHeight = 480;
        } break;

        case RES_1280x720: {
                currentScreenWidth = 1280; 
                currentScreenHeight = 720; 
        } break;

        case RES_1920x1080: {
                currentScreenWidth = 1920; 
                currentScreenHeight = 1080; 
        } break;
    }
    InitWindow(currentScreenWidth, currentScreenHeight, "Game Launcher");
    InitAudioDevice();
    SetMasterVolume(masterVolume);
    SetTargetFPS(60);

    GameScreen currentScreen = MAIN_MENU;
    bool shouldExit = false;
    RenderTexture2D target = LoadRenderTexture(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    // --- G³ówna Pêtla Gry ---
    while (!shouldExit && !WindowShouldClose()) {
        if (IsWindowResized()) {
            // Opcjonalnie: Dostosuj rozmiar RenderTexture, ale w tym przypadku utrzymujemy sta³y rozmiar logiczny
        }

        switch (currentScreen) {
        case MAIN_MENU: {
            BeginTextureMode(target);
            ClearBackground(RAYWHITE);
            DrawText("SELECT GAME", target.texture.width / 2 - MeasureText("SELECT GAME", 40) / 2, 80, 40, DARKGRAY);
            Rectangle motoristButton = { (float)target.texture.width / 2 - 150, 180, 300, 50 };
            Rectangle rainbowButton = { (float)target.texture.width / 2 - 150, 250, 300, 50 };
            Rectangle settingsButton = { (float)target.texture.width / 2 - 150, 320, 300, 50 };
            Rectangle exitButton = { (float)target.texture.width / 2 - 150, 390, 300, 50 };
            if (GuiButton(motoristButton, "Midnight Motorist")) currentScreen = PLAYING_MOTORIST;
            if (GuiButton(rainbowButton, "Chica's Magic Rainbow Land")) currentScreen = PLAYING_RAINBOW;
            if (GuiButton(settingsButton, "Settings")) currentScreen = SETTINGS;
            if (GuiButton(exitButton, "Exit")) shouldExit = true;
            EndTextureMode();
        } break;

        case SETTINGS: {
            BeginTextureMode(target);
            ClearBackground(LIGHTGRAY);
            DrawText("SETTINGS", target.texture.width / 2 - MeasureText("SETTINGS", 40) / 2, 80, 40, DARKGRAY);
            DrawText("Volume:", 100, 180, 20, DARKGRAY);
            Rectangle volumeSliderRect = { 200, 175, 300, 30 };
            DrawRectangleRec(volumeSliderRect, GRAY);
            DrawRectangleRec({ volumeSliderRect.x, volumeSliderRect.y, volumeSliderRect.width * masterVolume, volumeSliderRect.height }, RED);
            DrawRectangleLinesEx(volumeSliderRect, 2, DARKGRAY);
            Vector2 mousePos = GetMousePosition();

            // Przeskaluj pozycjê myszy do przestrzeni RenderTexture
            float scaleX = (float)DEFAULT_SCREEN_WIDTH / GetScreenWidth();
            float scaleY = (float)DEFAULT_SCREEN_HEIGHT / GetScreenHeight();
            Vector2 scaledMousePos = { mousePos.x * scaleX, mousePos.y * scaleY };

            if (CheckCollisionPointRec(scaledMousePos, volumeSliderRect) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                float mouseXRelative = scaledMousePos.x - volumeSliderRect.x;
                masterVolume = Clamp(mouseXRelative / volumeSliderRect.width, 0.0f, 1.0f);
                SetMasterVolume(masterVolume);
            }
            DrawText(TextFormat("%.0f%%", masterVolume * 100), volumeSliderRect.x + volumeSliderRect.width + 15, 180, 20, DARKGRAY);

            DrawText("Rozdzielczosc:", 100, 240, 20, DARKGRAY);
            Rectangle resButtonLow = { 300, 235, 100, 30 };
            ScreenResolution nextResolution = currentResolution; // Domyœlnie bez zmian
            if (GuiButton(resButtonLow, "640x360")) nextResolution = RES_640x360;
            Rectangle resButtonMed = { 410, 235, 100, 30 };
            if (GuiButton(resButtonMed, "854x480")) nextResolution = RES_854x480;
            Rectangle resButtonHD = { 520, 235, 120, 30 };
            if (GuiButton(resButtonHD, "1280x720")) nextResolution = RES_1280x720;
            Rectangle resButtonFHD = { 650, 235, 120, 30 };
            if (GuiButton(resButtonFHD, "1920x1080")) nextResolution = RES_1920x1080;

            if (nextResolution != currentResolution) {
                currentResolution = nextResolution;
                switch (currentResolution) {
                    case RES_640x360: {
                        currentScreenWidth = 640;
                        currentScreenHeight = 360; 
                    } break;

                    case RES_854x480: {
                        currentScreenWidth = 854; 
                        currentScreenHeight = 480;
                    } break;

                    case RES_1280x720: { 
                        currentScreenWidth = 1280; 
                        currentScreenHeight = 720; 
                    } break;

                    case RES_1920x1080: { 
                        currentScreenWidth = 1920;
                        currentScreenHeight = 1080; 
                    } break;
                }

                // --- Centrowanie okna ---
                int monitorWidth = GetMonitorWidth(GetCurrentMonitor());
                int monitorHeight = GetMonitorHeight(GetCurrentMonitor());
                int newX = (monitorWidth - currentScreenWidth) / 2;
                int newY = (monitorHeight - currentScreenHeight) / 2;
                SetWindowPosition(newX, newY);

                SetWindowSize(currentScreenWidth, currentScreenHeight);
                UnloadRenderTexture(target);
                target = LoadRenderTexture(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
                SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
            }

            DrawText("Quality:", 100, 280, 20, DARKGRAY);

            Rectangle qualityButtonLow = { 300, 275, 100, 30 };
            if (GuiButton(qualityButtonLow, "Low"))
                currentQuality = QUALITY_LOW;
            Rectangle qualityButtonMed = { 410, 275, 100, 30 };
            if (GuiButton(qualityButtonMed, "Medium"))
                currentQuality = QUALITY_MEDIUM;
            Rectangle qualityButtonHigh = { 520, 275, 100, 30 };
            if (GuiButton(qualityButtonHigh, "High")) 
                currentQuality = QUALITY_HIGH;

            Rectangle backButton = { 
                (float)target.texture.width / 2 - 100, target.texture.height - 100, 200, 50
            };

            if (GuiButton(backButton, "Back") || IsKeyPressed(KEY_ESCAPE))
                currentScreen = MAIN_MENU;

            EndTextureMode();
        } break;

        case PLAYING_MOTORIST: {
            MotoristGameResources motoristRes = LoadMotoristResources(currentQuality);
            BeginTextureMode(target);
            ClearBackground(BLACK);
            EndTextureMode();
            runMidnightMotorist(currentQuality);
            UnloadMotoristResources(motoristRes);
            currentScreen = MAIN_MENU;
        } break;

        case PLAYING_RAINBOW: {
            RainbowLandGameResources rainbowRes = LoadRainbowLandResources(currentQuality);
            BeginTextureMode(target);
            ClearBackground(BLACK);
            EndTextureMode();
            runMagicRainbowLand(currentQuality);
            UnloadRainbowLandResources(rainbowRes);
            currentScreen = MAIN_MENU;
        } break;

        case EXITING: {
            shouldExit = true;
        } break;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        float scale = fmin((float)GetScreenWidth() / target.texture.width, (float)GetScreenHeight() / target.texture.height);
        float scaledWidth = target.texture.width * scale;
        float scaledHeight = target.texture.height * scale;
        Vector2 drawPosition = { (GetScreenWidth() - scaledWidth) / 2.0f, (GetScreenHeight() - scaledHeight) / 2.0f };
        DrawTexturePro(target.texture, { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height }, { drawPosition.x, drawPosition.y, scaledWidth, scaledHeight }, { 0, 0 }, 0.0f, WHITE);
        DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadRenderTexture(target);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}