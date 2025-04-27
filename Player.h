#pragma once

#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

// Struktura przechowuj�ca dane gracza
struct PlayerData
{
    Vector2 position;
    Vector2 velocity;
    float jumpStartY;
    bool isGrounded;
    bool isJumping;     // Aktualny stan wykonywania skoku
    bool jumpButtonHeld; // Czy przycisk skoku jest trzymany
    bool facingRight;
    bool isMoving;      // Czy gracz si� porusza w tej klatce

    // Stan animacji
    Texture2D currentTexture; // Wska�nik na aktualnie u�ywan� tekstur� (�adowane w Game)
    int currentFrame;
    int animFrameCounter;
    int maxFramesForCurrentAnim;

    // Rzeczywista pozycja rysowania na wirtualnym ekranie
    Vector2 virtualDrawPos;
};

// Funkcje zarz�dzaj�ce graczem
void InitPlayer(PlayerData& player, float groundLevelY);
void UpdatePlayer(PlayerData& player, float groundLevelY, float scrollX, float maxScrollX); // Dodano scrollX, maxScrollX
void DrawPlayer(const PlayerData& player); // Przyjmuje sta�� referencj�, bo tylko rysuje

#endif // PLAYER_H