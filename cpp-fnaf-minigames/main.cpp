#include "raylib.h"
#include "Resources.h"

#include <cmath>
#include <string>
#include <vector>
#include <algorithm>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

// Define animations :p
void UpdatePlayerAnimation(bool isGrounded, bool isMoving, bool facingRight, Texture2D& currentTexture, int& currentFrame, int animUpdateRate, int maxFrames, int& frameCounter, Texture2D idleRight, Texture2D idleLeft, Texture2D walkRight, Texture2D walkLeft, Texture2D jumpRight, Texture2D jumpLeft)
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

int main()
{
    // ustawienia okna i przewijania tla TwT
    const int virtualScreenWidth = 800;
    const int virtualScreenHeight = 450;

    const float levelLogicalWidth = 5000.0f;
    const float bgScrollFactor = 0.3f;

    const float virtualAspectRatio = (float)virtualScreenWidth / (float)virtualScreenHeight;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(virtualScreenWidth, virtualScreenHeight, "Magic Rainbow Land >:3");
    SetWindowMinSize(virtualScreenWidth / 2, virtualScreenHeight / 2);
    SetTargetFPS(60);
    InitAudioDevice();

// --------- RESOURCE LOADER ---------
    GameResources resources = LoadGameResources();

    RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    // platform specs
    const float platformWidth = resources.platformTexture.id > 0 ? (float)resources.platformTexture.width : 80.0f;
    const float platformHeight = resources.platformTexture.id > 0 ? (float)resources.platformTexture.height : 21.0f;


// --------- REGULACJA LOGIKI RUCHU ---------

    const float playerTextureHeight = 70.0f;
    const float playerTextureWidth = 70.0f;

    // dostosowanie rozmiary hitboxa playera
    const float playerHitboxWidth = playerTextureWidth * 0.6f;
    const float playerHitboxHeight = playerTextureHeight * 0.8f;
    const float playerHitboxOffsetX = (playerTextureWidth - playerHitboxWidth) / 2.0f;
    const float playerHitboxOffsetY = (playerTextureHeight - playerHitboxHeight);


    const float groundLevelY = virtualScreenHeight - (resources.tile.id > 0 ? (float)resources.tile.height : 32.0f);
    const float playerVirtualScreenX = virtualScreenWidth / 2.0f - playerTextureWidth / 2.0f;


// --------- DOSTOSUJ FIZYKE RUCHU CHICI!! ---------
    Vector2 playerPos = { 225.0f, groundLevelY - playerTextureHeight };
    Vector2 playerVel = { 0.0f, 0.0f };
    const float playerSpeed = 3.8f;
    const float jumpSpeed = -5.0f;
    const float fallSpeed = 5.0f;
    const float maxJumpHeight = 270.0f;
    float jumpStartY = 0.0f;
    bool isGrounded = true;
    bool isJumping = false;
    bool jumpButtonHeld = false;
    bool facingRight = true;
    Texture2D currentTexture = resources.idleRight;
    int currentFrame = 0;
    int frameCounter = 0;
    const int animUpdateRate = 1;
    const int idleFrames = 16;
    const int walkFrames = 16;
    const int jumpFrames = 1;
    int maxFramesForCurrentAnim = idleFrames;


// --------- DOSTOSUJ FIZYKE RUCHU RAINBOWA!! ---------
    const float npcTargetY = 5.0f;
    const float npcPaddingX = 60.0f;
    const float npcSpeed = playerSpeed / 2.5f;
    Vector2 npcPos = { 0.0f, npcTargetY };
    int currentEyeIndex = 0;
    const float npcLeftEyeOffsetX = 90.0f;
    const float npcLeftEyeOffsetY = 18.0f;
    const float npcRightEyeOffsetX = 152.0f;
    const float npcRightEyeOffsetY = 18.0f;
    float npcTargetX = 0.0f;
    bool isNpcInDialogue = false;
    bool buttonCanBeClicked = true;

    // logika przewijania tla
    float scrollX = 0.0f;
    float bgScroll = 0.0f;
    const float maxScrollX = fmaxf(0.0f, levelLogicalWidth - virtualScreenWidth);

    // pozycjonowanie Rainbowa
    if (resources.npcBodyTexture.id > 0) {
        float initialTargetX = (playerPos.x + 100.0f) - (resources.npcBodyTexture.width / 2.0f);
        npcPos.x = initialTargetX;
    }
    else {
        npcPos.x = 50.0f;
    }

    // temp: DEBUG SCREEN
    bool showDebugInfo = false;

    // voices button
    const float buttonPadding = 5.0f;
    const float buttonTextureWidth = (resources.buttonVoicesOff.id > 0 ? (float)resources.buttonVoicesOff.width : 100.0f);
    const float buttonTextureHeight = (resources.buttonVoicesOff.id > 0 ? (float)resources.buttonVoicesOff.height : 30.0f);
    Vector2 buttonPos = { buttonPadding, (float)virtualScreenHeight - buttonTextureHeight - buttonPadding };
    Rectangle buttonRect = { buttonPos.x, buttonPos.y, buttonTextureWidth, buttonTextureHeight };
    bool isButtonClicked = false;
    Texture2D currentButtonTexture = resources.buttonVoicesOn;


// --------- POLOZENIE PLATFORM NA MAPIE!! ---------
    std::vector<Rectangle> platforms;
    platforms.push_back({ 300.0f, groundLevelY - 80.0f, platformWidth, platformHeight });
    platforms.push_back({ 550.0f, groundLevelY - 140.0f, platformWidth, platformHeight });
    platforms.push_back({ 650.0f, groundLevelY - 240.0f, platformWidth, platformHeight });
    platforms.push_back({ 900.0f, groundLevelY - 100.0f, platformWidth, platformHeight });
    platforms.push_back({ 1200.0f, groundLevelY - 180.0f, platformWidth, platformHeight });


    if (resources.musicLoaded)
    {
        PlayMusicStream(resources.backgroundMusic);
        SetMusicVolume(resources.backgroundMusic, 0.5f);
    }


// --------------- MAIN GAME LOOP ---------------
    while (!WindowShouldClose())
    {
        if (resources.musicLoaded) {
            UpdateMusicStream(resources.backgroundMusic);
            if (GetMusicTimePlayed(resources.backgroundMusic) >= GetMusicTimeLength(resources.backgroundMusic) - 0.1f) {
                SeekMusicStream(resources.backgroundMusic, 0.0f);
                PlayMusicStream(resources.backgroundMusic);
            }
        }

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

        Vector2 mousePosWindow = GetMousePosition();
        float scale = std::min((float)GetScreenWidth() / virtualScreenWidth, (float)GetScreenHeight() / virtualScreenHeight);
        float offsetX = ((float)GetScreenWidth() - (virtualScreenWidth * scale)) * 0.5f;
        float offsetY = ((float)GetScreenHeight() - (virtualScreenHeight * scale)) * 0.5f;
        float virtualMouseX = (mousePosWindow.x - offsetX) / scale;
        float virtualMouseY = (mousePosWindow.y - offsetY) / scale;
        Vector2 virtualMousePos = { virtualMouseX, virtualMouseY };

    // --------- PLAYER MOVEMENT INPUT ---------
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

        if (buttonCanBeClicked && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(virtualMousePos, buttonRect)) {
                isButtonClicked = true;
                currentButtonTexture = resources.buttonVoicesOff;
                isNpcInDialogue = true;
                buttonCanBeClicked = false;

                if (resources.npcDialogueSoundLoaded && !IsSoundPlaying(resources.npcDialogueSound)) {
                    PauseMusicStream(resources.backgroundMusic);
                    PlaySound(resources.npcDialogueSound);
                }
            }
        }


        jumpButtonHeld = IsKeyDown(KEY_W);
        if (IsKeyPressed(KEY_W) && isGrounded) {
            isGrounded = false;
            isJumping = true;
            jumpStartY = playerPos.y;
            playerVel.y = jumpSpeed;
            if (resources.jumpSoundLoaded) PlaySound(resources.jumpSound);
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


    // ---------- FIZYKA GRACZA NA PLATFORMIE ----------
        jumpButtonHeld = IsKeyDown(KEY_W);
        if (isGrounded) {
            if (IsKeyPressed(KEY_W)) {
                isGrounded = false; isJumping = true; jumpStartY = playerPos.y;
                playerVel.y = jumpSpeed;
                if (resources.jumpSoundLoaded) PlaySound(resources.jumpSound);
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

        float potentialX = playerPos.x + playerVel.x;
        float potentialY = playerPos.y + playerVel.y;
        Rectangle playerNextHitbox = { potentialX + playerHitboxOffsetX, potentialY + playerHitboxOffsetY, playerHitboxWidth, playerHitboxHeight };
        Rectangle playerCurrentHitbox = { playerPos.x + playerHitboxOffsetX, playerPos.y + playerHitboxOffsetY, playerHitboxWidth, playerHitboxHeight };

        bool resolvedY = false;
        isGrounded = false;

    // Kolizje:
        for (const auto& platform : platforms) {
            Rectangle platformRect = platform;

            // I. platforma a ladowanie
            if (playerVel.y > 0 && (playerCurrentHitbox.y + playerCurrentHitbox.height) <= platformRect.y + 1.0f) {
                Rectangle playerNextHitboxYOnly = { playerCurrentHitbox.x, potentialY + playerHitboxOffsetY, playerHitboxWidth, playerHitboxHeight };
                if (CheckCollisionRecs(playerNextHitboxYOnly, platformRect)) {
                    potentialY = platformRect.y - playerTextureHeight;
                    playerVel.y = 0;
                    isGrounded = true;
                    isJumping = false;
                    resolvedY = true;
                    break;
                }
            }
            // II. platforma a skakanie
            else if (playerVel.y < 0 && playerCurrentHitbox.y >= (platformRect.y + platformRect.height - 1.0f)) {
                Rectangle playerNextHitboxYOnly = { playerCurrentHitbox.x, potentialY + playerHitboxOffsetY, playerHitboxWidth, playerHitboxHeight };
                if (CheckCollisionRecs(playerNextHitboxYOnly, platformRect)) {
                    potentialY = platformRect.y + platformRect.height - playerHitboxOffsetY;
                    playerVel.y = 0;
                    isJumping = false;
                    resolvedY = true;
                    break;
                }
            }
        }


    // Ground collision handler
        if (!isGrounded && playerVel.y >= 0) {
            float potentialHitboxBottomY = potentialY + playerHitboxOffsetY + playerHitboxHeight;
            if (potentialHitboxBottomY >= groundLevelY) {
                potentialY = groundLevelY - playerTextureHeight;
                playerVel.y = 0;
                isGrounded = true;
                isJumping = false;
                resolvedY = true;
            }
        }

        if (!resolvedY && playerVel.y != 0) {
            isGrounded = false;
        }

        playerPos.y = potentialY;
        playerPos.x = potentialX;

        if (playerPos.x < 0.0f) { playerPos.x = 0.0f; }
        if (playerPos.x + playerTextureWidth > levelLogicalWidth) {
            playerPos.x = levelLogicalWidth - playerTextureWidth;
        }

        Rectangle feetCheckRect = { playerPos.x + playerHitboxOffsetX, playerPos.y + playerHitboxOffsetY + playerHitboxHeight + 0.1f, playerHitboxWidth, 1.0f };
        bool currentlyStandingOnSurface = false;
        Rectangle groundCheckArea = { 0.0f, groundLevelY, levelLogicalWidth, 10.0f };

        if (CheckCollisionRecs(feetCheckRect, groundCheckArea)) {
            if (playerPos.y + playerTextureHeight > groundLevelY) { playerPos.y = groundLevelY - playerTextureHeight; }
            currentlyStandingOnSurface = true; playerVel.y = 0;
        }
        if (!currentlyStandingOnSurface) {
            for (const auto& platform : platforms) {
                if (CheckCollisionRecs(feetCheckRect, platform)) {
                    if (playerPos.y + playerTextureHeight > platform.y) { playerPos.y = platform.y - playerTextureHeight; }
                    currentlyStandingOnSurface = true; playerVel.y = 0;
                    break;
                }
            }
        }
        isGrounded = currentlyStandingOnSurface;
        if (isGrounded) isJumping = false;


    // ----- Camera Scroll -----
        float targetScrollX = playerPos.x - playerVirtualScreenX;
        scrollX = fmaxf(0.0f, fminf(targetScrollX, maxScrollX));

    // ----- Background Scroll (Paralax) -----
        bgScroll = scrollX * bgScrollFactor;

    // Logika Rainbowa
        if (resources.npcBodyTexture.id > 0) {
            npcTargetX = (playerPos.x + playerTextureWidth / 2.0f) - (resources.npcBodyTexture.width / 2.0f);
            float diffX = npcTargetX - npcPos.x;
            float moveAmount = npcSpeed;
            if (fabsf(diffX) < moveAmount) { npcPos.x = npcTargetX; }
            else if (diffX > 0) { npcPos.x += moveAmount; }
            else if (diffX < 0) { npcPos.x -= moveAmount; }

            npcPos.y = npcTargetY;

            if (!isNpcInDialogue) {
                Vector2 npcCenter = { npcPos.x + resources.npcBodyTexture.width / 2.0f, npcPos.y + resources.npcBodyTexture.height / 2.0f };
                Vector2 playerCenter = { playerPos.x + playerTextureWidth / 2.0f, playerPos.y + playerTextureHeight / 2.0f };
                float angleX = playerCenter.x - npcCenter.x;
                float angleY = playerCenter.y - npcCenter.y;
                float angleRad = atan2f(-angleY, angleX);
                if (angleRad < 0) { angleRad += 2.0f * PI; }
                currentEyeIndex = (int)roundf(angleRad / (2.0f * PI) * resources.numEyeSprites);
                currentEyeIndex = currentEyeIndex % resources.numEyeSprites;
            }
        }

        // --- rainbow atakuje jesli nacisnales voice off ---
        if (isNpcInDialogue && resources.npcDialogueSoundLoaded && !IsSoundPlaying(resources.npcDialogueSound))
        {
            // TODO: dodaj atakowanie gracza
            // isNpcInDialogue = false;
        }

        if (!isGrounded) maxFramesForCurrentAnim = jumpFrames;
        else if (isMoving) maxFramesForCurrentAnim = walkFrames;
        else maxFramesForCurrentAnim = idleFrames;
        UpdatePlayerAnimation(isGrounded, isMoving, facingRight, currentTexture, currentFrame, animUpdateRate, maxFramesForCurrentAnim, frameCounter,
            resources.idleRight, resources.idleLeft, resources.walkRight, resources.walkLeft, resources.jumpRight, resources.jumpLeft);

        float actualPlayerVirtualDrawX;
        if (scrollX <= 0.0f) { actualPlayerVirtualDrawX = playerPos.x; }
        else if (scrollX >= maxScrollX) { actualPlayerVirtualDrawX = playerPos.x - maxScrollX; }
        else { actualPlayerVirtualDrawX = playerVirtualScreenX; }
        Vector2 playerDrawVirtualPos = { actualPlayerVirtualDrawX, playerPos.y };

        BeginTextureMode(target);
        ClearBackground(RAYWHITE);

        // --- Bg Draw ---
        if (resources.bg.id > 0) {
            float bgX = -fmodf(bgScroll, (float)resources.bg.width);
            DrawTextureEx(resources.bg, { bgX , 0.0f }, 0.0f, 1.0f, WHITE);
            DrawTextureEx(resources.bg, { bgX + resources.bg.width, 0.0f }, 0.0f, 1.0f, WHITE);
            if (bgX + 2 * resources.bg.width < virtualScreenWidth) {
                DrawTextureEx(resources.bg, { bgX + 2 * resources.bg.width, 0.0f }, 0.0f, 1.0f, WHITE);
            }
        }

        // --- Ground Tiles draw ---
        if (resources.tile.id > 0) {
            float startTileX = -fmodf(scrollX, (float)resources.tile.width);
            int tilesToDraw = (virtualScreenWidth / resources.tile.width) + 2;
            for (int i = 0; i < tilesToDraw; i++) {
                float tileX = startTileX + i * resources.tile.width;
                if (tileX + resources.tile.width > 0 && tileX < virtualScreenWidth) {
                    DrawTexture(resources.tile, (int)tileX, (int)groundLevelY, WHITE);
                }
            }
        }


        // --- Platform draw ---
        if (resources.platformTexture.id > 0) {
            for (const auto& platform : platforms) {
                if (platform.x - scrollX + platform.width > 0 && platform.x - scrollX < virtualScreenWidth) {
                    DrawTexture(resources.platformTexture, (int)(platform.x - scrollX), (int)platform.y, WHITE);
                }
            }
        }
        else {
            for (const auto& platform : platforms) {
                if (platform.x - scrollX + platform.width > 0 && platform.x - scrollX < virtualScreenWidth) {
                    DrawRectangleRec({ platform.x - scrollX, platform.y, platform.width, platform.height }, DARKGRAY);
                }
            }
        }

        // --- Rainbow draw ---
        if (resources.npcBodyTexture.id > 0) {
            Vector2 npcDrawPos = { npcPos.x - scrollX, npcPos.y };
            if (npcDrawPos.x + resources.npcBodyTexture.width > 0 && npcDrawPos.x < virtualScreenWidth) {
                Texture2D bodyToDraw = isNpcInDialogue ? resources.npcDialogueBodyTexture : resources.npcBodyTexture;

                if (bodyToDraw.id == 0) bodyToDraw = resources.npcBodyTexture;

                DrawTextureV(bodyToDraw, npcDrawPos, WHITE);

                // --- Rainbow Eyes draw ---
                if (!isNpcInDialogue) {
                    if (currentEyeIndex >= 0 && currentEyeIndex < resources.numEyeSprites && resources.npcEyeTextures[currentEyeIndex].id > 0) {
                        Texture2D currentEyeTexture = resources.npcEyeTextures[currentEyeIndex];
                        Vector2 leftEyeDrawPos = { npcDrawPos.x + npcLeftEyeOffsetX, npcDrawPos.y + npcLeftEyeOffsetY };
                        Vector2 rightEyeDrawPos = { npcDrawPos.x + npcRightEyeOffsetX, npcDrawPos.y + npcRightEyeOffsetY };
                        DrawTextureV(currentEyeTexture, leftEyeDrawPos, WHITE);
                        DrawTextureV(currentEyeTexture, rightEyeDrawPos, WHITE);
                    }
                }
            }
        }

        // --- Player draw ---
        Rectangle sourceRect = { (float)currentFrame * playerTextureWidth, 0, (float)playerTextureWidth, (float)playerTextureHeight };
        if (playerDrawVirtualPos.x + playerTextureWidth > 0 && playerDrawVirtualPos.x < virtualScreenWidth) {
            DrawTextureRec(currentTexture, sourceRect, playerDrawVirtualPos, WHITE);
        }

        // --- Voice Button draw ---
        DrawTextureV(currentButtonTexture, buttonPos, WHITE);

    // ------- DEBUG DRAW -------
        if (showDebugInfo) {
            const int gridSpacing = 100;
            const int labelFontSize = 10;
            Color gridLineColor = Fade(DARKGRAY, 0.5f);
            Color gridLabelColor = Fade(WHITE, 0.6f);

            float startWorldX = scrollX;
            float endWorldX = scrollX + virtualScreenWidth;

            int firstGridX = (int)floor(startWorldX / gridSpacing) * gridSpacing; // Zmieniono ceil na floor

            for (int worldX = firstGridX; worldX < endWorldX + gridSpacing; worldX += gridSpacing) {
                float screenX = (float)worldX - scrollX;
                DrawLine((int)screenX, 0, (int)screenX, virtualScreenHeight, gridLineColor);
                if (screenX >= 0 && screenX < virtualScreenWidth)
                    DrawText(TextFormat("%d", worldX), (int)screenX + 2, 2, labelFontSize, gridLabelColor);
            }

            for (int y = gridSpacing; y < virtualScreenHeight; y += gridSpacing) {
                DrawLine(0, y, virtualScreenWidth, y, gridLineColor);
                if (y >= 0 && y < virtualScreenHeight) 
                    DrawText(TextFormat("%d", y), 2, y + 2, labelFontSize, gridLabelColor);
            }

            DrawRectangle(5, 5, 280, 430, Fade(BLACK, 0.7f));
            int textY = 10; int lineHeight = 20;
            DrawText(TextFormat("FPS: %d", GetFPS()), 10, textY, lineHeight, WHITE); textY += lineHeight;
            DrawText(TextFormat("P.Pos: (%.1f, %.1f)", playerPos.x, playerPos.y), 10, textY, lineHeight, LIME); textY += lineHeight;

            DrawText(TextFormat("P.Log X: %.1f", playerPos.x), 10, textY, lineHeight, LIME); textY += lineHeight;
            DrawText(TextFormat("P.VDraw X: %.1f", playerDrawVirtualPos.x), 10, textY, lineHeight, LIME); textY += lineHeight;
            DrawText(TextFormat("P.Log Y: %.1f", playerPos.y), 10, textY, lineHeight, LIME); textY += lineHeight;
            DrawText(TextFormat("P.Vel Y: %.1f", playerVel.y), 10, textY, lineHeight, LIME); textY += lineHeight;
            DrawText(TextFormat("ScrollX (Cam): %.1f", scrollX), 10, textY, lineHeight, LIME); textY += lineHeight;
            DrawText(TextFormat("MaxScrollX: %.1f", maxScrollX), 10, textY, lineHeight, ORANGE); textY += lineHeight;
            DrawText(TextFormat("BGScroll: %.1f", bgScroll), 10, textY, lineHeight, SKYBLUE); textY += lineHeight;
            DrawText(TextFormat("LevelWidth: %.1f", levelLogicalWidth), 10, textY, lineHeight, SKYBLUE); textY += lineHeight;
            DrawText(TextFormat("Grounded: %s", isGrounded ? "True" : "False"), 10, textY, lineHeight, LIME); textY += lineHeight;
            DrawText(TextFormat("Jumping(Up): %s", isJumping ? "True" : "False"), 10, textY, lineHeight, YELLOW); textY += lineHeight;
            DrawText(TextFormat("Button Clicked: %s", isButtonClicked ? "Yes" : "No"), 10, textY, lineHeight, YELLOW); textY += lineHeight;
            DrawText(TextFormat("NPC Pos X: %.1f", npcPos.x), 10, textY, lineHeight, BLUE); textY += lineHeight;
            DrawText(TextFormat("NPC Pos Y: %.1f", npcPos.y), 10, textY, lineHeight, BLUE); textY += lineHeight;
            DrawText(TextFormat("NPC Target X: %.1f", npcTargetX), 10, textY, lineHeight, MAGENTA); textY += lineHeight;
            DrawText(TextFormat("NPC Eye Idx: %d", currentEyeIndex), 10, textY, lineHeight, WHITE); textY += lineHeight;
            DrawText(TextFormat("NPC In Dialog: %s", isNpcInDialogue ? "Yes" : "No"), 10, textY, lineHeight, PINK); textY += lineHeight;

            if (resources.musicLoaded) { DrawText(TextFormat("Music: %.2f/%.2f", GetMusicTimePlayed(resources.backgroundMusic), GetMusicTimeLength(resources.backgroundMusic)), 10, textY, lineHeight, SKYBLUE); }
            else { DrawText("Music: Not Loaded", 10, textY, lineHeight, RED); }
            textY += lineHeight;

            DrawText("DEBUG (F3) Full (F11)", virtualScreenWidth - MeasureText("DEBUG (F3) Full (F11)", 20) - 10, 10, 20, WHITE);

            for (const auto& platform : platforms) {
                Rectangle platformDrawRect = { platform.x - scrollX, platform.y, platform.width, platform.height };
                if (platformDrawRect.x + platformDrawRect.width > 0 && platformDrawRect.x < virtualScreenWidth)
                {
                    DrawRectangleLinesEx(platformDrawRect, 1.0f, DARKPURPLE);

                    const char* coordText = TextFormat("X:%.0f, Y:%.0f", platform.x, platform.y);
                    int coordFontSize = 10;
                    Color coordTextColor = WHITE;
                    Color coordBgColor = Fade(BLACK, 0.6f);

                    Vector2 textSize = MeasureTextEx(GetFontDefault(), coordText, (float)coordFontSize, 1.0f);
                    float textPosX = platformDrawRect.x + (platformDrawRect.width / 2.0f) - (textSize.x / 2.0f);
                    float textPosY = platformDrawRect.y - textSize.y - 4.0f;

                    Rectangle bgRect = { textPosX - 2.0f, textPosY - 1.0f, textSize.x + 4.0f, textSize.y + 2.0f };

                    if (textPosY > 0) {
                        DrawRectangleRec(bgRect, coordBgColor);
                        DrawText(coordText, (int)textPosX, (int)textPosY, coordFontSize, coordTextColor);
                    }
                    else {
                        textPosY = platformDrawRect.y + platformDrawRect.height + 4.0f;
                        bgRect.y = textPosY - 1.0f;
                        if (textPosY + textSize.y < virtualScreenHeight) {
                            DrawRectangleRec(bgRect, coordBgColor);
                            DrawText(coordText, (int)textPosX, (int)textPosY, coordFontSize, coordTextColor);
                        }
                    }
                }
            }

            Rectangle playerDrawHitbox = { playerDrawVirtualPos.x + playerHitboxOffsetX, playerDrawVirtualPos.y + playerHitboxOffsetY, playerHitboxWidth, playerHitboxHeight };
            DrawRectangleLinesEx(playerDrawHitbox, 2.0f, GREEN);

            Rectangle playerDrawSpriteBox = { playerDrawVirtualPos.x, playerDrawVirtualPos.y, playerTextureWidth, playerTextureHeight };
            DrawRectangleLinesEx(playerDrawSpriteBox, 1.0f, GRAY);

            DrawLineEx({ 0.0f, groundLevelY }, { (float)virtualScreenWidth, groundLevelY }, 2.0f, DARKGRAY);

            Rectangle feetCheckRectDraw = { playerDrawVirtualPos.x + playerHitboxOffsetX, playerDrawVirtualPos.y + playerHitboxOffsetY + playerHitboxHeight, playerHitboxWidth, 1.0f }; // U¿yto playerDrawVirtualPos
            DrawRectangleRec(feetCheckRectDraw, YELLOW);

        }

        EndTextureMode();

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

// --- Resource Unload ---
    UnloadRenderTexture(target);
    UnloadGameResources(resources);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}