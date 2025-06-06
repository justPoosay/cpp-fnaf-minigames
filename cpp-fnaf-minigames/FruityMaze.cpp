#include "FruityMazeResources.h"
#include "FruityMazeConst.h"
#include <sys/stat.h>
#include <iostream>

using namespace std;

// Power-up state structure
struct PowerUpState {
    bool lightningActive;
    float lightningTimeLeft;
    bool gummybearActive;
    float gummybearTimeLeft;
    bool magnetActive;
    float magnetTimeLeft;
};

// Game state structure
struct GameState {
    float gameTimer;
    int score;
    bool gameOver;
    float gameOverTimer;
    bool outOfBounds;
    int lastTickSecond;
    bool temporaryNoclip;
    bool gameWon;
};

// Prop structure
struct Prop {
    Vector3 position;
    PropType type;
    float rotationY;
    float bobOffset;
    bool collected;
    bool beingAttracted;
};

// Global player state
float playerRotationAngle = 0;
Vector3 playerPosition = { 0 };
vector<Prop> gameProps;
PowerUpState powerUps = { false, 0, false, 0, false, 0 };
GameState gameState = { 60, 0, false, 0, false, -1, false, false };

// Shader hot-reload variables
static const char* shaderPath = "FruityMaze.fs";  // Adjust path as needed
static time_t lastShaderModTime = 0;

static Rectangle GetPlayerHitbox(Vector3 playerPos, float rotationAngle) {
    return {
        playerPos.x - playerHitboxWidth / 2,
        playerPos.z - playerHitboxDepth / 2,
        playerHitboxWidth,
        playerHitboxDepth
    };
}

// Collision detection function
static bool IsColliding(Vector3 entityPos, Vector3 mapModelOrigin, const Image& mapImage, Color* mapImagePixels, float rotationAngle) {
    Rectangle playerHitbox = GetPlayerHitbox(entityPos, rotationAngle);

    // Check collision with all nearby wall cells
    int entityCellX = round(entityPos.x - mapModelOrigin.x);
    int entityCellZ = round(entityPos.z - mapModelOrigin.z);

    for (int zOffset = -2; zOffset <= 2; zOffset++) {
        for (int xOffset = -2; xOffset <= 2; xOffset++) {
            int wallCheckMapX = entityCellX + xOffset;
            int wallCheckMapZ = entityCellZ + zOffset;

            if (wallCheckMapX >= 0 && wallCheckMapX < mapImage.width &&
                wallCheckMapZ >= 0 && wallCheckMapZ < mapImage.height) {

                if (mapImagePixels[wallCheckMapZ * mapImage.width + wallCheckMapX].r == 255) {
                    Rectangle wallRec = {
                        mapModelOrigin.x + wallCheckMapX,
                        mapModelOrigin.z + wallCheckMapZ,
                        1, 1
                    };

                    if (CheckCollisionRecs(playerHitbox, wallRec))
                        return true;
                }
            }
        }
    }
    return false;
}

// Function to spawn props throughout the maze using manual coordinates
static void SpawnPropsInMaze(const FruityMazeGameResources& resources) {
    gameProps.clear();

    cout << "Spawning props in maze using manual coordinates..." << endl;

    int numPowerupPositions = sizeof(POWERUP_POSITIONS) / sizeof(POWERUP_POSITIONS[0]);
    for (int i = 0; i < numPowerupPositions; i++) {
        Vector2 powerupMapPos = POWERUP_POSITIONS[i];

        // Round coordinates to integers
        int mapX = round(powerupMapPos.x);
        int mapZ = round(powerupMapPos.y);

        // Check if this position is valid (within bounds and on a path)
        if (mapX >= 0 && mapX < resources.mapWidth && mapZ >= 0 && mapZ < resources.mapHeight) {

            // Check if it's a path pixel (black = 0)
            if (resources.mapPixels[mapZ * resources.mapWidth + mapX].r == 0) {
                Vector3 worldPos = {
                    (resources.mapModelPosition.x + mapX) + 0.5,
                    0.25, // Start at ground level (was 0.5)
                    (resources.mapModelPosition.z + mapZ) + 0.5
                };

                // Randomly select a power-up type
                PropType powerupType;
                int randomPowerup = GetRandomValue(0, 2);
                switch (randomPowerup) {
                case 0: powerupType = PROP_LIGHTNING; break;
                case 1: powerupType = PROP_GUMMYBEAR; break;
                case 2: powerupType = PROP_MAGNET; break;
                default: powerupType = PROP_LIGHTNING; break;
                }

                Prop powerup;
                powerup.position = worldPos;
                powerup.type = powerupType;
                powerup.rotationY = (float)GetRandomValue(0, 360);
                powerup.bobOffset = (float)GetRandomValue(0, 628) / 100;
                powerup.collected = false;
                powerup.beingAttracted = false;

                gameProps.push_back(powerup);
            }
        }
    }

    // Then, spawn fruits at predefined coordinates
    int numFruitPositions = sizeof(FRUIT_POSITIONS) / sizeof(FRUIT_POSITIONS[0]);
    for (int i = 0; i < numFruitPositions; i++) {
        Vector2 fruitMapPos = FRUIT_POSITIONS[i];

        // Round coordinates to integers
        int mapX = round(fruitMapPos.x);
        int mapZ = round(fruitMapPos.y);

        // Check if this position is valid (within bounds and on a path)
        if (mapX >= 0 && mapX < resources.mapWidth &&
            mapZ >= 0 && mapZ < resources.mapHeight) {

            // Check if it's a path pixel (black = 0)
            if (resources.mapPixels[mapZ * resources.mapWidth + mapX].r == 0) {
                Vector3 worldPos = {
                    (float)(resources.mapModelPosition.x + mapX) + 0.5, // Center in the grid cell
                    0.15, // Start at ground level (was 0.3)
                    (float)(resources.mapModelPosition.z + mapZ) + 0.5  // Center in the grid cell
                };

                // Randomly select a fruit type
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
                fruit.rotationY = (float)GetRandomValue(0, 360);
                fruit.bobOffset = (float)GetRandomValue(0, 628) / 100;
                fruit.collected = false;
                fruit.beingAttracted = false;

                gameProps.push_back(fruit);
            }
        }
    }

    cout << "Spawned " << gameProps.size() << " props in the maze (" << numFruitPositions << " fruit positions checked, " << numPowerupPositions << " power-up positions checked)" << endl;
}

