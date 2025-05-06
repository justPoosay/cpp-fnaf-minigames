#include "raylib.h"
#include "RainbowLandResources.h"
#include "GameSettings.h"

#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

struct Butterfly {
    Vector2 position;        // Aktualna pozycja w œwiecie gry
    Vector2 startPosition;   // Pozycja pocz¹tkowa, wokó³ której bêdzie siê porusza³
    int animCurrentFrame;
    int animFrameCounter;
    float movementPhase;     // Faza ruchu (0 do 2*PI) do obliczeñ sin/cos
    float movementSpeedOffset; // Niewielkie przesuniêcie prêdkoœci dla ró¿norodnoœci
};

// Define animations :p
static void UpdatePlayerAnimation(bool isGrounded, bool isMoving, bool facingRight, Texture2D& currentTexture, int& currentFrame, int animUpdateRate, int maxFrames, int& frameCounter, Texture2D idleRight, Texture2D idleLeft, Texture2D walkRight, Texture2D walkLeft, Texture2D jumpRight, Texture2D jumpLeft)
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

// Butterfly animation function
static void UpdateButterflyAnimation(
    Texture2D& currentDisplayTexture, // Output: The texture to use for drawing (will be set to butterflySpriteSheet)
    int& currentFrame,                // Input/Output: The current frame index of the animation (0 to maxFrames-1)
    int animUpdateRate,               // Input: How many game frames to wait before advancing the animation frame
    int totalFramesInSheet,           // Input: The total number of frames in the butterfly spritesheet (e.g., 6)
    int& frameCounter,                // Input/Output: A counter to manage animation timing
    Texture2D butterfly)              // Input: The spritesheet texture containing all butterfly frames
{
    frameCounter++;
    if (frameCounter >= animUpdateRate) {
        frameCounter = 0; // Reset the counter
        currentFrame++;   // Move to the next animation frame
        if (currentFrame >= totalFramesInSheet) {
            currentFrame = 0; // Loop back to the first frame
        }
    }

    // Set the texture to be displayed. For the butterfly, it's always its single spritesheet.
    // This line makes the function's signature and usage pattern similar to 
    // UpdatePlayerAnimation, where 'currentTexture' is an output parameter.
    currentDisplayTexture = butterfly;
}

