#include "FruityMazeResources.h"
#include "FruityMazeConst.h"

typedef enum { 
    DIFFICULTY_EASY,
    DIFFICULTY_NORMAL 
} DifficultyLevel;

using namespace std;

// Global player state
float playerRotationAngle = 0;
Vector3 playerPosition = { 0 };
vector<Prop> gameProps;
PowerUpState powerUps = { false, 0, false, 0, false, 0 };
GameState gameState = { 60, 0, false, 0, false, -1, false, false };
TextState textGif = { false, 0, false, 0, false, 0, false };
static time_t lastShaderModTime = 0;

static Rectangle GetPlayerHitbox(Vector3 playerPos, float rotationAngle) {
    return {
        playerPos.x - playerHitboxWidth / 2,
        playerPos.z - playerHitboxDepth / 2,
        playerHitboxWidth,
        playerHitboxDepth
    };
}

static bool IsColliding(Vector3 entityPos, Vector3 mapModelOrigin, int mapWidth, int mapHeight, Color* mapImagePixels, float rotationAngle) {
    Rectangle playerHitbox = GetPlayerHitbox(entityPos, rotationAngle);
    int entityCellX = round(entityPos.x - mapModelOrigin.x);
    int entityCellZ = round(entityPos.z - mapModelOrigin.z);

    for (int xOffset = -2; xOffset <= 2; xOffset++) {
        for (int zOffset = -2; zOffset <= 2; zOffset++) {
            int wallCheckMapX = entityCellX + xOffset;
            int wallCheckMapZ = entityCellZ + zOffset;

            if (wallCheckMapX >= 0 && wallCheckMapX < mapWidth && wallCheckMapZ >= 0 && wallCheckMapZ < mapHeight) {
                if (mapImagePixels[wallCheckMapZ * mapWidth + wallCheckMapX].r == 255) {
                    Rectangle wallRec = {
                        mapModelOrigin.x + wallCheckMapX,
                        mapModelOrigin.z + wallCheckMapZ,
                        1, 1
                    };
                    if (CheckCollisionRecs(playerHitbox, wallRec)) return true;
                }
            }
        }
    }
    return false;
}

static void SpawnPropsInMaze(const FruityMazeGameResources& resources) {
    TraceLog(LOG_DEBUG, "Spawning props in maze...");
    int numPowerupPositions = sizeof(POWERUP_POSITIONS) / sizeof(POWERUP_POSITIONS[0]);
    int numFruitPositions = sizeof(FRUIT_POSITIONS) / sizeof(FRUIT_POSITIONS[0]);
    gameProps.clear();

    for (int i = 0; i < numPowerupPositions; i++) {
        Vector2 powerupMapPos = POWERUP_POSITIONS[i];
        int mapX = powerupMapPos.x;
        int mapZ = powerupMapPos.y;
        if (mapX >= 0 && mapX < resources.mapWidth && mapZ >= 0 && mapZ < resources.mapHeight) {
            if (resources.mapPixels[mapZ * resources.mapWidth + mapX].r == 0) {
                Vector3 worldPos = { (resources.mapModelPosition.x + mapX) + 0.5, 0.25, (resources.mapModelPosition.z + mapZ) + 0.5 };
                PropType powerupType;
                int randomPowerup = GetRandomValue(0, 2);
                switch (randomPowerup) {
                case 0: powerupType = PROP_LIGHTNING; break;
                case 1: powerupType = PROP_GUMMYBEAR; break;
                case 2: powerupType = PROP_MAGNET; break;
                default: break;
                }
                Prop powerup;
                powerup.position = worldPos;
                powerup.type = powerupType;
                powerup.rotationY = GetRandomValue(0, 360);
                powerup.bobOffset = GetRandomValue(0, 628) / 100;
                powerup.collected = false;
                powerup.beingAttracted = false;
                gameProps.push_back(powerup);
            }
        }
    }

    for (int i = 0; i < numFruitPositions; i++) {
        Vector2 fruitMapPos = FRUIT_POSITIONS[i];
        int mapX = fruitMapPos.x;
        int mapZ = fruitMapPos.y;
        if (mapX >= 0 && mapX < resources.mapWidth && mapZ >= 0 && mapZ < resources.mapHeight) {
            if (resources.mapPixels[mapZ * resources.mapWidth + mapX].r == 0) {
                Vector3 worldPos = { resources.mapModelPosition.x + mapX + 0.5, 0.15, resources.mapModelPosition.z + mapZ + 0.5 };
                PropType fruitType;
                int randomFruit = GetRandomValue(0, 2);
                switch (randomFruit) {
                case 0: fruitType = PROP_CHERRY; break;
                case 1: fruitType = PROP_ORANGE; break;
                case 2: fruitType = PROP_GRAPES; break;
                default: fruitType = PROP_CHERRY; break;
                }
                Prop fruit;
                fruit.position = worldPos;
                fruit.type = fruitType;
                fruit.rotationY = GetRandomValue(0, 360);
                fruit.bobOffset = GetRandomValue(0, 628) / 100;
                fruit.collected = false;
                fruit.beingAttracted = false;
                gameProps.push_back(fruit);
            }
        }
    }
    TraceLog(LOG_DEBUG, TextFormat("Successfully spawned %d props.", gameProps.size()));
}

static bool IsPlayerOutOfBounds(Vector3 playerPos, const FruityMazeGameResources& resources) {
    int mapX = playerPos.x - resources.mapModelPosition.x;
    int mapZ = playerPos.z - resources.mapModelPosition.z;
    if (mapX < 0 || mapX >= resources.mapWidth || mapZ < 0 || mapZ >= resources.mapHeight)
        return true;
    return false;
}