// Function to check if player is out of bounds
static bool IsPlayerOutOfBounds(Vector3 playerPos, const FruityMazeGameResources& resources) {
    if (!resources.mapPixels || !resources.mapLoaded)
        return false;

    // Convert world position to map coordinates
    int mapX = round(playerPos.x - resources.mapModelPosition.x);
    int mapZ = round(playerPos.z - resources.mapModelPosition.z);

    if (mapX < 0 || mapX >= resources.mapWidth || mapZ < 0 || mapZ >= resources.mapHeight)
        return true;

    return false;
}

// Function to add score and handle time bonus
static void AddScore(int points, const FruityMazeGameResources& resources) {
    int oldScore = gameState.score;
    gameState.score += points;

    // Check for time bonus (every 75 points)
    int oldBonuses = oldScore / 75;
    int newBonuses = gameState.score / 75;

    if (newBonuses > oldBonuses) {
        if (resources.timeExtendedSoundLoaded) PlaySound(resources.timeExtendedSound);

        gameState.gameTimer += 5;
        cout << "Time bonus! +5 seconds for reaching " << (newBonuses * 75) << " points!" << endl;
    }
}

// Function to update game timer and handle tick sounds
static void UpdateGameTimer(float deltaTime, const FruityMazeGameResources& resources) {
    if (gameState.gameOver) {
        gameState.gameOverTimer += deltaTime;
        return;
    }

    gameState.gameTimer -= deltaTime;

    // Handle tick sounds in last 10 seconds
    if (gameState.gameTimer <= 10 && gameState.gameTimer > 0) {
        int currentSecond = ceil(gameState.gameTimer);

        // Play tick sound when second changes
        if (currentSecond != gameState.lastTickSecond && currentSecond <= 10) {
            gameState.lastTickSecond = currentSecond;

            if (resources.timerPingSoundLoaded) PlaySound(resources.timerPingSound);
        }
    }

    // Game over when timer reaches 0
    if (gameState.gameTimer <= 0) {
        gameState.gameOver = true;
        gameState.gameTimer = 0;
        gameState.gameOverTimer = 0;
        cout << "Time's up! Final score: " << gameState.score << endl;
    }
}

static void UpdatePowerUps(float deltaTime, const FruityMazeGameResources& resources) {
    if (powerUps.lightningActive) {
        powerUps.lightningTimeLeft -= deltaTime;
        if (powerUps.lightningTimeLeft <= 0) {
            powerUps.lightningTimeLeft = 0;
            powerUps.lightningActive = false;
            cout << "Lightning speed boost ended!" << endl;
        }
    }

    // Update gummybear power-up
    if (powerUps.gummybearActive) {
        powerUps.gummybearTimeLeft -= deltaTime;
        if (powerUps.gummybearTimeLeft <= 0) {
            powerUps.gummybearTimeLeft = 0;
            powerUps.gummybearActive = false;
            cout << "Gummybear noclip ended!" << endl;

            if (IsColliding(playerPosition, resources.mapModelPosition, resources.mapImage, resources.mapPixels, playerRotationAngle)) {
                gameState.temporaryNoclip = true;
                cout << "Player stuck in wall! Activating emergency noclip..." << endl;
            }
        }
    }

    if (gameState.temporaryNoclip) {
        if (!IsColliding(playerPosition, resources.mapModelPosition, resources.mapImage, resources.mapPixels, playerRotationAngle)) {
            gameState.temporaryNoclip = false;
            cout << "Player freed from wall! Emergency noclip disabled." << endl;
        }
    }

    // Update magnet power-up
    if (powerUps.magnetActive) {
        powerUps.magnetTimeLeft -= deltaTime;
        if (powerUps.magnetTimeLeft <= 0) {
            powerUps.magnetTimeLeft = 0;
            powerUps.magnetActive = false;
            cout << "Magnet effect ended! Props continue being attracted until collected." << endl;
            // Note: We don't reset prop.beingAttracted here - they continue until collected
        }
    }
}

// Function to update prop animations and magnet attraction
static void UpdateProps(float deltaTime) {
    for (auto& prop : gameProps) {
        if (!prop.collected) {
            prop.rotationY += propRotationSpeed * deltaTime;
            if (prop.rotationY >= 360) prop.rotationY -= 360;

            prop.bobOffset += propBobSpeed * deltaTime;

            // **Enhanced magnet logic with natural pickup animation**
            if (prop.type != PROP_MAGNET) {
                float distanceToPlayer = Vector3Distance(prop.position, playerPosition);

                // **Natural pickup magnet effect - very close range for smooth collection**
                const float naturalPickupRange = 0.8f; // Small range for natural pickup animation
                const float naturalPickupSpeed = 3.0f; // Faster speed for pickup animation

                // Start natural pickup attraction when very close
                if (distanceToPlayer <= naturalPickupRange) {
                    Vector3 directionToPlayer = Vector3Normalize(Vector3Subtract(playerPosition, prop.position));
                    Vector3 naturalAttraction = Vector3Scale(directionToPlayer, naturalPickupSpeed * deltaTime);
                    prop.position = Vector3Add(prop.position, naturalAttraction);
                }
                // **Magnet power-up effect - larger range, slower speed**
                else if (powerUps.magnetActive && distanceToPlayer <= magnetRange) {
                    prop.beingAttracted = true;
                }

                // Continue attraction if prop is already being attracted by magnet power-up
                if (prop.beingAttracted) {
                    Vector3 directionToPlayer = Vector3Normalize(Vector3Subtract(playerPosition, prop.position));
                    Vector3 attraction = Vector3Scale(directionToPlayer, magnetPropSpeed * deltaTime);
                    prop.position = Vector3Add(prop.position, attraction);

                    // Props continue being attracted until collected, even after magnet ends
                }
            }
        }
    }
}

