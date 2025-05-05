#include "raylib.h"
#include "raymath.h"
#include "MotoristResources.h"
#include "GameSettings.h"
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>

// Domyœlne wymiary t³a dla obliczania proporcji
const float defaultBackgroundWidth = 1920.0f;
const float defaultBackgroundHeight = 1080.0f;

// --- Konfiguracja poziomu Midnight Motorist ---
const float virtualScreenWidth = 960.0f;
const float virtualScreenHeight = 540.0f;
const float motoristLevelLogicalWidth = 5000.0f; // (Not currently used for width limit)
const float motoristRoadMargin = 25.0f; // Top/bottom margin for road appearance (visual only)
const float motoristRoadYTop = motoristRoadMargin;
const float motoristRoadYBottom = virtualScreenHeight - motoristRoadMargin;
const float motoristPlayerSpeed = 5.0f;
const float npcSpeed = 3.5f;
const float npcSpawnRate = 3.75f; // Adjusted spawn rate, might need tuning for 8 lanes
const float minNpcSpacing = 30.0f; // Adjusted minimum spacing, might need tuning
float backgroundScrollMultiplier = 3.0f; // How much faster background scrolls than player base speed

// --- 8 Lane Definitions (Updated based on user input) ---
const float laneCenterY1Ratio = 95.0f / defaultBackgroundHeight;
const float laneCenterY2Ratio = 191.0f / defaultBackgroundHeight;
const float laneCenterY3Ratio = 349.0f / defaultBackgroundHeight;
const float laneCenterY4Ratio = 455.0f / defaultBackgroundHeight;
const float laneCenterY5Ratio = 611.0f / defaultBackgroundHeight;
const float laneCenterY6Ratio = 717.0f / defaultBackgroundHeight;
const float laneCenterY7Ratio = 877.0f / defaultBackgroundHeight;
const float laneCenterY8Ratio = 981.0f / defaultBackgroundHeight;

// --- NPC Spawn Configuration for 8 Lanes ---
std::vector<float> npcSpawnYPositions = {
    virtualScreenHeight * laneCenterY1Ratio,
    virtualScreenHeight * laneCenterY2Ratio,
    virtualScreenHeight * laneCenterY3Ratio,
    virtualScreenHeight * laneCenterY4Ratio,
    virtualScreenHeight * laneCenterY5Ratio,
    virtualScreenHeight * laneCenterY6Ratio,
    virtualScreenHeight * laneCenterY7Ratio,
    virtualScreenHeight * laneCenterY8Ratio
};
// Distribution to pick one of the 8 lanes (indices 0-7)
std::uniform_int_distribution<> spawnYDist(0, npcSpawnYPositions.size() - 1);

// Struktura dla samochodów NPC
struct Car {
    Vector2 position;
    float speed;
    bool movingLeft;
    int textureIndex; // 0 = right, 1 = left (or based on resource loading)
    Rectangle collisionBox;
};