static void AddScore(int points, const FruityMazeGameResources& resources) {
    int oldScore = gameState.score;
    gameState.score += points;
    int oldBonuses = oldScore / 75;
    int newBonuses = gameState.score / 75;
    if (newBonuses > oldBonuses) {
        gameState.gameTimer += 5;
        PlaySound(resources.timeExtendedSound);
        textGif.showTimeExtended = true;
        textGif.timeExtendedTimer = 1;
        StartAnimation((AnimationData&)resources.timeExtendedAnim);
        TraceLog(LOG_DEBUG, TextFormat("Time bonus with %d points.", (newBonuses * 75)));
    }
}

static void UpdateGameTimer(float deltaTime, const FruityMazeGameResources& resources) {
    if (gameState.gameOver) {
        gameState.gameOverTimer += deltaTime;
        return;
    }
    float previousTime = gameState.gameTimer;
    gameState.gameTimer -= deltaTime;
    if (gameState.gameTimer < 10 && gameState.gameTimer > 0) {
        int currentSecond = floor(gameState.gameTimer);
        int previousSecond = floor(previousTime);
        if (currentSecond < previousSecond) PlaySound(resources.timerPingSound);
    }
    if (gameState.gameTimer <= 0) {
        if (floor(previousTime) > 0) PlaySound(resources.timerPingSound);
        gameState.gameOver = true;
        gameState.gameTimer = 0;
        gameState.gameOverTimer = 0;
        textGif.showTimeIsUp = true;
        textGif.timeIsUpTimer = 3;
        StartAnimation((AnimationData&)resources.timeIsUpAnim);
        TraceLog(LOG_DEBUG, TextFormat("Game Over - Out of Time | Final score: %d", gameState.score));
    }
}

static void UpdateGifText(float deltaTime) {
    if (textGif.showTimeExtended) {
        textGif.timeExtendedTimer -= deltaTime;
        if (textGif.timeExtendedTimer <= 0) textGif.showTimeExtended = false;
    }
    if (textGif.showAmazing) {
        textGif.amazingTimer -= deltaTime;
        if (textGif.amazingTimer <= 0) textGif.showAmazing = false;
    }
    if (textGif.showTimeIsUp) {
        textGif.timeIsUpTimer -= deltaTime;
        if (textGif.timeIsUpTimer <= 0) textGif.showTimeIsUp = false;
    }
}

static void UpdateAnimations(float deltaTime, const FruityMazeGameResources& resources) {
    UpdateAnimation((AnimationData&)resources.amazingAnim, deltaTime);
    UpdateAnimation((AnimationData&)resources.timeIsUpAnim, deltaTime);
    UpdateAnimation((AnimationData&)resources.timeExtendedAnim, deltaTime);
}

static void UpdatePowerUps(float deltaTime, const FruityMazeGameResources& resources) {
    if (powerUps.lightningActive) {
        powerUps.lightningTimeLeft -= deltaTime;
        if (powerUps.lightningTimeLeft <= 0) {
            powerUps.lightningTimeLeft = 0;
            powerUps.lightningActive = false;
        }
    }
    if (powerUps.gummybearActive) {
        powerUps.gummybearTimeLeft -= deltaTime;
        if (powerUps.gummybearTimeLeft <= 0) {
            powerUps.gummybearTimeLeft = 0;
            powerUps.gummybearActive = false;
            if (IsColliding(playerPosition, resources.mapModelPosition, resources.mapWidth, resources.mapHeight, resources.mapPixels, playerRotationAngle)) {
                gameState.temporaryNoclip = true;
                TraceLog(LOG_DEBUG, "Emergency noclip enabled.");
            }
            else {
                gameState.temporaryNoclip = false;
                TraceLog(LOG_DEBUG, "Emergency noclip disabled.");
            }
        }
    }
    if (powerUps.magnetActive) {
        powerUps.magnetTimeLeft -= deltaTime;
        if (powerUps.magnetTimeLeft <= 0) {
            powerUps.magnetTimeLeft = 0;
            powerUps.magnetActive = false;
        }
    }
}

static void UpdateProps(float deltaTime) {
    for (auto& prop : gameProps) {
        if (!prop.collected) {
            prop.rotationY += propRotationSpeed * deltaTime;
            prop.bobOffset += propBobSpeed * deltaTime;
            if (prop.type != PROP_MAGNET) {
                float distanceToPlayer = Vector3Distance(prop.position, playerPosition);
                if (distanceToPlayer <= 3) {
                    Vector3 directionToPlayer = Vector3Normalize(Vector3Subtract(playerPosition, prop.position));
                    Vector3 naturalAttraction = Vector3Scale(directionToPlayer, magnetPropSpeed * deltaTime);
                    prop.position = Vector3Add(prop.position, naturalAttraction);
                }
                else if (powerUps.magnetActive && distanceToPlayer <= magnetRange) prop.beingAttracted = true;
                if (prop.beingAttracted) {
                    Vector3 directionToPlayer = Vector3Normalize(Vector3Subtract(playerPosition, prop.position));
                    Vector3 attraction = Vector3Scale(directionToPlayer, magnetPropSpeed * deltaTime);
                    prop.position = Vector3Add(prop.position, attraction);
                }
            }
        }
    }
}

