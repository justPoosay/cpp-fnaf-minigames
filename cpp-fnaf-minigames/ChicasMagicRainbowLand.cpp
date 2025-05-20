#include "raylib.h"
#include "RainbowLandResources.h"
#include "RainbowLandConst.h"

struct CheckpointFlag {
    Rectangle rect;
    bool activated;
};

struct Butterfly {
    Vector2 position;
    Vector2 startPosition;
    int animCurrentFrame;
    int animFrameCounter;
    float movementPhase;
    float movementSpeedOffset;
};

struct PetalProjectile {
    Vector2 position;
    Vector2 velocity;
    bool active;
    float lifetime;
    int textureIndex;
};

struct RainbowProjectile {
    Vector2 position;
    Vector2 velocity;
    bool active;
    float lifetime;
};

struct SpikesActivationEvent {
    float triggerX;
    bool directionUp;
    bool hasBeenActivated;
};

enum SpikeState {
    SPIKE_IDLE,
    SPIKE_EXTENDING,
    SPIKE_RETRACTING
};

struct ActiveSpikeTrap {
    SpikeState state;
    bool directionUp;
    float startBaseY;
    float targetBaseY;
    float currentBaseY;
    float spikeHeight;
    Rectangle hitbox;
};

struct ShootingSunflower {
    int id;
    Vector2 position;
};

struct SunflowerTriggerConfig {
    int sunflowerId;
    float triggerX;
};

struct SunflowerActivationEvent {
    float triggerX;
    vector<int> sunflowerIdsToFire;
    bool hasBeenActivated;
};