// --- Main Function for the Midnight Motorist Game ---
int runMidnightMotorist(GraphicsQuality quality, Shader postProcessingShader, bool applyShader) {
    std::cout << "Starting Midnight Motorist | Quality: " << quality << " | Shader Active: " << applyShader << std::endl;

    // --- Load Game-Specific Resources ---
    MotoristGameResources resources = LoadMotoristResources(quality);
    if (!resources.car.id > 0 || !resources.leftCarNPC.id > 0 || !resources.rightCarNPC.id > 0 || !resources.bg.id > 0) {
        TraceLog(LOG_ERROR, "MOTORIST: Failed to load essential textures. Aborting.");
        // Maybe unload partially loaded resources here if necessary
        return 1; // Indicate error
    }


    // --- Setup Render Target ---
    RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    if (target.id == 0) {
        TraceLog(LOG_ERROR, "MOTORIST: Failed to create render texture. Aborting.");
        // Unload resources if needed
        return 1; // Indicate error
    }
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    // --- Setup Shader (If Applicable) ---
    int shaderTimeLoc = -1;
    int shaderResolutionLoc = -1;
    if (applyShader && postProcessingShader.id > 0) {
        shaderTimeLoc = GetShaderLocation(postProcessingShader, "time");
        shaderResolutionLoc = GetShaderLocation(postProcessingShader, "resolution");
        if (shaderResolutionLoc != -1) {
            float gameResolution[2] = { (float)virtualScreenWidth, (float)virtualScreenHeight };
            SetShaderValue(postProcessingShader, shaderResolutionLoc, gameResolution, SHADER_UNIFORM_VEC2);
        }
        else {
            TraceLog(LOG_WARNING, "MOTORIST: Shader 'resolution' uniform not found.");
        }
        if (shaderTimeLoc == -1) {
            TraceLog(LOG_WARNING, "MOTORIST: Shader 'time' uniform not found.");
        }
    }

    // --- Game Initialization ---
    // Player Dimensions (Original intended size and scaling based on fitting background)
    const float playerScaleFactor = 0.75f; // <-- NEW scaling factor for Player
    const float defaultPlayerWidth = 160.0f;
    const float defaultPlayerHeight = 131.0f;
    // Calculate base scale to fit default background height to virtual screen height
    const float baseScaleY = (float)virtualScreenHeight / defaultBackgroundHeight;
    const float baseScaleX = baseScaleY; // Assuming aspect ratio of elements is maintained
    // Final player dimensions based on base scale
    const float playerTextureWidth = defaultPlayerWidth * baseScaleX * playerScaleFactor;
    const float playerTextureHeight = defaultPlayerHeight * baseScaleY * playerScaleFactor;
    const float playerHalfWidth = playerTextureWidth / 2.0f;
    const float playerHalfHeight = playerTextureHeight / 2.0f;
    Rectangle playerRect; // Player collision rectangle

    // NPC Dimensions (Apply 50% scaling *in addition* to base scaling)
    const float npcScaleFactor = 0.75f; // <-- NEW scaling factor for NPCs
    const float defaultNpcWidth = 160.0f; // Assuming same base size as player, adjust if different
    const float defaultNpcHeight = 131.0f;
    // Final NPC dimensions using base scale AND the new npcScaleFactor
    const float npcTextureWidth = defaultNpcWidth * baseScaleX * npcScaleFactor;
    const float npcTextureHeight = defaultNpcHeight * baseScaleY * npcScaleFactor;
    const float npcHalfWidth = npcTextureWidth / 2.0f;
    const float npcHalfHeight = npcTextureHeight / 2.0f;
    // Collision margins (absolute values, adjust if they feel too large/small on scaled NPCs)
    const float npcCollisionMarginX = 10.0f * npcScaleFactor; // Scale margin roughly too
    const float npcCollisionMarginY = 5.0f * npcScaleFactor;

    // Game State Variables
    Vector2 motoristPlayerPos = { virtualScreenWidth * 0.1f, (motoristRoadYTop + motoristRoadYBottom) / 2.0f };
    Vector2 motoristPlayerVel = { 0.0f, 0.0f };
    float motoristVirtualSpeed = motoristPlayerSpeed; // Controls background scroll speed
    int playerLives = 5; // Player Lives

    std::vector<Car> motoristCars; // List of active NPC cars
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> spawnChanceDist(0.0f, 1.5f); // For spawn probability check

    float motoristBgScroll = 0.0f; // Background horizontal offset
    float npcSpawnTimer = 0.0f; // Timer for NPC spawning frequency

    bool motoristGameOver = false;
    bool showDebugInfo = false;
    bool collisionDetectedThisFrame = false; // Track collision per frame
    float accelerationRate = 25.0f; // Rate at which virtual speed increases

    // Player Spin Animation State
    bool isSpinning = false;
    int spinFrame = 0;
    float spinTimer = 0.0f;
    float spinDurationPerFrame = 0.05f;

    // Start Background Music
    if (resources.backgroundMusicLoaded) {
        PlayMusicStream(resources.backgroundMusic);
        SetMusicVolume(resources.backgroundMusic, 0.5f); // Set desired volume level
    }

    // --------------- MAIN GAME LOOP (Midnight Motorist) ---------------
    while (!WindowShouldClose()) {

        // --- Update Music Stream ---
        if (resources.backgroundMusicLoaded) {
            UpdateMusicStream(resources.backgroundMusic);
            // Simple loop check
            if (GetMusicTimePlayed(resources.backgroundMusic) >= GetMusicTimeLength(resources.backgroundMusic) - 0.1f) {
                SeekMusicStream(resources.backgroundMusic, 0.0f);
            }
        }

        // --- Input Handling ---
        if (IsKeyPressed(KEY_F3)) showDebugInfo = !showDebugInfo;
        if (IsKeyPressed(KEY_ESCAPE)) {
            break; // Exit the Midnight Motorist game loop, return to launcher
        }

        // --- Game Logic Update ---
        if (!motoristGameOver) {
            // --- Player Movement ---
            motoristPlayerVel = { 0.0f, 0.0f }; // Reset velocity each frame
            if (IsKeyDown(KEY_A)) motoristPlayerVel.x = -motoristPlayerSpeed;
            if (IsKeyDown(KEY_D)) motoristPlayerVel.x = motoristPlayerSpeed;
            if (IsKeyDown(KEY_W)) motoristPlayerVel.y = -motoristPlayerSpeed;
            if (IsKeyDown(KEY_S)) motoristPlayerVel.y = motoristPlayerSpeed;

            motoristPlayerPos.x += motoristPlayerVel.x;
            motoristPlayerPos.y += motoristPlayerVel.y;

            // Clamp player position within screen bounds (considering player size)
            motoristPlayerPos.x = Clamp(motoristPlayerPos.x, playerHalfWidth, virtualScreenWidth - playerHalfWidth);
            // Clamp player Y position slightly more restrictively than road edges to prevent edge clipping
            motoristPlayerPos.y = Clamp(motoristPlayerPos.y, motoristRoadYTop + playerHalfHeight * 0.8f, motoristRoadYBottom - playerHalfHeight * 0.8f);


            // Update player collision rectangle
            playerRect = { motoristPlayerPos.x - playerHalfWidth, motoristPlayerPos.y - playerHalfHeight, playerTextureWidth, playerTextureHeight };

            // --- Collision Detection ---
            collisionDetectedThisFrame = false;
            for (auto& car : motoristCars) {
                if (CheckCollisionRecs(playerRect, car.collisionBox)) {
                    collisionDetectedThisFrame = true; // Mark collision happened this frame
                    if (!isSpinning) { // Start spinning only on the first collision detection
                        isSpinning = true;
                        spinFrame = 0;
                        spinTimer = 0.0f;
                        motoristVirtualSpeed = motoristPlayerSpeed; // Reset scroll speed on collision
                        if (resources.carCrashSoundLoaded) 
                            PlaySound(resources.carCrash);

                        playerLives--;
                    }
                    // Don't break here if you want to detect multiple collisions per frame (though unlikely useful)
                }
            }

            // --- Update Virtual Speed (Background Scroll) ---
            if (!isSpinning) {
                motoristVirtualSpeed += accelerationRate * GetFrameTime();
                motoristVirtualSpeed = Clamp(motoristVirtualSpeed, motoristPlayerSpeed, motoristPlayerSpeed * 150.0f); // Added upper limit
            }
            else {
                motoristVirtualSpeed = motoristPlayerSpeed; // Maintain base speed during spin
            }

            // --- NPC Spawning (8 Lanes) ---
            npcSpawnTimer += GetFrameTime();
            if (npcSpawnTimer >= (1.0f / npcSpawnRate)) {
                npcSpawnTimer = 0.0f; // Reset timer

                // Spawn based on probability (e.g., 75% chance per interval)
                if (spawnChanceDist(gen) < 0.9f) {
                    Car newCar;
                    int randomLaneIndex = spawnYDist(gen); // Pick one of the 8 lanes (0-7)
                    newCar.position.y = npcSpawnYPositions[randomLaneIndex];
                    newCar.speed = npcSpeed; // Constant speed for now

                    // Determine direction based on lane index (top 4 vs bottom 4)
                    if (randomLaneIndex < 4) { // Lanes 0, 1, 2, 3 move LEFT
                        newCar.movingLeft = true;
                        newCar.textureIndex = 1; // Assuming index 1 is left-facing texture
                        newCar.position.x = virtualScreenWidth + npcTextureWidth; // Spawn off-screen right
                    }
                    else { // Lanes 4, 5, 6, 7 move RIGHT
                        newCar.movingLeft = false;
                        newCar.textureIndex = 0; // Assuming index 0 is right-facing texture
                        newCar.position.x = -npcTextureWidth; // Spawn off-screen left
                    }

                    // Check minimum spacing with existing cars ON THE SAME LANE AND DIRECTION
                    bool canSpawn = true;
                    for (const auto& existingCar : motoristCars) {
                        // Check if on the same Y position (within a small tolerance) and moving same direction
                        if (std::abs(existingCar.position.y - newCar.position.y) < 1.0f && existingCar.movingLeft == newCar.movingLeft) {
                            // Calculate distance based on direction
                            float distance = newCar.movingLeft ?
                                (newCar.position.x - existingCar.position.x) : // If new car moves left, check if it's behind existing
                                (existingCar.position.x - newCar.position.x);  // If new car moves right, check if existing is behind

                            // Check if the new car would spawn too close *behind* an existing one
                             // Use the scaled npcTextureWidth here
                            if (distance > 0 && distance < (npcTextureWidth + minNpcSpacing)) {
                                canSpawn = false;
                                break; // No need to check further cars on this lane
                            }
                        }
                    }

                    // If spacing is okay, add the new car
                    if (canSpawn) {
                        // Define collision box based on the *final* scaled size and margins
                        newCar.collisionBox = {
                            newCar.position.x - npcHalfWidth + npcCollisionMarginX,
                            newCar.position.y - npcHalfHeight + npcCollisionMarginY,
                            npcTextureWidth - 2 * npcCollisionMarginX,
                            npcTextureHeight - 2 * npcCollisionMarginY
                        };
                        motoristCars.push_back(newCar);
                    }
                }
            }

            // --- Update NPC Positions & Collision Boxes ---
            for (auto& car : motoristCars) {
                // Move car based on direction and speed (constant speed)
                car.position.x += car.movingLeft ? -(car.speed) : (car.speed);
                // Update the collision box X position to follow the car
                car.collisionBox.x = car.position.x - npcHalfWidth + npcCollisionMarginX;
            }

            // --- Remove Off-Screen NPCs ---
            motoristCars.erase(std::remove_if(motoristCars.begin(), motoristCars.end(), [&](const Car& car) {
                // Remove if well past the screen edge (using scaled width)
                return (car.movingLeft && car.position.x < -npcTextureWidth * 1.5f) || (!car.movingLeft && car.position.x > virtualScreenWidth + npcTextureWidth * 1.5f);
                }), motoristCars.end());

            // --- Update Background Scroll ---
            motoristBgScroll -= motoristVirtualSpeed * backgroundScrollMultiplier * GetFrameTime();
            // Wrap background scroll based on scaled background width
            if (resources.bg.id > 0) {
                float scaledBgWidth = (float)resources.bg.width * baseScaleY; // Use baseScaleY for background consistency
                if (motoristBgScroll <= -scaledBgWidth) {
                    motoristBgScroll += scaledBgWidth;
                }
            }

            // --- Update Player Spin Animation ---
            if (isSpinning) {
                spinTimer += GetFrameTime();
                if (spinTimer >= spinDurationPerFrame) {
                    spinTimer -= spinDurationPerFrame; // Subtract delay instead of resetting
                    spinFrame++;
                    if (spinFrame >= resources.carSpinFrames.size()) {
                        isSpinning = false; // Stop spinning after the last frame
                        spinFrame = 0;
                        
                        if (playerLives == 0)
                            motoristGameOver = true;
                    }
                }
            }

        }
        else { // --- Game Over State Logic ---
            // Allow restarting or exiting
            if (IsKeyPressed(KEY_R)) {
                // Reset game variables to initial state
                motoristGameOver = false;
                playerLives = 5; // Reset player lives on restart
                motoristPlayerPos = { virtualScreenWidth * 0.1f, (motoristRoadYTop + motoristRoadYBottom) / 2.0f };
                motoristPlayerVel = { 0.0f, 0.0f };
                motoristVirtualSpeed = motoristPlayerSpeed;
                motoristCars.clear(); // Remove all NPCs
                isSpinning = false;
                spinFrame = 0;
                spinTimer = 0.0f;
                npcSpawnTimer = 0.0f;   // Reset spawn timer
                motoristBgScroll = 0.0f; // Reset background scroll
                collisionDetectedThisFrame = false;
                // Optionally restart music if stopped
                if (resources.backgroundMusicLoaded && !IsMusicStreamPlaying(resources.backgroundMusic)) {
                    SeekMusicStream(resources.backgroundMusic, 0.0f);
                    PlayMusicStream(resources.backgroundMusic);
                }
                TraceLog(LOG_INFO, "MOTORIST: Game Restarted.");
            }
            // Escape key handled above to exit the loop
        }

        // --- Drawing ---
        BeginTextureMode(target); // Start drawing to the game's render texture
        ClearBackground(BLACK);   // Clear the texture

        // --- Draw Scrolling Background ---
        if (resources.bg.id > 0) {
            float scaleYBg = (float)virtualScreenHeight / resources.bg.height; // Scale factor for background
            float scaledBgWidth = (float)resources.bg.width * scaleYBg;
            // Draw multiple copies of the background to cover the screen seamlessly during scroll
            for (float i = -1; i < (virtualScreenWidth / scaledBgWidth) + 1; ++i) {
                DrawTexturePro(resources.bg,
                    { 0.0f, 0.0f, (float)resources.bg.width, (float)resources.bg.height }, // Source rect (full texture)
                    { motoristBgScroll + i * scaledBgWidth, 0.0f, scaledBgWidth, (float)virtualScreenHeight }, // Destination rect
                    { 0.0f, 0.0f }, // Origin (top-left)
                    0.0f, WHITE);
            }
        }

        // --- Draw NPC Cars ---
        for (const auto& car : motoristCars) {
            // Select texture based on movement direction
            Texture2D textureToDraw = (car.movingLeft) ? resources.leftCarNPC : resources.rightCarNPC;
            if (textureToDraw.id > 0) {
                DrawTexturePro(textureToDraw,
                    { 0, 0, (float)textureToDraw.width, (float)textureToDraw.height }, // Source rect
                    { car.position.x, car.position.y, npcTextureWidth, npcTextureHeight }, // Destination rect (using SCALED dimensions)
                    { npcHalfWidth, npcHalfHeight }, // Origin (center, using SCALED half dimensions)
                    0.0f, WHITE);
            }
            // Optional: Draw NPC collision boxes for debugging
            if (showDebugInfo) DrawRectangleLinesEx(car.collisionBox, 1.0f, RED);
        }

        // --- Draw Player ---
        if (isSpinning && spinFrame >= 0 && spinFrame < resources.carSpinFrames.size()) {
            // Draw current frame of spin animation
            Texture2D spinTex = resources.carSpinFrames[spinFrame];
            if (spinTex.id > 0) {
                DrawTexturePro(spinTex,
                    { 0, 0, (float)spinTex.width, (float)spinTex.height },
                    { motoristPlayerPos.x, motoristPlayerPos.y, playerTextureWidth, playerTextureHeight }, // Use player dimensions
                    { playerHalfWidth, playerHalfHeight }, // Use player origin
                    0.0f, WHITE);
            }
        }
        else if (resources.car.id > 0) {
            // Draw normal player car texture
            DrawTexturePro(resources.car,
                { 0, 0, (float)resources.car.width, (float)resources.car.height },
                { motoristPlayerPos.x, motoristPlayerPos.y, playerTextureWidth, playerTextureHeight }, // Use player dimensions
                { playerHalfWidth, playerHalfHeight }, // Use player origin
                0.0f, WHITE);
        }

        // --- Draw Lives Counter ---
        if (!motoristGameOver) {
            DrawText(TextFormat("Lives: %d", playerLives), 10, 10, 20, WHITE);
        }

        // Optional: Draw player collision box for debugging
        if (showDebugInfo) 
            DrawRectangleLinesEx(playerRect, 1.0f, LIME);


        // --- Draw Game Over UI ---
        if (motoristGameOver) {
            // Draw semi-transparent overlay? (Optional)
            DrawRectangle(0, 0, virtualScreenWidth, virtualScreenHeight, Fade(BLACK, 0.5f));
            DrawText("GAME OVER", virtualScreenWidth / 2 - MeasureText("GAME OVER", 40) / 2, virtualScreenHeight / 2 - 60, 40, RED);
            DrawText("Press R to Restart", virtualScreenWidth / 2 - MeasureText("Press R to Restart", 20) / 2, virtualScreenHeight / 2 + 0, 20, WHITE);
            DrawText("Press ESC to Menu", virtualScreenWidth / 2 - MeasureText("Press ESC to Menu", 20) / 2, virtualScreenHeight / 2 + 30, 20, LIGHTGRAY);
        }

        EndTextureMode(); // Finish drawing to the game's render texture

        // --- Draw Final Output (Render Texture to Screen) ---
        BeginDrawing();
        ClearBackground(BLACK); // Clear window background (for letterboxing/pillarboxing)

        // Calculate scaling parameters to fit the game render texture into the window
        float finalScale = std::min((float)GetScreenWidth() / virtualScreenWidth, (float)GetScreenHeight() / virtualScreenHeight);
        float finalOffsetX = ((float)GetScreenWidth() - (virtualScreenWidth * finalScale)) * 0.5f;
        float finalOffsetY = ((float)GetScreenHeight() - (virtualScreenHeight * finalScale)) * 0.5f;

        Rectangle src = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height }; // Flip Y for render texture
        Rectangle dst = { finalOffsetX, finalOffsetY, virtualScreenWidth * finalScale, virtualScreenHeight * finalScale };

        // --- Apply Post-Processing Shader (If Enabled) ---
        if (applyShader && postProcessingShader.id > 0) {
            // Update time uniform ONLY if it was found previously
            if (shaderTimeLoc != -1) {
                float currentTime = (float)GetTime();
                SetShaderValue(postProcessingShader, shaderTimeLoc, &currentTime, SHADER_UNIFORM_FLOAT);
            }
            // Note: Resolution uniform is set once before the loop. If it needed dynamic updates,
            // you would add a similar conditional check here using shaderResolutionLoc.

            BeginShaderMode(postProcessingShader);
            DrawTexturePro(target.texture, src, dst, { 0,0 }, 0.0f, WHITE); // Draw the target texture with shader active
            EndShaderMode();
        }
        else {
            // Draw without shader if shader not requested or invalid
            DrawTexturePro(target.texture, src, dst, { 0,0 }, 0.0f, WHITE);
        }

        // --- Draw Debug Info (On Top, Scaled to Window) ---
        if (showDebugInfo) {
            int debugY = 10; int lineHeight = 15; // Smaller line height for debug
            DrawRectangle(5, 5, 200, 125, Fade(DARKGRAY, 0.7f)); // Debug background
            DrawText(TextFormat("FPS: %d", GetFPS()), GetScreenWidth() - 90, debugY, 20, LIME);
            DrawText(TextFormat("Player: %.0f, %.0f", motoristPlayerPos.x, motoristPlayerPos.y), 10, debugY, 10, GREEN); debugY += lineHeight;
            DrawText(TextFormat("V Speed: %.1f", motoristVirtualSpeed), 10, debugY, 10, YELLOW); debugY += lineHeight;
            DrawText(TextFormat("NPCs: %zu", motoristCars.size()), 10, debugY, 10, GREEN); debugY += lineHeight;
            DrawText(TextFormat("Collision: %s", collisionDetectedThisFrame ? "YES" : "NO"), 10, debugY, 10, collisionDetectedThisFrame ? RED : WHITE); debugY += lineHeight;
            DrawText(TextFormat("Spinning: %s [%d]", isSpinning ? "YES" : "NO", spinFrame), 10, debugY, 10, isSpinning ? ORANGE : WHITE); debugY += lineHeight;
            DrawText(TextFormat("Music: %.1f / %.1f", GetMusicTimePlayed(resources.backgroundMusic), GetMusicTimeLength(resources.backgroundMusic)), 10, debugY, 10, PURPLE); debugY += lineHeight;
            DrawText(TextFormat("Game Over: %s", motoristGameOver ? "YES" : "NO"), 10, debugY, 10, motoristGameOver ? RED : WHITE); debugY += lineHeight;
            DrawText(TextFormat("Lives: %d", playerLives), 10, debugY, 10, GREEN);
        }

        EndDrawing(); // Present the final frame to the window
    } // --------------- End of Midnight Motorist Game Loop ---------------

    // --- Cleanup before returning ---
    TraceLog(LOG_INFO, "MOTORIST: Exiting game loop.");
    if (resources.backgroundMusicLoaded) {
        StopMusicStream(resources.backgroundMusic); // Ensure music is stopped
    }
    UnloadRenderTexture(target); // Unload the game-specific render texture

    return 0;
}