#include "raylib.h"
#include "raymath.h"
#include "MotoristResources.h" // Upewnij siê, ¿e ta definicja jest poprawna i zawiera flagi loaded
#include "GameSettings.h"
#include "const.h"
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <iomanip> // Dla std::fixed i std::setprecision (opcjonalnie, dla formatowania prêdkoœci)

float backgroundScrollMultiplier = 1.5f;

enum MotoristGameState {
    COUNTDOWN_INITIAL_DELAY,
    COUNTDOWN_SHOW_3,
    COUNTDOWN_SHOW_2,
    COUNTDOWN_SHOW_1,
    COUNTDOWN_SHOW_GO,
    COUNTDOWN_POST_GO_DELAY,
    PLAYING,
    GAME_OVER_STATE
};


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
std::uniform_int_distribution<> spawnYDist(0, static_cast<int>(npcSpawnYPositions.size()) - 1);

struct Car {
    Vector2 position;
    float speed;
    bool movingLeft;
    int textureIndex;
    Rectangle collisionBox;
};


int runMidnightMotorist(GraphicsQuality quality, Shader postProcessingShader, bool applyShader) {
    std::cout << "Starting Midnight Motorist | Quality: " << quality << " | Shader Active: " << applyShader << std::endl;

    MotoristGameResources resources = LoadMotoristResources(quality);

    RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    if (target.id == 0) {
        TraceLog(LOG_ERROR, "MOTORIST: Failed to create render texture. Aborting.");
        UnloadMotoristResources(resources);
        return 1;
    }
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    bool essentialResourcesLoaded = resources.car.id > 0 &&
        resources.leftCarNPC.id > 0 &&
        resources.rightCarNPC.id > 0 &&
        resources.bg.id > 0;

    if (!essentialResourcesLoaded) {
        TraceLog(LOG_ERROR, "MOTORIST: Failed to load essential textures. Aborting.");
        UnloadRenderTexture(target);
        UnloadMotoristResources(resources);
        return 1;
    }

    int shaderTimeLoc = -1;
    int shaderResolutionLoc = -1;
    if (applyShader && postProcessingShader.id > 0) {
        shaderTimeLoc = GetShaderLocation(postProcessingShader, "time");
        shaderResolutionLoc = GetShaderLocation(postProcessingShader, "resolution");
        if (shaderResolutionLoc != -1) {
            float gameResolution[2] = { (float)virtualScreenWidth, (float)virtualScreenHeight };
            SetShaderValue(postProcessingShader, shaderResolutionLoc, gameResolution, SHADER_UNIFORM_VEC2);
        }
        else { TraceLog(LOG_WARNING, "MOTORIST: Shader 'resolution' uniform not found."); }
        if (shaderTimeLoc == -1) { TraceLog(LOG_WARNING, "MOTORIST: Shader 'time' uniform not found."); }
    }

    Rectangle playerRect;


    MotoristGameState currentGameState = COUNTDOWN_INITIAL_DELAY;
    float countdownTimer = 2.0f;

    Vector2 motoristPlayerPos = { virtualScreenWidth * 0.1f, (motoristRoadYTop + motoristRoadYBottom) / 2.0f };
    Vector2 motoristPlayerVel = { 0.0f, 0.0f }; // Prêdkoœæ w jednostkach/sekundê
    float motoristVirtualSpeed = 0.0f; // predkosc swiata, zaczyna sie od zera

    //float playerTargetSpeed = motoristPlayerSpeed; // Docelowa prêdkoœæ gracza (jednostki/sekundê)
    float playerCurrentSpeed = 0.0f; // Aktualna prêdkoœæ gracza (dla akceleracji/deceleracji t³a)


    int playerLives = 5;
    int currentLap = 0;
    float distanceThisLap = 0.0f;

    // ----- NOWE ZMIENNE STANU DLA MIGAJACEGO LAP SPRITE'A -----
    bool showLapAnim = false;     // Czy wskaŸnik jest TERAZ widoczny
    float lapAnimTimer = 0.0f;    // Timer dla aktualnej fazy migniêcia
    int lapAnimBlinkCount = 0;    // Ile PE£NYCH migniêæ (ON+OFF) pozosta³o


    std::vector<Car> motoristCars;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> spawnChanceDist(0.0f, 1.0f); // Zmieniono zakres na 0-1
    std::uniform_real_distribution<> npcSpeedDist(MIN_NPC_SPEED, MAX_NPC_SPEED);

    std::vector<float> currentLaneSpeeds(npcSpawnYPositions.size());
    for (size_t i = 0; i < currentLaneSpeeds.size(); ++i) {
        currentLaneSpeeds[i] = npcSpeedDist(gen);
    }

    float motoristBgScroll = 0.0f;
    float npcSpawnTimer = 0.0f;
    bool showDebugInfo = false;
    bool collisionDetectedThisFrame = false;

    bool playBgNoiseLoop = false; // Nowa flaga do kontrolowania pêtli bgNoise

    bool isSpinning = false;
    int spinFrame = 0;
    float spinTimer = 0.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (resources.backgroundMusicLoaded && (currentGameState == PLAYING || currentGameState == GAME_OVER_STATE)) {
            if (IsMusicStreamPlaying(resources.backgroundMusic)) {
                UpdateMusicStream(resources.backgroundMusic);
                if (GetMusicTimePlayed(resources.backgroundMusic) >= GetMusicTimeLength(resources.backgroundMusic) - 0.1f) {
                    SeekMusicStream(resources.backgroundMusic, 0.0f); // P³ynne zapêtlanie g³ównej muzyki
                }
            }
        }

        // Aktualizacja i zapêtlanie bgNoise
        if (playBgNoiseLoop && resources.bgNoiseLoaded) {
            if (IsMusicStreamPlaying(resources.bgNoise)) {
                UpdateMusicStream(resources.bgNoise);
                if (GetMusicTimePlayed(resources.bgNoise) >= GetMusicTimeLength(resources.bgNoise)) { // Ma³y margines na koñcu
                    SeekMusicStream(resources.bgNoise, 0.0f); // Zapêtl bgNoise
                    // Jeœli PlayMusicStream nie zosta³o wywo³ane ponownie, a tylko Seek, to mo¿e przestaæ graæ.
                    // Bezpieczniej mo¿e byæ PlayMusicStream, jeœli nie gra.
                }
            }
            else {
                // Jeœli flaga jest ustawiona, a dŸwiêk nie gra (np. po pierwszym SeekMusicStream lub jeœli siê zatrzyma³)
                PlayMusicStream(resources.bgNoise);
            }
        }

        if (IsKeyPressed(KEY_F3)) showDebugInfo = !showDebugInfo;
        if (IsKeyPressed(KEY_ESCAPE)) {
            break;
        }

        switch (currentGameState) {
        case COUNTDOWN_INITIAL_DELAY:
            countdownTimer -= dt;
            if (countdownTimer <= 0) {
                currentGameState = COUNTDOWN_SHOW_3;
                countdownTimer = 1.0f;
                if (resources.countdownSoundLoaded) PlaySound(resources.countdown);
            }
            break;
        case COUNTDOWN_SHOW_3:
            countdownTimer -= dt;
            if (countdownTimer <= 0) {
                currentGameState = COUNTDOWN_SHOW_2;
                countdownTimer = 1.0f;
                if (resources.countdownSoundLoaded) PlaySound(resources.countdown);
            }
            break;
        case COUNTDOWN_SHOW_2:
            countdownTimer -= dt;
            if (countdownTimer <= 0) {
                currentGameState = COUNTDOWN_SHOW_1;
                countdownTimer = 1.0f;
                if (resources.countdownSoundLoaded) PlaySound(resources.countdown);
            }
            break;
        case COUNTDOWN_SHOW_1:
            countdownTimer -= dt;
            if (countdownTimer <= 0) {
                currentGameState = COUNTDOWN_SHOW_GO;
                countdownTimer = 1.0f;
                if (resources.goSoundLoaded) PlaySound(resources.ctdwnGo);


                // --- ROZPOCZNIJ ODTWARZANIE bgNoise ---
                if (resources.bgNoiseLoaded) {
                    playBgNoiseLoop = true; // Ustaw flagê
                    // Jeœli bgNoise by³o wczeœniej zatrzymane, upewnij siê, ¿e zacznie graæ
                    if (IsMusicStreamPlaying(resources.bgNoise)) { // Jeœli ju¿ gra (np. z poprzedniego uruchomienia)
                        StopMusicStream(resources.bgNoise); // Zatrzymaj, ¿eby zacz¹æ od pocz¹tku
                    }
                    SeekMusicStream(resources.bgNoise, 0.0f); // Ustaw na pocz¹tek
                    PlayMusicStream(resources.bgNoise);       // Zacznij graæ
                    SetMusicVolume(resources.bgNoise, 0.2f); // Ustaw g³oœnoœæ (opcjonalnie)
                }
            }
            break;
        case COUNTDOWN_SHOW_GO:
            countdownTimer -= dt;
            if (countdownTimer <= 0) {
                currentGameState = COUNTDOWN_POST_GO_DELAY;
                countdownTimer = 0.0f;
            }
            break;
        case COUNTDOWN_POST_GO_DELAY:
            countdownTimer -= dt;
            if (countdownTimer <= 0) {
                currentGameState = PLAYING;
                npcSpawnTimer = 0.0f;
                motoristVirtualSpeed = VIRTUAL_SPEED_INITIAL; // Ustaw prêdkoœæ œwiata na pocz¹tkow¹
                for (size_t i = 0; i < currentLaneSpeeds.size(); ++i) {
                    currentLaneSpeeds[i] = npcSpeedDist(gen); // Zresetuj bazowe prêdkoœci pasów
                }
                if (resources.backgroundMusicLoaded && !IsMusicStreamPlaying(resources.backgroundMusic)) {
                    PlayMusicStream(resources.backgroundMusic);
                    SetMusicVolume(resources.backgroundMusic, 0.5f);
                }
            }
            break;

        case PLAYING:
            motoristPlayerVel = { 0.0f, 0.0f };
            if (IsKeyDown(KEY_A)) motoristPlayerVel.x = -PLAYER_MOVE_SPEED;
            if (IsKeyDown(KEY_D)) motoristPlayerVel.x = PLAYER_MOVE_SPEED;
            if (IsKeyDown(KEY_W)) motoristPlayerVel.y = -PLAYER_MOVE_SPEED;
            if (IsKeyDown(KEY_S)) motoristPlayerVel.y = PLAYER_MOVE_SPEED;


            motoristPlayerPos.x += motoristPlayerVel.x * dt;
            motoristPlayerPos.y += motoristPlayerVel.y * dt;

            motoristPlayerPos.x = Clamp(motoristPlayerPos.x, playerHalfWidth, virtualScreenWidth - playerHalfWidth);
            motoristPlayerPos.y = Clamp(motoristPlayerPos.y, motoristRoadYTop + playerHalfHeight * 0.8f, motoristRoadYBottom - playerHalfHeight * 0.8f);
            playerRect = { motoristPlayerPos.x - playerHalfWidth, motoristPlayerPos.y - playerHalfHeight, playerTextureWidth, playerTextureHeight };

            collisionDetectedThisFrame = false;
            for (auto& car : motoristCars) {
                if (CheckCollisionRecs(playerRect, car.collisionBox)) {
                    collisionDetectedThisFrame = true;
                    if (!isSpinning) {
                        isSpinning = true;
                        spinFrame = 0;
                        spinTimer = 0.0f;
                        motoristVirtualSpeed = VIRTUAL_SPEED_INITIAL; // Reset do prêdkoœci pocz¹tkowej
                        if (resources.carCrashSoundLoaded) PlaySound(resources.carCrash);
                        playerLives--;
                        if (playerLives <= 0) {
                            currentGameState = GAME_OVER_STATE;
                        }
                    }
                }
            }

            if (currentGameState == PLAYING) {
                if (!isSpinning) {
                    // P³ynna zmiana prêdkoœci t³a/gry
                    motoristVirtualSpeed += VIRTUAL_SPEED_ACCELERATION * dt;
                    if (motoristVirtualSpeed > VIRTUAL_SPEED_MAX) {
                        motoristVirtualSpeed = VIRTUAL_SPEED_MAX;
                    }
                }
                else { // Podczas krêcenia siê, zredukuj prêdkoœæ t³a
                    //playerCurrentSpeed -= playerDeceleration * dt * 2.0f; // Szybsze hamowanie
                    //if (playerCurrentSpeed < 0) playerCurrentSpeed = 0;
                }

                // Logika okr¹¿eñ
                distanceThisLap += motoristVirtualSpeed * dt;
                if (distanceThisLap >= LAP_DISTANCE) {
                    currentLap++;
                    distanceThisLap -= LAP_DISTANCE;
                    if (resources.lapSoundLoaded)
                        PlaySound(resources.lapReached);

                    lapAnimBlinkCount = LAP_ANIM_TOTAL_BLINKS; // Ustaw liczbê migniêæ
                    showLapAnim = true; // Zacznij od pokazania wskaŸnika
                    lapAnimTimer = LAP_ANIM_BLINK_DURATION; // Ustaw timer dla pierwszej fazy (ON)

                }

                // +++ AKTUALIZACJA MIGAJ¥CEGO WSKANIKA OKR¥¯ENIA +++
                if (lapAnimBlinkCount > 0) { // Jeœli miganie jest aktywne
                    lapAnimTimer -= dt;

                    if (lapAnimTimer <= 0.0f) {
                        // Czas fazy min¹³, prze³¹cz widocznoœæ
                        showLapAnim = !showLapAnim;
                        lapAnimTimer = LAP_ANIM_BLINK_DURATION; // Zresetuj timer dla nowej fazy

                        // Jeœli w³aœnie ukryliœmy wskaŸnik (zakoñczyliœmy cykl ON), zmniejsz licznik migniêæ
                        if (!showLapAnim) {
                            lapAnimBlinkCount--;
                            // Jeœli to by³o ostatnie migniêcie, zakoñcz ca³kowicie
                            if (lapAnimBlinkCount <= 0) {
                                showLapAnim = false; // Upewnij siê, ¿e jest ukryty na koniec
                            }
                        }
                    }
                }


                // --- Spawnowanie NPC ---
                npcSpawnTimer += dt;
                if (npcSpawnTimer >= (1.0f / npcSpawnRate)) {
                    npcSpawnTimer = 0.0f;
                    if (spawnChanceDist(gen) < 0.82f) {
                        Car newCar;
                        int randomLaneIndex = spawnYDist(gen);
                        newCar.position.y = npcSpawnYPositions[randomLaneIndex];

                        float NpcSpeedForLane;
                        if (randomLaneIndex >= 0 && randomLaneIndex < currentLaneSpeeds.size()) {
                            NpcSpeedForLane = currentLaneSpeeds[randomLaneIndex];
                        }
                        else {
                            NpcSpeedForLane = npcSpeedDist(gen); // Fallback
                        }
                        // Prêdkoœæ NPC = bazowa prêdkoœæ + pewien u³amek aktualnej prêdkoœci œwiata
                        // To sprawi, ¿e NPC bêd¹ generalnie szybsze, gdy gra przyspiesza.
                        // Wspó³czynnik (np. 0.5f) mo¿na dostosowaæ.
                        newCar.speed = NpcSpeedForLane + motoristVirtualSpeed * 0.2f;
                        newCar.speed = Clamp(newCar.speed, MIN_NPC_SPEED, MAX_NPC_SPEED + VIRTUAL_SPEED_MAX * 0.3f); // Ogranicz prêdkoœæ NPC


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
                            if (std::abs(existingCar.position.y - newCar.position.y) < 1.0f && existingCar.movingLeft == newCar.movingLeft) {
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

                motoristCars.erase(std::remove_if(motoristCars.begin(), motoristCars.end(), [&](const Car& car) {
                    return (car.movingLeft && car.position.x < -npcTextureWidth * 1.5f) || (!car.movingLeft && car.position.x > virtualScreenWidth + npcTextureWidth * 1.5f);
                    }), motoristCars.end());

                motoristBgScroll -= motoristVirtualSpeed * backgroundScrollMultiplier * dt;
                if (resources.bg.id > 0) {
                    float scaledBgWidth = (float)resources.bg.width * baseScaleY;
                    if (motoristBgScroll <= -scaledBgWidth) {
                        motoristBgScroll += scaledBgWidth;
                    }
                }

                if (isSpinning) {
                    spinTimer += dt;
                    if (spinTimer >= spinDurationPerFrame) {
                        spinTimer -= spinDurationPerFrame;
                        spinFrame++;
                        if (!resources.carSpinFrames.empty() && spinFrame >= resources.carSpinFrames.size()) {
                            isSpinning = false;
                            spinFrame = 0;
                        }
                        else if (resources.carSpinFrames.empty()) {
                            isSpinning = false;
                            spinFrame = 0;
                        }
                    }
                }
            }
            break;

        case GAME_OVER_STATE:
            if (IsKeyPressed(KEY_R)) {
                currentGameState = COUNTDOWN_INITIAL_DELAY;
                countdownTimer = 2.0f;
                playerLives = 5;
                motoristPlayerPos = { virtualScreenWidth * 0.1f, (motoristRoadYTop + motoristRoadYBottom) / 2.0f };
                motoristPlayerVel = { 0.0f, 0.0f };
                motoristVirtualSpeed = 0.0f;
                motoristCars.clear();
                isSpinning = false;
                spinFrame = 0;
                spinTimer = 0.0f;
                npcSpawnTimer = 0.0f;
                motoristBgScroll = 0.0f;
                collisionDetectedThisFrame = false;
                currentLap = 0;
                distanceThisLap = 0.0f;
                showLapAnim = false;
                lapAnimBlinkCount = 0;

                if (resources.backgroundMusicLoaded && IsMusicStreamPlaying(resources.backgroundMusic)) {
                    StopMusicStream(resources.backgroundMusic);
                    SeekMusicStream(resources.backgroundMusic, 0.0f);
                }

                if (resources.bgNoiseLoaded && IsMusicStreamPlaying(resources.bgNoise))
                    StopMusicStream(resources.bgNoise);
                
                playBgNoiseLoop = false; // Zresetuj flagê
                TraceLog(LOG_INFO, "MOTORIST: Game Restarted.");
            }
            break;
        }

        BeginTextureMode(target);
        ClearBackground(BLACK);

        if (resources.bg.id > 0) {
            float scaleYBg = (float)virtualScreenHeight / resources.bg.height;
            float scaledBgWidth = (float)resources.bg.width * scaleYBg;
            float currentScroll = (currentGameState == PLAYING || currentGameState == GAME_OVER_STATE) ? motoristBgScroll : 0.0f; // Scrolluj te¿ na game over
            for (float i = -1; i < (virtualScreenWidth / scaledBgWidth) + 2; ++i) { // +2 dla pewnoœci
                DrawTexturePro(resources.bg,
                    { 0.0f, 0.0f, (float)resources.bg.width, (float)resources.bg.height },
                    { currentScroll + i * scaledBgWidth, 0.0f, scaledBgWidth, (float)virtualScreenHeight },
                    { 0.0f, 0.0f }, 0.0f, WHITE);
            }
        }

        if (currentGameState == PLAYING || currentGameState == GAME_OVER_STATE) { // Rysuj NPC te¿ na game over
            for (const auto& car : motoristCars) {
                Texture2D textureToDraw = (car.movingLeft) ? resources.leftCarNPC : resources.rightCarNPC;
                if (textureToDraw.id > 0) {
                    DrawTexturePro(textureToDraw,
                        { 0, 0, (float)textureToDraw.width, (float)textureToDraw.height },
                        { car.position.x, car.position.y, npcTextureWidth, npcTextureHeight },
                        { npcHalfWidth, npcHalfHeight }, 0.0f, WHITE);
                }
                if (showDebugInfo) DrawRectangleLinesEx(car.collisionBox, 1.0f, RED);
            }
        }

        if (currentGameState == PLAYING || currentGameState == COUNTDOWN_POST_GO_DELAY || currentGameState == COUNTDOWN_SHOW_GO || currentGameState == GAME_OVER_STATE) { // Rysuj gracza te¿ na game over
            if (isSpinning && !resources.carSpinFrames.empty() && spinFrame >= 0 && spinFrame < resources.carSpinFrames.size() && (currentGameState == PLAYING || currentGameState == GAME_OVER_STATE)) {
                Texture2D spinTex = resources.carSpinFrames[spinFrame];
                if (spinTex.id > 0) {
                    DrawTexturePro(spinTex, { 0, 0, (float)spinTex.width, (float)spinTex.height },
                        { motoristPlayerPos.x, motoristPlayerPos.y, playerTextureWidth, playerTextureHeight },
                        { playerHalfWidth, playerHalfHeight }, 0.0f, WHITE);
                }
            }
            else if (resources.car.id > 0) {
                DrawTexturePro(resources.car, { 0, 0, (float)resources.car.width, (float)resources.car.height },
                    { motoristPlayerPos.x, motoristPlayerPos.y, playerTextureWidth, playerTextureHeight },
                    { playerHalfWidth, playerHalfHeight }, 0.0f, WHITE);
            }
        }


    // --- RYSOWANIE ANIMACJI UKOÑCZENIA OKR¥¯ENIA ---
        if (showLapAnim) {
            // Pobierz wymiary tekstury
            float lapAnimTextureWidth = (float)resources.lap.width;
            float lapAnimTextureHeight = (float)resources.lap.height;

            // Oblicz wymiary i pozycjê rysowania uwzglêdniaj¹c skalê
            float drawWidth = lapAnimTextureWidth * LAP_ANIM_SCALE;
            float drawHeight = lapAnimTextureHeight * LAP_ANIM_SCALE;
            Vector2 drawPos = {
                virtualScreenWidth / 2.0f - drawWidth / 2.0f,
                virtualScreenHeight / 2.0f - drawHeight / 2.0f // Œrodek ekranu (mo¿esz zmieniæ pozycjê)
            };

            DrawTextureEx(resources.lap, drawPos, 0.0f, LAP_ANIM_SCALE, WHITE);
        }


        Texture2D textureToDrawForCountdown = { 0 };
        float countdownImageScale = 0.5f;
        switch (currentGameState) {
        case COUNTDOWN_SHOW_3: textureToDrawForCountdown = resources.three; break;
        case COUNTDOWN_SHOW_2: textureToDrawForCountdown = resources.two; break;
        case COUNTDOWN_SHOW_1: textureToDrawForCountdown = resources.one; break;
        case COUNTDOWN_SHOW_GO: textureToDrawForCountdown = resources.go; break;
        default: break;
        }
        if (textureToDrawForCountdown.id > 0) {
            float imgWidth = (float)textureToDrawForCountdown.width * countdownImageScale;
            float imgHeight = (float)textureToDrawForCountdown.height * countdownImageScale;
            Vector2 imgPos = { virtualScreenWidth / 2.0f - imgWidth / 2.0f, virtualScreenHeight / 2.0f - imgHeight / 2.0f };
            DrawTextureEx(textureToDrawForCountdown, imgPos, 0.0f, countdownImageScale, WHITE);
        }

        if (currentGameState == PLAYING || currentGameState == GAME_OVER_STATE) {
            int displayGameSpeed = static_cast<int>(motoristVirtualSpeed * 0.1f); // Konwersja na "pseudo km/h" (mno¿nik do dostosowania)
            std::string speedText = "SPEED: " + std::to_string(displayGameSpeed);
            std::string lapText = "LAP: " + std::to_string(currentLap);
            Font currentFont = resources.gameFont;
            Vector2 speedTextSize = MeasureTextEx(currentFont, speedText.c_str(), UI_TEXT_SIZE_INFO, UI_TEXT_SPACING);
            DrawTextEx(currentFont, speedText.c_str(), { UI_MARGIN, UI_MARGIN }, UI_TEXT_SIZE_INFO, UI_TEXT_SPACING, WHITE);
            DrawTextEx(currentFont, lapText.c_str(), { UI_MARGIN, UI_MARGIN + speedTextSize.y + UI_LINE_SPACING }, UI_TEXT_SIZE_INFO, UI_TEXT_SPACING, WHITE);

            // Lives (prawy górny róg)
            std::string livesTextStr = "LIVES: " + std::to_string(playerLives);
            float livesTextSizeValue = 20.0f;
            Font livesFont = GetFontDefault(); // Mo¿esz u¿yæ resources.gameFont
            float livesTextSpacingValue = 1.0f;
            Vector2 livesTextDimensions = MeasureTextEx(livesFont, livesTextStr.c_str(), livesTextSizeValue, livesTextSpacingValue);
            DrawTextEx(livesFont, livesTextStr.c_str(), { virtualScreenWidth - livesTextDimensions.x - UI_MARGIN, UI_MARGIN }, livesTextSizeValue, livesTextSpacingValue, WHITE);
        }

        if (showDebugInfo && (currentGameState == PLAYING || currentGameState == GAME_OVER_STATE)) {
            DrawRectangleLinesEx(playerRect, 1.0f, LIME);
        }

        if (currentGameState == GAME_OVER_STATE) {
            DrawRectangle(0, 0, virtualScreenWidth, virtualScreenHeight, Fade(BLACK, 0.5f));
            DrawText("GAME OVER", virtualScreenWidth / 2 - MeasureText("GAME OVER", 40) / 2, virtualScreenHeight / 2 - 60, 40, RED);
            DrawText("Press R to Restart", virtualScreenWidth / 2 - MeasureText("Press R to Restart", 20) / 2, virtualScreenHeight / 2 + 0, 20, WHITE);
            DrawText("Press ESC to Menu", virtualScreenWidth / 2 - MeasureText("Press ESC to Menu", 20) / 2, virtualScreenHeight / 2 + 30, 20, LIGHTGRAY);
        }
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        float finalScale = std::min((float)GetScreenWidth() / virtualScreenWidth, (float)GetScreenHeight() / virtualScreenHeight);
        float finalOffsetX = ((float)GetScreenWidth() - (virtualScreenWidth * finalScale)) * 0.5f;
        float finalOffsetY = ((float)GetScreenHeight() - (virtualScreenHeight * finalScale)) * 0.5f;
        Rectangle src = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
        Rectangle dst = { finalOffsetX, finalOffsetY, virtualScreenWidth * finalScale, virtualScreenHeight * finalScale };

        if (applyShader && postProcessingShader.id > 0) {
            if (shaderTimeLoc != -1) {
                float currentTime = (float)GetTime();
                SetShaderValue(postProcessingShader, shaderTimeLoc, &currentTime, SHADER_UNIFORM_FLOAT);
            }
            BeginShaderMode(postProcessingShader);
            DrawTexturePro(target.texture, src, dst, { 0,0 }, 0.0f, WHITE);
            EndShaderMode();
        }
        else {
            DrawTexturePro(target.texture, src, dst, { 0,0 }, 0.0f, WHITE);
        }

        if (showDebugInfo) {
            int debugY = 10; int lineHeight = 15;
            DrawRectangle(5, 5, 220, 170, Fade(DARKGRAY, 0.7f));
            DrawText(TextFormat("FPS: %d", GetFPS()), GetScreenWidth() - 90, debugY, 20, LIME);
            if (currentGameState == PLAYING || currentGameState == GAME_OVER_STATE || currentGameState == COUNTDOWN_POST_GO_DELAY || currentGameState == COUNTDOWN_SHOW_GO) {
                DrawText(TextFormat("PlayerPos: %.0f, %.0f", motoristPlayerPos.x, motoristPlayerPos.y), 10, debugY, 10, GREEN); debugY += lineHeight;
            }
            if (currentGameState == PLAYING || currentGameState == GAME_OVER_STATE) {
                DrawText(TextFormat("VirtualSpeed: %.1f", motoristVirtualSpeed), 10, debugY, 10, YELLOW); debugY += lineHeight;
                DrawText(TextFormat("NPCs: %zu", motoristCars.size()), 10, debugY, 10, GREEN); debugY += lineHeight;
                DrawText(TextFormat("Collision: %s", collisionDetectedThisFrame ? "YES" : "NO"), 10, debugY, 10, collisionDetectedThisFrame ? RED : WHITE); debugY += lineHeight;
                DrawText(TextFormat("Spinning: %s [%d]", isSpinning ? "YES" : "NO", spinFrame), 10, debugY, 10, isSpinning ? ORANGE : WHITE); debugY += lineHeight;
                DrawText(TextFormat("Current Lap: %d", currentLap), 10, debugY, 10, SKYBLUE); debugY += lineHeight;
                DrawText(TextFormat("Dist This Lap: %.1f", distanceThisLap), 10, debugY, 10, SKYBLUE); debugY += lineHeight;
            }
            if (resources.backgroundMusicLoaded) DrawText(TextFormat("Music: %.1f/%.1f", GetMusicTimePlayed(resources.backgroundMusic), GetMusicTimeLength(resources.backgroundMusic)), 10, debugY, 10, PURPLE); debugY += lineHeight;
            DrawText(TextFormat("State: %d, CTimer: %.2f", currentGameState, countdownTimer), 10, debugY, 10, GOLD); debugY += lineHeight;
            DrawText(TextFormat("Lives: %d", playerLives), 10, debugY, 10, (playerLives > 0) ? GREEN : RED);
        }
        EndDrawing();
    }

    TraceLog(LOG_INFO, "MOTORIST: Exiting game loop.");
    if (resources.backgroundMusicLoaded && IsMusicStreamPlaying(resources.backgroundMusic)) {
        StopMusicStream(resources.backgroundMusic);
    }
    UnloadRenderTexture(target);
    UnloadMotoristResources(resources);

    return 0;
}