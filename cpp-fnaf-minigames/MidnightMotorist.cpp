#include "raylib.h"

#include "MotoristResources.h"
#include "MotoristConst.h"

enum MotoristGameState {
    COUNTDOWN_INITIAL_DELAY,
    COUNTDOWN_SHOW_3,
    COUNTDOWN_SHOW_2,
    COUNTDOWN_SHOW_1,
    COUNTDOWN_SHOW_GO,
    PLAYING,
    GAME_OVER_STATE
};

vector<float> npcSpawnYPositions = {
    virtualScreenHeight * laneCenterY1Ratio,
    virtualScreenHeight * laneCenterY2Ratio,
    virtualScreenHeight * laneCenterY3Ratio,
    virtualScreenHeight * laneCenterY4Ratio,
    virtualScreenHeight * laneCenterY5Ratio,
    virtualScreenHeight * laneCenterY6Ratio,
    virtualScreenHeight * laneCenterY7Ratio,
    virtualScreenHeight * laneCenterY8Ratio
};

uniform_int_distribution<> spawnYDist(0, static_cast<int>(npcSpawnYPositions.size()) - 1);

struct Car {
    Vector2 position;
    float speed;
    bool movingLeft;
    int textureIndex;
    Rectangle collisionBox;
};

int runMidnightMotorist(GraphicsQuality quality, Shader postProcessingShader, bool applyShader) {
    cout << "Starting Midnight Motorist | Quality: " << quality << " | Shader Active: " << applyShader << endl;

    MotoristGameResources resources = LoadMotoristResources(quality);

    RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    if (target.id == 0) {
        UnloadMotoristResources(resources);
        return 1;
    }

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

    Rectangle playerRect;

    MotoristGameState currentGameState = COUNTDOWN_INITIAL_DELAY;
    float countdownTimer = 2;

    // Player start position
    Vector2 motoristPlayerPos = { virtualScreenWidth / 10, (motoristRoadYTop + motoristRoadYBottom) / 2 };
    Vector2 motoristPlayerVel = { 0, 0 };
    float motoristVirtualSpeed = 0;

    int playerLives = 5;
    int currentLap = 0;
    float distanceThisLap = 0;

    bool showLapAnim = false;
    float lapAnimTimer = 0;
    int lapAnimBlinkCount = 0;

    vector<Car> motoristCars;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> spawnChanceDist(0, 1);
    uniform_real_distribution<> npcSpeedDist(minNpcSpeed, maxNpcSpeed);

    vector<float> currentLaneSpeeds(npcSpawnYPositions.size());

    float motoristBgScroll = 0;
    float npcSpawnTimer = 0;
    bool showDebugInfo = false;
    bool collisionDetectedThisFrame = false;
    bool playBgNoiseLoop = false;
    bool isSpinning = false;
    int spinFrame = 0;
    float spinTimer = 0;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (currentGameState == PLAYING || currentGameState == GAME_OVER_STATE) {
            if (IsMusicStreamPlaying(resources.backgroundMusic)) {
                UpdateMusicStream(resources.backgroundMusic);
                if (GetMusicTimePlayed(resources.backgroundMusic) >= GetMusicTimeLength(resources.backgroundMusic))
                    SeekMusicStream(resources.backgroundMusic, 0);
            }
        }

        if (playBgNoiseLoop) {
            if (IsMusicStreamPlaying(resources.bgNoise)) {
                UpdateMusicStream(resources.bgNoise);
                if (GetMusicTimePlayed(resources.bgNoise) >= GetMusicTimeLength(resources.bgNoise)) 
                    SeekMusicStream(resources.bgNoise, 0);
            }
            else 
                PlayMusicStream(resources.bgNoise);
        }

        if (IsKeyPressed(KEY_F3)) showDebugInfo = !showDebugInfo; // temp debug
        if (IsKeyPressed(KEY_ESCAPE)) break;
        

        switch (currentGameState) {
        case COUNTDOWN_INITIAL_DELAY:
            countdownTimer -= dt;
            if (countdownTimer <= 0) {
                currentGameState = COUNTDOWN_SHOW_3;
                countdownTimer = 1;
                PlaySound(resources.countdown);
            }
            break;
        case COUNTDOWN_SHOW_3:
            countdownTimer -= dt;
            if (countdownTimer <= 0) {
                currentGameState = COUNTDOWN_SHOW_2;
                countdownTimer = 1;
                PlaySound(resources.countdown);
            }
            break;
        case COUNTDOWN_SHOW_2:
            countdownTimer -= dt;
            if (countdownTimer <= 0) {
                currentGameState = COUNTDOWN_SHOW_1;
                countdownTimer = 1;
                PlaySound(resources.countdown);
            }
            break;
        case COUNTDOWN_SHOW_1:
            countdownTimer -= dt;
            if (countdownTimer <= 0) {
                currentGameState = COUNTDOWN_SHOW_GO;
                countdownTimer = 1;
                PlaySound(resources.ctdwnGo);
                playBgNoiseLoop = true;
                SeekMusicStream(resources.bgNoise, 0);
                PlayMusicStream(resources.bgNoise);
                SetMusicVolume(resources.bgNoise, 0.2); 
            }
            break;
        case COUNTDOWN_SHOW_GO:
            countdownTimer -= dt;
            if (countdownTimer <= 0) {
                currentGameState = PLAYING;
                countdownTimer = 1.2;
                npcSpawnTimer = 0;
                motoristVirtualSpeed = virtualSpeedInitial;
                for (int i = 0; i < currentLaneSpeeds.size(); i++) 
                    currentLaneSpeeds[i] = npcSpeedDist(gen);
                
                if (!IsMusicStreamPlaying(resources.backgroundMusic)) {
                    PlayMusicStream(resources.backgroundMusic);
                    SetMusicVolume(resources.backgroundMusic, 0.5);
                }
            }
            break;
        case PLAYING:
            motoristPlayerVel = { 0, 0 };
            if (IsKeyDown(KEY_A)) 
                motoristPlayerVel.x = -playerMoveSpeed;
            if (IsKeyDown(KEY_D)) 
                motoristPlayerVel.x = playerMoveSpeed;
            if (IsKeyDown(KEY_W)) 
                motoristPlayerVel.y = -playerMoveSpeed;
            if (IsKeyDown(KEY_S))
                motoristPlayerVel.y = playerMoveSpeed;

            motoristPlayerPos.x += motoristPlayerVel.x * dt;
            motoristPlayerPos.y += motoristPlayerVel.y * dt;

            motoristPlayerPos.x = Clamp(motoristPlayerPos.x, playerHalfWidth, virtualScreenWidth - playerHalfWidth);
            motoristPlayerPos.y = Clamp(motoristPlayerPos.y, motoristRoadYTop + playerHalfHeight * 0.8, motoristRoadYBottom - playerHalfHeight * 0.8);
            playerRect = { motoristPlayerPos.x - playerHalfWidth, motoristPlayerPos.y - playerHalfHeight, playerTextureWidth, playerTextureHeight };

            collisionDetectedThisFrame = false;
            for (auto& car : motoristCars) {
                if (!showDebugInfo && CheckCollisionRecs(playerRect, car.collisionBox)) {
                    collisionDetectedThisFrame = true;
                    if (!isSpinning) {
                        isSpinning = true;
                        spinFrame = 0;
                        spinTimer = 0;
                        motoristVirtualSpeed = virtualSpeedInitial;
                        PlaySound(resources.carCrash);
                        playerLives--;
                        if (playerLives <= 0) 
                            currentGameState = GAME_OVER_STATE;
                    }
                }
            }

            if (currentGameState == PLAYING) {
                if (!isSpinning) {
                    motoristVirtualSpeed += virtualSpeedAcceleration * dt;
                    if (motoristVirtualSpeed > virtualSpeedMax)
                        motoristVirtualSpeed = virtualSpeedMax;
                }

                distanceThisLap += motoristVirtualSpeed * dt;
                if (distanceThisLap >= lapDistance) {
                    currentLap++;
                    distanceThisLap -= lapDistance;
                    PlaySound(resources.lapReached);
                    lapAnimBlinkCount = lapAnimBlinks;
                    showLapAnim = true;
                    lapAnimTimer = lapAnimBlinkDuration;
                }

                if (lapAnimBlinkCount > 0) {
                    lapAnimTimer -= dt;
                    if (lapAnimTimer <= 0) {
                        showLapAnim = !showLapAnim;
                        lapAnimTimer = lapAnimBlinkDuration;
                        if (!showLapAnim) {
                            lapAnimBlinkCount--;
                            if (lapAnimBlinkCount <= 0) showLapAnim = false;
                        }
                    }
                }

                npcSpawnTimer += dt;
                if (npcSpawnTimer >= (1 / npcSpawnRate)) {
                    npcSpawnTimer = 0;
                    if (spawnChanceDist(gen) < 0.82) {
                        Car newCar;
                        int randomLaneIndex = spawnYDist(gen);
                        newCar.position.y = npcSpawnYPositions[randomLaneIndex];

                        float NpcSpeedForLane = currentLaneSpeeds[randomLaneIndex];
                        newCar.speed = NpcSpeedForLane + motoristVirtualSpeed / 5;
                        newCar.speed = Clamp(newCar.speed, minNpcSpeed, maxNpcSpeed + virtualSpeedMax * 0.3);

                        if (randomLaneIndex < 4) {
                            newCar.movingLeft = true;
                            newCar.textureIndex = 1;
                            newCar.position.x = virtualScreenWidth + npcTextureWidth;
                        }
                        else {
                            newCar.movingLeft = false;
                            newCar.textureIndex = 0;
                            newCar.position.x = -npcTextureWidth;
                        }

                        bool canSpawn = true;
                        for (const auto& existingCar : motoristCars) {
                            if (abs(existingCar.position.y - newCar.position.y) < 1 && existingCar.movingLeft == newCar.movingLeft) {
                                float distance = newCar.movingLeft ? (newCar.position.x - existingCar.position.x) : (existingCar.position.x - newCar.position.x);
                                if (distance > 0 && distance < (npcTextureWidth + minNpcSpacing)) {
                                    canSpawn = false;
                                    break;
                                }
                            }
                        }
                        if (canSpawn) {
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

                for (auto& car : motoristCars) {
                    car.position.x += (car.movingLeft ? -(car.speed) : (car.speed)) * dt;
                    car.collisionBox.x = car.position.x - npcHalfWidth + npcCollisionMarginX;
                }

                motoristCars.erase(remove_if(motoristCars.begin(), motoristCars.end(), [&](const Car& car) {
                    return (car.movingLeft && car.position.x < -npcTextureWidth * 1.5) || (!car.movingLeft && car.position.x > virtualScreenWidth + npcTextureWidth * 1.5); }), motoristCars.end());

                motoristBgScroll -= motoristVirtualSpeed * bgScrollMultiplier * dt;
                if (resources.bg.id > 0) {
                    float scaledBgWidth = resources.bg.width * baseScaleY;
                    if (motoristBgScroll <= -scaledBgWidth)
                        motoristBgScroll += scaledBgWidth;
                    
                }

                if (isSpinning) {
                    spinTimer += dt;
                    if (spinTimer >= spinFrameDuration) {
                        spinTimer -= spinFrameDuration;
                        spinFrame++;
                        if (spinFrame >= resources.carSpinFrames.size()) {
                            isSpinning = false;
                            spinFrame = 0;
                        }
                        else if(resources.carSpinFrames.empty()) {
                            isSpinning = false;
                            spinFrame = 0;
                        }
                    }
                }
            }
            break;

        case GAME_OVER_STATE:
            StopMusicStream(resources.backgroundMusic);
            playBgNoiseLoop = false;

            if (IsKeyPressed(KEY_R)) {
                currentGameState = COUNTDOWN_INITIAL_DELAY;
                countdownTimer = 2;
                playerLives = 5;
                motoristPlayerPos = { virtualScreenWidth / 10, (motoristRoadYTop + motoristRoadYBottom) / 2 };
                motoristPlayerVel = { 0, 0 };
                motoristVirtualSpeed = 0;
                motoristCars.clear();
                isSpinning = false;
                spinFrame = 0;
                spinTimer = 0;
                npcSpawnTimer = 0;
                motoristBgScroll = 0;
                collisionDetectedThisFrame = false;
                currentLap = 0;
                distanceThisLap = 0;
                showLapAnim = false;
                lapAnimBlinkCount = 0;
                SeekMusicStream(resources.backgroundMusic, 0);
            }

            break;
        }

        BeginTextureMode(target);
        ClearBackground(BLACK);

        if (resources.bg.id > 0) {
            float scaleYBg = virtualScreenHeight / resources.bg.height;
            float scaledBgWidth = resources.bg.width * scaleYBg;
            float currentScroll = (currentGameState == PLAYING || currentGameState == GAME_OVER_STATE) ? motoristBgScroll : 0;
            for (float i = -1; i < (virtualScreenWidth / scaledBgWidth) + 2; i++) {
                DrawTexturePro(resources.bg,
                    { 0, 0, (float)resources.bg.width, (float)resources.bg.height },
                    { currentScroll + i * scaledBgWidth, 0, scaledBgWidth, virtualScreenHeight },
                    { 0, 0 }, 0, WHITE);
            }
        }

		// Draw NPC cars
        if (currentGameState == PLAYING || currentGameState == GAME_OVER_STATE) {
            for (const auto& car : motoristCars) {
                Texture2D textureToDraw = (car.movingLeft) ? resources.leftCarNPC : resources.rightCarNPC;
                if (textureToDraw.id > 0) {
                    DrawTexturePro(textureToDraw,
                        { 0,0, (float)textureToDraw.width, (float)textureToDraw.height },
                        { car.position.x, car.position.y, npcTextureWidth, npcTextureHeight },
                        { npcHalfWidth, npcHalfHeight }, 0, WHITE);
                }
                if (showDebugInfo) DrawRectangleLinesEx(car.collisionBox, 1, RED);
            }
        }

		// Draw player car
        if (currentGameState == PLAYING || currentGameState == COUNTDOWN_SHOW_GO || currentGameState == GAME_OVER_STATE) {
            if (isSpinning && !resources.carSpinFrames.empty() && spinFrame >= 0 && spinFrame < resources.carSpinFrames.size() && (currentGameState == PLAYING || currentGameState == GAME_OVER_STATE)) {
                Texture2D spinTex = resources.carSpinFrames[spinFrame];
                if (spinTex.id > 0) {
                    DrawTexturePro(spinTex, { 0,0, (float)spinTex.width, (float)spinTex.height },
                        { motoristPlayerPos.x, motoristPlayerPos.y, playerTextureWidth, playerTextureHeight },
                        { playerHalfWidth, playerHalfHeight }, 0, WHITE);
                }
            }
            else if (resources.car.id > 0) {
                DrawTexturePro(resources.car, { 0,0, (float)resources.car.width, (float)resources.car.height },
                    { motoristPlayerPos.x, motoristPlayerPos.y, playerTextureWidth, playerTextureHeight },
                    { playerHalfWidth, playerHalfHeight }, 0, WHITE);
            }
        }

        if (showLapAnim && resources.lap.id > 0) {
            float lapAnimTextureWidth = resources.lap.width;
            float lapAnimTextureHeight = resources.lap.height;
            float drawWidth = lapAnimTextureWidth * lapAnimScale;
            float drawHeight = lapAnimTextureHeight * lapAnimScale;
            Vector2 drawPos = {
                virtualScreenWidth / 2 - drawWidth / 2,
                virtualScreenHeight / 2 - drawHeight / 2
            };
            DrawTextureEx(resources.lap, drawPos, 0, lapAnimScale, WHITE);
        }

        Texture2D textureToDrawForCountdown = { 0 };
        float countdownImageScale = 0.5;
        switch (currentGameState) {
            case COUNTDOWN_SHOW_3: 
                textureToDrawForCountdown = resources.three; 
                break;
            case COUNTDOWN_SHOW_2: 
                textureToDrawForCountdown = resources.two;
                break;
            case COUNTDOWN_SHOW_1:
                textureToDrawForCountdown = resources.one;
                break;
            case COUNTDOWN_SHOW_GO:
                textureToDrawForCountdown = resources.go;
                break;
            default: break;
        }
        if (textureToDrawForCountdown.id > 0) {
            float imgWidth = textureToDrawForCountdown.width * countdownImageScale;
            float imgHeight = textureToDrawForCountdown.height * countdownImageScale;
            Vector2 imgPos = { (virtualScreenWidth - imgWidth) / 2, (virtualScreenHeight - imgHeight) / 2 };
            DrawTextureEx(textureToDrawForCountdown, imgPos, 0, countdownImageScale, WHITE);
        }

        // Rysowanie UI
        if (currentGameState == PLAYING || currentGameState == GAME_OVER_STATE) {
            Font uiFont = resources.gameFont;

            string lapTextStr = "LAP " + to_string(currentLap);
            Vector2 lapTextSize = MeasureTextEx(uiFont, lapTextStr.c_str(), uiTextSize, uiTextSpacing);
            Vector2 lapTextPos = { uiMargin, uiMargin };
            DrawTextEx(uiFont, lapTextStr.c_str(), lapTextPos, uiTextSize, uiTextSpacing, WHITE);

            string speedTextStr = "MPH " + to_string(static_cast<int>(motoristVirtualSpeed / 10));
            float speedTextY = lapTextSize.y + uiMargin;
            Vector2 speedTextSize = MeasureTextEx(uiFont, speedTextStr.c_str(), uiTextSize, uiTextSpacing);
            Vector2 speedTextPos = { uiMargin, speedTextY };
            DrawTextEx(uiFont, speedTextStr.c_str(), speedTextPos, uiTextSize, uiTextSpacing, WHITE);

            string livesTextStr = "LIVES: " + to_string(playerLives);
            Vector2 livesTextSize = MeasureTextEx(uiFont, livesTextStr.c_str(), uiTextSize, uiTextSpacing);
            Vector2 livesTextPos = { uiMargin, virtualScreenHeight - livesTextSize.y - uiMargin };
            DrawTextEx(uiFont, livesTextStr.c_str(), livesTextPos, uiTextSize, uiTextSpacing, WHITE);
        }

        if (showDebugInfo && (currentGameState == PLAYING || currentGameState == GAME_OVER_STATE)) {
            DrawRectangleLinesEx(playerRect, 1, LIME);
        }

        if (currentGameState == GAME_OVER_STATE) {
            DrawRectangle(0, 0, virtualScreenWidth, virtualScreenHeight, Fade(BLACK, 0.5));
            DrawText("GAME OVER", virtualScreenWidth / 2 - MeasureText("GAME OVER", 40) / 2, virtualScreenHeight / 2 - 60, 40, RED);
            DrawText("Press R to Restart", virtualScreenWidth / 2 - MeasureText("Press R to Restart", 20) / 2, virtualScreenHeight / 2 + 0, 20, WHITE);
            DrawText("Press ESC to Menu", virtualScreenWidth / 2 - MeasureText("Press ESC to Menu", 20) / 2, virtualScreenHeight / 2 + 30, 20, LIGHTGRAY);
        }
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        float finalScale = min(GetScreenWidth() / virtualScreenWidth, GetScreenHeight() / virtualScreenHeight);
        float finalOffsetX = (GetScreenWidth() - (virtualScreenWidth * finalScale)) / 2;
        float finalOffsetY = (GetScreenHeight() - (virtualScreenHeight * finalScale)) / 2;
        Rectangle src = { 0, 0, target.texture.width, -target.texture.height };
        Rectangle dst = { finalOffsetX, finalOffsetY, virtualScreenWidth * finalScale, virtualScreenHeight * finalScale };

        if (applyShader && postProcessingShader.id > 0) {
            if (shaderTimeLoc != -1) {
                float currentTime = GetTime();
                SetShaderValue(postProcessingShader, shaderTimeLoc, &currentTime, SHADER_UNIFORM_FLOAT);
            }
            BeginShaderMode(postProcessingShader);
            DrawTexturePro(target.texture, src, dst, { 0,0 }, 0, WHITE);
            EndShaderMode();
        }
        else {
            DrawTexturePro(target.texture, src, dst, { 0,0 }, 0, WHITE);
        }

        if (showDebugInfo) {
            int debugY = 10;
            int lineHeight = 15;
            DrawRectangle(5, 5, 220, 170, Fade(DARKGRAY, 0.7));
            DrawText(TextFormat("FPS: %d", GetFPS()), GetScreenWidth() - 90, debugY, 20, LIME);

            if (currentGameState == PLAYING || currentGameState == GAME_OVER_STATE || currentGameState == COUNTDOWN_SHOW_GO) {
                DrawText(TextFormat("PlayerPos: %, %", motoristPlayerPos.x, motoristPlayerPos.y), 10, debugY, 10, GREEN); debugY += lineHeight;
            }
            if (currentGameState == PLAYING || currentGameState == GAME_OVER_STATE) {
                DrawText(TextFormat("VirtualSpeed: %.1", motoristVirtualSpeed), 10, debugY, 10, YELLOW); debugY += lineHeight;
                DrawText(TextFormat("NPCs: %zu", motoristCars.size()), 10, debugY, 10, GREEN); debugY += lineHeight;
                DrawText(TextFormat("Collision: %s", collisionDetectedThisFrame ? "YES" : "NO"), 10, debugY, 10, collisionDetectedThisFrame ? RED : WHITE); debugY += lineHeight;
                DrawText(TextFormat("Spinning: %s [%d]", isSpinning ? "YES" : "NO", spinFrame), 10, debugY, 10, isSpinning ? ORANGE : WHITE); debugY += lineHeight;
                DrawText(TextFormat("Current Lap: %d", currentLap), 10, debugY, 10, SKYBLUE); debugY += lineHeight;
                DrawText(TextFormat("Dist This Lap: %.1", distanceThisLap), 10, debugY, 10, SKYBLUE); debugY += lineHeight;
            }
            DrawText(TextFormat("Music: %.1/%.1", GetMusicTimePlayed(resources.backgroundMusic), GetMusicTimeLength(resources.backgroundMusic)), 10, debugY, 10, PURPLE); debugY += lineHeight;
            DrawText(TextFormat("BG Noise: %s", IsMusicStreamPlaying(resources.bgNoise) ? "Playing" : "Stopped"), 10, debugY, 10, DARKGREEN); debugY += lineHeight;
            DrawText(TextFormat("State: %d, CTimer: %.2", currentGameState, countdownTimer), 10, debugY, 10, GOLD); debugY += lineHeight;
            DrawText(TextFormat("Lives: %d", playerLives), 10, debugY, 10, (playerLives > 0) ? GREEN : RED);
        }

        EndDrawing();
    }

    if (IsMusicStreamPlaying(resources.backgroundMusic)) StopMusicStream(resources.backgroundMusic);
    if (IsMusicStreamPlaying(resources.bgNoise)) StopMusicStream(resources.bgNoise);
    
    UnloadRenderTexture(target);
    UnloadMotoristResources(resources);

    return 0;
}