// Define animations :p
static void UpdatePlayerAnimation(bool isGrounded, bool isMoving, bool facingRight, Texture2D& currentTexture, int& currentFrame, int animUpdateRate, int maxFrames, int& frameCounter, Texture2D idleRight, Texture2D idleLeft, Texture2D walkRight, Texture2D walkLeft, Texture2D jumpRight, Texture2D jumpLeft) {
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
static void UpdateButterflyAnimation(Texture2D& currentDisplayTexture, int& currentFrame, int animUpdateRate, int totalFramesInSheet, int& frameCounter, Texture2D butterfly) {
    frameCounter++;
    if (frameCounter >= animUpdateRate) {
        frameCounter = 0;
        currentFrame++;
        if (currentFrame >= totalFramesInSheet)
            currentFrame = 0;
    }
    currentDisplayTexture = butterfly;
}

static int CalculateEyeIndexForAngle(Vector2 eyePosition, Vector2 targetPosition, int numEyeSprites) {
    if (numEyeSprites <= 0) return 0; 

    float deltaX = targetPosition.x - eyePosition.x;
    float deltaY = targetPosition.y - eyePosition.y;
    float angleRad = atan2f(-deltaY, deltaX);

    if (angleRad < 0) angleRad += 2 * PI;
    

    int index = roundf(angleRad / (2 * PI) * numEyeSprites);
    index = index % numEyeSprites;

    return index;
}

static float MoveTowards(float current, float target, float maxDelta) {
    if (fabsf(target - current) <= maxDelta) return target;
    
    return current + copysignf(maxDelta, target - current);
}

static void DrawProps(const vector<Rectangle>& props, Texture2D texture, float scrollX, float screenWidth) {
    for (const auto& prop : props) 
        if (prop.x - scrollX + prop.width > 0 && prop.x - scrollX < screenWidth) 
            DrawTexture(texture, (prop.x - scrollX), prop.y, WHITE);
}

static void StopAllRainbowLandSounds(RainbowLandGameResources& res) {

    if (IsMusicStreamPlaying(res.backgroundMusic)) 
        StopMusicStream(res.backgroundMusic);

    for (int i = 0; i < res.rbowDialogues.size(); i++) 
        if (res.rbowDialogues[i].frameCount > 0 && IsSoundPlaying(res.rbowDialogues[i])) 
            StopSound(res.rbowDialogues[i]);
        
    if (IsSoundPlaying(res.jump)) StopSound(res.jump);
    if (IsSoundPlaying(res.petalShoot)) StopSound(res.petalShoot);
    if (IsSoundPlaying(res.spikesPush)) StopSound(res.spikesPush);
    if (IsSoundPlaying(res.rbowVoiceOff)) StopSound(res.rbowVoiceOff);
    if (IsSoundPlaying(res.death)) StopSound(res.death);
    if (IsSoundPlaying(res.checkpointSound)) StopSound(res.checkpointSound);
    if (IsSoundPlaying(res.rbowYouStillSuck)) StopSound(res.rbowYouStillSuck);
}


int runMagicRainbowLand(GraphicsQuality quality) {
    bool gameShouldEnd = false;

    cout << "Launching Chica's Magic Rainbow with quality " << quality << endl;
    HideCursor();

    int konamiCodeProgress = 0;
    bool isSecretActivated = false;
    bool showDebugInfo = false;

// --------- RESOURCE LOADER ---------
    RainbowLandGameResources resources = LoadRainbowLandResources(quality);
    RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    PlayMusicStream(resources.backgroundMusic);
    SetMusicVolume(resources.backgroundMusic, 1);


// --- Player ---
    Vector2 playerPos = { 225, groundLevelY - playerTextureHeight };
    float playerPrevX = playerPos.x;
    Vector2 playerVel = { 0, 0 };
    float jumpStartY = 0;
    bool isGrounded = true;
    bool isJumping = false;
    bool facingRight = true;
    bool firstCheckpointReached = false;

    Texture2D currentTexture = resources.idleRight;
    int currentFrame = 0;
    int frameCounter = 0;
    int maxFramesForCurrentAnim = idleFrames;

    bool isPlayerRespawning = false;
    float playerRespawnTimer = 0;
    const float playerRespawnDelay = 1;


// --- Rainbow ---
    Vector2 rainbowPos = { 0, 5 };
    int currentEyeIndex = 0;
    float rainbowTargetX = 0;
    bool israinbowInDialogue = false;
    bool buttonCanBeClicked = true;

    int currentDialogueIndex = 0;
    bool playDialogueOnRespawn = true;

    vector<RainbowProjectile> activeRainbowProjectiles;
    float rainbowLeftEyeShootTimer = 0;
    float rainbowRightEyeShootTimer = 0;
    const float rainbowShootDelay = 0.35;
    const float rainbowProjectileSpeed = 450;
    const float rainbowProjectileLifetime = 3;
    bool rainbowIsAttacking = false;

    float scrollX = 0;
    float bgScroll = 0;
    rainbowPos.x = (playerPos.x + 100) - ((float)resources.rbowBodyTexture.width / 2);

    PlaySound(resources.rbowDialogues[0]);


// --- Butterflies ---
    Texture2D currentButterflySprite;
    int butterflyCurrentAnimFrame = 0;
    int butterflyAnimFrameCounter = 0;

    Texture2D butterflySheet = resources.butterfly;


// --- Voices button ---
    bool isButtonClicked = false;
    Texture2D currentButtonTexture = resources.buttonVoicesOn;


// --- Objects/Props ---
    vector<Rectangle> platforms = { // platforms
        { platform1, groundLevelY - 185, platformWidth, platformHeight },
        { platform2, groundLevelY - 100, platformWidth, platformHeight },
        { platform3, groundLevelY - 185, platformWidth, platformHeight },
        { platform4, groundLevelY - 100, platformWidth, platformHeight },
        { platform5, groundLevelY - 185, platformWidth, platformHeight },
        { platform6, groundLevelY - 185, platformWidth, platformHeight },
        { platform7, groundLevelY - 185, platformWidth, platformHeight },
    };

    vector<Rectangle> fenceProps = { // fence
        { fence1, groundLevelY - 43, fenceWidth, fenceHeight },
        { fence2, groundLevelY - 43, fenceWidth, fenceHeight },
        { fence3, groundLevelY - 43, fenceWidth, fenceHeight },
        { fence4, groundLevelY - 43, fenceWidth, fenceHeight },
        { fence5, groundLevelY - 43, fenceWidth, fenceHeight }
    };

    vector<CheckpointFlag> checkpointFlags = { // flag
        {{ flag1, groundLevelY - 51, checkpointFlagWidth, checkpointFlagHeight }, false },
        {{ flag2, groundLevelY - 51, checkpointFlagWidth, checkpointFlagHeight }, false },
        {{ flag3, groundLevelY - 51, checkpointFlagWidth, checkpointFlagHeight }, false },
        {{ flag4, groundLevelY - 51, checkpointFlagWidth, checkpointFlagHeight }, false },
    };

    vector<Rectangle> flowerSmallProps = { // small flowers
        { smalFlow1, groundLevelY - 60, flowerSmallWidth, flowerSmallHeight },
    };

    vector<Rectangle> flowerBigProps = { // big flowers
        { bigFlow1, groundLevelY - 100, flowerBigWidth, flowerBigHeight },
        { bigFlow2, groundLevelY - 100, flowerBigWidth, flowerBigHeight },
        { bigFlow3, groundLevelY - 100, flowerBigWidth, flowerBigHeight },
        { bigFlow4, groundLevelY - 100, flowerBigWidth, flowerBigHeight },
    };

    vector<Rectangle> deadlySunflowers = { // sunflowers
        { sunfl1, groundLevelY - 296, sunflowerWidth, sunflowerHeight },     // [0]
        { sunfl2, groundLevelY - 296, sunflowerWidth, sunflowerHeight },     // [1]
        { sunfl3, groundLevelY - 140, sunflowerWidth, sunflowerHeight },     // [2]

        { sunfl3_1, groundLevelY - 100, sunflowerWidth, sunflowerHeight },   // [3]
        { sunfl3_2, groundLevelY - 100, sunflowerWidth, sunflowerHeight },   // [4]

        { sunfl4, groundLevelY - 286, sunflowerWidth, sunflowerHeight },     // [5]
        { sunfl5, groundLevelY - 286, sunflowerWidth, sunflowerHeight },     // [6]
        { sunfl6, groundLevelY - 286, sunflowerWidth, sunflowerHeight },     // [7]
        { sunfl7, groundLevelY - 286, sunflowerWidth, sunflowerHeight },     // [8]
    };

    vector<SunflowerActivationEvent> sunflowerEvents = { // sunflower tr9ggers
        { CalculateTriggerX(deadlySunflowers[0], deadlySunflowers[1]), {0, 1}, false },
        { CalculateTriggerX(deadlySunflowers[1], deadlySunflowers[2]), {1, 2}, false },
        { CalculateTriggerX(deadlySunflowers[2], deadlySunflowers[3]), {2, 3, 5, 6}, false },
        { CalculateTriggerX(deadlySunflowers[6], deadlySunflowers[7]), {3, 4, 5, 6, 7, 8}, false },
        { sunfl7 + sunflowerWidth, {4, 7, 8}, false},
    };

    vector<SpikesActivationEvent> spikeEvents = { // spike triggers
        { spike1, true, false },
        { spike2, true, false },
        { spike3, true, false },
    };

    vector<Butterfly> butterflies = {
        { {150, groundLevelY - 70}, {150, groundLevelY - 70}, GetRandomValue(0, butterflyTotalFrames - 1), 0, (float)GetRandomValue(0, 628) / 100, (float)GetRandomValue(-20, 20) / 100 },
        { {750, groundLevelY - 200}, {750, groundLevelY - 200}, GetRandomValue(0, butterflyTotalFrames - 1), 0, (float)GetRandomValue(0, 628) / 100, (float)GetRandomValue(-20, 20) / 100 },
        { {1100, groundLevelY - 120}, {1100, groundLevelY - 120}, GetRandomValue(0, butterflyTotalFrames - 1), 0, (float)GetRandomValue(0, 628) / 100, (float)GetRandomValue(-20, 20) / 100 },
    };

    vector<ShootingSunflower> shootingSunflowers;

    for (int i = 0; i < deadlySunflowers.size(); i++) {
        const auto& sfRect = deadlySunflowers[i];
        ShootingSunflower ss;
        ss.id = i;
        ss.position = {
            sfRect.x + sunflowerDiscCenterX,
            sfRect.y + sunflowerDiscCenterY
        };
        shootingSunflowers.push_back(ss);
    }



    vector<PetalProjectile> activePetals;
    Vector2 playerStartPos = playerPos;

    vector<ActiveSpikeTrap> activeSpikes;
    float spikeHeight = 0; 
    spikeHeight = resources.spikesUp.height;

    Vector2 lastCheckpointPos = playerStartPos;
    Vector2 checkpointEffectPos = {0, 0};

    bool touchingAnyCheckpoint = false;
    bool checkpointEffectActive = false;

    int checkpointEffectFrame = 0;
    int checkpointEffectLoopCount = 0;
    float checkpointEffectTimer = 0;
    float checkpointEffectYoffset = 0;



// --------------- MAIN GAME LOOP ---------------
    while (!WindowShouldClose()) {
        UpdateMusicStream(resources.backgroundMusic);
        float dt = GetFrameTime();
        bool isTouchingCheckpoint = false;

    // --------- PLAYER INPUT ---------
        playerVel.x = 0;
        bool isMoving = false;

        if (!isPlayerRespawning) {
            if (IsKeyDown(KEY_LEFT)) {
                playerVel.x = -playerSpeed;
                isMoving = true;
                facingRight = false;
            }

            if (IsKeyDown(KEY_RIGHT)) {
                playerVel.x = playerSpeed;
                isMoving = true;
                facingRight = true;
            }

            if (IsKeyDown(KEY_Z)) {
                if (isGrounded) {
                    isGrounded = false;
                    isJumping = true;
                    jumpStartY = playerPos.y;
                    playerVel.y = jumpSpeed;
                    PlaySound(resources.jump);
                }
                else {
                    if (isJumping && (jumpStartY - playerPos.y < maxJumpHeight))
                        playerVel.y = jumpSpeed;
                    else {
                        if (isJumping) isJumping = false;
                        playerVel.y = fallSpeed;
                    }
                }
            }
        }

        if (IsKeyPressed(KEY_ESCAPE)) gameShouldEnd = true;
        if (IsKeyPressed(KEY_F3)) showDebugInfo = !showDebugInfo; // temp debug mode
        if (IsKeyPressed(KEY_F11)) {
            if (IsWindowFullscreen()) {
                ToggleFullscreen();
                SetWindowSize(virtualScreenWidth, virtualScreenHeight);
            }
            else {
                SetWindowSize(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));
                ToggleFullscreen();
            }
        }

    // secret konami code
        if (GetKeyPressed() != 0) {
            if (GetKeyPressed() == konamiSequence[konamiCodeProgress]) {
                konamiCodeProgress++;

                if (konamiCodeProgress == konamiSequenceLength) {
                    isSecretActivated = !isSecretActivated;
                    konamiCodeProgress = 0;
                }
            }
            else {
                if (GetKeyPressed() == konamiSequence[0]) konamiCodeProgress = 1;
                else konamiCodeProgress = 0;
            }
        }

        Rectangle playerHitbox;
        if (!isPlayerRespawning) {
            playerHitbox = {
                playerPos.x + playerHitboxOffsetX,
                playerPos.y + playerHitboxOffsetY,
                playerHitboxWidth,
                playerHitboxHeight
            };
        }
        else
            playerHitbox = { playerPos.x, playerPos.y, 0, 0 };

        if (GetMusicTimePlayed(resources.backgroundMusic) >= GetMusicTimeLength(resources.backgroundMusic)) {
            SeekMusicStream(resources.backgroundMusic, 0);
            PlayMusicStream(resources.backgroundMusic);
        }

        Vector2 mousePosWindow = GetMousePosition();
        float scale = min((float)GetScreenWidth() / virtualScreenWidth, (float)GetScreenHeight() / virtualScreenHeight);

        Vector2 virtualMousePos = { 
            Clamp(((GetMousePosition().x - ((GetScreenWidth() - virtualScreenWidth * scale) / 2)) / scale), 0, virtualScreenWidth),
            Clamp(((GetMousePosition().y - ((GetScreenHeight() - virtualScreenHeight * scale) / 2)) / scale), 0, virtualScreenHeight)
        };

    // --- Check and Update Respawn Timer ---
        if (isPlayerRespawning) {
            playerRespawnTimer -= dt;

            if (playerRespawnTimer <= 0) {
                playerPos = lastCheckpointPos;
                playerVel = { 0, 0 };
                isGrounded = true;
                isJumping = false;

                for (auto& event : sunflowerEvents)
                    event.hasBeenActivated = false;

                for (auto& event : spikeEvents)
                    event.hasBeenActivated = false;

                activePetals.clear();
                activeSpikes.clear();

                isPlayerRespawning = false;
                if (!checkpointFlags.empty()) 
                    firstCheckpointReached = checkpointFlags[1].activated;
                else 
                    firstCheckpointReached = false;
            }
        }

        if (buttonCanBeClicked && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(virtualMousePos, buttonRect)) {
                isButtonClicked = true;
                currentButtonTexture = resources.buttonVoicesOff;
                israinbowInDialogue = true;
                buttonCanBeClicked = false;

                if (!IsSoundPlaying(resources.rbowVoiceOff)) {
                    if (IsMusicStreamPlaying(resources.backgroundMusic))
                        PauseMusicStream(resources.backgroundMusic);

                    if (currentDialogueIndex >= 0 && currentDialogueIndex < resources.rbowDialogues.size() && IsSoundPlaying(resources.rbowDialogues[currentDialogueIndex]))
                        PauseSound(resources.rbowDialogues[currentDialogueIndex]);

                    PlaySound(resources.rbowVoiceOff);
                }
            }
        }

        UpdateButterflyAnimation(currentButterflySprite, butterflyCurrentAnimFrame, butterflyFrameRate, butterflyTotalFrames, butterflyAnimFrameCounter, butterflySheet);


    // --- Platform logic ---
        if (isGrounded) {
            if (IsKeyDown(KEY_Z)) {
                isGrounded = false;
                isJumping = true;
                jumpStartY = playerPos.y;
                playerVel.y = jumpSpeed;
                PlaySound(resources.jump);
            }
            else {
                playerVel.y = 0;
                isJumping = false;
            }
        }
        else {
            if (isJumping && IsKeyDown(KEY_Z) && (jumpStartY - playerPos.y < maxJumpHeight))
                playerVel.y = jumpSpeed;

            else {
                if (isJumping)
                    isJumping = false;
                playerVel.y = fallSpeed;
            }
        }

        float potentialX = playerPos.x + playerVel.x;
        float potentialY = playerPos.y + playerVel.y;

        bool resolvedY = false;
        isGrounded = false;

    // Platform collision handler
        for (const auto& platform : platforms) {
            Rectangle platformRect = platform;
            Rectangle playerNextHitboxYOnly = { playerHitbox.x, potentialY + playerHitboxOffsetY, playerHitboxWidth, playerHitboxHeight };

            // I. Landing on platform
            if (playerVel.y > 0 && (playerHitbox.y + playerHitbox.height) <= platformRect.y + 1) {
                if (CheckCollisionRecs(playerNextHitboxYOnly, platformRect)) {
                    potentialY = platformRect.y - playerTextureHeight;
                    playerVel.y = 0;
                    isGrounded = true;
                    isJumping = false;
                    resolvedY = true;
                    break;
                }
            }
            // II. Jumping on platform
            else if (playerVel.y < 0 && playerHitbox.y >= (platformRect.y + platformRect.height - 1)) {
                if (CheckCollisionRecs(playerNextHitboxYOnly, platformRect)) {
                    potentialY = platformRect.y + platformRect.height - playerHitboxOffsetY;
                    playerVel.y = 0;
                    isJumping = false;
                    resolvedY = true;
                    break;
                }
            }
        }


    // Checkpoint collision handler
        touchingAnyCheckpoint = false;
        for (auto& flag : checkpointFlags) {
            if (CheckCollisionRecs(playerHitbox, flag.rect)) {
                touchingAnyCheckpoint = true;

                if (!flag.activated) {
                    PlaySound(resources.checkpointSound);

                    lastCheckpointPos = {
                        flag.rect.x + flag.rect.width / 2 - playerTextureWidth / 2,
                        flag.rect.y - playerTextureHeight
                    };

                    checkpointEffectActive = true;
                    checkpointEffectTimer = 0;
                    checkpointEffectFrame = 0;
                    checkpointEffectYoffset = 0;
                    checkpointEffectLoopCount = 0;
                    checkpointEffectPos = {
                        flag.rect.x + flag.rect.width / 2 - (resources.checkpoint.width / 4),
                        flag.rect.y - resources.checkpoint.height
                    };
                    flag.activated = true;

                    if (!firstCheckpointReached)
                        firstCheckpointReached = true;
                }
                break;
            }
        }


        if (checkpointEffectActive) {
            checkpointEffectTimer += dt;

            int totalFrames = 4;
            float frameDuration = 0.025;
            int totalLoops = 10;

            int currentFrame = (checkpointEffectTimer / frameDuration);
            checkpointEffectFrame = currentFrame % totalFrames;

            if (currentFrame / totalFrames > checkpointEffectLoopCount) {
                checkpointEffectLoopCount++;
                checkpointEffectYoffset += 8;
            }

            if (checkpointEffectLoopCount >= totalLoops) {
                checkpointEffectActive = false;
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

        if (!resolvedY && playerVel.y != 0)
            isGrounded = false;

        playerPos.y = potentialY;
        playerPos.x = potentialX;

        if (playerPos.x < 0)
            playerPos.x = 0;

        if (playerPos.x + playerTextureWidth > levelLogicalWidth)
            playerPos.x = levelLogicalWidth - playerTextureWidth;

        Rectangle feetCheckRect = { playerPos.x + playerHitboxOffsetX, playerPos.y + playerHitboxOffsetY + playerHitboxHeight + 0.1, playerHitboxWidth, 1 };
        bool currentlyStandingOnSurface = false;
        Rectangle groundCheckArea = { 0, groundLevelY, levelLogicalWidth, 10 };

        if (CheckCollisionRecs(feetCheckRect, groundCheckArea)) {
            if (playerPos.y + playerTextureHeight > groundLevelY)
                playerPos.y = groundLevelY - playerTextureHeight;

            currentlyStandingOnSurface = true;
            playerVel.y = 0;
        }
        if (!currentlyStandingOnSurface) {
            for (const auto& platform : platforms) {
                if (CheckCollisionRecs(feetCheckRect, platform)) {
                    if (playerPos.y + playerTextureHeight > platform.y)
                        playerPos.y = platform.y - playerTextureHeight;

                    currentlyStandingOnSurface = true;
                    playerVel.y = 0;

                    break;
                }
            }
        }
        isGrounded = currentlyStandingOnSurface;

        if (isGrounded)
            isJumping = false;


    // Camera Scroll
        float targetScrollX = playerPos.x - playerVirtualScreenX;
        scrollX = fmaxf(0, fminf(targetScrollX, maxScrollX));

    // Background Scroll (Paralax)
        bgScroll = scrollX * bgScrollFactor;

    // --- Rainbow logic ---
        rainbowTargetX = (playerPos.x + playerTextureWidth / 2) - ((float)resources.rbowBodyTexture.width / 2);
        float diffX = rainbowTargetX - rainbowPos.x;
        float moveAmount = rainbowSpeed;
        if (fabsf(diffX) < moveAmount)
            rainbowPos.x = rainbowTargetX;
        else if (diffX > 0)
            rainbowPos.x += moveAmount;
        else if (diffX < 0)
            rainbowPos.x -= moveAmount;

        rainbowPos.y = 5;

        if (!israinbowInDialogue) {
            Vector2 rainbowCenter = { rainbowPos.x + resources.rbowBodyTexture.width / 2, rainbowPos.y + resources.rbowBodyTexture.height / 2 };
            Vector2 playerCenter = { playerPos.x + playerTextureWidth / 2, playerPos.y + playerTextureHeight / 2 };
            float angleX = playerCenter.x - rainbowCenter.x;
            float angleY = playerCenter.y - rainbowCenter.y;
            float angleRad = atan2f(-angleY, angleX);

            if (angleRad < 0)
                angleRad += 2 * PI;

            currentEyeIndex = roundf(angleRad / (2 * PI) * resources.numEyeSprites);
            currentEyeIndex = currentEyeIndex % resources.numEyeSprites;
        }

    // --- Rainbow attack on mute ---
        if (israinbowInDialogue && !IsSoundPlaying(resources.rbowVoiceOff)) {
            if (!rainbowIsAttacking) {
                rainbowIsAttacking = true;
                rainbowLeftEyeShootTimer = rainbowShootDelay;
                rainbowRightEyeShootTimer = rainbowShootDelay;
            }
        }
        else rainbowIsAttacking = false;
        
        if (rainbowIsAttacking) {
            rainbowLeftEyeShootTimer -= dt;
            rainbowRightEyeShootTimer -= dt;
            bool soundPlayedThisFrame = false;

            if (rainbowLeftEyeShootTimer <= 0) {
                rainbowLeftEyeShootTimer = rainbowShootDelay;

                Vector2 projectileStartPosLeft = {
                    rainbowPos.x + rainbowLeftEyeOffsetX + (resources.rbowEyeTextures[0].width / 2),
                    rainbowPos.y + rainbowLeftEyeOffsetY + (resources.rbowEyeTextures[0].height / 2)
                };

                RainbowProjectile newProjectile;
                newProjectile.position = projectileStartPosLeft;
                newProjectile.velocity = {0, rainbowProjectileSpeed};
                newProjectile.active = true;
                newProjectile.lifetime = rainbowProjectileLifetime;
                activeRainbowProjectiles.push_back(newProjectile);

                if (!soundPlayedThisFrame) {
                    PlaySound(resources.rbowLaserShoot);
                    soundPlayedThisFrame = true;
                }
            }

            if (rainbowRightEyeShootTimer <= 0) {
                rainbowRightEyeShootTimer = rainbowShootDelay;

                Vector2 projectileStartPosRight = {
                    rainbowPos.x + rainbowRightEyeOffsetX + (resources.rbowEyeTextures[0].width / 2),
                    rainbowPos.y + rainbowRightEyeOffsetY + (resources.rbowEyeTextures[0].height / 2)
                };

                RainbowProjectile newProjectile;
                newProjectile.position = projectileStartPosRight;
                newProjectile.velocity = {0, rainbowProjectileSpeed};
                newProjectile.active = true;
                newProjectile.lifetime = rainbowProjectileLifetime;
                activeRainbowProjectiles.push_back(newProjectile);

                if (!soundPlayedThisFrame) {
                    PlaySound(resources.bflyLaserShoot);
                    soundPlayedThisFrame = true;
                }
            }
        }

    // Rainbow lasers updater
        for (int i = activeRainbowProjectiles.size() - 1; i >= 0; i--) {
            RainbowProjectile& p = activeRainbowProjectiles[i];
            if (p.active) {

                p.position.x += p.velocity.x * dt;
                p.position.y += p.velocity.y * dt;
                p.lifetime -= dt;

                if (p.lifetime <= 0 || p.position.y > virtualScreenHeight + 20) p.active = false;
                

            // Laser collision with player
                Rectangle projectileHitbox = { p.position.x - 2, p.position.y - 2, 4, 4 };

                if (CheckCollisionRecs(playerHitbox, projectileHitbox)) {
                    p.active = false;
                    PlaySound(resources.death);

                    if (currentDialogueIndex >= 0 && currentDialogueIndex < (int)resources.rbowDialogues.size() && resources.rbowDialogues[currentDialogueIndex].frameCount > 0 && IsSoundPlaying(resources.rbowDialogues[currentDialogueIndex]))
                        StopSound(resources.rbowDialogues[currentDialogueIndex]);

                    currentDialogueIndex++;

                    if (buttonCanBeClicked && currentDialogueIndex < (int)resources.rbowDialogues.size() && resources.rbowDialogues[currentDialogueIndex].frameCount > 0)
                        PlaySound(resources.rbowDialogues[currentDialogueIndex]);
                    else
                        PlaySound(resources.rbowYouStillSuck);

                    isPlayerRespawning = true;
                    playerRespawnTimer = playerRespawnDelay;
                }
            }
            else activeRainbowProjectiles.erase(activeRainbowProjectiles.begin() + i);

        }


    // --- Butterfly logic ---
        for (auto& butterfly : butterflies) {
            float currentMovementSpeed = butterflyMovementSpeed + butterfly.movementSpeedOffset;
            butterfly.movementPhase += currentMovementSpeed * dt;
            if (butterfly.movementPhase >= 2 * PI) butterfly.movementPhase -= 2 * PI;

            butterfly.position.x = butterfly.startPosition.x + butterflyMovementRadiusX * cosf(butterfly.movementPhase);
            butterfly.position.y = butterfly.startPosition.y + butterflyMovementRadiusY * sinf(butterfly.movementPhase);

            Texture2D dummyTextureForAnimFunc;
            UpdateButterflyAnimation(
                dummyTextureForAnimFunc,
                butterfly.animCurrentFrame,
                butterflyFrameRate,
                butterflyTotalFrames,
                butterfly.animFrameCounter,
                resources.butterfly
            );
        }

    // --- Petal projectile logic ---
        bool petalShootSoundPlayedThisFrame = false;
        Rectangle currentWorldPlayerHitboxForEvents = { playerPos.x + playerHitboxOffsetX, playerPos.y + playerHitboxOffsetY, playerHitboxWidth, playerHitboxHeight };
        Rectangle prevWorldPlayerHitboxForEvents = { playerPrevX + playerHitboxOffsetX, playerPos.y + playerHitboxOffsetY, playerHitboxWidth, playerHitboxHeight };

        for (auto& event : sunflowerEvents) {
            if (event.hasBeenActivated) continue;

            if (!isPlayerRespawning && (currentWorldPlayerHitboxForEvents.x + currentWorldPlayerHitboxForEvents.width) > event.triggerX && (prevWorldPlayerHitboxForEvents.x + prevWorldPlayerHitboxForEvents.width) <= event.triggerX) {

                event.hasBeenActivated = true;
                bool soundPlayedForThisSpecificEvent = false;

                for (int sunflowerIdToFire : event.sunflowerIdsToFire) {
                    ShootingSunflower* targetSunflower = nullptr;
                    for (auto& ss : shootingSunflowers) {
                        if (ss.id == sunflowerIdToFire) {
                            targetSunflower = &ss;
                            break;
                        }
                    }
                    if (targetSunflower) {
                        if (!soundPlayedForThisSpecificEvent && !petalShootSoundPlayedThisFrame) {
                            PlaySound(resources.petalShoot);
                            soundPlayedForThisSpecificEvent = true;
                            petalShootSoundPlayedThisFrame = true;
                        }
                        for (int i = 0; i < 8; i++) {
                            PetalProjectile petal;
                            petal.position = targetSunflower->position;
                            petal.velocity.x = petalDirections[i].x * petalSpeed;
                            petal.velocity.y = petalDirections[i].y * petalSpeed;
                            petal.active = true;
                            petal.lifetime = petalLifespan;
                            petal.textureIndex = i;
                            activePetals.push_back(petal);
                        }
                    }
                }
            }
        }

    // Petal update and collision
        for (int i = activePetals.size() - 1; i >= 0; i--) {
            PetalProjectile& currentPetal = activePetals[i];
            if (currentPetal.active) {
                currentPetal.position.x += currentPetal.velocity.x * dt;
                currentPetal.position.y += currentPetal.velocity.y * dt;
                currentPetal.lifetime -= dt;

                float petalScreenX = currentPetal.position.x - scrollX;
                if (currentPetal.lifetime <= 0 || petalScreenX < -20 || petalScreenX > virtualScreenWidth + 20 || currentPetal.position.y < -20 || currentPetal.position.y > virtualScreenHeight + 20) currentPetal.active = false;
                

            // Petal collision with player
                if (currentPetal.active && !showDebugInfo && !isPlayerRespawning) {
                    Rectangle petalHitbox = { currentPetal.position.x, currentPetal.position.y, 8, 8 };

                    if (CheckCollisionRecs(playerHitbox, petalHitbox)) {
                        currentPetal.active = false;
                        PlaySound(resources.death);

                        if (currentDialogueIndex >= 0 && currentDialogueIndex < (int)resources.rbowDialogues.size() && resources.rbowDialogues[currentDialogueIndex].frameCount > 0 && IsSoundPlaying(resources.rbowDialogues[currentDialogueIndex]))
                            StopSound(resources.rbowDialogues[currentDialogueIndex]);

                        currentDialogueIndex++;

                        if (buttonCanBeClicked && currentDialogueIndex < (int)resources.rbowDialogues.size() && resources.rbowDialogues[currentDialogueIndex].frameCount > 0)
                            PlaySound(resources.rbowDialogues[currentDialogueIndex]);
                        else 
                            PlaySound(resources.rbowYouStillSuck);

                        isPlayerRespawning = true;
                        playerRespawnTimer = playerRespawnDelay;
                    }
                }
            }
            if (!currentPetal.active) {
                activePetals.erase(activePetals.begin() + i);
            }
        }

    // --- Spike Activation Logic ---
        for (auto& event : spikeEvents) {
            if (event.hasBeenActivated) continue;
            float playerRightEdgeX = playerHitbox.x + playerHitbox.width;

            if (playerRightEdgeX > event.triggerX) {
                event.hasBeenActivated = true;
                PlaySound(resources.spikesPush);

                ActiveSpikeTrap newSpike;
                newSpike.state = SPIKE_EXTENDING;
                newSpike.directionUp = event.directionUp;
                newSpike.spikeHeight = spikeHeight;

                if (newSpike.directionUp) {
                    newSpike.startBaseY = virtualScreenHeight;
                    newSpike.targetBaseY = 200;
                }
                else {
                    newSpike.startBaseY = -newSpike.spikeHeight;
                    newSpike.targetBaseY = 0; 
                }
                newSpike.currentBaseY = newSpike.startBaseY;
                newSpike.hitbox = {0, 0, virtualScreenWidth, newSpike.spikeHeight};
                activeSpikes.push_back(newSpike);
            }
        }

    // --- Spike Update and Collision ---
        for (int i = activeSpikes.size() - 1; i >= 0; i--) {
            if (i >= (int)activeSpikes.size()) continue;
            ActiveSpikeTrap& currentSpike = activeSpikes[i];

            if (currentSpike.state == SPIKE_EXTENDING) {
                currentSpike.currentBaseY = MoveTowards(currentSpike.currentBaseY, currentSpike.targetBaseY, spikeSpeed * dt);

                if (currentSpike.currentBaseY == currentSpike.targetBaseY)
                    currentSpike.state = SPIKE_RETRACTING;
            }
            else if (currentSpike.state == SPIKE_RETRACTING) {
                currentSpike.currentBaseY = MoveTowards(currentSpike.currentBaseY, currentSpike.startBaseY, spikeSpeed * dt);

                if (currentSpike.currentBaseY == currentSpike.startBaseY)
                    currentSpike.state = SPIKE_IDLE;
            }
            currentSpike.hitbox.y = currentSpike.currentBaseY;

            if (currentSpike.state != SPIKE_IDLE && !showDebugInfo && !isPlayerRespawning) {
                float playerScreenX = playerPos.x - scrollX;
                Rectangle playerScreenHitbox = { playerScreenX + playerHitboxOffsetX, playerPos.y + playerHitboxOffsetY, playerHitboxWidth, playerHitboxHeight };

                if (CheckCollisionRecs(playerScreenHitbox, currentSpike.hitbox)) {
                    PlaySound(resources.death);
                    if (currentDialogueIndex >= 0 && currentDialogueIndex < (int)resources.rbowDialogues.size() && resources.rbowDialogues[currentDialogueIndex].frameCount > 0 && IsSoundPlaying(resources.rbowDialogues[currentDialogueIndex])) 
                        StopSound(resources.rbowDialogues[currentDialogueIndex]);
                    
                    currentDialogueIndex++;

                    if (buttonCanBeClicked && currentDialogueIndex < (int)resources.rbowDialogues.size() && resources.rbowDialogues[currentDialogueIndex].frameCount > 0) 
                        PlaySound(resources.rbowDialogues[currentDialogueIndex]);
                    else
                        PlaySound(resources.rbowYouStillSuck);
                    
                    isPlayerRespawning = true;
                    playerRespawnTimer = playerRespawnDelay;
                    break;
                }
            }

            if (currentSpike.state == SPIKE_IDLE)
                if (i < (int)activeSpikes.size())
                    activeSpikes.erase(activeSpikes.begin() + i);

        }

        if (!isPlayerRespawning) {
            if (!isGrounded) maxFramesForCurrentAnim = jumpFrames;
            else if (isMoving) maxFramesForCurrentAnim = walkFrames;
            else maxFramesForCurrentAnim = idleFrames;
            UpdatePlayerAnimation(isGrounded, isMoving, facingRight, currentTexture, currentFrame, animUpdateRate, maxFramesForCurrentAnim, frameCounter, resources.idleRight, resources.idleLeft, resources.walkRight, resources.walkLeft, resources.jumpRight, resources.jumpLeft);
        }


        float actualPlayerVirtualDrawX;
        if (scrollX <= 0) { actualPlayerVirtualDrawX = playerPos.x; }
        else if (scrollX >= maxScrollX) { actualPlayerVirtualDrawX = playerPos.x - maxScrollX; }
        else { actualPlayerVirtualDrawX = playerVirtualScreenX; }
        Vector2 playerDrawVirtualPos = { actualPlayerVirtualDrawX, playerPos.y };

        playerPrevX = playerPos.x;



        BeginTextureMode(target);
        ClearBackground(RAYWHITE);

    // --- Bg Draw ---
        float bgX = -fmodf(bgScroll, resources.bg.width);
        DrawTextureEx(resources.bg, { bgX , 0 }, 0, 1, WHITE);
        DrawTextureEx(resources.bg, { bgX + resources.bg.width, 0 }, 0, 1, WHITE);
        if (bgX + 2 * resources.bg.width < virtualScreenWidth)
            DrawTextureEx(resources.bg, { bgX + 2 * resources.bg.width, 0 }, 0, 1, WHITE);

    // --- Rainbow draw ---
        Vector2 rainbowDrawPos = { rainbowPos.x - scrollX, rainbowPos.y };
        if (rainbowDrawPos.x + resources.rbowBodyTexture.width > 0 && rainbowDrawPos.x < virtualScreenWidth) {
            Texture2D bodyToDraw = israinbowInDialogue ? resources.rbowVoiceOffBodyTexture : resources.rbowBodyTexture;

            if (bodyToDraw.id == 0)
                bodyToDraw = resources.rbowBodyTexture;

            DrawTextureV(bodyToDraw, rainbowDrawPos, WHITE);

        // --- Rainbow Eyes draw ---
            if (!israinbowInDialogue && resources.numEyeSprites > 0) { 
                Vector2 playerCenter = { playerPos.x + playerTextureWidth / 2, playerPos.y + playerTextureHeight / 2 };
                Vector2 leftEyeWorldPos = { rainbowPos.x + rainbowLeftEyeOffsetX, rainbowPos.y + rainbowLeftEyeOffsetY };
                Vector2 rightEyeWorldPos = { rainbowPos.x + rainbowRightEyeOffsetX, rainbowPos.y + rainbowRightEyeOffsetY };
                int leftEyeTargetIndex = CalculateEyeIndexForAngle(leftEyeWorldPos, playerCenter, resources.numEyeSprites);
                int rightEyeTargetIndex = CalculateEyeIndexForAngle(rightEyeWorldPos, playerCenter, resources.numEyeSprites);

            // --- Squint Logic ---
                const int straightAheadIndex = resources.numEyeSprites / 2;
                const int squintTriggerRange = 1;
                bool useSquintTexture = false;

                if (abs(leftEyeTargetIndex - straightAheadIndex) <= squintTriggerRange && abs(rightEyeTargetIndex - straightAheadIndex) <= squintTriggerRange)
                    useSquintTexture = true;
                 
                int finalLeftEyeIndex = leftEyeTargetIndex;
                int finalRightEyeIndex = rightEyeTargetIndex;

                if (useSquintTexture) {
                    finalLeftEyeIndex = straightAheadIndex;
                    finalRightEyeIndex = straightAheadIndex;
                }

                if (finalLeftEyeIndex >= 0 && finalLeftEyeIndex < resources.numEyeSprites && finalRightEyeIndex >= 0 && finalRightEyeIndex < resources.numEyeSprites) {
                    Texture2D leftEyeTexture = resources.rbowEyeTextures[finalLeftEyeIndex];
                    Texture2D rightEyeTexture = resources.rbowEyeTextures[finalRightEyeIndex];

                    Vector2 leftEyeDrawPos = { rainbowDrawPos.x + rainbowLeftEyeOffsetX, rainbowDrawPos.y + rainbowLeftEyeOffsetY };
                    Vector2 rightEyeDrawPos = { rainbowDrawPos.x + rainbowRightEyeOffsetX, rainbowDrawPos.y + rainbowRightEyeOffsetY };

                    DrawTextureV(leftEyeTexture, leftEyeDrawPos, WHITE);
                    DrawTextureV(rightEyeTexture, rightEyeDrawPos, WHITE);
                }
            }
        }

    // --- Ground tiles draw ---
        float startTileX = -fmodf(scrollX, resources.tile.width);
        int tilesToDraw = (virtualScreenWidth / resources.tile.width) + 2;
        for (int i = 0; i < tilesToDraw; i++) {
            float tileX = startTileX + i * resources.tile.width;
            if (tileX + resources.tile.width > 0 && tileX < virtualScreenWidth)
                DrawTexture(resources.tile, tileX, groundLevelY, WHITE);
        }


    // --- Props draw ---
        for (const auto& fence : fenceProps)
            if (fence.x - scrollX + fence.width > 0 && fence.x - scrollX < virtualScreenWidth)
                DrawTexture(resources.fenceProp, (fence.x - scrollX), fence.y, WHITE);

        for (const auto& flowerSmall : flowerSmallProps)
            if (flowerSmall.x - scrollX + flowerSmall.width > 0 && flowerSmall.x - scrollX < virtualScreenWidth)
                DrawTexture(resources.flowerSmallProp, (flowerSmall.x - scrollX), flowerSmall.y, WHITE);

        for (const auto& flowerBig : flowerBigProps)
            if (flowerBig.x - scrollX + flowerBig.width > 0 && flowerBig.x - scrollX < virtualScreenWidth)
                DrawTexture(resources.flowerBigProp, (flowerBig.x - scrollX), flowerBig.y, WHITE);

        for (const auto& flag : checkpointFlags)
            if (flag.rect.x - scrollX + flag.rect.width > 0 && flag.rect.x - scrollX < virtualScreenWidth)
                DrawTexture(resources.checkpointFlag, (flag.rect.x - scrollX), flag.rect.y, WHITE);

        for (const auto& platform : platforms)
            if (platform.x - scrollX + platform.width > 0 && platform.x - scrollX < virtualScreenWidth)
                DrawTexture(resources.platformTexture, (platform.x - scrollX), platform.y, WHITE);


    // --- Player draw ---
        if (!isPlayerRespawning) {
            Rectangle sourceRect = {currentFrame * playerTextureWidth, 0, playerTextureWidth, playerTextureHeight};

            if (playerDrawVirtualPos.x + playerTextureWidth > 0 && playerDrawVirtualPos.x < virtualScreenWidth)
                DrawTextureRec(currentTexture, sourceRect, playerDrawVirtualPos, WHITE);
        }


    // --- Sunflowers draw ---
        for (const auto& sunflowers : deadlySunflowers)
            if (sunflowers.x - scrollX + sunflowers.width > 0 && sunflowers.x - scrollX < virtualScreenWidth)
                DrawTexture(resources.sunflower, (sunflowers.x - scrollX), sunflowers.y, WHITE);


    // --- Butterflies draw ---
        for (const auto& butterfly : butterflies) {
            int butterflyFrameActualWidth = resources.butterfly.width / butterflyTotalFrames;
            int butterflyFrameActualHeight = resources.butterfly.height;

            Rectangle butterflySourceRect = { butterfly.animCurrentFrame * butterflyFrameActualWidth, 0, butterflyFrameActualWidth, butterflyFrameActualHeight };
            Vector2 butterflyDrawPos = {
                butterfly.position.x - scrollX,
                butterfly.position.y
            };

            if (butterflyDrawPos.x + butterflyFrameActualWidth > 0 && butterflyDrawPos.x < virtualScreenWidth && butterflyDrawPos.y + butterflyFrameActualHeight > 0 && butterflyDrawPos.y < virtualScreenHeight)
                DrawTextureRec(resources.butterfly, butterflySourceRect, butterflyDrawPos, WHITE);
        }

    // Petals draw
        for (const auto& petal : activePetals) {
            if (petal.active) {
                if (petal.textureIndex >= 0 && petal.textureIndex < 8) {
                    Texture2D currentPetalTexture = resources.sunflowerPetals[petal.textureIndex];
                    Vector2 petalDrawPos = {
                        petal.position.x - scrollX - currentPetalTexture.width / 2,
                        petal.position.y - currentPetalTexture.height / 2
                    };
                    DrawTextureV(currentPetalTexture, petalDrawPos, WHITE);
                }
                else DrawCircleV({ petal.position.x - scrollX, petal.position.y }, 4, RED);
            }
        }

    // Rainbow lasers draw
        for (const auto& p : activeRainbowProjectiles) {
            if (p.active) {
                Vector2 laserDrawPos = {(p.position.x - scrollX) - (resources.laserBeamFrame.width) / 2, p.position.y};
                DrawTextureV(resources.laserBeamFrame, laserDrawPos, WHITE);
            }
        }

    // --- Spikes Draw ---
        for (const auto& spike : activeSpikes) {
            if (spike.state != SPIKE_IDLE) {
                Texture2D currentSpikeTexture = spike.directionUp ? resources.spikesUp : resources.spikesDown;
                int drawPosX = (virtualScreenWidth / 2) - (currentSpikeTexture.width / 2);
                DrawTexture(currentSpikeTexture, drawPosX, spike.currentBaseY, WHITE);
            }
        }
       
    //--- Control keys info draw ---
        if (!firstCheckpointReached) DrawTexture(resources.controlKeysInfo, 10, 10, WHITE);


    // ------- DEBUG DRAW -------
        if (showDebugInfo) {
            const int gridSpacing = 100;
            const int labelFontSize = 10;
            Color gridLineColor = Fade(BLACK, 0.4);
            Color gridLabelColor = MAROON;

            float startWorldX = scrollX;
            float endWorldX = scrollX + virtualScreenWidth;

            int firstGridX = floor(startWorldX / gridSpacing) * gridSpacing;

            for (int worldX = firstGridX; worldX < endWorldX + gridSpacing; worldX += gridSpacing) {
                float screenX = worldX - scrollX;
                DrawLine(screenX, 0, screenX, virtualScreenHeight, gridLineColor);

                if (screenX >= 0 && screenX < virtualScreenWidth)
                    DrawText(TextFormat("%d", worldX), screenX + 2, 2, labelFontSize, gridLabelColor);
            }

            for (int y = gridSpacing; y < virtualScreenHeight; y += gridSpacing) {
                DrawLine(0, y, virtualScreenWidth, y, gridLineColor);

                if (y >= 0 && y < virtualScreenHeight)
                    DrawText(TextFormat("%d", y), 2, y + 2, labelFontSize, gridLabelColor);
            }

            DrawRectangle(0, 0, 140, 180, BLACK);
            int textY = 10;
            int lineHeight = 10;

            DrawText(TextFormat("Position: (%.0f, %.0f)", playerPos.x, playerPos.y), 10, textY, lineHeight, LIME); textY += lineHeight;
            DrawText(TextFormat("P.Vel Y: %.0f", playerVel.y), 10, textY, lineHeight, LIME); textY += lineHeight;
            DrawText(TextFormat("ScrollX (Cam): %.0f", scrollX), 10, textY, lineHeight, LIME); textY += lineHeight;
            DrawText(TextFormat("MaxScrollX: %.0f", maxScrollX), 10, textY, lineHeight, ORANGE); textY += lineHeight;
            DrawText(TextFormat("BGScroll: %.0f", bgScroll), 10, textY, lineHeight, SKYBLUE); textY += lineHeight;
            DrawText(TextFormat("LevelWidth: %.0f", levelLogicalWidth), 10, textY, lineHeight, SKYBLUE); textY += lineHeight;
            DrawText(TextFormat("Grounded: %s", isGrounded ? "True" : "False"), 10, textY, lineHeight, LIME); textY += lineHeight;
            DrawText(TextFormat("Jumping(Up): %s", isJumping ? "True" : "False"), 10, textY, lineHeight, YELLOW); textY += lineHeight;
            DrawText(TextFormat("Button Clicked: %s", isButtonClicked ? "Yes" : "No"), 10, textY, lineHeight, YELLOW); textY += lineHeight;
            DrawText(TextFormat("rainbow Pos X: %.0f", rainbowPos.x), 10, textY, lineHeight, BLUE); textY += lineHeight;
            DrawText(TextFormat("rainbow Pos Y: %.0f", rainbowPos.y), 10, textY, lineHeight, BLUE); textY += lineHeight;
            DrawText(TextFormat("rainbow Target X: %.0f", rainbowTargetX), 10, textY, lineHeight, MAGENTA); textY += lineHeight;
            DrawText(TextFormat("rainbow Eye Idx: %d", currentEyeIndex), 10, textY, lineHeight, WHITE); textY += lineHeight;
            DrawText(TextFormat("rainbow In Dialog: %s", israinbowInDialogue ? "Yes" : "No"), 10, textY, lineHeight, PINK); textY += lineHeight;
            DrawText(TextFormat("Konami code: %s", isSecretActivated ? "On" : "Off"), 10, textY, lineHeight, PINK); textY += lineHeight;
            DrawText(TextFormat("Music: %.0f/%.0f", GetMusicTimePlayed(resources.backgroundMusic), GetMusicTimeLength(resources.backgroundMusic)), 10, textY, lineHeight, SKYBLUE); textY += lineHeight;
            DrawText("DEBUG (F3) Full (F11)", virtualScreenWidth - MeasureText("DEBUG (F3) Full (F11)", 20) - 10, 10, 20, WHITE);

            for (const auto& platform : platforms) {
                Rectangle platformDrawRect = { platform.x - scrollX, platform.y, platform.width, platform.height };
                if (platformDrawRect.x + platformDrawRect.width > 0 && platformDrawRect.x < virtualScreenWidth)
                {
                    DrawRectangleLinesEx(platformDrawRect, 1, VIOLET);

                    const char* coordText = TextFormat("X:%.0f, Y:%.0f", platform.x, platform.y);

                    int coordFontSize = 10;
                    Color coordTextColor = WHITE;
                    Color coordBgColor = Fade(BLACK, 0.6);

                    Vector2 textSize = MeasureTextEx(GetFontDefault(), coordText, coordFontSize, 1);
                    float textPosX = platformDrawRect.x + (platformDrawRect.width / 2) - (textSize.x / 2);
                    float textPosY = platformDrawRect.y - textSize.y - 4;

                    Rectangle bgRect = { textPosX - 2, textPosY - 1, textSize.x + 4, textSize.y + 2 };

                    if (textPosY > 0) {
                        DrawRectangleRec(bgRect, coordBgColor);
                        DrawText(coordText, textPosX, textPosY, coordFontSize, coordTextColor);
                    }
                    else {
                        textPosY = platformDrawRect.y + platformDrawRect.height + 4;
                        bgRect.y = textPosY - 1;
                        if (textPosY + textSize.y < virtualScreenHeight) {
                            DrawRectangleRec(bgRect, coordBgColor);
                            DrawText(coordText, textPosX, textPosY, coordFontSize, coordTextColor);
                        }
                    }
                }
            }

            Rectangle playerDrawHitbox = { playerDrawVirtualPos.x + playerHitboxOffsetX, playerDrawVirtualPos.y + playerHitboxOffsetY, playerHitboxWidth, playerHitboxHeight };
            DrawRectangleLinesEx(playerDrawHitbox, 2, GREEN);

            Rectangle playerDrawSpriteBox = { playerDrawVirtualPos.x, playerDrawVirtualPos.y, playerTextureWidth, playerTextureHeight };
            DrawRectangleLinesEx(playerDrawSpriteBox, 1, GRAY);

            DrawLineEx({ 0, groundLevelY }, { virtualScreenWidth, groundLevelY }, 2, DARKGRAY);

            Rectangle feetCheckRectDraw = { playerDrawVirtualPos.x + playerHitboxOffsetX, playerDrawVirtualPos.y + playerHitboxOffsetY + playerHitboxHeight, playerHitboxWidth, 1 };
            DrawRectangleRec(feetCheckRectDraw, YELLOW);
        }

    // Triggers draw (debug/godmode)
        if (showDebugInfo || isSecretActivated) {
            for (const auto& event : sunflowerEvents) {
                if (event.hasBeenActivated) continue;
                float triggerScreenX = event.triggerX - scrollX;
                if (triggerScreenX >= 0 && triggerScreenX < virtualScreenWidth) {
                    DrawLineBezier({ triggerScreenX, 0 }, { triggerScreenX, virtualScreenHeight }, 5, BLACK);
                    for (int sfId : event.sunflowerIdsToFire) {
                        for (const auto& ss : shootingSunflowers) {
                            if (ss.id == sfId) {
                                Vector2 sunflowerShootPointScreen = { ss.position.x - scrollX, ss.position.y };
                                if (sunflowerShootPointScreen.x >= 0 && sunflowerShootPointScreen.x < virtualScreenWidth &&
                                    sunflowerShootPointScreen.y >= 0 && sunflowerShootPointScreen.y < virtualScreenHeight) {
                                    DrawLineEx({ triggerScreenX, 40 }, sunflowerShootPointScreen, 5, RED);
                                    DrawCircleV(sunflowerShootPointScreen, 3, RED);
                                }
                                break;
                            }
                        }
                    }
                }
            }
            for (const auto& event : spikeEvents) {
                float triggerScreenX = event.triggerX - scrollX;
                Color triggerColor = event.hasBeenActivated ? GRAY : BLUE;
                if (triggerScreenX >= -50 && triggerScreenX < virtualScreenWidth + 50)
                    DrawLineBezier({ triggerScreenX, 0 }, { triggerScreenX, virtualScreenHeight }, 5, triggerColor);
            }
        }

    // --- Voice Button draw ---
        DrawTextureV(currentButtonTexture, buttonPos, WHITE);

    // --- Checkpoint draw ---
        if (checkpointEffectActive) {
            int frameWidth = resources.checkpoint.width / 4;
            Rectangle src = { checkpointEffectFrame * frameWidth, 0, frameWidth, resources.checkpoint.height };
            Vector2 drawPos = { checkpointEffectPos.x - scrollX, checkpointEffectPos.y - checkpointEffectYoffset };
            DrawTextureRec(resources.checkpoint, src, drawPos, WHITE);
        }

    //--- Cursor draw ---
        float scaledCursorWidth = resources.cursor.width / 1.5;
        float scaledCursorHeight = resources.cursor.height / 1.5;
        Rectangle cursorDestRect = { virtualMousePos.x, virtualMousePos.y, scaledCursorWidth, scaledCursorHeight };
        Rectangle cursorSourceRect = { 0, 0, resources.cursor.width, resources.cursor.height };
        DrawTexturePro(resources.cursor, cursorSourceRect, cursorDestRect, { 0, 0 }, 0, WHITE);

        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        float finalScale = min((float)GetScreenWidth() / virtualScreenWidth, (float)GetScreenHeight() / virtualScreenHeight);
        float finalOffsetX = ((float)GetScreenWidth() - (virtualScreenWidth * finalScale)) / 2;
        float finalOffsetY = ((float)GetScreenHeight() - (virtualScreenHeight * finalScale)) / 2;
        Rectangle src = {0, 0, target.texture.width, -target.texture.height};
        Rectangle dst = {finalOffsetX, finalOffsetY, virtualScreenWidth * finalScale, virtualScreenHeight * finalScale};
        DrawTexturePro(target.texture, src, dst, {0, 0}, 0, WHITE);
        EndDrawing();
    }
    StopAllRainbowLandSounds(resources);

    return 0;
}