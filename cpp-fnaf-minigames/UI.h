#pragma once

#ifndef UI_H
#define UI_H

#include "raylib.h"

struct UIData {
    Texture2D buttonVoicesOffTexture;
    Texture2D buttonVoicesOnTexture;
    Texture2D currentButtonTexture; // Wskaünik na aktualnπ
    Rectangle buttonRect;
    Vector2 buttonPos;
    bool isButtonClicked;
};

void InitUI(UIData& ui, const Texture2D& texOff, const Texture2D& texOn);
void UpdateUI(UIData& ui, Vector2 virtualMousePos);
void DrawUI(const UIData& ui);
void UnloadUITextures(UIData& ui); // Podobnie jak w Level, tekstury sπ ≥adowane w Game

#endif // UI_H