int runMagicRainbowLand(GraphicsQuality quality) {
    // Teraz masz dostêp do zmiennej 'quality' w tej funkcji
    std::cout << "Uruchamiam Chica's Magic Rainbow z jakoœci¹: " << quality << std::endl;

// ustawienia okna i przewijania tla TwT
    const int virtualScreenWidth = 800;
    const int virtualScreenHeight = 450;

    const float levelLogicalWidth = 5000.0f;
    const float bgScrollFactor = 0.3f;

    const float virtualAspectRatio = (float)virtualScreenWidth / (float)virtualScreenHeight;

// --------- RESOURCE LOADER ---------
    RainbowLandGameResources resources = LoadRainbowLandResources(quality);

    RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    // props specs
    const float fenceWidth = resources.fenceProp.id > 0 ? (float)resources.fenceProp.width : 159.0f;
    const float fenceHeight = resources.fenceProp.id > 0 ? (float)resources.fenceProp.height : 43.0f;

    const float flowerSmallWidth = resources.flowerSmallProp.id > 0 ? (float)resources.flowerSmallProp.width : 60.0f;
    const float flowerSmallHeight = resources.flowerSmallProp.id > 0 ? (float)resources.flowerSmallProp.height : 60.0f;

    const float flowerBigWidth = resources.flowerBigProp.id > 0 ? (float)resources.flowerBigProp.width : 100.0f;
    const float flowerBigHeight = resources.flowerBigProp.id > 0 ? (float)resources.flowerBigProp.height : 100.0f;

    const float checkpointFlagWidth = resources.checkpointFlag.id > 0 ? (float)resources.checkpointFlag.width : 49.0f;
    const float checkpointFlagHeight = resources.checkpointFlag.id > 0 ? (float)resources.checkpointFlag.height : 51.0f;

    // sunflower specs
    const float sunflowerWidth = resources.sunflower.id > 0 ? (float)resources.sunflower.width : 250.0f;
    const float sunflowerHeight = resources.sunflower.id > 0 ? (float)resources.sunflower.height : 300.0f;

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
    const float rainbowTargetY = 5.0f;
    const float rainbowPaddingX = 60.0f;
    const float rainbowSpeed = playerSpeed / 1.75f;
    Vector2 rainbowPos = { 0.0f, rainbowTargetY };
    int currentEyeIndex = 0;
    const float rainbowLeftEyeOffsetX = 90.0f;
    const float rainbowLeftEyeOffsetY = 18.0f;
    const float rainbowRightEyeOffsetX = 152.0f;
    const float rainbowRightEyeOffsetY = 18.0f;
    float rainbowTargetX = 0.0f;
    bool israinbowInDialogue = false;
    bool buttonCanBeClicked = true;


// --------- DOSTOSUJ FIZYKE RUCHU MOTYLKA!! ---------
     // Example for one butterfly
     Texture2D currentButterflySprite; // This will be set by UpdateButterflyAnimation
     int butterflyCurrentAnimFrame = 0;
     int butterflyAnimFrameCounter = 0;

     // If you have the texture in your resources struct:
     Texture2D butterflySheet = resources.butterfly; // Assuming it's loaded


    // logika przewijania tla
    float scrollX = 0.0f;
    float bgScroll = 0.0f;
    const float maxScrollX = fmaxf(0.0f, levelLogicalWidth - virtualScreenWidth);

    // pozycjonowanie Rainbowa
    if (resources.rbowBodyTexture.id > 0) {
        float initialTargetX = (playerPos.x + 100.0f) - (resources.rbowBodyTexture.width / 2.0f);
        rainbowPos.x = initialTargetX;
    }
    else {
        rainbowPos.x = 50.0f;
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


// --------- POLOZENIE PROPSOW NA MAPIE!! ---------
    std::vector<Rectangle> fenceProps; // fence
    fenceProps.push_back({ 275.0f, groundLevelY - 43.0f, fenceWidth, fenceHeight });
    fenceProps.push_back({ 434.0f, groundLevelY - 43.0f, fenceWidth, fenceHeight });
    fenceProps.push_back({ 593.0f, groundLevelY - 43.0f, fenceWidth, fenceHeight });
    fenceProps.push_back({ 752.0f, groundLevelY - 43.0f, fenceWidth, fenceHeight });
    fenceProps.push_back({ 911.0f, groundLevelY - 43.0f, fenceWidth, fenceHeight });


    std::vector<Rectangle> checkpointFlags; // flag
    checkpointFlags.push_back({ 100.0f, groundLevelY - 51.0f, checkpointFlagWidth, checkpointFlagHeight });
    checkpointFlags.push_back({ 820.0f, groundLevelY - 51.0f, checkpointFlagWidth, checkpointFlagHeight });


    std::vector<Rectangle> flowerSmallProps; // small flower
    flowerSmallProps.push_back({ 600.0f, groundLevelY - 60.0f, flowerSmallWidth, flowerSmallHeight });
    flowerSmallProps.push_back({ 700.0f, groundLevelY - 60.0f, flowerSmallWidth, flowerSmallHeight });


    std::vector<Rectangle> flowerBigProps; // big flower
    flowerBigProps.push_back({ 1000.0f, groundLevelY - 100.0f, flowerBigWidth, flowerBigHeight });


// --------- POLOZENIE SLONECZNIKOW NA MAPIE!! ---------
    std::vector<Rectangle> deadlySunflowers; // sunflower
    deadlySunflowers.push_back({ 920.0f, groundLevelY - 300.0f, sunflowerWidth, sunflowerHeight });
    deadlySunflowers.push_back({ 1220.0f, groundLevelY - 300.0f, sunflowerWidth, sunflowerHeight });

// --------- POLOZENIE PLATFORM NA MAPIE!! ---------
    std::vector<Rectangle> platforms;
    platforms.push_back({ 260.0f, groundLevelY - 180.0f, platformWidth, platformHeight });
    platforms.push_back({ 360.0f, groundLevelY - 100.0f, platformWidth, platformHeight });
    platforms.push_back({ 460.0f, groundLevelY - 180.0f, platformWidth, platformHeight });
    platforms.push_back({ 560.0f, groundLevelY - 100.0f, platformWidth, platformHeight });
    platforms.push_back({ 660.0f, groundLevelY - 180.0f, platformWidth, platformHeight });


    if (resources.backgroundMusicLoaded) {
        PlayMusicStream(resources.backgroundMusic);
        SetMusicVolume(resources.backgroundMusic, 0.5f);
    }

// ------- BUTTERFLY SETUP -------
    std::vector<Butterfly> butterflies;
    const int BUTTERFLY_TOTAL_FRAMES = 6;     // Zgodnie z poleceniem
    const int BUTTERFLY_ANIM_UPDATE_RATE = 3; // Co ile klatek gry zmienia siê klatka animacji motyla
    const float BUTTERFLY_MOVEMENT_RADIUS_X = 40.0f;
    const float BUTTERFLY_MOVEMENT_RADIUS_Y = 20.0f;
    const float BUTTERFLY_BASE_MOVEMENT_SPEED = 0.8f; // Radiany na sekundê (wp³ywa na szybkoœæ cyklu)
    // Obliczanie szerokoœci/wysokoœci klatki motyla na podstawie za³adowanej tekstury
    const float BUTTERFLY_SPRITE_WIDTH = (resources.butterfly.id > 0) ? (float)resources.butterfly.width / BUTTERFLY_TOTAL_FRAMES : 32.0f;
    const float BUTTERFLY_SPRITE_HEIGHT = (resources.butterfly.id > 0) ? (float)resources.butterfly.height : 32.0f;


    if (resources.butterfly.id > 0) { // Tylko jeœli tekstura jest za³adowana
        // Dodaj motyle w ró¿nych miejscach

        butterflies.push_back({
                {350.0f, groundLevelY - 150.0f}, // Pozycja startowa (bêdzie te¿ centrum ruchu)
                {350.0f, groundLevelY - 150.0f},
                0, 0, 0.0f, // animFrame, animCounter, movementPhase
                0.0f // movementSpeedOffset
            });

        butterflies.push_back({
                {750.0f, groundLevelY - 200.0f},
                {750.0f, groundLevelY - 200.0f},
                GetRandomValue(0, BUTTERFLY_TOTAL_FRAMES - 1), 0, // Losowa klatka startowa animacji
                (float)GetRandomValue(0, 628) / 100.0f, // Losowa faza ruchu (0 do 2*PI)
                (float)GetRandomValue(-20, 20) / 100.0f // Losowe ma³e przesuniêcie prêdkoœci
            });

        butterflies.push_back({
                {1100.0f, groundLevelY - 120.0f},
                {1100.0f, groundLevelY - 120.0f},
                GetRandomValue(0, BUTTERFLY_TOTAL_FRAMES - 1), 0,
                (float)GetRandomValue(0, 628) / 100.0f,
                (float)GetRandomValue(-20, 20) / 100.0f
            });

        butterflies.push_back({
               {150.0f, groundLevelY - 70.0f},
               {150.0f, groundLevelY - 70.0f},
               GetRandomValue(0, BUTTERFLY_TOTAL_FRAMES - 1), 0,
               (float)GetRandomValue(0, 628) / 100.0f,
               (float)GetRandomValue(-20, 20) / 100.0f
           });
    }
    // --- END BUTTERFLY SETUP ---

// -------- PLAYING RAINBOW DIALOGUES --------
    if (!resources.rbowDialogues.empty() && resources.rbowDialogues[0].frameCount > 0) {
        TraceLog(LOG_INFO, "RAINBOW LAND: Playing first dialogue line.");
        PlaySound(resources.rbowDialogues[0]); // Odtwórz pierwszy dŸwiêk z wektora
    }

    // TODO: while() loop that plays every next voiceline after death

// --------------- MAIN GAME LOOP ---------------
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (resources.backgroundMusicLoaded) {
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
                israinbowInDialogue = true;
                buttonCanBeClicked = false;

                if (resources.rbowVoiceOffSoundLoaded && !IsSoundPlaying(resources.rbowVoiceOff)) {
                    PauseMusicStream(resources.backgroundMusic);
                    PlaySound(resources.rbowVoiceOff);
                }
            }
        }


        jumpButtonHeld = IsKeyDown(KEY_W);
        if (IsKeyPressed(KEY_W) && isGrounded) {
            isGrounded = false;
            isJumping = true;
            jumpStartY = playerPos.y;
            playerVel.y = jumpSpeed;
            if (resources.jumpSoundLoaded) PlaySound(resources.jump);
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


       if (butterflySheet.id > 0) 
           UpdateButterflyAnimation(
           currentButterflySprite,          // Output texture for drawing
           butterflyCurrentAnimFrame,       // Current frame
           BUTTERFLY_ANIM_UPDATE_RATE,      // Speed
           BUTTERFLY_TOTAL_FRAMES,          // Total frames
           butterflyAnimFrameCounter,       // Frame counter
           butterflySheet                   // The butterfly spritesheet
        );
    

    // ---------- FIZYKA GRACZA NA PLATFORMIE ----------
        jumpButtonHeld = IsKeyDown(KEY_W);
        if (isGrounded) {
            if (IsKeyPressed(KEY_W)) {
                isGrounded = false;
                isJumping = true; 
                jumpStartY = playerPos.y;
                playerVel.y = jumpSpeed;
                if (resources.jumpSoundLoaded)
                    PlaySound(resources.jump);
            }
            else {
                playerVel.y = 0;
                isJumping = false;
            }
        }
        else {
            if (isJumping && jumpButtonHeld && (jumpStartY - playerPos.y < maxJumpHeight)) {
                playerVel.y = jumpSpeed;
            }
            else {
                if (isJumping)
                    isJumping = false;
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

        for (const auto& flag : checkpointFlags) {
            Rectangle flagRect = flag;
            if (CheckCollisionRecs(playerCurrentHitbox, flagRect)) {
                //tutaj daj kod do wyswietlenia CHECKPOINT
                break;
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
            currentlyStandingOnSurface = true;
            playerVel.y = 0;
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
        if (resources.rbowBodyTexture.id > 0) {
            rainbowTargetX = (playerPos.x + playerTextureWidth / 2.0f) - (resources.rbowBodyTexture.width / 2.0f);
            float diffX = rainbowTargetX - rainbowPos.x;
            float moveAmount = rainbowSpeed;
            if (fabsf(diffX) < moveAmount) 
                rainbowPos.x = rainbowTargetX;
            else if (diffX > 0)
                rainbowPos.x += moveAmount; 
            else if (diffX < 0) 
                rainbowPos.x -= moveAmount; 

            rainbowPos.y = rainbowTargetY;

            if (!israinbowInDialogue) {
                Vector2 rainbowCenter = { rainbowPos.x + resources.rbowBodyTexture.width / 2.0f, rainbowPos.y + resources.rbowBodyTexture.height / 2.0f };
                Vector2 playerCenter = { playerPos.x + playerTextureWidth / 2.0f, playerPos.y + playerTextureHeight / 2.0f };
                float angleX = playerCenter.x - rainbowCenter.x;
                float angleY = playerCenter.y - rainbowCenter.y;
                float angleRad = atan2f(-angleY, angleX);
                if (angleRad < 0) { angleRad += 2.0f * PI; }
                currentEyeIndex = (int)roundf(angleRad / (2.0f * PI) * resources.numEyeSprites);
                currentEyeIndex = currentEyeIndex % resources.numEyeSprites;
            }
        }

        // --- rainbow atakuje jesli nacisnales voice off ---
        if (israinbowInDialogue && resources.rbowVoiceOffSoundLoaded && !IsSoundPlaying(resources.rbowVoiceOff))
        {
            // TODO: dodaj atakowanie gracza
            // israinbowInDialogue = false;
        }




    // --- BUTTERFLY LOGIC & ANIMATION UPDATE ---
        if (resources.butterfly.id > 0) {
            for (auto& butterfly : butterflies) {
                // Aktualizacja fazy ruchu
                float currentMovementSpeed = BUTTERFLY_BASE_MOVEMENT_SPEED + butterfly.movementSpeedOffset;
                butterfly.movementPhase += currentMovementSpeed * dt; // dt dla p³ynnoœci
                if (butterfly.movementPhase >= 2.0f * PI) {
                    butterfly.movementPhase -= 2.0f * PI;
                }

                // Aktualizacja pozycji na podstawie ruchu cyklicznego
                butterfly.position.x = butterfly.startPosition.x + BUTTERFLY_MOVEMENT_RADIUS_X * cosf(butterfly.movementPhase);
                butterfly.position.y = butterfly.startPosition.y + BUTTERFLY_MOVEMENT_RADIUS_Y * sinf(butterfly.movementPhase);

                // Aktualizacja animacji motyla
                Texture2D dummyTextureForAnimFunc; // Funkcja tego oczekuje, ale nie u¿ywamy jej outputu tutaj
                UpdateButterflyAnimation(
                    dummyTextureForAnimFunc,
                    butterfly.animCurrentFrame,
                    BUTTERFLY_ANIM_UPDATE_RATE,
                    BUTTERFLY_TOTAL_FRAMES,
                    butterfly.animFrameCounter,
                    resources.butterfly // Przekazujemy g³ówny arkusz sprite'ów motyla
                );
            }
        }
        // --- END BUTTERFLY LOGIC ---

        
        if (!isGrounded) 
            maxFramesForCurrentAnim = jumpFrames;
        else if (isMoving)
            maxFramesForCurrentAnim = walkFrames;
        else
            maxFramesForCurrentAnim = idleFrames;
        UpdatePlayerAnimation(isGrounded, isMoving, facingRight, currentTexture, currentFrame, animUpdateRate, maxFramesForCurrentAnim, frameCounter, resources.idleRight, resources.idleLeft, resources.walkRight, resources.walkLeft, resources.jumpRight, resources.jumpLeft);

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

        // --- Rainbow draw ---
        if (resources.rbowBodyTexture.id > 0) {
            Vector2 rainbowDrawPos = { rainbowPos.x - scrollX, rainbowPos.y };
            if (rainbowDrawPos.x + resources.rbowBodyTexture.width > 0 && rainbowDrawPos.x < virtualScreenWidth) {
                Texture2D bodyToDraw = israinbowInDialogue ? resources.rbowVoiceOffBodyTexture : resources.rbowBodyTexture;

                if (bodyToDraw.id == 0) 
                    bodyToDraw = resources.rbowBodyTexture;

                DrawTextureV(bodyToDraw, rainbowDrawPos, WHITE);

                // --- Rainbow Eyes draw ---
                if (!israinbowInDialogue) {
                    if (currentEyeIndex >= 0 && currentEyeIndex < resources.numEyeSprites && resources.rbowEyeTextures[currentEyeIndex].id > 0) {
                        Texture2D currentEyeTexture = resources.rbowEyeTextures[currentEyeIndex];
                        Vector2 leftEyeDrawPos = { rainbowDrawPos.x + rainbowLeftEyeOffsetX, rainbowDrawPos.y + rainbowLeftEyeOffsetY };
                        Vector2 rightEyeDrawPos = { rainbowDrawPos.x + rainbowRightEyeOffsetX, rainbowDrawPos.y + rainbowRightEyeOffsetY };
                        DrawTextureV(currentEyeTexture, leftEyeDrawPos, WHITE);
                        DrawTextureV(currentEyeTexture, rightEyeDrawPos, WHITE);
                    }
                }
            }
        }

        // --- Ground Tiles draw ---
        if (resources.tile.id > 0) {
            float startTileX = -fmodf(scrollX, (float)resources.tile.width);
            int tilesToDraw = (virtualScreenWidth / resources.tile.width) + 2;
            for (int i = 0; i < tilesToDraw; i++) {
                float tileX = startTileX + i * resources.tile.width;
                if (tileX + resources.tile.width > 0 && tileX < virtualScreenWidth) 
                    DrawTexture(resources.tile, (int)tileX, (int)groundLevelY, WHITE);
            }
        }


        // --- Props draw ---
        if (resources.fenceProp.id > 0) {
            for (const auto& fence : fenceProps) {
                if (fence.x - scrollX + fence.width > 0 && fence.x - scrollX < virtualScreenWidth)
                    DrawTexture(resources.fenceProp, (int)(fence.x - scrollX), (int)fence.y, WHITE);
            }
        }

        if (resources.flowerSmallProp.id > 0) {
            for (const auto& flowerSmall : flowerSmallProps) {
                if (flowerSmall.x - scrollX + flowerSmall.width > 0 && flowerSmall.x - scrollX < virtualScreenWidth)
                    DrawTexture(resources.flowerSmallProp, (int)(flowerSmall.x - scrollX), (int)flowerSmall.y, WHITE);
            }
        }

        if (resources.flowerBigProp.id > 0) {
            for (const auto& flowerBig : flowerBigProps) {
                if (flowerBig.x - scrollX + flowerBig.width > 0 && flowerBig.x - scrollX < virtualScreenWidth)
                    DrawTexture(resources.flowerBigProp, (int)(flowerBig.x - scrollX), (int)flowerBig.y, WHITE);
            }
        }

        if (resources.checkpointFlag.id > 0) {
            for (const auto& flag : checkpointFlags) {
                if (flag.x - scrollX + flag.width > 0 && flag.x - scrollX < virtualScreenWidth)
                    DrawTexture(resources.checkpointFlag, (int)(flag.x - scrollX), (int)flag.y, WHITE);
            }
        }

        if (resources.sunflower.id > 0) {
            for (const auto& sunflowers : deadlySunflowers) {
                if (sunflowers.x - scrollX + sunflowers.width > 0 && sunflowers.x - scrollX < virtualScreenWidth)
                    DrawTexture(resources.sunflower, (int)(sunflowers.x - scrollX), (int)sunflowers.y, WHITE);
            }
        }

        // --- Platform draw ---
        if (resources.platformTexture.id > 0) {
            for (const auto& platform : platforms) {
                if (platform.x - scrollX + platform.width > 0 && platform.x - scrollX < virtualScreenWidth)
                    DrawTexture(resources.platformTexture, (int)(platform.x - scrollX), (int)platform.y, WHITE);
            }
        }
        else {
            for (const auto& platform : platforms) {
                if (platform.x - scrollX + platform.width > 0 && platform.x - scrollX < virtualScreenWidth)
                    DrawRectangleRec({ platform.x - scrollX, platform.y, platform.width, platform.height }, DARKGRAY);
  
            }
        }



        // --- Butterfly draw ---
        if (resources.butterfly.id > 0) {
            // Szerokoœæ i wysokoœæ pojedynczej klatki motyla
            float butterflyFrameActualWidth = (float)resources.butterfly.width / BUTTERFLY_TOTAL_FRAMES;
            float butterflyFrameActualHeight = (float)resources.butterfly.height; // Zak³adaj¹c, ¿e wszystkie klatki s¹ w jednym rzêdzie

            for (const auto& butterfly : butterflies) {
                Rectangle butterflySourceRect = {
                    (float)butterfly.animCurrentFrame * butterflyFrameActualWidth, // X na arkuszu sprite'ów
                    0.0f,                                                        // Y na arkuszu (zak³adaj¹c górny rz¹d)
                    butterflyFrameActualWidth,
                    butterflyFrameActualHeight
                };
                Vector2 butterflyDrawPos = { butterfly.position.x - scrollX, butterfly.position.y };

                // Proste culling dla motyli
                if (butterflyDrawPos.x + butterflyFrameActualWidth > 0 && butterflyDrawPos.x < virtualScreenWidth &&
                    butterflyDrawPos.y + butterflyFrameActualHeight > 0 && butterflyDrawPos.y < virtualScreenHeight) {
                    DrawTextureRec(resources.butterfly, butterflySourceRect, butterflyDrawPos, WHITE);
                }
            }
        }
        // --- END BUTTERFLY DRAW ---




        // --- Player draw ---
        Rectangle sourceRect = { (float)currentFrame * playerTextureWidth, 0, (float)playerTextureWidth, (float)playerTextureHeight };
        if (playerDrawVirtualPos.x + playerTextureWidth > 0 && playerDrawVirtualPos.x < virtualScreenWidth)
            DrawTextureRec(currentTexture, sourceRect, playerDrawVirtualPos, WHITE);

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
            DrawText(TextFormat("rainbow Pos X: %.1f", rainbowPos.x), 10, textY, lineHeight, BLUE); textY += lineHeight;
            DrawText(TextFormat("rainbow Pos Y: %.1f", rainbowPos.y), 10, textY, lineHeight, BLUE); textY += lineHeight;
            DrawText(TextFormat("rainbow Target X: %.1f", rainbowTargetX), 10, textY, lineHeight, MAGENTA); textY += lineHeight;
            DrawText(TextFormat("rainbow Eye Idx: %d", currentEyeIndex), 10, textY, lineHeight, WHITE); textY += lineHeight;
            DrawText(TextFormat("rainbow In Dialog: %s", israinbowInDialogue ? "Yes" : "No"), 10, textY, lineHeight, PINK); textY += lineHeight;

            if (resources.backgroundMusicLoaded)
                DrawText(TextFormat("Music: %.2f/%.2f", GetMusicTimePlayed(resources.backgroundMusic), GetMusicTimeLength(resources.backgroundMusic)), 10, textY, lineHeight, SKYBLUE);
            else 
                DrawText("Music: Not Loaded", 10, textY, lineHeight, RED);
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

    return 0;
}