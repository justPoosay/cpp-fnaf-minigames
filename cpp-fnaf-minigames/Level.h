#pragma once

#ifndef LEVEL_H
#define LEVEL_H

#include "raylib.h"
#include "Player.h" // Potrzebuje PlayerData do obliczenia scrolla

// Struktura przechowuj�ca dane poziomu
struct LevelData
{
    float scrollX;
    float bgScroll;
    float maxScrollX;
    float groundLevelY;
    Texture2D tileTexture;
    Texture2D backgroundTexture;
};

// Funkcje zarz�dzaj�ce poziomem
void InitLevel(LevelData& level, const Texture2D& tileTex, const Texture2D& bgTex);
void UpdateLevelScroll(LevelData& level, const PlayerData& player);
void DrawLevel(const LevelData& level);
void UnloadLevelTextures(LevelData& level); // Do zwolnienia zasob�w

#endif // LEVEL_H