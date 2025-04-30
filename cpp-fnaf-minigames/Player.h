#pragma once

#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

// Struktura przechowuj¹ca dane gracza
struct PlayerData
{
    Vector2 position;
    Vector2 velocity;
    float jumpStartY;
    bool isGrounded;
    bool isJumping;     // Aktualny stan wykonywania skoku
    bool jumpButtonHeld; // Czy przycisk skoku jest trzymany
    bool facingRight;
    bool isMoving;      // Czy gracz siê porusza w tej klatce

    // Stan animacji
    Texture2D currentTexture; // WskaŸnik na aktualnie u¿ywan¹ teksturê (³adowane w Game)
    int currentFrame;
    int animFrameCounter;
    int maxFramesForCurrentAnim;

    // Rzeczywista pozycja rysowania na wirtualnym ekranie
    Vector2 virtualDrawPos;
};

// Funkcje zarz¹dzaj¹ce graczem
void InitPlayer(PlayerData& player, float groundLevelY);
void UpdatePlayer(PlayerData& player, float groundLevelY, float scrollX, float maxScrollX); // Dodano scrollX, maxScrollX
void DrawPlayer(const PlayerData& player); // Przyjmuje sta³¹ referencjê, bo tylko rysuje

#endif // PLAYER_H