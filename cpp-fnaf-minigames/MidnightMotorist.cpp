#include "raylib.h"

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

// Proporcje Y œrodków pasów wzglêdem domyœlnej wysokoœci t³a
const float laneCenterY1Ratio = 143.0f / defaultBackgroundHeight;
const float laneCenterY2Ratio = 403.0f / defaultBackgroundHeight;
const float laneCenterY3Ratio = 663.0f / defaultBackgroundHeight;
const float laneCenterY4Ratio = 929.0f / defaultBackgroundHeight;

// Konfiguracja poziomu Midnight Motorist
const float virtualScreenWidth = 960.0f; // Przyk³ad aktualnych wirtualnych wymiarów
const float virtualScreenHeight = 540.0f; // Przyk³ad aktualnych wirtualnych wymiarów
const float motoristLevelLogicalWidth = 5000.0f;
const float motoristRoadMargin = 25.0f;
const float motoristRoadYTop = motoristRoadMargin; // Zmniejszamy górn¹ granicê
const float motoristRoadYBottom = virtualScreenHeight - motoristRoadMargin; // Zwiêkszamy doln¹ granicê
const float motoristPlayerSpeed = 5.0f;
const float npcSpeed = 4.0f; // Nieco zwiêkszona jednolita prêdkoœæ dla utrudnienia
const float npcSpawnRate = 1.5f; // Œrednia liczba samochodów na sekundê
const float minNpcSpacing = 80.0f; // Minimalny odstêp miêdzy samochodami

// Konfiguracja spawnowania NPC z uwzglêdnieniem proporcji
std::vector<float> npcSpawnYPositions = {
    virtualScreenHeight * laneCenterY1Ratio,
    virtualScreenHeight * laneCenterY2Ratio,
    virtualScreenHeight * laneCenterY3Ratio,
    virtualScreenHeight * laneCenterY4Ratio
};
std::uniform_int_distribution<> spawnYDist(0, npcSpawnYPositions.size() - 1);

// Struktura dla samochodów NPC
struct Car {
    Vector2 position;
    float speed;
    bool movingLeft;
    int textureIndex;
    Rectangle collisionBox; // Wirtualny hitbox
};