// **MODIFIED Function to activate power-ups - Now adds time instead of hard-setting**
static void ActivatePowerUp(PropType powerUpType, const FruityMazeGameResources& resources) {
    switch (powerUpType) {
    case PROP_LIGHTNING:
        if (powerUps.lightningActive) {
            // Add time to existing effect
            powerUps.lightningTimeLeft += lightningDuration;
            cout << "Lightning speed boost extended! (+" << lightningDuration << "s, total: " << powerUps.lightningTimeLeft << "s)" << endl;
        }
        else {
            // Start new effect
            powerUps.lightningActive = true;
            powerUps.lightningTimeLeft = lightningDuration;
            cout << "Lightning speed boost activated! (" << lightningDuration << "s)" << endl;
        }
        break;

    case PROP_GUMMYBEAR:
        if (powerUps.gummybearActive) {
            // Add time to existing effect
            powerUps.gummybearTimeLeft += gummybearDuration;
            cout << "Gummybear noclip extended! (+" << gummybearDuration << "s, total: " << powerUps.gummybearTimeLeft << "s)" << endl;
        }
        else {
            // Start new effect
            powerUps.gummybearActive = true;
            powerUps.gummybearTimeLeft = gummybearDuration;
            cout << "Gummybear noclip activated! (" << gummybearDuration << "s)" << endl;
        }
        break;

    case PROP_MAGNET:
        if (powerUps.magnetActive) {
            // Add time to existing effect
            powerUps.magnetTimeLeft += magnetDuration;
            cout << "Magnet attraction extended! (+" << magnetDuration << "s, total: " << powerUps.magnetTimeLeft << "s)" << endl;
        }
        else {
            // Start new effect
            powerUps.magnetActive = true;
            powerUps.magnetTimeLeft = magnetDuration;
            cout << "Magnet attraction activated! (" << magnetDuration << "s)" << endl;
        }
        break;

    default: break;
    }

    if (resources.powerUpSoundLoaded) PlaySound(resources.powerUpSound);
}

// Function to get current player speed (with power-up modifiers)
static float GetCurrentPlayerSpeed() {
    float baseSpeed = playerMoveSpeed;
    if (powerUps.lightningActive) baseSpeed *= lightningSpeedMultiplier;

    return baseSpeed;
}

// **Function to check if all fruits are collected (for win condition)**
static bool AreAllFruitsCollected() {
    for (const auto& prop : gameProps) {
        if (!prop.collected && (prop.type == PROP_CHERRY || prop.type == PROP_ORANGE || prop.type == PROP_GRAPES)) {
            return false; // Found an uncollected fruit
        }
    }
    return true; // All fruits collected
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
                // **Reset attraction state when collected (safety cleanup)**
                prop.beingAttracted = false;

                if (prop.type == PROP_LIGHTNING || prop.type == PROP_GUMMYBEAR || prop.type == PROP_MAGNET)
                    ActivatePowerUp(prop.type, resources);
                else {
                    AddScore(5, resources);

                    switch (prop.type) {
                    case PROP_ORANGE:
                        if (resources.fruit1SoundLoaded) PlaySound(resources.fruit1Sound);
                        break;

                    case PROP_CHERRY:
                        if (resources.fruit2SoundLoaded) PlaySound(resources.fruit2Sound);
                        break;

                    case PROP_GRAPES:
                        if (resources.fruit3SoundLoaded) PlaySound(resources.fruit3Sound);
                        break;

                    default: break;
                    }
                    // **Check for win condition after collecting a fruit**
                    if (AreAllFruitsCollected() && !gameState.gameOver) {
                        gameState.gameWon = true;
                        gameState.gameOver = true;
                        gameState.gameOverTimer = 0;
                        cout << "Congratulations! All fruits collected! You win! Final score: " << gameState.score << endl;
                    }
                }
                cout << "Collected prop at (" << prop.position.x << ", " << prop.position.z << ") | Score: " << gameState.score << endl;
            }
        }
    }
}