static void ActivatePowerUp(PropType powerUpType, const FruityMazeGameResources& resources) {
    switch (powerUpType) {
    case PROP_LIGHTNING:
        powerUps.lightningActive = true;
        powerUps.lightningTimeLeft += lightningDuration;
        break;
    case PROP_GUMMYBEAR:
        powerUps.gummybearActive = true;
        powerUps.gummybearTimeLeft += gummybearDuration;
        break;
    case PROP_MAGNET:
        powerUps.magnetActive = true;
        powerUps.magnetTimeLeft = magnetDuration;
        break;
    default: break;
    }
    PlaySound(resources.powerUpSound);
}

static float GetCurrentPlayerSpeed() {
    float baseSpeed = playerMoveSpeed;
    if (powerUps.lightningActive) baseSpeed *= lightningSpeedMultiplier;
    return baseSpeed;
}

static bool AreAllFruitsCollected() {
    for (const auto& prop : gameProps)
        if (!prop.collected && (prop.type == PROP_CHERRY || prop.type == PROP_ORANGE || prop.type == PROP_GRAPES))
            return false;
    return true;
}

static void CheckPropCollection(Vector3 playerPos, const FruityMazeGameResources& resources) {
    for (auto& prop : gameProps) {
        if (!prop.collected) {
            Rectangle playerHitbox = GetPlayerHitbox(playerPos, playerRotationAngle);
            Rectangle propHitbox = {
                prop.position.x - collisionRadius,
                prop.position.z - collisionRadius,
                collisionRadius * 2,
                collisionRadius * 2
            };
            if (CheckCollisionRecs(playerHitbox, propHitbox)) {
                prop.collected = true;
                prop.beingAttracted = false;
                if (prop.type == PROP_LIGHTNING || prop.type == PROP_GUMMYBEAR || prop.type == PROP_MAGNET)
                    ActivatePowerUp(prop.type, resources);
                else {
                    AddScore(5, resources);
                    switch (prop.type) {
                    case PROP_ORANGE: PlaySound(resources.fruit1Sound); break;
                    case PROP_CHERRY: PlaySound(resources.fruit2Sound); break;
                    case PROP_GRAPES: PlaySound(resources.fruit3Sound); break;
                    default: break;
                    }
                    if (AreAllFruitsCollected() && !gameState.gameOver) {
                        gameState.gameWon = true;
                        gameState.gameOver = true;
                        gameState.gameOverTimer = 0;
                        textGif.showAmazing = true;
                        textGif.amazingTimer = 1;
                        StartAnimation((AnimationData&)resources.amazingAnim);
                    }
                }
                TraceLog(LOG_DEBUG, TextFormat("Prop: %.0f, %.0f | Score: %d", prop.position.x, prop.position.z, gameState.score));
            }
        }
    }
}

static void DrawProps(const FruityMazeGameResources& resources, float time, const Camera& camera, bool debugMode = false) {
    Vector3 cameraPos = camera.position;
    Vector3 cameraTarget = camera.target;
    Vector3 forward = Vector3Normalize(Vector3Subtract(cameraTarget, cameraPos));
    for (const auto& prop : gameProps) {
        if (!prop.collected) {
            Vector3 toProp = Vector3Normalize(Vector3Subtract(prop.position, cameraPos));
            float dot = Vector3DotProduct(forward, toProp);
            if (dot < -0.3) continue;

            Vector3 drawPosition = prop.position;
            if (!prop.beingAttracted) drawPosition.y += sinf(time * propBobSpeed + prop.bobOffset) * propBobHeight;

            Model* modelToDraw = nullptr;
            bool modelAvailable = false;
            switch (prop.type) {
            case PROP_CHERRY: modelToDraw = (Model*)&resources.cherryModel; modelAvailable = true; break;
            case PROP_ORANGE: modelToDraw = (Model*)&resources.orangeModel; modelAvailable = true; break;
            case PROP_GRAPES: modelToDraw = (Model*)&resources.grapesModel; modelAvailable = true; break;
            case PROP_LIGHTNING: modelToDraw = (Model*)&resources.lightningModel; modelAvailable = true; break;
            case PROP_GUMMYBEAR: modelToDraw = (Model*)&resources.gummybearModel; modelAvailable = true; break;
            case PROP_MAGNET: modelToDraw = (Model*)&resources.magnetModel; modelAvailable = true; break;
            default: break;
            }

            if (modelAvailable && modelToDraw)
                DrawModelEx(*modelToDraw, drawPosition, { 0, 1, 0 }, prop.rotationY, { propScale, propScale, propScale }, WHITE);
            else {
                Color propColor = BLANK;
                switch (prop.type) {
                case PROP_CHERRY: propColor = RED; break;
                case PROP_ORANGE: propColor = ORANGE; break;
                case PROP_GRAPES: propColor = PURPLE; break;
                case PROP_LIGHTNING: propColor = YELLOW; break;
                case PROP_GUMMYBEAR: propColor = GREEN; break;
                case PROP_MAGNET: propColor = GRAY; break;
                default: break;
                }
                DrawCube(drawPosition, propScale, propScale, propScale, propColor);
            }
        }
    }
}

