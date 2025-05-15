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
    // Dodaj wi�cej zasob�w w miar� potrzeb
};


struct GameState {
    RenderTexture2D targetRenderTexture;
    bool showDebugInfo;
    Vector2 virtualMousePos;
    float renderScale;
    Rectangle renderSourceRect;
    Rectangle renderDestRect;

    // Dane poszczeg�lnych modu��w
    PlayerData player;
    LevelData level;
    AudioData audio;
    UIData ui;
    GameAssets assets; // Przechowujemy za�adowane zasoby
};


class Game {
public:
    Game();
    ~Game(); // Destruktor do zwolnienia zasob�w

    bool Init();      // Inicjalizacja wszystkiego
    void RunLoop();   // G��wna p�tla gry
    void Shutdown();  // Zwolnienie zasob�w

private:
    void LoadResources();
    void UnloadResources();
    void Update();    // Aktualizacja logiki gry
    void Draw();      // Rysowanie sceny
    void DrawDebug(); // Rysowanie informacji debugowania

    GameState state;
    bool initialized; // Flaga zapobiegaj�ca podw�jnej inicjalizacji/zamykaniu
};

#endif // GAME_H