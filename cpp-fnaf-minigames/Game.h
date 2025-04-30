#pragma once

#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "Player.h"
#include "Level.h"
#include "Audio.h"
#include "UI.h"

struct GameAssets {
    Texture2D tile;
    Texture2D bg;
    Texture2D idleRight;
    Texture2D idleLeft;
    Texture2D walkRight;
    Texture2D walkLeft;
    Texture2D jumpRight;
    Texture2D jumpLeft;
    Texture2D buttonVoicesOff;
    Texture2D buttonVoicesOn;
    // Dodaj wiêcej zasobów w miarê potrzeb
};


struct GameState {
    RenderTexture2D targetRenderTexture;
    bool showDebugInfo;
    Vector2 virtualMousePos;
    float renderScale;
    Rectangle renderSourceRect;
    Rectangle renderDestRect;

    // Dane poszczególnych modu³ów
    PlayerData player;
    LevelData level;
    AudioData audio;
    UIData ui;
    GameAssets assets; // Przechowujemy za³adowane zasoby
};


class Game {
public:
    Game();
    ~Game(); // Destruktor do zwolnienia zasobów

    bool Init();      // Inicjalizacja wszystkiego
    void RunLoop();   // G³ówna pêtla gry
    void Shutdown();  // Zwolnienie zasobów

private:
    void LoadResources();
    void UnloadResources();
    void Update();    // Aktualizacja logiki gry
    void Draw();      // Rysowanie sceny
    void DrawDebug(); // Rysowanie informacji debugowania

    GameState state;
    bool initialized; // Flaga zapobiegaj¹ca podwójnej inicjalizacji/zamykaniu
};

#endif // GAME_H