static void DrawTextGif(const FruityMazeGameResources& resources) {
    float centerX = virtualScreenWidth / 2;
    float centerY = virtualScreenHeight / 2;

    if (textGif.showTimeExtended && resources.timeExtendedAnim.spriteSheet.id > 0) {
        Rectangle sourceRect = GetCurrentAnimationFrame(resources.timeExtendedAnim);
        float texWidth = sourceRect.width * gifScale;
        float texHeight = sourceRect.height * gifScale;
        Rectangle destRect = { centerX - texWidth / 2, centerY - texHeight / 2, texWidth, texHeight };
        DrawTexturePro(resources.timeExtendedAnim.spriteSheet, sourceRect, destRect, { 0, 0 }, 0, WHITE);
    }
    if (textGif.showAmazing && resources.amazingAnim.spriteSheet.id > 0) {
        Rectangle sourceRect = GetCurrentAnimationFrame(resources.amazingAnim);
        float texWidth = sourceRect.width * gifScale;
        float texHeight = sourceRect.height * gifScale;
        Rectangle destRect = { centerX - texWidth / 2, centerY - texHeight / 2, texWidth, texHeight };
        DrawTexturePro(resources.amazingAnim.spriteSheet, sourceRect, destRect, { 0, 0 }, 0, WHITE);
    }
    if (textGif.showTimeIsUp && resources.timeIsUpAnim.spriteSheet.id > 0) {
        Rectangle sourceRect = GetCurrentAnimationFrame(resources.timeIsUpAnim);
        float texWidth = sourceRect.width * gifScale;
        float texHeight = sourceRect.height * gifScale;
        Rectangle destRect = { centerX - texWidth / 2, centerY - texHeight / 2, texWidth, texHeight };
        DrawTexturePro(resources.timeIsUpAnim.spriteSheet, sourceRect, destRect, { 0, 0 }, 0, WHITE);
    }
    if (textGif.showOutOfBounds && resources.outOfBounds.id > 0) {
        float texWidth = resources.outOfBounds.width * gifScale;
        float texHeight = resources.outOfBounds.height * gifScale;
        Rectangle destRect = { centerX - texWidth / 2, centerY - texHeight / 2, texWidth, texHeight };
        DrawTexturePro(resources.outOfBounds, { 0, 0, (float)resources.outOfBounds.width, (float)resources.outOfBounds.height }, destRect, { 0, 0 }, 0, WHITE);
    }
}