// Function to draw props with frustum culling optimization
static void DrawProps(const FruityMazeGameResources& resources, float time, const Camera& camera, bool debugMode = false) {
    Vector3 cameraPos = camera.position;
    Vector3 cameraTarget = camera.target;
    Vector3 forward = Vector3Normalize(Vector3Subtract(cameraTarget, cameraPos));

    int propsDrawn = 0;
    int propsCulled = 0;

    for (const auto& prop : gameProps) {
        if (!prop.collected) {
            // Calculate distance from camera to prop
            float distanceToCamera = Vector3Distance(cameraPos, prop.position);

            // Simple frustum culling - check if prop is roughly in front of camera
            Vector3 toProp = Vector3Normalize(Vector3Subtract(prop.position, cameraPos));
            float dot = Vector3DotProduct(forward, toProp);

            // If dot product is negative, prop is behind camera (cull it)
            // Allow some tolerance for props slightly behind camera
            if (dot < -0.3) {
                propsCulled++;
                continue;
            }

            Vector3 drawPosition = prop.position;

            if (!prop.beingAttracted)
                drawPosition.y += sinf(time * propBobSpeed + prop.bobOffset) * propBobHeight;

            Model* modelToDraw = nullptr;
            bool modelAvailable = false;

            // Select the appropriate model
            switch (prop.type) {
            case PROP_CHERRY:
                if (resources.cherryLoaded) {
                    modelAvailable = true;
                    modelToDraw = (Model*)&resources.cherryModel;
                }
                break;
            case PROP_ORANGE:
                if (resources.orangeLoaded) {
                    modelToDraw = (Model*)&resources.orangeModel;
                    modelAvailable = true;
                }
                break;
            case PROP_GRAPES:
                if (resources.grapesLoaded) {
                    modelToDraw = (Model*)&resources.grapesModel;
                    modelAvailable = true;
                }
                break;
            case PROP_LIGHTNING:
                if (resources.lightningLoaded) {
                    modelAvailable = true;
                    modelToDraw = (Model*)&resources.lightningModel;
                }
                break;
            case PROP_GUMMYBEAR:
                if (resources.gummybearLoaded) {
                    modelToDraw = (Model*)&resources.gummybearModel;
                    modelAvailable = true;
                }
                break;
            case PROP_MAGNET:
                if (resources.magnetLoaded) {
                    modelToDraw = (Model*)&resources.magnetModel;
                    modelAvailable = true;
                }
                break;
            }

            // Draw the model if available, otherwise draw a placeholder
            if (modelAvailable && modelToDraw)
                DrawModelEx(*modelToDraw, drawPosition, { 0, 1, 0 }, prop.rotationY, { propScale, propScale, propScale }, WHITE);
            else {
                // Draw a simple cube as placeholder
                Color propColor = RED;
                switch (prop.type) {
                case PROP_CHERRY: propColor = RED; break;
                case PROP_ORANGE: propColor = ORANGE; break;
                case PROP_GRAPES: propColor = PURPLE; break;
                case PROP_LIGHTNING: propColor = YELLOW; break;
                case PROP_GUMMYBEAR: propColor = GREEN; break;
                case PROP_MAGNET: propColor = GRAY; break;
                }

                DrawCube(drawPosition, propScale, propScale, propScale, propColor);
            }
            propsDrawn++;
        }
    }
}

