#pragma once

#ifndef ANIMATION_H
#define ANIMATION_H

#include "raylib.h"
#include "Player.h" // Potrzebuje definicji PlayerData (lub jej stanu)

// Przekazujemy teraz referencjê do PlayerData, aby modyfikowaæ jego stan animacji
void UpdatePlayerAnimation(PlayerData& player,
    const Texture2D& idleRight, const Texture2D& idleLeft,
    const Texture2D& walkRight, const Texture2D& walkLeft,
    const Texture2D& jumpRight, const Texture2D& jumpLeft);

#endif // ANIMATION_H