int runMidnightMotorist(GraphicsQuality quality) {
    std::cout << "Uruchamiam Midnight Motorist z jakoœci¹: " << quality << std::endl;


    // --------- RESOURCE LOADER ---------
    MotoristGameResources resources = LoadMotoristResources(quality);

    RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    // --------- INICJALIZACJA GRY MIDNIGHT MOTORIST ---------

    // Domyœlne rozmiary gracza (przy za³o¿eniu t³a 1920x1080) - NOWE WARTOŒCI
    const float defaultPlayerWidth = 160.0f;
    const float defaultPlayerHeight = 131.0f;

    // Domyœlne rozmiary NPC (przy za³o¿eniu t³a 1920x1080) - NOWE WARTOŒCI
    const float defaultNpcWidth = 160.0f;
    const float defaultNpcHeight = 131.0f;

    // Dodajemy margines do hitboxów NPC
    const float npcCollisionMarginX = 15.0f;
    const float npcCollisionMarginY = 10.0f;

    // Obliczanie wspó³czynnika skalowania dla osi Y (u¿ywamy go do skalowania wysokoœci)
    const float scaleY = (float)virtualScreenHeight / defaultBackgroundHeight;
    const float scaleX = scaleY; // Zak³adamy, ¿e chcemy zachowaæ proporcje, wiêc skalujemy szerokoœæ tak samo

    // Obliczanie relatywnych rozmiarów gracza
    const float playerTextureWidth = defaultPlayerWidth * scaleX;
    const float playerTextureHeight = defaultPlayerHeight * scaleY;
    const float playerHalfWidth = playerTextureWidth / 2.0f;
    const float playerHalfHeight = playerTextureHeight / 2.0f;
    Rectangle playerRect;

    // Obliczanie relatywnych rozmiarów NPC
    const float npcTextureWidth = defaultNpcWidth * scaleX;
    const float npcTextureHeight = defaultNpcHeight * scaleY;
    const float npcHalfWidth = npcTextureWidth / 2.0f;
    const float npcHalfHeight = npcTextureHeight / 2.0f;

    Vector2 motoristPlayerPos = { virtualScreenWidth * 0.1f, (motoristRoadYTop + motoristRoadYBottom) / 2.0f };
    Vector2 motoristPlayerVel = { 0.0f, 0.0f };
    float motoristVirtualSpeed = motoristPlayerSpeed;

    std::vector<Car> motoristCars;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> textureDist(0, 1);
    std::uniform_real_distribution<> spawnDist(0.0f, 1.0f); // Szansa na spawn w ka¿dej klatce

    float motoristBgScroll = 0.0f;
    float motoristBgScrollSpeed = 0.1f;
    float npcSpawnTimer = 0.0f;

    bool motoristGameOver = false;
    bool showDebugInfo = false;
    bool collisionDetected = false;
    float accelerationRate = 20.0f;
    bool isSpinning = false;
    int spinFrame = 0;
    float spinTimer = 0.0f;
    float spinDurationPerFrame = 0.05f;

    PlayMusicStream(resources.backgroundMusic);
    SetMusicVolume(resources.backgroundMusic, 0.5f);


    // --------------- MAIN GAME LOOP ---------------
    while (!WindowShouldClose()) {
        static bool musicShouldLoop = true; // Dodajemy statyczn¹ flagê

        if (resources.backgroundMusicLoaded) {
            UpdateMusicStream(resources.backgroundMusic);
            if (musicShouldLoop && GetMusicTimePlayed(resources.backgroundMusic) >= GetMusicTimeLength(resources.backgroundMusic) - 0.05f) {
                SeekMusicStream(resources.backgroundMusic, 0.0f);
                PlayMusicStream(resources.backgroundMusic);
                musicShouldLoop = false; // Ustawiamy flagê, aby unikn¹æ ponownych wywo³añ w tej samej klatce
            }
            // Resetujemy flagê, jeœli muzyka nie jest ju¿ odtwarzana (sprawdzamy stan)
            if (GetMusicTimePlayed(resources.backgroundMusic) <= 0.0f || IsMusicStreamPlaying(resources.backgroundMusic) == false) {
                musicShouldLoop = true;
            }
        }

        // --------- INPUT ---------
        if (IsKeyPressed(KEY_F3)) {
            showDebugInfo = !showDebugInfo;
        }

        // --------- UPDATE MIDNIGHT MOTORIST ---------
        if (!motoristGameOver) {
            // Player Input
            motoristPlayerVel = { 0.0f, 0.0f };
            if (IsKeyDown(KEY_A)) motoristPlayerVel.x = -motoristPlayerSpeed;
            if (IsKeyDown(KEY_D)) motoristPlayerVel.x = motoristPlayerSpeed;
            if (IsKeyDown(KEY_W)) motoristPlayerVel.y = -motoristPlayerSpeed;
            if (IsKeyDown(KEY_S)) motoristPlayerVel.y = motoristPlayerSpeed;

            motoristPlayerPos.x += motoristPlayerVel.x;
            motoristPlayerPos.y += motoristPlayerVel.y;

            // Ograniczenie ruchu gracza
            if (motoristPlayerPos.y < motoristRoadYTop + playerHalfHeight) motoristPlayerPos.y = motoristRoadYTop + playerHalfHeight;
            if (motoristPlayerPos.y > motoristRoadYBottom - playerHalfHeight) motoristPlayerPos.y = motoristRoadYBottom - playerHalfHeight;
            if (motoristPlayerPos.x < playerHalfWidth) motoristPlayerPos.x = playerHalfWidth;
            if (motoristPlayerPos.x > virtualScreenWidth - playerHalfWidth) motoristPlayerPos.x = virtualScreenWidth - playerHalfWidth;

            // Aktualizacja prostok¹ta gracza
            playerRect = { motoristPlayerPos.x - playerHalfWidth, motoristPlayerPos.y - playerHalfHeight, playerTextureWidth, playerTextureHeight };

            collisionDetected = false;
            // Kolizje z NPC
            for (auto& car : motoristCars) {
                if (CheckCollisionRecs(playerRect, car.collisionBox)) {
                    if (!isSpinning) {
                        isSpinning = true;
                        spinFrame = 0;
                        motoristVirtualSpeed = motoristPlayerSpeed;
                        if (resources.carCrashSoundLoaded) PlaySound(resources.carCrash);
                        // motoristGameOver = true;
                    }
                    collisionDetected = true;
                    break;
                }
            }

            // Aktualizacja wirtualnej prêdkoœci
            if (!isSpinning) {
                motoristVirtualSpeed += accelerationRate * GetFrameTime();
                if (motoristVirtualSpeed > motoristPlayerSpeed * 100.0f) motoristVirtualSpeed = motoristPlayerSpeed * 100.0f;
            }
            else {
                motoristVirtualSpeed = motoristPlayerSpeed;
            }

            // Spawnowanie NPC
            npcSpawnTimer += GetFrameTime();
            if (npcSpawnTimer > 1.0f / npcSpawnRate) {
                npcSpawnTimer -= 1.0f / npcSpawnRate;
                if (spawnDist(gen) < 0.75f) { // Zwiêkszamy szansê na spawn
                    Car newCar;
                    int randomLaneIndex = spawnYDist(gen);
                    newCar.position.y = npcSpawnYPositions[randomLaneIndex];
                    newCar.speed = npcSpeed;

                    // Okreœl kierunek i teksturê na podstawie wylosowanego pasa
                    if (randomLaneIndex == 0 || randomLaneIndex == 1) { // Górne pasy - jad¹ w lewo
                        newCar.movingLeft = true;
                        newCar.textureIndex = 1;
                        newCar.position.x = virtualScreenWidth + 50;
                    }
                    else { // Dolne pasy - jad¹ w prawo
                        newCar.movingLeft = false;
                        newCar.textureIndex = 0;
                        newCar.position.x = -50 - npcTextureWidth;
                    }

                    // SprawdŸ minimalny odstêp od ostatnio dodanego samochodu na tym samym pasie
                    bool canSpawn = true;
                    for (const auto& existingCar : motoristCars) {
                        if (std::abs(existingCar.position.y - newCar.position.y) < 1.0f) { // Ten sam pas
                            if (newCar.movingLeft && existingCar.movingLeft && newCar.position.x > existingCar.position.x && newCar.position.x - existingCar.position.x < minNpcSpacing) {
                                canSpawn = false;
                                break;
                            }
                            if (!newCar.movingLeft && !existingCar.movingLeft && existingCar.position.x > newCar.position.x && existingCar.position.x - newCar.position.x < minNpcSpacing) {
                                canSpawn = false;
                                break;
                            }
                        }
                    }

                    if (canSpawn) {
                        newCar.collisionBox = { newCar.position.x - npcHalfWidth + npcCollisionMarginX, newCar.position.y - npcHalfHeight + npcCollisionMarginY, npcTextureWidth - 2 * npcCollisionMarginX, npcTextureHeight - 2 * npcCollisionMarginY };
                        motoristCars.push_back(newCar);
                    }
                }
            }

            // Aktualizacja pozycji NPC
            for (auto& car : motoristCars) {
                car.position.x += car.movingLeft ? -car.speed : car.speed;
                car.collisionBox.x = car.position.x - npcHalfWidth + npcCollisionMarginX;
            }

            // Usuwanie NPC poza ekranem
            motoristCars.erase(std::remove_if(motoristCars.begin(), motoristCars.end(), [&](const Car& car) {
                return (car.movingLeft && car.position.x < -100) || (!car.movingLeft && car.position.x > virtualScreenWidth + 100);
                }), motoristCars.end());

            // T³o - scrollowanie
            float backgroundScrollMultiplier = 2.0f; // Przyk³adowy mno¿nik
            motoristBgScroll -= motoristVirtualSpeed * backgroundScrollMultiplier * GetFrameTime();

            if (motoristBgScroll < -resources.bg.width) {
                motoristBgScroll += resources.bg.width;
            }

            // Aktualizacja animacji obrotu
            if (isSpinning) {
                spinTimer += GetFrameTime();
                if (spinTimer >= spinDurationPerFrame) {
                    spinTimer -= spinDurationPerFrame;
                    spinFrame++;
                    if (spinFrame >= 19) {
                        isSpinning = false;
                    }
                }
            }
        }
        else {
            if (IsKeyPressed(KEY_R)) {
                motoristGameOver = false;
                motoristPlayerPos = { virtualScreenWidth * 0.1f, (motoristRoadYTop + motoristRoadYBottom) / 2.0f };
                motoristVirtualSpeed = motoristPlayerSpeed;
                motoristCars.clear();
                isSpinning = false;
                spinFrame = 0;
                spinTimer = 0.0f;
                npcSpawnTimer = 0.0f;
            }
        }

        // --------- DRAW MIDNIGHT MOTORIST ---------
        BeginTextureMode(target);
        ClearBackground(BLACK);

        // Rysuj t³o (powtarzaj¹ce siê - "oszustwo z kafelkami")
        if (resources.bg.id > 0) {
            float scaleY = (float)virtualScreenHeight / resources.bg.height;
            float scaledBgWidth = (float)resources.bg.width * scaleY;
            int numTiles = 3; // Rysujemy 3 kafelki obok siebie

            for (int i = 0; i < numTiles; ++i) {
                float drawX = motoristBgScroll + i * scaledBgWidth;
                Rectangle srcRect = { 0.0f, 0.0f, (float)resources.bg.width, (float)resources.bg.height };
                Rectangle destRect = { drawX, 0.0f, scaledBgWidth, (float)virtualScreenHeight };
                DrawTexturePro(resources.bg, srcRect, destRect, { 0.0f, 0.0f }, 0.0f, WHITE);
            }

            // Korekcja przesuniêcia, aby t³o siê "zawija³o"
            if (motoristBgScroll < -scaledBgWidth) {
                motoristBgScroll += scaledBgWidth;
            }
        }

        // Rysuj samochody NPC
        for (const auto& car : motoristCars) {
            Texture2D textureToDraw;
            if (car.movingLeft) {
                textureToDraw = resources.leftCarNPC;
            }
            else {
                textureToDraw = resources.rightCarNPC;
            }
            DrawTexturePro(textureToDraw, { 0, 0, (float)textureToDraw.width, (float)textureToDraw.height }, { car.position.x, car.position.y, npcTextureWidth, npcTextureHeight }, { npcHalfWidth, npcHalfHeight }, 0.0f, WHITE);

            // Rysuj wirtualny hitbox (tylko do debugowania)
            if (showDebugInfo) {
                DrawRectangleLinesEx(car.collisionBox, 2.0f, RED);
            }
        }

        // Rysuj gracza
        if (isSpinning && spinFrame >= 0 && spinFrame < resources.carSpinFrames.size()) {
            DrawTexturePro(resources.carSpinFrames[spinFrame], { 0, 0, (float)resources.carSpinFrames[spinFrame].width, (float)resources.carSpinFrames[spinFrame].height }, { motoristPlayerPos.x, motoristPlayerPos.y, playerTextureWidth, playerTextureHeight }, { playerHalfWidth, playerHalfHeight }, 0.0f, WHITE);
        }
        else {
            DrawTexturePro(resources.car, { 0, 0, (float)resources.car.width, (float)resources.car.height }, { motoristPlayerPos.x, motoristPlayerPos.y, playerTextureWidth, playerTextureHeight }, { playerHalfWidth, playerHalfHeight }, 0.0f, WHITE);
        }

        if (motoristGameOver) {
            DrawText("GAME OVER", virtualScreenWidth / 2 - MeasureText("GAME OVER", 40) / 2, virtualScreenHeight / 2 - 20, 40, RED);
            DrawText("Press R to Restart", virtualScreenWidth / 2 - MeasureText("Press R to Restart", 20) / 2, virtualScreenHeight / 2 + 20, 20, WHITE);
        }

        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        float finalScale =
            std::min((float)GetScreenWidth() / virtualScreenWidth, (float)GetScreenHeight() / virtualScreenHeight);
        float finalOffsetX = ((float)GetScreenWidth() - (virtualScreenWidth * finalScale)) * 0.5f;
        float finalOffsetY = ((float)GetScreenHeight() - (virtualScreenHeight * finalScale)) * 0.5f;
        Rectangle src = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
        Rectangle dst = { finalOffsetX, finalOffsetY, (float)virtualScreenWidth * finalScale, (float)virtualScreenHeight * finalScale };
        DrawTexturePro(target.texture, src, dst, { 0, 0 }, 0.0f, WHITE);

        // ------- DEBUG DRAW -------
        if (showDebugInfo) {
            int debugY = 10;
            int lineHeight = 20;
            DrawText(TextFormat("Player Pos X: %.2f", motoristPlayerPos.x), 10, debugY, 20, GREEN);
            debugY += lineHeight;
            DrawText(TextFormat("Player Pos Y: %.2f", motoristPlayerPos.y), 10, debugY, 20, GREEN);
            debugY += lineHeight;
            DrawText(TextFormat("Player Vel X: %.2f", motoristPlayerVel.x), 10, debugY, 20, GREEN);
            debugY += lineHeight;
            DrawText(TextFormat("Player Vel Y: %.2f", motoristPlayerVel.y), 10, debugY, 20, GREEN);
            debugY += lineHeight;
            DrawText(TextFormat("Virtual Speed: %.2f", motoristVirtualSpeed), 10, debugY, 20, YELLOW);
            debugY += lineHeight;
            DrawText(TextFormat("NPC Count: %zu", motoristCars.size()), 10, debugY, 20, GREEN);
            debugY += lineHeight;
            DrawText(TextFormat("Collision: %s", collisionDetected ? "true" : "false"), 10, debugY, 20, RED);
            debugY += lineHeight;
            DrawText(TextFormat("Spinning: %s, Frame: %d", isSpinning ? "true" : "false", spinFrame), 10, debugY, 20, ORANGE);
            debugY += lineHeight;
            DrawText(TextFormat("NPC Spawn Timer: %.2f", npcSpawnTimer), 10, debugY, 20, BLUE);
        }

        EndDrawing();
    }

    ////--- Resource Unload ---
    //UnloadRenderTexture(target);
    //UnloadGameResources(resources);

    //CloseAudioDevice();
    //CloseWindow();

    return 0;
}