int runFruityMaze(GraphicsQuality quality, Shader postProcessingShader, bool applyShader) {
    cout << "Starting Fruity 3D Maze | Quality: " << quality << " | Shader Active: " << applyShader << endl;

    SetTraceLogLevel(LOG_WARNING);
    HideCursor();

    // Initialize power-ups and game state
    powerUps = { false, 0, false, 0, false, 0 };
    gameState = { 60, 0, false, 0, false, -1, false, false };

    // Load game resources
    FruityMazeGameResources resources = LoadFruityMazeResources(quality);
    if (!CheckFruityMazeResourcesLoaded(resources)) {
        UnloadFruityMazeResources(resources);
        ShowCursor();
        return 1;
    }

    // Create render texture for the game
    RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    if (target.id == 0) {
        UnloadFruityMazeResources(resources);
        ShowCursor();
        return 1;
    }

    // Setup shader if needed
    int shaderTimeLoc = -1;
    int shaderResolutionLoc = -1;
    if (applyShader && postProcessingShader.id > 0) {
        shaderTimeLoc = GetShaderLocation(postProcessingShader, "time");
        shaderResolutionLoc = GetShaderLocation(postProcessingShader, "resolution");
        if (shaderResolutionLoc != -1) {
            float gameResolution[2] = { virtualScreenWidth, virtualScreenHeight };
            SetShaderValue(postProcessingShader, shaderResolutionLoc, gameResolution, SHADER_UNIFORM_VEC2);
        }
    }

    // Initialize camera
    Camera camera = { 0 };
    camera.up = { 0, 1, 0 };
    camera.projection = CAMERA_PERSPECTIVE;
    camera.fovy = 60;

    ViewCameraMode currentCameraMode = VIEW_CAMERA_THIRD_PERSON;

    // Find spawn point
    bool startFound = false;
    Vector3 initialSpawnPoint = { 0 };

    if (resources.pathPixelCount > 0 && resources.mapPixels) {
        vector<Vector3> potentialSpawnPoints;

        // Find all safe spawn points
        for (int i = 0; i < resources.mapWidth * resources.mapHeight; i++) {
            int x = i % resources.mapWidth;
            int z = i / resources.mapWidth;

            if (resources.mapPixels[i].r == 0) {
                Vector3 safePoint = { resources.mapModelPosition.x + x, 0, resources.mapModelPosition.z + z };

                if (!IsColliding(safePoint, resources.mapModelPosition, resources.mapImage, resources.mapPixels, 0))
                    potentialSpawnPoints.push_back(safePoint);
            }
        }

        if (!potentialSpawnPoints.empty()) {
            // Use random safe spawn point
            int randomIndex = GetRandomValue(0, potentialSpawnPoints.size() - 1);
            initialSpawnPoint = potentialSpawnPoints[randomIndex];
            startFound = true;
            TraceLog(LOG_INFO, "Found %d potential safe spawn points. Selected one at random.", potentialSpawnPoints.size());
        }
    }

    if (startFound) {
        playerRotationAngle = 0;
        playerPosition = initialSpawnPoint;
        if (!resources.playerModelLoaded)
            currentCameraMode = VIEW_CAMERA_FIRST_PERSON;

        SpawnPropsInMaze(resources);
    }
    else {
        camera.position = { resources.mapModelPosition.x + resources.mapWidth / 2, 15, resources.mapModelPosition.z + resources.mapHeight / 2 - 10 };
        camera.target = { resources.mapModelPosition.x + resources.mapWidth / 2, 0, resources.mapModelPosition.z + resources.mapHeight / 2 };
    }

    SetTargetFPS(60);

    bool debug = false;

    if (resources.backgroundMusicLoaded) {
        PlayMusicStream(resources.backgroundMusic);
        SetMusicVolume(resources.backgroundMusic, 1);
    }

    // Main game loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // **Safer Shader Hot-Reload - Press F5 to reload shader**
        if (IsKeyPressed(KEY_F5)) {
            cout << "Manual shader reload..." << endl;

            // Try to load new shader first
            Shader newShader = LoadShader(0, shaderPath);

            if (newShader.id > 0) {
                // Only unload old shader if new one loaded successfully
                if (postProcessingShader.id > 0) {
                    UnloadShader(postProcessingShader);
                }

                // Replace with new shader
                postProcessingShader = newShader;

                // Reconfigure shader uniforms
                if (applyShader) {
                    shaderTimeLoc = GetShaderLocation(postProcessingShader, "time");
                    shaderResolutionLoc = GetShaderLocation(postProcessingShader, "resolution");
                    if (shaderResolutionLoc != -1) {
                        float gameResolution[2] = { virtualScreenWidth, virtualScreenHeight };
                        SetShaderValue(postProcessingShader, shaderResolutionLoc, gameResolution, SHADER_UNIFORM_VEC2);
                    }
                }
                cout << "Shader reloaded successfully!" << endl;
            }
            else {
                cout << "Failed to reload shader - keeping current version" << endl;
            }
        }

        if (resources.backgroundMusicLoaded) {
            UpdateMusicStream(resources.backgroundMusic);

            if (GetMusicTimePlayed(resources.backgroundMusic) >= GetMusicTimeLength(resources.backgroundMusic))
                SeekMusicStream(resources.backgroundMusic, 0);
        }

        // Update game timer
        UpdateGameTimer(dt, resources);

        // Check for out of bounds when noclipping
        if ((powerUps.gummybearActive || gameState.temporaryNoclip) && !gameState.gameOver) {
            if (IsPlayerOutOfBounds(playerPosition, resources)) {
                if (!gameState.outOfBounds) {
                    gameState.outOfBounds = true;
                    gameState.gameOver = true;
                    gameState.gameOverTimer = 0;

                    // Play bounds sound if loaded
                    if (resources.boundsSoundLoaded)
                        PlaySound(resources.boundsSound);

                    cout << "Player went out of bounds! Game Over! Final score: " << gameState.score << endl;
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
            if (currentCameraMode == VIEW_CAMERA_FIRST_PERSON && !resources.playerModelLoaded)
                currentCameraMode = VIEW_CAMERA_SECOND_PERSON;

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

                Vector3 tempPos = playerPosition;
                tempPos.x += desiredMovement.x;
                tempPos.z += desiredMovement.z;

                bool canMove = true;
                if (!powerUps.gummybearActive && !gameState.temporaryNoclip)
                    canMove = !IsColliding(tempPos, resources.mapModelPosition, resources.mapImage, resources.mapPixels, playerRotationAngle);

                if (canMove) {
                    playerPosition.x = tempPos.x;
                    playerPosition.z = tempPos.z;
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

        // Update camera based on mode
        if (startFound) {
            camera.fovy = 60;
            camera.up = { 0, 1, 0 };

            switch (currentCameraMode) {
            case VIEW_CAMERA_FIRST_PERSON:
                if (resources.playerModelLoaded) {
                    camera.fovy = 75;
                    Matrix playerMat = MatrixRotateY(playerRotationAngle * DEG2RAD);
                    Vector3 fppOffsetTransformed = Vector3Transform(FirstPersonOffset, playerMat);
                    camera.position = Vector3Add(playerPosition, fppOffsetTransformed);

                    Vector3 playerCameraForward = {
                        -sinf(playerRotationAngle * DEG2RAD),
                        0,
                        -cosf(playerRotationAngle * DEG2RAD)
                    };
                    camera.target = Vector3Add(camera.position, playerCameraForward);
                }
                else {
                    currentCameraMode = VIEW_CAMERA_SECOND_PERSON;
                    camera.position = { playerPosition.x, playerPosition.y + SecondPersonOffset.y, playerPosition.z };
                    camera.target = playerPosition;
                    camera.up = { 0, 0, 1 };
                    EnableCursor();
                }
                break;
            case VIEW_CAMERA_SECOND_PERSON:
                camera.position = { playerPosition.x, playerPosition.y + SecondPersonOffset.y, playerPosition.z };
                camera.target = playerPosition;
                camera.up = { 0, 0, 1 };
                break;
            case VIEW_CAMERA_THIRD_PERSON:
            default:
                camera.position = Vector3Add(playerPosition, ThirdPersonOffset);
                camera.target = playerPosition;
                break;
            }
        }
        else UpdateCamera(&camera, CAMERA_ORBITAL);

        // Rendering
        BeginTextureMode(target);
        ClearBackground(BLACK);
        BeginMode3D(camera);

        // Draw maze
        if (resources.mazeModel.meshCount > 0 && resources.mazeModel.meshes[0].vertexCount > 0) {
            DrawPlane({ resources.mapModelPosition.x + (resources.mapWidth / 2), 0.01, resources.mapModelPosition.z + (resources.mapHeight / 2) }, { (float)resources.mapWidth + 20, (float)resources.mapHeight + 20 }, BLACK);
            DrawModel(resources.mazeModel, resources.mapModelPosition, 1, WHITE);
        }

        // Apply shaders
        if (resources.lightingShaderLoaded) {
            // Set lighting uniforms
            Vector3 lightPos = { playerPosition.x, playerPosition.y + 10, playerPosition.z }; // Light above player
            Vector3 lightColor = { 1.0, 0.9, 0.8 }; // Warm white
            float ambientStrength = 0.3;

            SetShaderValue(resources.lightingShader, GetShaderLocation(resources.lightingShader, "lightPos"), &lightPos, SHADER_UNIFORM_VEC3);
            SetShaderValue(resources.lightingShader, GetShaderLocation(resources.lightingShader, "lightColor"), &lightColor, SHADER_UNIFORM_VEC3);
            SetShaderValue(resources.lightingShader, GetShaderLocation(resources.lightingShader, "viewPos"), &camera.position, SHADER_UNIFORM_VEC3);
            SetShaderValue(resources.lightingShader, GetShaderLocation(resources.lightingShader, "ambientStrength"), &ambientStrength, SHADER_UNIFORM_FLOAT);

            // Apply to maze model
            resources.mazeModel.materials[0].shader = resources.lightingShader;

            // Apply to player model
            if (resources.playerModelLoaded) {
                resources.playerModel.materials[0].shader = resources.lightingShader;
            }
        }

        // Draw player model
        if (startFound && resources.playerModelLoaded) {
            bool shouldDrawPlayer = true;
            if (currentCameraMode == VIEW_CAMERA_FIRST_PERSON && !debug)
                shouldDrawPlayer = false;

            if (shouldDrawPlayer) {
                Color playerTint = WHITE;

                // Apply noclip visual effect (both regular and emergency)
                if (powerUps.gummybearActive || gameState.temporaryNoclip) {
                    float pulse = (sinf(GetTime() * 8) + 1) * 0.5;
                    // Use different color for emergency noclip
                    Color noclipColor = gameState.temporaryNoclip ? ORANGE : LIME;
                    playerTint = ColorLerp(WHITE, noclipColor, pulse * 0.6);
                }

                DrawModelEx(resources.playerModel, playerPosition, { 0, 1, 0 },
                    playerRotationAngle, { playerScale, playerScale, playerScale }, playerTint);
            }
        }

        // Draw props
        DrawProps(resources, GetTime(), camera, debug);

        // Debug visualization
        if (debug && startFound) {
            if (currentCameraMode == VIEW_CAMERA_FIRST_PERSON && resources.playerModelLoaded) {
                Vector3 playerForwardViz = {
                    -sinf(playerRotationAngle * DEG2RAD),
                    0,
                    -cosf(playerRotationAngle * DEG2RAD)
                };
                Vector3 rayStart = camera.position;
                Vector3 rayEnd = Vector3Add(rayStart, Vector3Scale(playerForwardViz, 5));
                DrawLine3D(rayStart, rayEnd, RED);
            }

            // Draw collision debug info
            if (resources.mapPixels) {
                int entityCellX = roundf(playerPosition.x - resources.mapModelPosition.x);
                int entityCellZ = roundf(playerPosition.z - resources.mapModelPosition.z);

                for (int z = -1; z <= 1; z++) {
                    for (int x = -1; x <= 1; x++) {
                        int wallCheckMapX = entityCellX + x;
                        int wallCheckMapZ = entityCellZ + z;

                        if (wallCheckMapX >= 0 && wallCheckMapX < resources.mapWidth &&
                            wallCheckMapZ >= 0 && wallCheckMapZ < resources.mapHeight) {

                            DrawCubeWires({
                                resources.mapModelPosition.x + wallCheckMapX,
                                -0.45,
                                resources.mapModelPosition.z + wallCheckMapZ
                                }, 2, 0.2, 2, RED);

                            if (x == 0 && z == 0) {
                                DrawCubeWires({
                                    resources.mapModelPosition.x + wallCheckMapX,
                                    -0.4,
                                    resources.mapModelPosition.z + wallCheckMapZ
                                    }, 2, 0.2, 2, BLUE);
                            }
                        }
                    }
                }
                DrawCubeWires(playerPosition, playerHitboxWidth, 1.5, playerHitboxDepth, RED);

                // Draw magnet range when active
                if (powerUps.magnetActive)
                    DrawCylinderWires(playerPosition, magnetRange, magnetRange, 0.1, 32, BLUE);
            }
        }

        EndMode3D();

        // Draw minimap
        if (resources.minimapTexture.id > 0) {
            Vector3 posMinimap;
            if (startFound) posMinimap = playerPosition;
            else posMinimap = {
                resources.mapModelPosition.x + resources.mapWidth / 2,
                0,
                resources.mapModelPosition.z + resources.mapHeight / 2
            };

            float minimapScale = 2;
            float scaledMapWidth = resources.mapWidth * minimapScale;
            float scaledMapHeight = resources.mapHeight * minimapScale;

            Vector2 minimapVisualTopLeft = {
                virtualScreenWidth - scaledMapWidth - 10,
                10
            };
            Vector2 minimapTextureDrawPos = {
                minimapVisualTopLeft.x + scaledMapWidth,
                minimapVisualTopLeft.y + scaledMapHeight
            };

            DrawTextureEx(resources.minimapTexture, minimapTextureDrawPos, 180, minimapScale, WHITE);
            DrawRectangleLines(minimapVisualTopLeft.x, minimapVisualTopLeft.y, scaledMapWidth, scaledMapHeight, LIME);

            if (startFound) {
                float playerMapX = (posMinimap.x - resources.mapModelPosition.x + 0.5);
                float playerMapZ = (posMinimap.z - resources.mapModelPosition.z + 0.5);

                float playerRelXOnMinimap = (resources.mapWidth - playerMapX) * minimapScale;
                float playerRelZOnMinimap = (resources.mapHeight - playerMapZ) * minimapScale;

                Color playerDotColor = RED;
                if (powerUps.gummybearActive || gameState.temporaryNoclip) {
                    // Pulse the player dot when noclipping (orange for emergency, lime for regular)
                    float pulse = (sinf(GetTime() * 8) + 1) * 0.5;
                    Color noclipColor = gameState.temporaryNoclip ? ORANGE : LIME;
                    playerDotColor = ColorLerp(RED, noclipColor, pulse);
                }

                DrawCircle((minimapVisualTopLeft.x + playerRelXOnMinimap),
                    (minimapVisualTopLeft.y + playerRelZOnMinimap),
                    collisionRadius * minimapScale > 2 ? collisionRadius * minimapScale : 3, playerDotColor);

                // Draw magnet range on minimap when active
                if (powerUps.magnetActive) {
                    DrawCircleLines((minimapVisualTopLeft.x + playerRelXOnMinimap),
                        (minimapVisualTopLeft.y + playerRelZOnMinimap),
                        magnetRange * minimapScale, BLUE);
                }
            }
        }

        DrawFPS(10, 10);

        // **REPOSITIONED UI SECTION**
        if (startFound) {
            const char* camModeStr;
            switch (currentCameraMode) {
            case VIEW_CAMERA_FIRST_PERSON: camModeStr = "FPP (Mouse Look)"; break;
            case VIEW_CAMERA_SECOND_PERSON: camModeStr = "SPP"; break;
            case VIEW_CAMERA_THIRD_PERSON: camModeStr = "TPP"; break;
            default: camModeStr = "UNKNOWN"; break;
            }

            // Define consistent padding
            const int textPadding = 30;

            // **TIME - Top-left corner**
            DrawText(TextFormat("%.0f", gameState.gameTimer),
                textPadding, textPadding, UI_LARGE_FONT_SIZE, WHITE);

            // **SCORE - Bottom-right corner with label**
            const char* scoreLabel = "SCORE";
            const char* scoreValue = TextFormat("%d", gameState.score);
            int scoreLabelWidth = MeasureText(scoreLabel, UI_LARGE_FONT_SIZE);
            int scoreValueWidth = MeasureText(scoreValue, UI_LARGE_FONT_SIZE);
            int maxScoreWidth = max(scoreLabelWidth, scoreValueWidth);

            // Position from right edge
            int scoreX = virtualScreenWidth - maxScoreWidth - textPadding;
            DrawText(scoreLabel, scoreX, virtualScreenHeight - (UI_LARGE_FONT_SIZE * 2) - textPadding, UI_LARGE_FONT_SIZE, WHITE);
            DrawText(scoreValue, scoreX, virtualScreenHeight - UI_LARGE_FONT_SIZE - textPadding, UI_LARGE_FONT_SIZE, WHITE);

            // **POWER-UP TIMERS - Top-right corner in column**
            float powerUpYOffset = textPadding; // Starting position for power-up display

            // **Lightning Speed Power-up Timer**
            if (powerUps.lightningActive) {
                Color lightningColor = YELLOW;
                // Add pulsing effect when time is running low (last 3 seconds)
                if (powerUps.lightningTimeLeft <= 3.0f) {
                    float pulse = (sinf(GetTime() * 6) + 1) * 0.5f;
                    lightningColor = ColorLerp(YELLOW, RED, pulse * 0.7f);
                }

                const char* lightningText = TextFormat("%.0f", powerUps.lightningTimeLeft);
                int lightningWidth = MeasureText(lightningText, UI_LARGE_FONT_SIZE);
                DrawText(lightningText,
                    virtualScreenWidth - lightningWidth - textPadding, powerUpYOffset, UI_LARGE_FONT_SIZE, lightningColor);
                powerUpYOffset += UI_LARGE_FONT_SIZE + 10;
            }

            // **Gummybear Noclip Power-up Timer**
            if (powerUps.gummybearActive) {
                Color gummybearColor = LIME;
                // Add pulsing effect when time is running low (last 3 seconds)
                if (powerUps.gummybearTimeLeft <= 3.0f) {
                    float pulse = (sinf(GetTime() * 6) + 1) * 0.5f;
                    gummybearColor = ColorLerp(LIME, RED, pulse * 0.7f);
                }

                const char* gummybearText = TextFormat("%.0f", powerUps.gummybearTimeLeft);
                int gummybearWidth = MeasureText(gummybearText, UI_LARGE_FONT_SIZE);
                DrawText(gummybearText,
                    virtualScreenWidth - gummybearWidth - textPadding, powerUpYOffset, UI_LARGE_FONT_SIZE, gummybearColor);
                powerUpYOffset += UI_LARGE_FONT_SIZE + 10;
            }

            // **Magnet Power-up Timer**
            if (powerUps.magnetActive) {
                Color magnetColor = BLUE;
                // Add pulsing effect when time is running low (last 3 seconds)
                if (powerUps.magnetTimeLeft <= 3.0f) {
                    float pulse = (sinf(GetTime() * 6) + 1) * 0.5f;
                    magnetColor = ColorLerp(BLUE, RED, pulse * 0.7f);
                }

                const char* magnetText = TextFormat("%.0f", powerUps.magnetTimeLeft);
                int magnetWidth = MeasureText(magnetText, UI_LARGE_FONT_SIZE);
                DrawText(magnetText,
                    virtualScreenWidth - magnetWidth - textPadding, powerUpYOffset, UI_LARGE_FONT_SIZE, magnetColor);
                powerUpYOffset += UI_LARGE_FONT_SIZE + 10;
            }

            // **Emergency Noclip Status (non-timer based)**
            if (gameState.temporaryNoclip) {
                // Pulsing orange for emergency noclip
                float pulse = (sinf(GetTime() * 8) + 1) * 0.5f;
                Color emergencyColor = ColorLerp(ORANGE, RED, pulse * 0.5f);

                const char* emergencyText = "EMERGENCY";
                int emergencyWidth = MeasureText(emergencyText, UI_SMALL_FONT_SIZE);
                DrawText(emergencyText,
                    virtualScreenWidth - emergencyWidth - textPadding, powerUpYOffset, UI_SMALL_FONT_SIZE, emergencyColor);
                powerUpYOffset += UI_SMALL_FONT_SIZE + 5;
            }

            // **Camera and position info (moved to debug mode only)**
            if (debug) {
                float debugYOffset = textPadding + UI_LARGE_FONT_SIZE + 20; // Below the time display
                DrawText(TextFormat("Pos: X:%.1f Z:%.1f (%s)", playerPosition.x, playerPosition.z, camModeStr),
                    textPadding, debugYOffset, UI_SMALL_FONT_SIZE - 2, LIGHTGRAY);
                DrawText(TextFormat("Angle: %.1f | Speed: %.1f", playerRotationAngle, GetCurrentPlayerSpeed()),
                    textPadding, debugYOffset + UI_SMALL_FONT_SIZE + 2, UI_SMALL_FONT_SIZE - 2, LIGHTGRAY);
            }

            // Draw game over message
            if (gameState.gameOver) {
                const char* gameOverMsg;
                Color messageColor;

                if (gameState.gameWon) {
                    gameOverMsg = "CONGRATULATIONS! YOU WIN!";
                    messageColor = GREEN;
                }
                else if (gameState.outOfBounds) {
                    gameOverMsg = "GAME OVER - OUT OF BOUNDS!";
                    messageColor = RED;
                }
                else {
                    gameOverMsg = "GAME OVER - TIME'S UP!";
                    messageColor = RED;
                }

                int textWidth = MeasureText(gameOverMsg, UI_LARGE_FONT_SIZE);
                DrawText(gameOverMsg,
                    (virtualScreenWidth - textWidth) / 2,
                    virtualScreenHeight / 2 - 50, UI_LARGE_FONT_SIZE, messageColor);

                const char* finalScoreMsg = TextFormat("Final Score: %d", gameState.score);
                int scoreWidth = MeasureText(finalScoreMsg, UI_MEDIUM_FONT_SIZE);
                DrawText(finalScoreMsg,
                    (virtualScreenWidth - scoreWidth) / 2,
                    virtualScreenHeight / 2 - 10, UI_MEDIUM_FONT_SIZE, YELLOW);

                float timeRemaining = (gameState.gameWon ? 3 : 4) - gameState.gameOverTimer;
                const char* returnMsg = TextFormat("Returning to menu in %.1fs...", timeRemaining);
                int returnWidth = MeasureText(returnMsg, UI_SMALL_FONT_SIZE);
                DrawText(returnMsg,
                    (virtualScreenWidth - returnWidth) / 2,
                    virtualScreenHeight / 2 + 20, UI_SMALL_FONT_SIZE, LIGHTGRAY);
            }

            // Controls help text (bottom center, with safe margin from shader border)
            const char* controlsText;
            if (debug) {
                controlsText = "ESC: Return to Menu | T: Switch Camera | F3: Debug | F5: Reload Shader | L: Speed | G: Noclip | M: Magnet";
            }
            else {
                controlsText = "ESC: Return to Menu | T: Switch Camera | F3: Debug | F5: Reload Shader";
            }
            int controlsWidth = MeasureText(controlsText, 12);
            // Position with safe margins from both sides to avoid shader border
            int safeControlsX = Clamp((virtualScreenWidth - controlsWidth) / 2, 40, virtualScreenWidth - controlsWidth - 40);
            DrawText(controlsText, safeControlsX, virtualScreenHeight - 40, 12, LIGHTGRAY);
        }
        else {
            DrawText("CANNOT START GAME. Check console/map. Orbiting...", 10, 40, 20, RED);
        }

        EndTextureMode();

        // Final rendering with optional shader
        BeginDrawing();
        ClearBackground(BLACK);

        float finalScale = min((float)GetScreenWidth() / virtualScreenWidth, (float)GetScreenHeight() / virtualScreenHeight);
        float finalOffsetX = ((float)GetScreenWidth() - (virtualScreenWidth * finalScale)) / 2;
        float finalOffsetY = ((float)GetScreenHeight() - (virtualScreenHeight * finalScale)) / 2;

        Rectangle src = { 0, 0, (float)target.texture.width, -(float)target.texture.height };
        Rectangle dst = { finalOffsetX, finalOffsetY, virtualScreenWidth * finalScale, virtualScreenHeight * finalScale };

        if (applyShader && postProcessingShader.id > 0) {
            if (shaderTimeLoc != -1) {
                float currentTime = GetTime();
                SetShaderValue(postProcessingShader, shaderTimeLoc, &currentTime, SHADER_UNIFORM_FLOAT);
            }
            BeginShaderMode(postProcessingShader);
            DrawTexturePro(target.texture, src, dst, { 0, 0 }, 0, WHITE);
            EndShaderMode();
        }
        else DrawTexturePro(target.texture, src, dst, { 0, 0 }, 0, WHITE);

        EndDrawing();
    }

    // Cleanup
    if (resources.backgroundMusicLoaded && IsMusicStreamPlaying(resources.backgroundMusic))
        StopMusicStream(resources.backgroundMusic);

    EnableCursor();
    UnloadRenderTexture(target);
    UnloadFruityMazeResources(resources);

    cout << "Fruity Maze ended." << endl;
    return 0;
}