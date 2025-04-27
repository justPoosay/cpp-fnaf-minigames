#include "raylib.h"
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

// --- Definicja funkcji animacji ---
void UpdatePlayerAnimation(bool isGrounded, bool isMoving, bool facingRight,
    Texture2D& currentTexture, int& currentFrame,
    int animUpdateRate, int maxFrames, int& frameCounter,
    Texture2D idleRight, Texture2D idleLeft,
    Texture2D walkRight, Texture2D walkLeft,
    Texture2D jumpRight, Texture2D jumpLeft)
{
    frameCounter++;
    if (frameCounter >= animUpdateRate) {
        frameCounter = 0;
        currentFrame++;
        if (currentFrame >= maxFrames) currentFrame = 0;
    }
    if (!isGrounded) {
        currentTexture = facingRight ? jumpRight : jumpLeft;
        if (maxFrames == 1) currentFrame = 0;
    }
    else if (isMoving) {
        currentTexture = facingRight ? walkRight : walkLeft;
    }
    else {
        currentTexture = facingRight ? idleRight : idleLeft;
    }
}
// --- Koniec definicji funkcji animacji ---

int main()
{
    // --- Wirtualna/Logiczna Rozdzielczoœæ Gry ---
    const int virtualScreenWidth = 800;
    const int virtualScreenHeight = 450;
    const float virtualAspectRatio = (float)virtualScreenWidth / (float)virtualScreenHeight;

    // --- Ustawienia Poziomu i Przewijania ---
    const float levelLogicalWidth = 5000.0f;
    const float bgScrollFactor = 0.3f;

    // --- Inicjalizacja Okna, Audio, Render Target ---
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(virtualScreenWidth, virtualScreenHeight, "Chica's Magic Rainbow Land - Correct Dialogue Toggle");
    SetWindowMinSize(virtualScreenWidth / 2, virtualScreenHeight / 2);
    SetTargetFPS(60);
    InitAudioDevice();
    RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    // --- £adowanie Tekstur (Gracz, T³o, Kafelki, Przycisk) ---
    Texture2D idleRight = LoadTexture("resources/textures/sprites/chica/chica_idle_right.png");
    Texture2D idleLeft = LoadTexture("resources/textures/sprites/chica/chica_idle_left.png");
    Texture2D walkRight = LoadTexture("resources/textures/sprites/chica/chica_walk_right.png");
    Texture2D walkLeft = LoadTexture("resources/textures/sprites/chica/chica_walk_left.png");
    Texture2D jumpRight = LoadTexture("resources/textures/sprites/chica/chica_jump_right.png");
    Texture2D jumpLeft = LoadTexture("resources/textures/sprites/chica/chica_jump_left.png");

    Texture2D bg = LoadTexture("resources/background.png");
    Texture2D tile = LoadTexture("resources/textures/misc/dirt_tile.png");
    Texture2D platformTexture = LoadTexture("resources/textures/misc/platform_tile.png");

    Texture2D buttonVoicesOn = LoadTexture("resources/textures/text/voices_on.png");
    Texture2D buttonVoicesOff = LoadTexture("resources/textures/text/voices_off.png");

    const float platformWidth = platformTexture.id > 0 ? (float)platformTexture.width : 80.0f; // Domyœlna szerokoœæ
    const float platformHeight = platformTexture.id > 0 ? (float)platformTexture.height : 21.0f; // Domyœlna wysokoœæ

    // --- £adowanie Tekstur NPC ---
    Texture2D npcBodyTexture = LoadTexture("resources/textures/sprites/rainbow/chica_rainbow.png");
    Texture2D npcDialogueBodyTexture = LoadTexture("resources/textures/sprites/rainbow/chica_rainbow_pissed.png");
    const int numEyeSprites = 32;
    std::vector<Texture2D> npcEyeTextures(numEyeSprites);
    TraceLog(LOG_INFO, "Rêczne ³adowanie tekstur oczu NPC (sta³e œcie¿ki)...");
    npcEyeTextures[0] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-0 Frame-0.png");
    npcEyeTextures[1] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-1 Frame-0.png");
    npcEyeTextures[2] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-2 Frame-0.png");
    npcEyeTextures[3] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-3 Frame-0.png");
    npcEyeTextures[4] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-4 Frame-0.png");
    npcEyeTextures[5] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-5 Frame-0.png");
    npcEyeTextures[6] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-6 Frame-0.png");
    npcEyeTextures[7] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-7 Frame-0.png");
    npcEyeTextures[8] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-8 Frame-0.png");
    npcEyeTextures[9] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-9 Frame-0.png");
    npcEyeTextures[10] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-10 Frame-0.png");
    npcEyeTextures[11] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-11 Frame-0.png");
    npcEyeTextures[12] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-12 Frame-0.png");
    npcEyeTextures[13] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-13 Frame-0.png");
    npcEyeTextures[14] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-14 Frame-0.png");
    npcEyeTextures[15] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-15 Frame-0.png");
    npcEyeTextures[16] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-16 Frame-0.png");
    npcEyeTextures[17] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-17 Frame-0.png");
    npcEyeTextures[18] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-18 Frame-0.png");
    npcEyeTextures[19] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-19 Frame-0.png");
    npcEyeTextures[20] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-20 Frame-0.png");
    npcEyeTextures[21] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-21 Frame-0.png");
    npcEyeTextures[22] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-22 Frame-0.png");
    npcEyeTextures[23] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-23 Frame-0.png");
    npcEyeTextures[24] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-24 Frame-0.png");
    npcEyeTextures[25] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-25 Frame-0.png");
    npcEyeTextures[26] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-26 Frame-0.png");
    npcEyeTextures[27] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-27 Frame-0.png");
    npcEyeTextures[28] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-28 Frame-0.png");
    npcEyeTextures[29] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-29 Frame-0.png");
    npcEyeTextures[30] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-30 Frame-0.png");
    npcEyeTextures[31] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-31 Frame-0.png");
    TraceLog(LOG_INFO, "Zakoñczono próbê ³adowania tekstur oczu NPC.");

    // --- £adowanie Muzyki i DŸwiêków ---
    const char* musicPath = "resources/rainbow.mp3";
    const char* jumpPath = "resources/audio/sfx/jump.wav";
    const char* npcDialoguePath = "resources/audio/voice/voice_off_dialogue.mp3";
    const char* laserShootPath = "resources/audio/sfx/.mp3";

    Music backgroundMusic = LoadMusicStream(musicPath);
    Sound jumpSound = LoadSound(jumpPath);
    Sound npcDialogueSound = LoadSound(npcDialoguePath);

    bool musicLoaded = (GetMusicTimeLength(backgroundMusic) > 0);
    bool jumpSoundLoaded = (jumpSound.frameCount > 0);
    bool npcDialogueSoundLoaded = (npcDialogueSound.frameCount > 0);

    if (musicLoaded) { PlayMusicStream(backgroundMusic); SetMusicVolume(backgroundMusic, 0.5f); }
    else { TraceLog(LOG_WARNING, "Nie uda³o siê za³adowaæ muzyki: %s", musicPath); }
    if (!jumpSoundLoaded) { TraceLog(LOG_WARNING, "Nie uda³o siê za³adowaæ dŸwiêku skoku: %s", jumpPath); }
    if (!npcDialogueSoundLoaded) { TraceLog(LOG_WARNING, "Nie uda³o siê za³adowaæ dŸwiêku dialogu NPC: %s", npcDialoguePath); }

    // --- Sta³e i Zmienne Gracza ---
    const float playerTextureHeight = 70.0f; // Twoja oryginalna wartoœæ
    const float playerTextureWidth = 70.0f;  // Twoja oryginalna wartoœæ

    // *** NOWE: Wymiary i Offsety Hitboxa ***
    // Za³o¿enie: playerTextureWidth/Height to wymiary JEDNEJ klatki animacji
    const float playerHitboxWidth = playerTextureWidth * 0.6f;   // Np. 70% szerokoœci
    const float playerHitboxHeight = playerTextureHeight * 0.8f; // Np. 90% wysokoœci
    const float playerHitboxOffsetX = (playerTextureWidth - playerHitboxWidth) / 2.0f; // Centrowanie X
    const float playerHitboxOffsetY = (playerTextureHeight - playerHitboxHeight);     // Wyrównanie do do³u


    const float groundLevelY = virtualScreenHeight - (float)tile.height;
    const float playerVirtualScreenX = virtualScreenWidth / 2.0f - playerTextureWidth / 2.0f; // Centrujemy pe³ny sprite
    Vector2 playerPos = { 225.0f, groundLevelY - playerTextureHeight }; // Pozycja LEWEGO GÓRNEGO rogu SPRITE'A
    Vector2 playerVel = { 0.0f, 0.0f };
    const float playerSpeed = 3.8f;   // Twoje oryginalne wartoœci
    const float jumpSpeed = -5.0f;    // Twoje oryginalne wartoœci
    const float fallSpeed = 5.0f;     // Twoje oryginalne wartoœci
    const float maxJumpHeight = 270.0f; // Twoja oryginalna wartoœæ
    float jumpStartY = 0.0f;
    bool isGrounded = true;
    bool isJumping = false;
    bool jumpButtonHeld = false;
    bool facingRight = true;
    Texture2D currentTexture = idleRight;
    int currentFrame = 0;
    int frameCounter = 0;
    const int animUpdateRate = 1; // Twoja oryginalna wartoœæ
    const int idleFrames = 16;    // Twoje oryginalne wartoœci
    const int walkFrames = 16;    // Twoje oryginalne wartoœci
    const int jumpFrames = 1;     // Twoje oryginalne wartoœci
    int maxFramesForCurrentAnim = idleFrames;

    // --- Sta³e i Zmienne NPC ---
    const float npcTargetY = 5.0f;
    const float npcPaddingX = 60.0f; // Nie u¿ywane do celu
    const float npcSpeed = playerSpeed / 2.5f;
    Vector2 npcPos = { 0.0f, npcTargetY };
    int currentEyeIndex = 0;
    const float npcLeftEyeOffsetX = 90.0f;
    const float npcLeftEyeOffsetY = 18.0f;
    const float npcRightEyeOffsetX = 152.0f;
    const float npcRightEyeOffsetY = 18.0f;
    float npcTargetX = 0.0f;
    bool isNpcInDialogue = false; // *** STARTUJ POZA DIALOGIEM ***
    bool buttonCanBeClicked = true; // *** STARTUJ Z MO¯LIWOŒCI¥ KLIKNIÊCIA ***

    // --- Przewijanie ---
    float scrollX = 0.0f;
    float bgScroll = 0.0f;
    const float maxScrollX = fmaxf(0.0f, levelLogicalWidth - virtualScreenWidth);

    // --- Inicjalizacja pozycji X NPC ---
    if (npcBodyTexture.id > 0) {
        float initialTargetX = (playerPos.x + 100.0f) - (npcBodyTexture.width / 2.0f);
        npcPos.x = initialTargetX;
    }
    else {
        npcPos.x = 50.0f;
    }

    // --- Debugowanie F3 ---
    bool showDebugInfo = false;

    // --- Zmienne Przycisku ---
    const float buttonPadding = 5.0f;
    const float buttonTextureWidth = (float)buttonVoicesOff.width;
    const float buttonTextureHeight = (float)buttonVoicesOff.height;
    Vector2 buttonPos = { buttonPadding, (float)virtualScreenHeight - buttonTextureHeight - buttonPadding };
    Rectangle buttonRect = { buttonPos.x, buttonPos.y, buttonTextureWidth, buttonTextureHeight };
    bool isButtonClicked = false; // *** STARTUJ JAKO WY£¥CZONY ***
    Texture2D currentButtonTexture = buttonVoicesOn; // *** STARTUJ Z TEKSTUR¥ "ON" ***

    // --- *** DEFINICJA PLATFORM *** ---
    std::vector<Rectangle> platforms;
    // Dodaj kilka przyk³adowych platform: { x, y, width, height }
    // Upewnij siê, ¿e Y jest wzglêdem groundLevelY lub absolutne, jak wolisz
    platforms.push_back({ 300.0f, groundLevelY - 80.0f, platformWidth, platformHeight }); // <-- Zmieniono
    platforms.push_back({ 550.0f, groundLevelY - 140.0f, platformWidth, platformHeight });// <-- Zmieniono
    platforms.push_back({ 650.0f, groundLevelY - 240.0f, platformWidth, platformHeight });// <-- Zmieniono
    platforms.push_back({ 900.0f, groundLevelY - 100.0f, platformWidth, platformHeight }); // <-- Zmieniono
    platforms.push_back({ 1200.0f, groundLevelY - 180.0f, platformWidth, platformHeight });// <-- Zmieniono

    // --- G³ówna Pêtla Gry ---
    while (!WindowShouldClose())
    {
        // --- Aktualizacja Muzyki ---
        if (musicLoaded) {
            UpdateMusicStream(backgroundMusic);
            if (GetMusicTimePlayed(backgroundMusic) >= GetMusicTimeLength(backgroundMusic) - 0.1f) {
                SeekMusicStream(backgroundMusic, 0.0f);
                PlayMusicStream(backgroundMusic);
            }
        }

        // --- Obs³uga Pe³nego Ekranu ---
        if (IsKeyPressed(KEY_F11)) {
            int d = GetCurrentMonitor();
            if (IsWindowFullscreen()) {
                ToggleFullscreen();
                SetWindowSize(virtualScreenWidth, virtualScreenHeight);
            }
            else {
                SetWindowSize(GetMonitorWidth(d), GetMonitorHeight(d));
                ToggleFullscreen();
            }
        }

        // --- Przeliczanie pozycji myszy ---
        Vector2 mousePosWindow = GetMousePosition();
        float scale = std::min((float)GetScreenWidth() / virtualScreenWidth, (float)GetScreenHeight() / virtualScreenHeight);
        float offsetX = ((float)GetScreenWidth() - (virtualScreenWidth * scale)) * 0.5f;
        float offsetY = ((float)GetScreenHeight() - (virtualScreenHeight * scale)) * 0.5f;
        float virtualMouseX = (mousePosWindow.x - offsetX) / scale;
        float virtualMouseY = (mousePosWindow.y - offsetY) / scale;
        Vector2 virtualMousePos = { virtualMouseX, virtualMouseY };

        // --- Wejœcie Gracza ---
        playerVel.x = 0.0f;
        bool isMoving = false;
        if (IsKeyDown(KEY_A)) {
            playerVel.x = -playerSpeed;
            isMoving = true;
            facingRight = false;
        }
        if (IsKeyDown(KEY_D)) {
            playerVel.x = playerSpeed;
            isMoving = true;
            facingRight = true;
        }
        if (IsKeyPressed(KEY_F3)) {
            showDebugInfo = !showDebugInfo;
        }

        // --- Obs³uga Klikniêcia Przycisku ---
        // Sprawdzaj klikniêcie TYLKO jeœli przycisk *mo¿e* byæ klikniêty
        if (buttonCanBeClicked && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(virtualMousePos, buttonRect)) {
                // Klikniêcie ZAWSZE w³¹cza dialog i blokuje przycisk (bo sprawdzamy tylko gdy mo¿na klikn¹æ)
                isButtonClicked = true;
                currentButtonTexture = buttonVoicesOff;
                isNpcInDialogue = true; // W³¹cz stan dialogu (zatrzyma oczy, zmieni teksturê)
                buttonCanBeClicked = false; // ZABLOKUJ przycisk do koñca dialogu

                TraceLog(LOG_INFO, "Button turned ON. NPC dialogue mode enabled.");

                // Odtwórz dŸwiêk dialogu
                if (npcDialogueSoundLoaded && !IsSoundPlaying(npcDialogueSound)) {
					PauseMusicStream(backgroundMusic); // Wstrzymaj muzykê
                    PlaySound(npcDialogueSound);
                    TraceLog(LOG_INFO, "Playing NPC dialogue sound.");
                }
            }
        }


        jumpButtonHeld = IsKeyDown(KEY_W);
        if (IsKeyPressed(KEY_W) && isGrounded) {
            isGrounded = false;
            isJumping = true;
            jumpStartY = playerPos.y;
            playerVel.y = jumpSpeed;
            if (jumpSoundLoaded) PlaySound(jumpSound);
        }
        if (!isGrounded) {
            if (isJumping && jumpButtonHeld && (jumpStartY - playerPos.y < maxJumpHeight)) {
                playerVel.y = jumpSpeed;
            }
            else {
                if (isJumping) {
                    isJumping = false;
                }
                playerVel.y = fallSpeed;
            }
        }

        // --- FIZYKA GRACZA Z KOLIZJ¥ PLATFORM (Hitbox V5 - poprawione nazwy) ---

        // 1. Okreœlenie prêdkoœci pionowej Y (Twoja logika)
        // ----------------------------------------------------
        jumpButtonHeld = IsKeyDown(KEY_W);
        if (isGrounded) {
            if (IsKeyPressed(KEY_W)) {
                isGrounded = false; isJumping = true; jumpStartY = playerPos.y;
                playerVel.y = jumpSpeed;
                if (jumpSoundLoaded) PlaySound(jumpSound);
            }
            else { playerVel.y = 0; isJumping = false; }
        }
        else {
            if (isJumping && jumpButtonHeld && (jumpStartY - playerPos.y < maxJumpHeight)) {
                playerVel.y = jumpSpeed;
            }
            else {
                if (isJumping) isJumping = false;
                playerVel.y = fallSpeed;
            }
        }
        // ----------------------------------------------------

        // 2. Obliczenie potencjalnej nastêpnej pozycji
        // ----------------------------------------------------
        float potentialX = playerPos.x + playerVel.x;
        float potentialY = playerPos.y + playerVel.y;
        // Potencjalny HITBOX w nastêpnej klatce
        Rectangle playerNextHitbox = { potentialX + playerHitboxOffsetX, potentialY + playerHitboxOffsetY, playerHitboxWidth, playerHitboxHeight };
        // Aktualny HITBOX
        Rectangle playerCurrentHitbox = { playerPos.x + playerHitboxOffsetX, playerPos.y + playerHitboxOffsetY, playerHitboxWidth, playerHitboxHeight };
        // ----------------------------------------------------

        // 3. Rozwi¹zywanie kolizji i aktualizacja pozycji
        // ----------------------------------------------------
        bool resolvedY = false;
        isGrounded = false;

        // Kolizje pionowe (Y)
        // --------------------
        for (const auto& platform : platforms) {
            Rectangle platformRect = platform;

            // a) L¹dowanie na platformie (spadanie)
            if (playerVel.y > 0 && (playerCurrentHitbox.y + playerCurrentHitbox.height) <= platformRect.y + 1.0f) {
                // U¿ywamy hitboxa do sprawdzenia kolizji Y
                Rectangle playerNextHitboxYOnly = { playerCurrentHitbox.x, potentialY + playerHitboxOffsetY, playerHitboxWidth, playerHitboxHeight };
                if (CheckCollisionRecs(playerNextHitboxYOnly, platformRect)) {
                    potentialY = platformRect.y - playerTextureHeight; // Ustaw GÓRÊ SPRITE'A
                    playerVel.y = 0; isGrounded = true; isJumping = false; resolvedY = true;
                    break;
                }
            }
            // b) Uderzenie w spód platformy (skakanie)
            else if (playerVel.y < 0 && playerCurrentHitbox.y >= (platformRect.y + platformRect.height - 1.0f)) {
                // U¿ywamy hitboxa do sprawdzenia kolizji Y
                Rectangle playerNextHitboxYOnly = { playerCurrentHitbox.x, potentialY + playerHitboxOffsetY, playerHitboxWidth, playerHitboxHeight };
                if (CheckCollisionRecs(playerNextHitboxYOnly, platformRect)) {
                    potentialY = platformRect.y + platformRect.height - playerHitboxOffsetY; // Ustaw GÓRÊ SPRITE'A
                    playerVel.y = 0; isJumping = false; resolvedY = true;
                    break;
                }
            }
        } // Koniec pêtli platform

        // Kolizja z G£ÓWNYM POD£O¯EM (jeœli nie wyl¹dowano na platformie)
        if (!isGrounded && playerVel.y >= 0) {
            // Sprawdzamy kolizjê potencjalnego hitboxa z lini¹ ziemi
            // Obliczamy Y do³u hitboxa w potencjalnej pozycji
            float potentialHitboxBottomY = potentialY + playerHitboxOffsetY + playerHitboxHeight;
            if (potentialHitboxBottomY >= groundLevelY) { // Jeœli dó³ hitboxa jest na lub poni¿ej ziemi
                potentialY = groundLevelY - playerTextureHeight; // Ustaw GÓRÊ SPRITE'A
                playerVel.y = 0; isGrounded = true; isJumping = false; resolvedY = true;
            }
        }

        // Jeœli nie by³o kolizji l¹dowania, jesteœmy w powietrzu (chyba ¿e stoimy)
        if (!resolvedY && playerVel.y != 0) {
            isGrounded = false;
        }

        // Aktualizacja pozycji X i Y
        playerPos.y = potentialY;
        playerPos.x = potentialX;

        // Ograniczenia poziome (X) - U¿ywamy pe³nej szerokoœci SPRITE'A do ograniczeñ na krawêdziach poziomu
        if (playerPos.x < 0.0f) { playerPos.x = 0.0f; }
        if (playerPos.x + playerTextureWidth > levelLogicalWidth) { // U¿yj playerTextureWidth
            playerPos.x = levelLogicalWidth - playerTextureWidth; // U¿yj playerTextureWidth
        }
        // ----------------------------------------------------

        // --- *** Dodatkowe Sprawdzenie Stania Na Powierzchni (Stabilizacja Animacji - z hitboxem) *** ---
        Rectangle feetCheckRect = { playerPos.x + playerHitboxOffsetX, playerPos.y + playerHitboxOffsetY + playerHitboxHeight, playerHitboxWidth, 1.0f };
        bool currentlyStandingOnSurface = false;
        Vector2 feetPoint = { feetCheckRect.x + feetCheckRect.width / 2.0f, feetCheckRect.y };
        Rectangle groundCheckArea = { 0.0f, groundLevelY, levelLogicalWidth, 10.0f };

        if (CheckCollisionPointRec(feetPoint, groundCheckArea)) {
            if (playerPos.y + playerTextureHeight > groundLevelY) { playerPos.y = groundLevelY - playerTextureHeight; }
            currentlyStandingOnSurface = true; playerVel.y = 0;
        }
        if (!currentlyStandingOnSurface) {
            for (const auto& platform : platforms) {
                if (CheckCollisionRecs(feetCheckRect, platform) && (playerPos.y + playerTextureHeight <= platform.y + 1.5f)) {
                    if (playerPos.y + playerTextureHeight > platform.y) { playerPos.y = platform.y - playerTextureHeight; }
                    currentlyStandingOnSurface = true; playerVel.y = 0;
                    break;
                }
            }
        }
        isGrounded = currentlyStandingOnSurface;
        if (isGrounded) isJumping = false;
        // --- *** Koniec Dodatkowego Sprawdzenia *** ---
   

        // --- Przewijanie Kamery ---
        float targetScrollX = playerPos.x - playerVirtualScreenX;
        scrollX = fmaxf(0.0f, fminf(targetScrollX, maxScrollX));

        // --- Przewijanie T³a (Paralaksa) ---
        bgScroll = scrollX * bgScrollFactor;

        // --- Aktualizacja Logiki NPC ---
        if (npcBodyTexture.id > 0) {
            npcTargetX = (playerPos.x + playerTextureWidth / 2.0f) - (npcBodyTexture.width / 2.0f);
            float diffX = npcTargetX - npcPos.x;
            float moveAmount = npcSpeed;
            if (fabsf(diffX) < moveAmount) { npcPos.x = npcTargetX; }
            else if (diffX > 0) { npcPos.x += moveAmount; }
            else if (diffX < 0) { npcPos.x -= moveAmount; }

            // Pozycja Y jest zawsze sta³a
            npcPos.y = npcTargetY;

            // Obliczaj k¹t oczu tylko jeœli NIE ma dialogu
            if (!isNpcInDialogue) {
                Vector2 npcCenter = { npcPos.x + npcBodyTexture.width / 2.0f, npcPos.y + npcBodyTexture.height / 2.0f };
                Vector2 playerCenter = { playerPos.x + playerTextureWidth / 2.0f, playerPos.y + playerTextureHeight / 2.0f };
                float angleX = playerCenter.x - npcCenter.x;
                float angleY = playerCenter.y - npcCenter.y;
                float angleRad = atan2f(-angleY, angleX);
                if (angleRad < 0) { angleRad += 2.0f * PI; }
                currentEyeIndex = (int)roundf(angleRad / (2.0f * PI) * numEyeSprites);
                currentEyeIndex = currentEyeIndex % numEyeSprites;
            }
        }

        // --- Sprawdzenie koñca dialogu ---
        // Warunek: NPC jest w dialogu ORAZ dŸwiêk jest za³adowany ORAZ dŸwiêk w³aœnie przesta³ graæ
        if (isNpcInDialogue && npcDialogueSoundLoaded && !IsSoundPlaying(npcDialogueSound))
        {
            TraceLog(LOG_INFO, "NPC dialogue finished,.");
		    // dodaj atakowanie gracza
        }

        // --- Aktualizacja Animacji Gracza ---
        if (!isGrounded) maxFramesForCurrentAnim = jumpFrames;
        else if (isMoving) maxFramesForCurrentAnim = walkFrames;
        else maxFramesForCurrentAnim = idleFrames;
        UpdatePlayerAnimation(isGrounded, isMoving, facingRight, currentTexture, currentFrame, animUpdateRate, maxFramesForCurrentAnim, frameCounter, idleRight, idleLeft, walkRight, walkLeft, jumpRight, jumpLeft);

        // --- Obliczanie Pozycji Rysowania Gracza na Ekranie ---
        float actualPlayerVirtualDrawX;
        if (scrollX <= 0.0f) { actualPlayerVirtualDrawX = playerPos.x; }
        else if (scrollX >= maxScrollX) { actualPlayerVirtualDrawX = playerPos.x - maxScrollX; }
        else { actualPlayerVirtualDrawX = playerVirtualScreenX; }
        Vector2 playerDrawVirtualPos = { actualPlayerVirtualDrawX, playerPos.y };

        // --- Rysowanie (do Render Texture) ---
        BeginTextureMode(target);
        ClearBackground(RAYWHITE);

        // --- Rysowanie T³a ---
        if (bg.width > 0) {
            float bgX = -fmodf(bgScroll, (float)bg.width);
            DrawTextureEx(bg, { bgX , 0.0f }, 0.0f, 1.0f, WHITE);
            DrawTextureEx(bg, { bgX + bg.width, 0.0f }, 0.0f, 1.0f, WHITE);
            if (bgX + 2 * bg.width < virtualScreenWidth) {
                DrawTextureEx(bg, { bgX + 2 * bg.width, 0.0f }, 0.0f, 1.0f, WHITE);
            }
        }

        // --- Rysowanie Pod³o¿a ---
        if (tile.width > 0) {
            int tc = (virtualScreenWidth / tile.width) + 2;
            float tsX = fmodf(scrollX, (float)tile.width);
            for (int i = -1; i < tc; i++) {
                float tX = (float)i * tile.width - tsX;
                DrawTexture(tile, (int)tX, (int)groundLevelY, WHITE);
            }
        }


        // ---  RYSOWANIE PLATFORM  ---
        if (platformTexture.id > 0) { // Rysuj tylko, jeœli tekstura zosta³a poprawnie za³adowana
            for (const auto& platform : platforms) {
                // Rysuj teksturê platformy w odpowiednim miejscu, uwzglêdniaj¹c scrollX
                DrawTexture(platformTexture, (int)(platform.x - scrollX), (int)platform.y, WHITE);
            }
        }
        else {
            // Opcjonalny Fallback: Jeœli tekstura siê nie za³adowa³a, nadal rysuj prostok¹ty
            for (const auto& platform : platforms) {
                DrawRectangleRec({ platform.x - scrollX, platform.y, platform.width, platform.height }, DARKGRAY); // Inny kolor dla fallbacku
            }
        }

        // --- Rysowanie NPC ---
        if (npcBodyTexture.id > 0) {
            Vector2 npcDrawPos = { npcPos.x - scrollX, npcPos.y };
            Texture2D bodyToDraw = isNpcInDialogue ? npcDialogueBodyTexture : npcBodyTexture;

            if (bodyToDraw.id > 0) {
                DrawTextureV(bodyToDraw, npcDrawPos, WHITE);
            }
            else {
                DrawTextureV(npcBodyTexture, npcDrawPos, WHITE); // Fallback
                if (isNpcInDialogue) TraceLog(LOG_WARNING, "Próba rysowania NPC w dialogu, ale tekstura dialogu nieza³adowana.");
            }

            // Rysuj oczy tylko jeœli NIE ma dialogu
            if (!isNpcInDialogue) {
                if (currentEyeIndex >= 0 && currentEyeIndex < numEyeSprites && npcEyeTextures[currentEyeIndex].id > 0) {
                    Texture2D currentEyeTexture = npcEyeTextures[currentEyeIndex];
                    Vector2 leftEyeDrawPos = { npcDrawPos.x + npcLeftEyeOffsetX, npcDrawPos.y + npcLeftEyeOffsetY };
                    Vector2 rightEyeDrawPos = { npcDrawPos.x + npcRightEyeOffsetX, npcDrawPos.y + npcRightEyeOffsetY };
                    DrawTextureV(currentEyeTexture, leftEyeDrawPos, WHITE);
                    DrawTextureV(currentEyeTexture, rightEyeDrawPos, WHITE);
                }
            }
        }

        // --- Rysowanie Gracza ---
        Rectangle sourceRect = { (float)playerTextureWidth * currentFrame, 0, (float)playerTextureWidth, (float)playerTextureHeight };
        if (playerDrawVirtualPos.x < virtualScreenWidth && playerDrawVirtualPos.x + playerTextureWidth > 0) {
            DrawTextureRec(currentTexture, sourceRect, playerDrawVirtualPos, WHITE);
        }

        // --- Rysowanie Przycisku ---
        DrawTextureV(currentButtonTexture, buttonPos, WHITE);

        // --- Rysowanie Debug Info (F3) ---
        if (showDebugInfo) {

            // >>> TUTAJ DODAJEMY RYSOWANIE SIATKI WSPÓ£RZÊDNYCH ŒWIATA GRY <<<
            // ----------------------------------------------------------
            const int gridSpacing = 100; // Odstêp miêdzy liniami siatki (w pikselach œwiata gry)
            const int labelFontSize = 10;
            Color gridLineColor = Fade(DARKGRAY, 0.5f);
            Color gridLabelColor = Fade(WHITE, 0.6f);

            // Oblicz zakres X œwiata gry widoczny na ekranie
            float startWorldX = scrollX;
            float endWorldX = scrollX + virtualScreenWidth;

            // Oblicz pierwsz¹ liniê pionow¹ X, która ma byæ narysowana
            // ZnajdŸ najbli¿sz¹ wielokrotnoœæ gridSpacing >= startWorldX
            int firstGridX = (int)ceil(startWorldX / gridSpacing) * gridSpacing;

            // Rysuj linie pionowe (oœ X) w widocznym zakresie
            for (int worldX = firstGridX; worldX < endWorldX; worldX += gridSpacing) {
                // Oblicz pozycjê X linii na ekranie
                float screenX = (float)worldX - scrollX;
                DrawLine((int)screenX, 0, (int)screenX, virtualScreenHeight, gridLineColor);
                // Rysuj etykietê X (wspó³rzêdna œwiata gry)
                DrawText(TextFormat("%d", worldX), (int)screenX + 2, 2, labelFontSize, gridLabelColor);
            }

            // Rysuj linie poziome (oœ Y) - one siê nie przesuwaj¹ z kamer¹ X
            // Zakres Y jest sta³y (od 0 do virtualScreenHeight)
            for (int y = gridSpacing; y < virtualScreenHeight; y += gridSpacing) {
                DrawLine(0, y, virtualScreenWidth, y, gridLineColor);
                DrawText(TextFormat("%d", y), 2, y + 2, labelFontSize, gridLabelColor);
            }
            // ----------------------------------------------------------
            // >>> KONIEC RYSOWANIA SIATKI WSPÓ£RZÊDNYCH ŒWIATA GRY <<<

            // Najpierw rysujemy t³o i tekst (teraz PO siatce)
            DrawRectangle(5, 5, 280, 430, Fade(BLACK, 0.7f)); // Zwiêkszona wysokoœæ, jeœli potrzeba
            int textY = 10; int lineHeight = 20;
            DrawText(TextFormat("FPS: %d", GetFPS()), 10, textY, lineHeight, WHITE); textY += lineHeight;
            DrawText(TextFormat("P.Pos: (%.1f, %.1f)", playerPos.x, playerPos.y), 10, textY, lineHeight, LIME); textY += lineHeight;

            DrawRectangle(5, 5, 260, 350, Fade(BLACK, 0.6f));
            DrawText(TextFormat("P.Log X: %.1f", playerPos.x), 10, 10, 20, LIME);
            DrawText(TextFormat("P.VDraw X: %.1f", playerDrawVirtualPos.x), 10, 30, 20, LIME);
            DrawText(TextFormat("P.Log Y: %.1f", playerPos.y), 10, 50, 20, LIME);
            DrawText(TextFormat("P.Vel Y: %.1f", playerVel.y), 10, 70, 20, LIME);
            DrawText(TextFormat("ScrollX (Cam): %.1f", scrollX), 10, 90, 20, LIME);
            DrawText(TextFormat("MaxScrollX: %.1f", maxScrollX), 10, 110, 20, ORANGE);
            DrawText(TextFormat("BGScroll: %.1f", bgScroll), 10, 130, 20, SKYBLUE);
            DrawText(TextFormat("LevelWidth: %.1f", levelLogicalWidth), 10, 150, 20, SKYBLUE);
            DrawText(TextFormat("Grounded: %s", isGrounded ? "True" : "False"), 10, 170, 20, LIME);
            DrawText(TextFormat("Jumping(Up): %s", isJumping ? "True" : "False"), 10, 190, 20, YELLOW);
            DrawText(TextFormat("Button Clicked: %s", isButtonClicked ? "Yes" : "No"), 10, 210, 20, YELLOW);
            DrawText(TextFormat("NPC Pos X: %.1f", npcPos.x), 10, 230, 20, BLUE);
            DrawText(TextFormat("NPC Pos Y: %.1f", npcPos.y), 10, 250, 20, BLUE);
            DrawText(TextFormat("NPC Target X: %.1f", npcTargetX), 10, 270, 20, MAGENTA);
            DrawText(TextFormat("NPC Eye Idx: %d", currentEyeIndex), 10, 290, 20, WHITE);
            DrawText(TextFormat("NPC In Dialog: %s", isNpcInDialogue ? "Yes" : "No"), 10, 310, 20, PINK);

            if (musicLoaded) { DrawText(TextFormat("Music: %.2f/%.2f", GetMusicTimePlayed(backgroundMusic), GetMusicTimeLength(backgroundMusic)), 10, 330, 20, SKYBLUE); }
            else { DrawText("Music: Not Loaded", 10, 330, 20, RED); }
            DrawText("DEBUG (F3) Full (F11)", virtualScreenWidth - MeasureText("DEBUG (F3) Full (F11)", 20) - 10, 10, 20, WHITE);

            for (const auto& platform : platforms) {
                // Oblicz pozycjê rysowania platformy (uwzglêdniaj¹c scrollX)
                Rectangle platformDrawRect = { platform.x - scrollX, platform.y, platform.width, platform.height };

                // 1. Narysuj ramkê hitboxa platformy (ju¿ to pewnie masz)
                DrawRectangleLinesEx(platformDrawRect, 2.0f, DARKPURPLE);

                // >>> TUTAJ DODAJEMY RYSOWANIE WSPÓ£RZÊDNYCH PLATFORMY <<<
                // ----------------------------------------------------------
                // Przygotuj tekst ze wspó³rzêdnymi
                const char* coordText = TextFormat("X:%.0f, Y:%.0f", platform.x, platform.y);
                int coordFontSize = 10; // Rozmiar czcionki
                Color coordTextColor = WHITE; // Kolor tekstu
                Color coordBgColor = Fade(BLACK, 0.6f); // Kolor t³a pod tekstem

                // Oblicz rozmiar tekstu, aby wycentrowaæ t³o
                Vector2 textSize = MeasureTextEx(GetFontDefault(), coordText, (float)coordFontSize, 1.0f);

                // Oblicz pozycjê tekstu (nad œrodkiem platformy)
                float textPosX = platformDrawRect.x + (platformDrawRect.width / 2.0f) - (textSize.x / 2.0f);
                float textPosY = platformDrawRect.y - textSize.y - 4.0f; // Pozycja Y nad platform¹ (z ma³ym marginesem 4px)

                // Oblicz prostok¹t t³a pod tekst
                Rectangle bgRect = { textPosX - 2.0f, textPosY - 1.0f, textSize.x + 4.0f, textSize.y + 2.0f }; // Ma³y padding wokó³ tekstu

                // Narysuj t³o i tekst
                DrawRectangleRec(bgRect, coordBgColor);
                DrawText(coordText, (int)textPosX, (int)textPosY, coordFontSize, coordTextColor);
                // ----------------------------------------------------------
                // >>> KONIEC RYSOWANIA WSPÓ£RZÊDNYCH PLATFORMY <<<
            }

            // 1. Hitbox Gracza (Nowy - mniejszy i z offsetem)
            Rectangle playerDrawHitbox = { playerDrawVirtualPos.x + playerHitboxOffsetX, playerDrawVirtualPos.y + playerHitboxOffsetY, playerHitboxWidth, playerHitboxHeight };
            DrawRectangleLinesEx(playerDrawHitbox, 2.0f, GREEN); // Zielona ramka dla hitboxa

            // 2. Hitboxy Platform
            for (const auto& platform : platforms) {
                // Oblicz pozycjê rysowania platformy (uwzglêdniaj¹c scrollX)
                Rectangle platformDrawRect = { platform.x - scrollX, platform.y, platform.width, platform.height };
                DrawRectangleLinesEx(platformDrawRect, 2.0f, DARKPURPLE); // Czerwona ramka dla platform
            }

            // 3. Linia G³ównego Pod³o¿a (opcjonalnie, dla wizualizacji)
            // Rysujemy liniê na ca³ej szerokoœci wirtualnego ekranu na poziomie groundLevelY
            DrawLineEx({ 0.0f, groundLevelY }, { (float)virtualScreenWidth, groundLevelY }, 2.0f, DARKGRAY);

            // 4. Hitbox "Stóp" (u¿ywany do stabilizacji isGrounded - opcjonalnie)
            // Mo¿na odkomentowaæ, aby zobaczyæ, gdzie dzia³a sprawdzanie stania
              Rectangle feetCheckRectDraw = { playerPos.x - scrollX, playerPos.y + playerTextureHeight, playerTextureWidth, 1.0f };
              DrawRectangleRec(feetCheckRectDraw, YELLOW); // ¯ó³ty, cienki prostok¹t

              DrawRectangleLinesEx(playerDrawHitbox, 2.0f, GREEN);
              Rectangle playerDrawSpriteBox = { playerDrawVirtualPos.x, playerDrawVirtualPos.y, playerTextureWidth, playerTextureHeight };
              DrawRectangleLinesEx(playerDrawSpriteBox, 1.0f, GRAY);
              for (const auto& platform : platforms) {
                  Rectangle platformDrawRect = { platform.x - scrollX, platform.y, platform.width, platform.height };
                  DrawRectangleLinesEx(platformDrawRect, 2.0f, DARKPURPLE);
              }
              DrawLineEx({ 0.0f, groundLevelY }, { (float)virtualScreenWidth, groundLevelY }, 2.0f, DARKGRAY);

        }

        EndTextureMode();

        // --- Rysowanie na Ekran (Skalowanie Render Texture) ---
        BeginDrawing();
        ClearBackground(BLACK);
        float finalScale = std::min((float)GetScreenWidth() / virtualScreenWidth, (float)GetScreenHeight() / virtualScreenHeight);
        float finalOffsetX = ((float)GetScreenWidth() - (virtualScreenWidth * finalScale)) * 0.5f;
        float finalOffsetY = ((float)GetScreenHeight() - (virtualScreenHeight * finalScale)) * 0.5f;
        Rectangle src = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
        Rectangle dst = { finalOffsetX, finalOffsetY, (float)virtualScreenWidth * finalScale, (float)virtualScreenHeight * finalScale };
        DrawTexturePro(target.texture, src, dst, { 0, 0 }, 0.0f, WHITE);
        EndDrawing();
    }

    // --- Zwolnienie Zasobów ---
    UnloadRenderTexture(target);
    UnloadTexture(bg); UnloadTexture(tile);
    UnloadTexture(idleRight); UnloadTexture(idleLeft);
    UnloadTexture(walkRight); UnloadTexture(walkLeft);
    UnloadTexture(jumpRight); UnloadTexture(jumpLeft);
    UnloadTexture(buttonVoicesOff); UnloadTexture(buttonVoicesOn);
    if (npcBodyTexture.id > 0) UnloadTexture(npcBodyTexture);
    if (npcDialogueBodyTexture.id > 0) UnloadTexture(npcDialogueBodyTexture);
    if (platformTexture.id > 0) UnloadTexture(platformTexture);
    TraceLog(LOG_INFO, "Zwalnianie tekstur oczu NPC...");
    for (int i = 0; i < numEyeSprites; ++i) { UnloadTexture(npcEyeTextures[i]); }
    TraceLog(LOG_INFO, "Zasoby NPC zwolnione.");
    if (musicLoaded) UnloadMusicStream(backgroundMusic);
    if (jumpSoundLoaded) UnloadSound(jumpSound);
    if (npcDialogueSoundLoaded) UnloadSound(npcDialogueSound);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}