int runFruityMaze(GraphicsQuality quality, Shader postProcessingShader, bool applyShader, DifficultyLevel difficulty) {
    TraceLog(LOG_DEBUG, TextFormat("Starting Fruity 3D Maze | Quality: %d | Shader: %d | Difficulty: %d", quality, applyShader, difficulty));

    powerUps = { false, 0, false, 0, false, 0 };
    gameState = { 60, 0, false, 0, false, -1, false, false };
    textGif = { false, 0, false, 0, false, 0, false };

    FruityMazeGameResources resources = LoadFruityMazeResources(quality);
    if (!CheckFruityMazeResourcesLoaded(resources)) {
        UnloadFruityMazeResources(resources);
        return 1;
    }

    RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    if (target.id == 0) {
        UnloadFruityMazeResources(resources);
        return 1;
    }

    int shaderTimeLoc = -1;
    int shaderResolutionLoc = -1;
    if (applyShader) {
        shaderTimeLoc = GetShaderLocation(postProcessingShader, "time");
        shaderResolutionLoc = GetShaderLocation(postProcessingShader, "resolution");
        if (shaderResolutionLoc != -1) {
            float gameResolution[2] = { (float)virtualScreenWidth, (float)virtualScreenHeight };
            SetShaderValue(postProcessingShader, shaderResolutionLoc, gameResolution, SHADER_UNIFORM_VEC2);
        }
    }

    Camera camera = { 0 };
    camera.up = { 0, 1, 0 };
    camera.projection = CAMERA_PERSPECTIVE;
    camera.fovy = 60;

    ViewCameraMode currentCameraMode = VIEW_CAMERA_THIRD_PERSON;
    bool startFound = false;
    Vector3 initialSpawnPoint = { 0 };
    bool debug = false;

    if (resources.mapPixels) {
        vector<Vector3> potentialSpawnPoints;
        for (int i = 0; i < resources.mapWidth * resources.mapHeight; i++) {
            int x = i % resources.mapWidth;
            int z = i / resources.mapWidth;
            if (resources.mapPixels[i].r == 0) {
                Vector3 safePoint = { resources.mapModelPosition.x + x, 0, resources.mapModelPosition.z + z };
                if (!IsColliding(safePoint, resources.mapModelPosition, resources.mapWidth, resources.mapHeight, resources.mapPixels, 0))
                    potentialSpawnPoints.push_back(safePoint);
            }
        }
        if (!potentialSpawnPoints.empty()) {
            int randomIndex = GetRandomValue(0, potentialSpawnPoints.size() - 1);
            initialSpawnPoint = potentialSpawnPoints[randomIndex];
            startFound = true;
            TraceLog(LOG_DEBUG, TextFormat("Found %d potential safe spawn points.", potentialSpawnPoints.size()));
        }
    }

    if (startFound) {
        playerRotationAngle = 0;
        playerPosition = initialSpawnPoint;
        SpawnPropsInMaze(resources);
    }
    else {
        camera.position = { resources.mapModelPosition.x + resources.mapWidth / 2, 15, resources.mapModelPosition.z + resources.mapHeight / 2 - 10 };
        camera.target = { resources.mapModelPosition.x + resources.mapWidth / 2, 0, resources.mapModelPosition.z + resources.mapHeight / 2 };
    }

    PlayMusicStream(resources.backgroundMusic);
    SetMusicVolume(resources.backgroundMusic, 1);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsKeyPressed(KEY_F5)) {
            Shader newShader = LoadShader(0, "FruityMaze.fs");
            if (newShader.id > 0) {
                if (postProcessingShader.id > 0) UnloadShader(postProcessingShader);
                postProcessingShader = newShader;
                if (applyShader) {
                    shaderTimeLoc = GetShaderLocation(postProcessingShader, "time");
                    shaderResolutionLoc = GetShaderLocation(postProcessingShader, "resolution");
                    if (shaderResolutionLoc != -1) {
                        float gameResolution[2] = { (float)virtualScreenWidth, (float)virtualScreenHeight };
                        SetShaderValue(postProcessingShader, shaderResolutionLoc, gameResolution, SHADER_UNIFORM_VEC2);
                    }
                }
                TraceLog(LOG_DEBUG, "Shader reloaded successfully.");
            }
            else TraceLog(LOG_DEBUG, "Failed to reload shader.");
        }

        UpdateMusicStream(resources.backgroundMusic);
        if (GetMusicTimePlayed(resources.backgroundMusic) >= GetMusicTimeLength(resources.backgroundMusic))
            SeekMusicStream(resources.backgroundMusic, 0);

        UpdateGameTimer(dt, resources);
        UpdateGifText(dt);
        UpdateAnimations(dt, resources);

        if ((powerUps.gummybearActive || gameState.temporaryNoclip) && !gameState.gameOver) {
            if (IsPlayerOutOfBounds(playerPosition, resources)) {
                if (!gameState.outOfBounds) {
                    gameState.outOfBounds = true;
                    gameState.gameOver = true;
                    gameState.gameOverTimer = 0;
                    textGif.showOutOfBounds = true;
                    PauseMusicStream(resources.backgroundMusic);
                    PlaySound(resources.boundsSound);
                    TraceLog(LOG_DEBUG, TextFormat("Game Over - Out of bounds | Final Score: %d", gameState.score));
                }
            }
        }

        if (gameState.gameOver && gameState.gameOverTimer >= (gameState.gameWon ? 3 : 4))
            break;

        UpdatePowerUps(dt, resources);
        UpdateProps(dt);

        if (startFound && !gameState.gameOver)
            CheckPropCollection(playerPosition, resources);

        if (IsKeyPressed(KEY_ESCAPE)) break;

        if (IsKeyPressed(KEY_T)) {
            currentCameraMode = (ViewCameraMode)((currentCameraMode + 1) % 3);
            if (currentCameraMode == VIEW_CAMERA_FIRST_PERSON && startFound) DisableCursor();
            else EnableCursor();
        }

        if (IsKeyPressed(KEY_F3)) debug = !debug;

        if (debug) {
            if (IsKeyPressed(KEY_L)) ActivatePowerUp(PROP_LIGHTNING, resources);
            if (IsKeyPressed(KEY_G)) ActivatePowerUp(PROP_GUMMYBEAR, resources);
            if (IsKeyPressed(KEY_M)) ActivatePowerUp(PROP_MAGNET, resources);
        }

        if (startFound && currentCameraMode == VIEW_CAMERA_FIRST_PERSON) {
            float mouseDeltaX = GetMouseDelta().x;
            playerRotationAngle -= mouseDeltaX * mouseSensitivity;
            while (playerRotationAngle < 0) playerRotationAngle += 360;
            while (playerRotationAngle >= 360) playerRotationAngle -= 360;
        }

        if (startFound && resources.mapPixels && !gameState.gameOver) {
            Vector3 moveDirectionInput = { 0 };
            Vector3 forwardMovementDirection;
            Vector3 rightMovementDirection;
            if (currentCameraMode == VIEW_CAMERA_FIRST_PERSON) {
                forwardMovementDirection = { -sinf(playerRotationAngle * DEG2RAD), 0, -cosf(playerRotationAngle * DEG2RAD) };
                rightMovementDirection = { forwardMovementDirection.z, 0, -forwardMovementDirection.x };
            }
            else {
                forwardMovementDirection = { 0, 0, 1 };
                rightMovementDirection = { 1, 0, 0 };
            }
            if (IsKeyDown(KEY_W)) moveDirectionInput = Vector3Add(moveDirectionInput, forwardMovementDirection);
            if (IsKeyDown(KEY_S)) moveDirectionInput = Vector3Subtract(moveDirectionInput, forwardMovementDirection);
            if (IsKeyDown(KEY_A)) moveDirectionInput = Vector3Add(moveDirectionInput, rightMovementDirection);
            if (IsKeyDown(KEY_D)) moveDirectionInput = Vector3Subtract(moveDirectionInput, rightMovementDirection);

            if (Vector3LengthSqr(moveDirectionInput) > 0) {
                moveDirectionInput = Vector3Normalize(moveDirectionInput);
                float currentSpeed = GetCurrentPlayerSpeed();
                Vector3 desiredMovement = Vector3Scale(moveDirectionInput, currentSpeed * dt);
                if (powerUps.gummybearActive || gameState.temporaryNoclip) {
                    playerPosition.x += desiredMovement.x;
                    playerPosition.z += desiredMovement.z;
                }
                else {
                    Vector3 finalPosition = playerPosition;
                    Vector3 testPosX = playerPosition;
                    testPosX.x += desiredMovement.x;
                    if (!IsColliding(testPosX, resources.mapModelPosition, resources.mapWidth, resources.mapHeight, resources.mapPixels, playerRotationAngle))
                        finalPosition.x = testPosX.x;

                    Vector3 testPosZ = playerPosition;
                    testPosZ.z += desiredMovement.z;
                    if (!IsColliding(testPosZ, resources.mapModelPosition, resources.mapWidth, resources.mapHeight, resources.mapPixels, playerRotationAngle))
                        finalPosition.z = testPosZ.z;

                    playerPosition = finalPosition;
                }

                if (currentCameraMode != VIEW_CAMERA_FIRST_PERSON) {
                    float targetAngleDeg = atan2f(-moveDirectionInput.x, -moveDirectionInput.z) * RAD2DEG;
                    float finalTargetAngleDeg = roundf(targetAngleDeg / 45) * 45;
                    float angleDiff = finalTargetAngleDeg - playerRotationAngle;
                    float maxRotationStep = playerRotationSpeed * dt;
                    if (angleDiff > 180) angleDiff -= 360;
                    else if (angleDiff < -180) angleDiff += 360;
                    if (fabsf(angleDiff) > 0) {
                        if (fabsf(angleDiff) < maxRotationStep) playerRotationAngle = finalTargetAngleDeg;
                        else playerRotationAngle += (angleDiff > 0 ? maxRotationStep : -maxRotationStep);
                    }
                }
            }
        }

        if (startFound) {
            camera.fovy = 60;
            camera.up = { 0, 1, 0 };
            switch (currentCameraMode) {
            case VIEW_CAMERA_FIRST_PERSON: {
                Matrix playerMat = MatrixRotateY(playerRotationAngle * DEG2RAD);
                Vector3 fppOffsetTransformed = Vector3Transform(FirstPersonOffset, playerMat);
                Vector3 playerCameraForward = { -sinf(playerRotationAngle * DEG2RAD), 0, -cosf(playerRotationAngle * DEG2RAD) };
                camera.position = Vector3Add(playerPosition, fppOffsetTransformed);
                camera.target = Vector3Add(camera.position, playerCameraForward);
                camera.fovy = 75;
            } break;
            case VIEW_CAMERA_SECOND_PERSON: {
                camera.position = { playerPosition.x, playerPosition.y + SecondPersonOffset.y, playerPosition.z };
                camera.target = playerPosition;
                camera.up = { 0, 0, 1 };
            } break;
            case VIEW_CAMERA_THIRD_PERSON:
            default: {
                camera.position = Vector3Add(playerPosition, ThirdPersonOffset);
                camera.target = playerPosition;
            } break;
            }
        }
        else UpdateCamera(&camera, CAMERA_ORBITAL);

        BeginTextureMode(target);
        ClearBackground(BLACK);
        BeginMode3D(camera);

        DrawPlane({ resources.mapModelPosition.x + (resources.mapWidth / 2.0f), 0.01f, resources.mapModelPosition.z + (resources.mapHeight / 2.0f) }, { (float)resources.mapWidth + 20, (float)resources.mapHeight + 20 }, BLACK);
        DrawModel(resources.mazeModel, resources.mapModelPosition, 1.0f, WHITE);

        Vector3 lightPos = { playerPosition.x, playerPosition.y + 10, playerPosition.z };
        Vector3 lightColor = { 1.0, 0.9, 0.8 };
        float ambientStrength = 0.3f;
        SetShaderValue(resources.lightingShader, GetShaderLocation(resources.lightingShader, "lightPos"), &lightPos, SHADER_UNIFORM_VEC3);
        SetShaderValue(resources.lightingShader, GetShaderLocation(resources.lightingShader, "lightColor"), &lightColor, SHADER_UNIFORM_VEC3);
        SetShaderValue(resources.lightingShader, GetShaderLocation(resources.lightingShader, "viewPos"), &camera.position, SHADER_UNIFORM_VEC3);
        SetShaderValue(resources.lightingShader, GetShaderLocation(resources.lightingShader, "ambientStrength"), &ambientStrength, SHADER_UNIFORM_FLOAT);
        resources.mazeModel.materials[0].shader = resources.lightingShader;
        resources.playerModel.materials[0].shader = resources.lightingShader;

        if (startFound) {
            bool shouldDrawPlayer = (currentCameraMode != VIEW_CAMERA_FIRST_PERSON || debug);
            if (shouldDrawPlayer) {
                Color playerTint = WHITE;
                DrawModelEx(resources.playerModel, playerPosition, { 0, 1, 0 }, playerRotationAngle, { playerScale, playerScale, playerScale }, playerTint);
            }
        }
        DrawProps(resources, GetTime(), camera, debug);

        if (debug && startFound) {
            if (currentCameraMode == VIEW_CAMERA_FIRST_PERSON) {
                Vector3 rayStart = camera.position;
                Vector3 rayEnd = Vector3Add(rayStart, Vector3Scale({ -sinf(playerRotationAngle * DEG2RAD), 0, -cosf(playerRotationAngle * DEG2RAD) }, 5));
                DrawLine3D(rayStart, rayEnd, RED);
            }
            if (resources.mapPixels) {
                int entityCellX = roundf(playerPosition.x - resources.mapModelPosition.x);
                int entityCellZ = roundf(playerPosition.z - resources.mapModelPosition.z);
                for (int x = -1; x <= 1; x++) {
                    for (int z = -1; z <= 1; z++) {
                        int wallCheckMapX = entityCellX + x;
                        int wallCheckMapZ = entityCellZ + z;
                        if (wallCheckMapX >= 0 && wallCheckMapX < resources.mapWidth && wallCheckMapZ >= 0 && wallCheckMapZ < resources.mapHeight) {
                            if (x == 0 && z == 0) DrawCubeWires({ resources.mapModelPosition.x + wallCheckMapX, -0.4f, resources.mapModelPosition.z + wallCheckMapZ }, 2, 0.2f, 2, BLUE);
                            else DrawCubeWires({ resources.mapModelPosition.x + wallCheckMapX, -0.45f, resources.mapModelPosition.z + wallCheckMapZ }, 2, 0.2f, 2, RED);
                        }
                    }
                }
                DrawCubeWires(playerPosition, playerHitboxWidth, 1.5, playerHitboxDepth, RED);
                if (powerUps.magnetActive) DrawCylinderWires(playerPosition, magnetRange, magnetRange, 0.1, 32, BLUE);
            }
        }

        EndMode3D();

        bool isMinimapVisible = (difficulty == DIFFICULTY_EASY);
        bool isMinimapExpanded = isMinimapVisible && IsKeyDown(KEY_TAB);

        if (isMinimapVisible && !isMinimapExpanded) {
            float scaledMapWidth = resources.mapWidth * minimapScale;
            float scaledMapHeight = resources.mapHeight * minimapScale;
            Vector2 minimapVisualTopLeft = { virtualScreenWidth - scaledMapWidth - 10, 10 };
            Vector2 minimapTextureDrawPos = { minimapVisualTopLeft.x + scaledMapWidth, minimapVisualTopLeft.y + scaledMapHeight };
            DrawTextureEx(resources.minimapTexture, minimapTextureDrawPos, 180, minimapScale, WHITE);
            DrawRectangleLines(minimapVisualTopLeft.x, minimapVisualTopLeft.y, scaledMapWidth, scaledMapHeight, LIME);
            if (startFound) {
                float playerMapX = (playerPosition.x - resources.mapModelPosition.x + 0.5f);
                float playerMapZ = (playerPosition.z - resources.mapModelPosition.z + 0.5f);
                float playerRelXOnMinimap = (resources.mapWidth - playerMapX) * minimapScale;
                float playerRelZOnMinimap = (resources.mapHeight - playerMapZ) * minimapScale;
                DrawCircle(minimapVisualTopLeft.x + playerRelXOnMinimap, minimapVisualTopLeft.y + playerRelZOnMinimap, collisionRadius * minimapScale, RED);
                
                if (powerUps.magnetActive) DrawCircleLines(minimapVisualTopLeft.x + playerRelXOnMinimap, minimapVisualTopLeft.y + playerRelZOnMinimap, magnetRange * minimapScale, BLUE);
                for (const auto& prop : gameProps) {
                    if (!prop.collected) {
                        float propMapX = (prop.position.x - resources.mapModelPosition.x + 0.5f);
                        float propMapZ = (prop.position.z - resources.mapModelPosition.z + 0.5f);
                        float propRelXOnMinimap = (resources.mapWidth - propMapX) * minimapScale;
                        float propRelZOnMinimap = (resources.mapHeight - propMapZ) * minimapScale;
                        DrawRectangle(minimapVisualTopLeft.x + propRelXOnMinimap - minimapScale, minimapVisualTopLeft.y + propRelZOnMinimap - minimapScale, minimapScale * 2, minimapScale * 2, YELLOW);
                    }
                }
            }
        }

        if (startFound) {
            const char* camModeStr;
            switch (currentCameraMode) {
                case VIEW_CAMERA_FIRST_PERSON: camModeStr = "FPP (Mouse Look)"; break;
                case VIEW_CAMERA_SECOND_PERSON: camModeStr = "SPP"; break;
                case VIEW_CAMERA_THIRD_PERSON: camModeStr = "TPP"; break;
                default: camModeStr = "UNKNOWN"; break;
            }
            DrawText(TextFormat("%.0f", gameState.gameTimer), textPadding, textPadding, UI_LARGE_FONT_SIZE, WHITE);
            const char* scoreLabel = "SCORE";
            const char* scoreValue = TextFormat("%d", gameState.score);
            int scoreLabelWidth = MeasureText(scoreLabel, UI_LARGE_FONT_SIZE);
            int scoreValueWidth = MeasureText(scoreValue, UI_LARGE_FONT_SIZE);
            int maxScoreWidth = max(scoreLabelWidth, scoreValueWidth);
            int scoreX = virtualScreenWidth - maxScoreWidth - textPadding;
            DrawText(scoreLabel, scoreX, virtualScreenHeight - (UI_LARGE_FONT_SIZE * 2) - textPadding, UI_LARGE_FONT_SIZE, WHITE);
            DrawText(scoreValue, scoreX, virtualScreenHeight - UI_LARGE_FONT_SIZE - textPadding, UI_LARGE_FONT_SIZE, WHITE);
            float powerUpYOffset = textPadding;
            if (powerUps.lightningActive) {
                const char* lightningText = TextFormat("%.0f", powerUps.lightningTimeLeft);
                int lightningWidth = MeasureText(lightningText, UI_LARGE_FONT_SIZE);
                DrawText(lightningText, virtualScreenWidth - lightningWidth - textPadding, powerUpYOffset, UI_LARGE_FONT_SIZE, YELLOW);
                powerUpYOffset += UI_LARGE_FONT_SIZE + 10;
            }
            if (powerUps.gummybearActive) {
                const char* gummybearText = TextFormat("%.0f", powerUps.gummybearTimeLeft);
                int gummybearWidth = MeasureText(gummybearText, UI_LARGE_FONT_SIZE);
                DrawText(gummybearText, virtualScreenWidth - gummybearWidth - textPadding, powerUpYOffset, UI_LARGE_FONT_SIZE, LIME);
                powerUpYOffset += UI_LARGE_FONT_SIZE + 10;
            }
            if (powerUps.magnetActive) {
                const char* magnetText = TextFormat("%.0f", powerUps.magnetTimeLeft);
                int magnetWidth = MeasureText(magnetText, UI_LARGE_FONT_SIZE);
                DrawText(magnetText, virtualScreenWidth - magnetWidth - textPadding, powerUpYOffset, UI_LARGE_FONT_SIZE, BLUE);
                powerUpYOffset += UI_LARGE_FONT_SIZE + 10;
            }
            if (debug) {
                float debugYOffset = textPadding + UI_LARGE_FONT_SIZE + 20;
                DrawText(TextFormat("Pos: X:%.1f Z:%.1f (%s)", playerPosition.x, playerPosition.z, camModeStr), textPadding, debugYOffset, UI_SMALL_FONT_SIZE - 2, LIGHTGRAY);
                DrawText(TextFormat("Angle: %.1f | Speed: %.1f", playerRotationAngle, GetCurrentPlayerSpeed()), textPadding, debugYOffset + UI_SMALL_FONT_SIZE + 2, UI_SMALL_FONT_SIZE - 2, LIGHTGRAY);
            }
            if (gameState.gameOver) {
                const char* finalScoreMsg = TextFormat("Final Score: %d", gameState.score);
                int scoreWidth = MeasureText(finalScoreMsg, UI_MEDIUM_FONT_SIZE);
                DrawText(finalScoreMsg, (virtualScreenWidth - scoreWidth) / 2, virtualScreenHeight - 100, UI_MEDIUM_FONT_SIZE, YELLOW);
                float timeRemaining = (gameState.gameWon ? 3 : 4) - gameState.gameOverTimer;
                const char* returnMsg = TextFormat("Returning to menu in %.1fs...", timeRemaining > 0 ? timeRemaining : 0);
                int returnWidth = MeasureText(returnMsg, UI_SMALL_FONT_SIZE);
                DrawText(returnMsg, (virtualScreenWidth - returnWidth) / 2, virtualScreenHeight - 60, UI_SMALL_FONT_SIZE, LIGHTGRAY);
            }
            const char* controlsText = debug ? "ESC: Exit | T: Camera | F3: Debug | F5: Reload Shader | L,G,M: Cheats" : "ESC: Exit | T: Camera | F3: Debug | F5: Reload Shader";
            int controlsWidth = MeasureText(controlsText, 12);
            DrawText(controlsText, (virtualScreenWidth - controlsWidth) / 2, virtualScreenHeight - 40, 12, LIGHTGRAY);
        }
        else {
            DrawText("CANNOT START GAME. Check console/map. Orbiting...", 10, 40, 20, RED);
        }
        DrawTextGif(resources);

        if (isMinimapExpanded) {
            float expandedScale = min(((float)virtualScreenWidth - 50) / resources.mapWidth, ((float)virtualScreenHeight - 50) / resources.mapHeight);
            float expandedWidth = resources.mapWidth * expandedScale;
            float expandedHeight = resources.mapHeight * expandedScale;
            Vector2 expandedTopLeft = { (virtualScreenWidth - expandedWidth) / 2, (virtualScreenHeight - expandedHeight) / 2 };
            Vector2 expandedTexturePos = { expandedTopLeft.x + expandedWidth, expandedTopLeft.y + expandedHeight };

            DrawTextureEx(resources.minimapTexture, expandedTexturePos, 180, expandedScale, Fade(WHITE, 0.75f));

            if (startFound) {
                float playerMapX = (playerPosition.x - resources.mapModelPosition.x + 0.5f);
                float playerMapZ = (playerPosition.z - resources.mapModelPosition.z + 0.5f);
                float playerRelX = (resources.mapWidth - playerMapX) * expandedScale;
                float playerRelZ = (resources.mapHeight - playerMapZ) * expandedScale;
                DrawCircle(expandedTopLeft.x + playerRelX, expandedTopLeft.y + playerRelZ, 3.0f * expandedScale, RED);
                
                for (const auto& prop : gameProps) {
                    if (!prop.collected) {
                        float propMapX = (prop.position.x - resources.mapModelPosition.x + 0.5f);
                        float propMapZ = (prop.position.z - resources.mapModelPosition.z + 0.5f);
                        float propRelXOnMinimap = (resources.mapWidth - propMapX) * expandedScale;
                        float propRelZOnMinimap = (resources.mapHeight - propMapZ) * expandedScale;
                        DrawRectangle(expandedTopLeft.x + propRelXOnMinimap - expandedScale, expandedTopLeft.y + propRelZOnMinimap - expandedScale, expandedScale * 2, expandedScale * 2, YELLOW);
                    }
                }
            }
        }
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        float finalScale = min((float)GetScreenWidth() / virtualScreenWidth, (float)GetScreenHeight() / virtualScreenHeight);
        float finalOffsetX = (GetScreenWidth() - (virtualScreenWidth * finalScale)) / 2;
        float finalOffsetY = (GetScreenHeight() - (virtualScreenHeight * finalScale)) / 2;
        Rectangle src = { 0, 0, (float)target.texture.width, -(float)target.texture.height };
        Rectangle dst = { finalOffsetX, finalOffsetY, virtualScreenWidth * finalScale, virtualScreenHeight * finalScale };
        if (applyShader) {
            if (shaderTimeLoc != -1) {
                float currentTime = GetTime();
                SetShaderValue(postProcessingShader, shaderTimeLoc, &currentTime, SHADER_UNIFORM_FLOAT);
            }
            BeginShaderMode(postProcessingShader);
            DrawTexturePro(target.texture, src, dst, { 0, 0 }, 0, WHITE);
            EndShaderMode();
        }
        else {
            DrawTexturePro(target.texture, src, dst, { 0, 0 }, 0, WHITE);
        }
        if (debug) DrawFPS(15, 15);
        EndDrawing();
    }

    if (IsMusicStreamPlaying(resources.backgroundMusic)) StopMusicStream(resources.backgroundMusic);
    UnloadRenderTexture(target);
    UnloadFruityMazeResources(resources);
    return 0;
}