//#include "Player.h"
//#include "Constants.h"
//#include "Audio.h" // Potrzebne do odtworzenia d�wi�ku skoku
//#include <cmath>   // Dla fmaxf
//
//void InitPlayer(PlayerData& player, float groundLevelY)
//{
//    player.position = { 50.0f, groundLevelY - PLAYER_TEXTURE_HEIGHT };
//    player.velocity = { 0.0f, 0.0f };
//    player.jumpStartY = 0.0f;
//    player.isGrounded = true;
//    player.isJumping = false;
//    player.jumpButtonHeld = false;
//    player.facingRight = true;
//    player.isMoving = false;
//
//    // currentTexture zostanie ustawiona w p�tli gry przez UpdatePlayerAnimation
//    player.currentFrame = 0;
//    player.animFrameCounter = 0;
//    player.maxFramesForCurrentAnim = IDLE_FRAMES; // Domy�lnie
//
//    player.virtualDrawPos = player.position; // Pocz�tkowo
//}
//
//void UpdatePlayer(PlayerData& player, float groundLevelY, float scrollX, float maxScrollX)
//{
//    // --- Wej�cie ---
//    player.velocity.x = 0.0f;
//    player.isMoving = false;
//
//    if (IsKeyDown(KEY_A))
//    {
//        if (player.position.x > 0.0f) // Sprawdzenie granicy lewej
//        {
//            player.velocity.x = -PLAYER_SPEED;
//            player.isMoving = true;
//            player.facingRight = false;
//        }
//    }
//    if (IsKeyDown(KEY_D))
//    {
//        player.velocity.x = PLAYER_SPEED;
//        player.isMoving = true;
//        player.facingRight = true;
//    }
//
//    // --- Fizyka Skoku ---
//    player.jumpButtonHeld = IsKeyDown(KEY_W);
//
//    if (IsKeyPressed(KEY_W) && player.isGrounded)
//    {
//        player.isJumping = true;
//        player.isGrounded = false;
//        player.jumpStartY = player.position.y;
//        player.velocity.y = JUMP_SPEED;
//        PlayJumpSound(); // Odtw�rz d�wi�k skoku
//    }
//
//    if (!player.isGrounded)
//    {
//        // Zmienny skok
//        if (player.isJumping && player.jumpButtonHeld && (player.jumpStartY - player.position.y < MAX_JUMP_HEIGHT))
//        {
//            player.velocity.y = JUMP_SPEED; // Utrzymaj pr�dko�� wznoszenia
//        }
//        else // Opadanie (puszczono W lub osi�gni�to max wysoko��)
//        {
//            if (player.isJumping) // Zako�cz faz� aktywnego skoku
//            {
//                player.isJumping = false;
//            }
//            player.velocity.y = FALL_SPEED; // Zacznij opada�
//        }
//    }
//    else {
//        player.velocity.y = 0; // Zeruj pr�dko�� pionow� na ziemi
//    }
//
//
//    // --- Aktualizacja Logicznej Pozycji ---
//    player.position.x += player.velocity.x;
//    player.position.y += player.velocity.y;
//
//    // Proste ograniczenie lewej kraw�dzi �wiata
//    if (player.position.x < 0.0f)
//        player.position.x = 0.0f;
//
//    // --- Kolizja z Pod�o�em ---
//    if (player.position.y + PLAYER_TEXTURE_HEIGHT >= groundLevelY && player.velocity.y >= 0) // Sprawdzaj tylko przy ruchu w d� lub zerowym
//    {
//        player.position.y = groundLevelY - PLAYER_TEXTURE_HEIGHT;
//        if (!player.isGrounded) // Je�li w�a�nie wyl�dowa�
//        {
//            player.isGrounded = true;
//            player.isJumping = false; // Zresetuj stan skoku
//        }
//        player.velocity.y = 0; // Zawsze zeruj pr�dko�� Y na ziemi
//    }
//    else
//    {
//        // Je�li jest w powietrzu (poni�ej ziemi nie liczymy, ale fizyka powinna to obs�u�y�)
//        if (player.position.y + PLAYER_TEXTURE_HEIGHT < groundLevelY)
//            player.isGrounded = false;
//    }
//
//
//    // --- Obliczanie Rzeczywistej Pozycji Rysowania Gracza na WIRTUALNYM ekranie ---
//    const float playerVirtualScreenX = VIRTUAL_SCREEN_WIDTH / 2.0f - PLAYER_TEXTURE_WIDTH / 2.0f;
//
//    if (scrollX <= 0.0f) // Na lewej kraw�dzi
//    {
//        player.virtualDrawPos.x = player.position.x;
//    }
//    else if (scrollX >= maxScrollX) // Na prawej kraw�dzi (lub poza)
//    {
//        // Pozycja gracza wzgl�dem pocz�tku wirtualnego ekranu, gdy kamera jest zablokowana na ko�cu
//        player.virtualDrawPos.x = player.position.x - maxScrollX;
//    }
//    else // Kamera pod��a za graczem
//    {
//        player.virtualDrawPos.x = playerVirtualScreenX;
//    }
//    player.virtualDrawPos.y = player.position.y; // Pozycja Y jest zawsze taka sama jak logiczna
//}
//
//void DrawPlayer(const PlayerData& player)
//{
//    // Sprawdzenie, czy tekstura jest za�adowana (sanity check)
//    if (player.currentTexture.id > 0)
//    {
//        Rectangle sourceRect = {
//            PLAYER_TEXTURE_WIDTH * player.currentFrame,
//            0,
//            PLAYER_TEXTURE_WIDTH,
//            PLAYER_TEXTURE_HEIGHT
//        };
//        DrawTextureRec(player.currentTexture, sourceRect, player.virtualDrawPos, WHITE);
//    }
//    else {
//        // Opcjonalnie: Rysuj prostok�t zast�pczy, je�li tekstura nie jest gotowa
//        DrawRectangleV(player.virtualDrawPos, { PLAYER_TEXTURE_WIDTH, PLAYER_TEXTURE_HEIGHT }, MAGENTA);
//    }
//}