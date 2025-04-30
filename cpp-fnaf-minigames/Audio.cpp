//#include "raylib.h"
//#include <cmath>
//#include <string>
//#include <vector>
//#include <algorithm>
//#include "Audio.h" // <<<--- DO£¥CZAMY NAG£ÓWEK AUDIO (kluczowe!)
//
//#ifndef PI
//#define PI 3.14159265358979323846f
//#endif
//
//// --- SEKCJA ANIMACJI GRACZA ---
//// (Funkcja animacji pozostaje w main.cpp, bo dotyczy logiki gracza)
//void UpdatePlayerAnimation(bool isGrounded, bool isMoving, bool facingRight,
//    Texture2D& currentTexture, int& currentFrame,
//    int animUpdateRate, int maxFrames, int& frameCounter,
//    Texture2D idleRight, Texture2D idleLeft,
//    Texture2D walkRight, Texture2D walkLeft,
//    Texture2D jumpRight, Texture2D jumpLeft)
//{
//    frameCounter++;
//    if (frameCounter >= animUpdateRate) {
//        frameCounter = 0;
//        currentFrame++;
//        if (currentFrame >= maxFrames) currentFrame = 0;
//    }
//    if (!isGrounded) {
//        currentTexture = facingRight ? jumpRight : jumpLeft;
//        if (maxFrames == 1) currentFrame = 0; // Jump ma tylko 1 klatkê
//    }
//    else if (isMoving) {
//        currentTexture = facingRight ? walkRight : walkLeft;
//    }
//    else {
//        currentTexture = facingRight ? idleRight : idleLeft;
//    }
//}
//// --- KONIEC SEKCJI ANIMACJI GRACZA ---
//
//
//// --- G£ÓWNA FUNKCJA main ---
//int main()
//{
//    // --- Wirtualna/Logiczna Rozdzielczoœæ Gry ---
//    const int virtualScreenWidth = 800;
//    const int virtualScreenHeight = 450;
//    const float virtualAspectRatio = (float)virtualScreenWidth / (float)virtualScreenHeight;
//
//    // --- Ustawienia Poziomu i Przewijania ---
//    const float levelLogicalWidth = 5000.0f;
//    const float bgScrollFactor = 0.3f;
//
//    // --- Inicjalizacja Okna ---
//    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
//    InitWindow(virtualScreenWidth, virtualScreenHeight, "Chica's Magic Rainbow Land - U¿ywa Audio.h/.cpp");
//    SetWindowMinSize(virtualScreenWidth / 2, virtualScreenHeight / 2);
//    SetTargetFPS(60);
//
//    // --- Inicjalizacja Systemu Audio (wywo³anie funkcji z Audio.h) ---
//    if (!InitializeAudioSystem()) { // <<<--- Wywo³anie funkcji z Audio.h
//        TraceLog(LOG_ERROR, "MAIN: Failed to initialize audio system. Exiting.");
//        CloseWindow();
//        return 1;
//    }
//
//    // --- Inicjalizacja Render Target ---
//    RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
//    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
//
//    // --- £adowanie Tekstur (Gracz, T³o, Kafelki, Przycisk, NPC) ---
//    // (Logika ³adowania tekstur pozostaje w main.cpp)
//    Texture2D tile = LoadTexture("resources/textures/misc/dirt_tile.png");
//    Texture2D bg = LoadTexture("resources/background.png");
//    Texture2D idleRight = LoadTexture("resources/textures/sprites/chica/chica_idle_right.png");
//    Texture2D idleLeft = LoadTexture("resources/textures/sprites/chica/chica_idle_left.png");
//    Texture2D walkRight = LoadTexture("resources/textures/sprites/chica/chica_walk_right.png");
//    Texture2D walkLeft = LoadTexture("resources/textures/sprites/chica/chica_walk_left.png");
//    Texture2D jumpRight = LoadTexture("resources/textures/sprites/chica/chica_jump_right.png");
//    Texture2D jumpLeft = LoadTexture("resources/textures/sprites/chica/chica_jump_left.png");
//    Texture2D buttonVoicesOn = LoadTexture("resources/textures/text/voices_on.png");
//    Texture2D buttonVoicesOff = LoadTexture("resources/textures/text/voices_off.png");
//    Texture2D npcBodyTexture = LoadTexture("resources/textures/sprites/rainbow/chica_rainbow.png");
//    Texture2D npcDialogueBodyTexture = LoadTexture("resources/textures/sprites/rainbow/chica_rainbow_pissed.png");
//    const int numEyeSprites = 32;
//    std::vector<Texture2D> npcEyeTextures(numEyeSprites);
//    TraceLog(LOG_INFO, "Rêczne ³adowanie tekstur oczu NPC (sta³e œcie¿ki)...");
//    for (int i = 0; i < numEyeSprites; ++i) {
//        std::string eyePath = "resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-" + std::to_string(i) + " Frame-0.png";
//        npcEyeTextures[i] = LoadTexture(eyePath.c_str());
//        if (npcEyeTextures[i].id == 0) {
//            TraceLog(LOG_WARNING, "Nie uda³o siê za³adowaæ tekstury oka NPC: %s", eyePath.c_str());
//        }
//    }
//    TraceLog(LOG_INFO, "Zakoñczono próbê ³adowania tekstur oczu NPC.");
//
//    // --- Sta³e i Zmienne Gracza ---
//    // (Logika gracza pozostaje w main.cpp)
//    const float playerTextureHeight = (float)idleRight.height;
//    const float playerTextureWidth = (float)idleRight.width / 16;
//    const float groundLevelY = virtualScreenHeight - (float)tile.height;
//    const float playerVirtualScreenX = virtualScreenWidth / 2.0f - playerTextureWidth / 2.0f;
//    Vector2 playerPos = { 50.0f, groundLevelY - playerTextureHeight };
//    Vector2 playerVel = { 0.0f, 0.0f };
//    const float playerSpeed = 240.0f;
//    const float jumpSpeed = -350.0f;
//    const float gravity = 700.0f;
//    bool isGrounded = true;
//    bool facingRight = true;
//    Texture2D currentTexture = idleRight;
//    int currentFrame = 0;
//    int frameCounter = 0;
//    const int animFps = 12;
//    int animUpdateRate = GetTargetFPS() / animFps;
//    const int idleFrames = 16; // Liczba klatek w idleRight/Left
//    const int walkFrames = 16; // Liczba klatek w walkRight/Left
//    const int jumpFrames = 1;  // Liczba klatek w jumpRight/Left
//    int maxFramesForCurrentAnim = idleFrames;
//
//
//    // --- Sta³e i Zmienne NPC ---
//    // (Logika NPC pozostaje w main.cpp)
//    const float npcTargetY = 5.0f;
//    const float npcSpeedMultiplier = 5.0f; // Mno¿nik prêdkoœci œledzenia NPC
//    Vector2 npcPos = { 0.0f, npcTargetY };
//    int currentEyeIndex = 0;
//    float npcBodyWidth = npcBodyTexture.id > 0 ? (float)npcBodyTexture.width : 100.0f;
//    float npcBodyHeight = npcBodyTexture.id > 0 ? (float)npcBodyTexture.height : 50.0f;
//    const float npcLeftEyeOffsetX = 90.0f;
//    const float npcLeftEyeOffsetY = 18.0f;
//    const float npcRightEyeOffsetX = 152.0f;
//    const float npcRightEyeOffsetY = 18.0f;
//    float npcTargetX = 0.0f;
//    bool isNpcInDialogue = false;
//    bool buttonCanBeClicked = true;
//
//    // --- Przewijanie ---
//    float scrollX = 0.0f;
//    float bgScroll = 0.0f;
//    const float maxScrollX = fmaxf(0.0f, levelLogicalWidth - virtualScreenWidth);
//
//    // --- Inicjalizacja pozycji X NPC ---
//    if (npcBodyTexture.id > 0) {
//        npcTargetX = (playerPos.x + playerTextureWidth / 2.0f) - (npcBodyWidth / 2.0f);
//        npcPos.x = npcTargetX;
//    }
//    else {
//        npcPos.x = 50.0f;
//    }
//
//    // --- Debugowanie F3 ---
//    bool showDebugInfo = false;
//
//    // --- Zmienne Przycisku ---
//    const float buttonPadding = 5.0f;
//    const float buttonTextureWidth = (float)buttonVoicesOff.width;
//    const float buttonTextureHeight = (float)buttonVoicesOff.height;
//    Vector2 buttonPos = { buttonPadding, (float)virtualScreenHeight - buttonTextureHeight - buttonPadding };
//    Rectangle buttonRect = { buttonPos.x, buttonPos.y, buttonTextureWidth, buttonTextureHeight };
//    bool isButtonClicked = false;
//    Texture2D currentButtonTexture = buttonVoicesOn;
//
//
//    // --- G³ówna Pêtla Gry ---
//    while (!WindowShouldClose())
//    {
//        float dt = GetFrameTime(); // Czas klatki
//
//        // --- Aktualizacja Muzyki (wywo³anie funkcji z Audio.h) ---
//        UpdateAudioMusicStream(); // <<<--- Wywo³anie funkcji z Audio.h
//
//        // --- Obs³uga Pe³nego Ekranu ---
//        if (IsKeyPressed(KEY_F11)) {
//            int d = GetCurrentMonitor();
//            if (IsWindowFullscreen()) {
//                ToggleFullscreen();
//                SetWindowSize(virtualScreenWidth, virtualScreenHeight);
//            }
//            else {
//                SetWindowSize(GetMonitorWidth(d), GetMonitorHeight(d));
//                ToggleFullscreen();
//            }
//        }
//
//        // --- Przeliczanie pozycji myszy ---
//        Vector2 mousePosWindow = GetMousePosition();
//        float scale = std::min((float)GetScreenWidth() / virtualScreenWidth, (float)GetScreenHeight() / virtualScreenHeight);
//        float renderTargetOffsetX = ((float)GetScreenWidth() - (virtualScreenWidth * scale)) * 0.5f;
//        float renderTargetOffsetY = ((float)GetScreenHeight() - (virtualScreenHeight * scale)) * 0.5f;
//        Vector2 virtualMousePos = {
//            (mousePosWindow.x - renderTargetOffsetX) / scale,
//            (mousePosWindow.y - renderTargetOffsetY) / scale
//        };
//
//        // --- Wejœcie Gracza ---
//        bool isMoving = false;
//        float desiredVelX = 0.0f;
//        if (IsKeyDown(KEY_A)) {
//            desiredVelX = -playerSpeed; isMoving = true; facingRight = false;
//        }
//        if (IsKeyDown(KEY_D)) {
//            desiredVelX = playerSpeed; isMoving = true; facingRight = true;
//        }
//        playerVel.x = desiredVelX;
//
//        if (IsKeyPressed(KEY_F3)) { showDebugInfo = !showDebugInfo; }
//
//        // --- Obs³uga Klikniêcia Przycisku ---
//        if (buttonCanBeClicked && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//            if (CheckCollisionPointRec(virtualMousePos, buttonRect)) {
//                isButtonClicked = true;
//                currentButtonTexture = buttonVoicesOff;
//                isNpcInDialogue = true;
//                buttonCanBeClicked = false;
//                TraceLog(LOG_INFO, "Button turned ON. NPC dialogue mode enabled.");
//                PlayDialogueSoundEffect(); // <<<--- Wywo³anie funkcji z Audio.h
//            }
//        }
//
//        // --- Fizyka Gracza ---
//        if (IsKeyPressed(KEY_W) && isGrounded) {
//            playerVel.y = jumpSpeed;
//            isGrounded = false;
//            PlayJumpSoundEffect(); // <<<--- Wywo³anie funkcji z Audio.h
//        }
//        if (!isGrounded) { playerVel.y += gravity * dt; }
//
//        // --- Aktualizacja Pozycji Gracza ---
//        playerPos.x += playerVel.x * dt;
//        playerPos.y += playerVel.y * dt;
//
//        // --- Ograniczenia Pozycji Gracza ---
//        if (playerPos.x < 0.0f) { playerPos.x = 0.0f; }
//        if (playerPos.x + playerTextureWidth > levelLogicalWidth) { playerPos.x = levelLogicalWidth - playerTextureWidth; }
//
//        // --- Kolizja Gracza z Pod³o¿em ---
//        if (playerPos.y + playerTextureHeight >= groundLevelY && playerVel.y >= 0) {
//            playerPos.y = groundLevelY - playerTextureHeight;
//            playerVel.y = 0;
//            isGrounded = true;
//        }
//
//        // --- Przewijanie Kamery ---
//        targetScrollX = playerPos.x - playerVirtualScreenX;
//        scrollX = fmaxf(0.0f, fminf(targetScrollX, maxScrollX));
//
//        // --- Przewijanie T³a ---
//        bgScroll = scrollX * bgScrollFactor;
//
//        // --- Aktualizacja Logiki NPC ---
//        if (npcBodyTexture.id > 0) {
//            npcTargetX = (playerPos.x + playerTextureWidth / 2.0f) - (npcBodyWidth / 2.0f);
//            float diffX = npcTargetX - npcPos.x;
//            npcPos.x += diffX * npcSpeedMultiplier * dt; // P³ynne œledzenie
//            npcPos.y = npcTargetY;
//
//            if (!isNpcInDialogue) {
//                Vector2 npcCenter = { npcPos.x + npcBodyWidth / 2.0f, npcPos.y + npcBodyHeight / 2.0f };
//                Vector2 playerCenter = { playerPos.x + playerTextureWidth / 2.0f, playerPos.y + playerTextureHeight / 2.0f };
//                float angleVecX = playerCenter.x - npcCenter.x;
//                float angleVecY = playerCenter.y - npcCenter.y;
//                float angleRad = atan2f(-angleVecY, angleVecX);
//                if (angleRad < 0) { angleRad += 2.0f * PI; }
//                currentEyeIndex = (int)roundf(angleRad / (2.0f * PI) * numEyeSprites);
//                currentEyeIndex = currentEyeIndex % numEyeSprites;
//            }
//        }
//
//        // --- Sprawdzenie koñca dialogu ---
//        if (isNpcInDialogue && !IsDialogueAudioPlaying()) { // <<<--- Wywo³anie funkcji z Audio.h
//            if (!buttonCanBeClicked) {
//                TraceLog(LOG_INFO, "NPC dialogue finished.");
//                ResumeBackgroundMusic(); // <<<--- Wywo³anie funkcji z Audio.h
//                buttonCanBeClicked = true;
//                isNpcInDialogue = false;
//                TraceLog(LOG_INFO, "Resuming music and NPC normal behavior.");
//            }
//        }
//
//        // --- Aktualizacja Animacji Gracza ---
//        if (!isGrounded) maxFramesForCurrentAnim = jumpFrames;
//        else if (isMoving) maxFramesForCurrentAnim = walkFrames;
//        else maxFramesForCurrentAnim = idleFrames;
//        UpdatePlayerAnimation(isGrounded, isMoving, facingRight, currentTexture, currentFrame, animUpdateRate, maxFramesForCurrentAnim, frameCounter, idleRight, idleLeft, walkRight, walkLeft, jumpRight, jumpLeft);
//
//        // --- Obliczanie Pozycji Rysowania Gracza ---
//        Vector2 playerDrawVirtualPos = { playerPos.x - scrollX, playerPos.y };
//
//
//        // --- RYSOWANIE (do Render Texture) ---
//        BeginTextureMode(target);
//        ClearBackground(RAYWHITE);
//
//        // Rysowanie T³a
//        if (bg.id > 0) {
//            float bgX = -fmodf(bgScroll, (float)bg.width * ((float)virtualScreenHeight / bg.height)); // Uwzglêdnij skalowanie w fmodf
//            float bgScale = (float)virtualScreenHeight / bg.height;
//            DrawTextureEx(bg, { bgX, 0.0f }, 0.0f, bgScale, WHITE);
//            DrawTextureEx(bg, { bgX + bg.width * bgScale, 0.0f }, 0.0f, bgScale, WHITE);
//        }
//
//        // Rysowanie Pod³o¿a
//        if (tile.id > 0) {
//            int tilesNeeded = (int)ceilf((float)virtualScreenWidth / tile.width) + 2;
//            float tileScrollX = fmodf(scrollX, (float)tile.width);
//            for (int i = -1; i < tilesNeeded; i++) {
//                DrawTexture(tile, (int)((float)i * tile.width - tileScrollX), (int)groundLevelY, WHITE);
//            }
//        }
//
//        // Rysowanie NPC
//        if (npcBodyTexture.id > 0) {
//            Vector2 npcDrawPos = { npcPos.x - scrollX, npcPos.y };
//            Texture2D bodyToDraw = isNpcInDialogue ? npcDialogueBodyTexture : npcBodyTexture;
//            if (bodyToDraw.id > 0) DrawTextureV(bodyToDraw, npcDrawPos, WHITE);
//            else if (npcBodyTexture.id > 0) DrawTextureV(npcBodyTexture, npcDrawPos, WHITE); // Fallback
//
//            if (!isNpcInDialogue && currentEyeIndex >= 0 && currentEyeIndex < numEyeSprites && npcEyeTextures[currentEyeIndex].id > 0) {
//                Texture2D currentEyeTexture = npcEyeTextures[currentEyeIndex];
//                Vector2 leftEyeDrawPos = { npcDrawPos.x + npcLeftEyeOffsetX, npcDrawPos.y + npcLeftEyeOffsetY };
//                Vector2 rightEyeDrawPos = { npcDrawPos.x + npcRightEyeOffsetX, npcDrawPos.y + npcRightEyeOffsetY };
//                DrawTextureV(currentEyeTexture, leftEyeDrawPos, WHITE);
//                DrawTextureV(currentEyeTexture, rightEyeDrawPos, WHITE);
//            }
//        }
//
//        // Rysowanie Gracza
//        Rectangle sourceRect = { (float)currentFrame * playerTextureWidth, 0.0f, playerTextureWidth, playerTextureHeight };
//        DrawTextureRec(currentTexture, sourceRect, playerDrawVirtualPos, WHITE);
//
//        // Rysowanie Przycisku
//        DrawTextureV(currentButtonTexture, buttonPos, WHITE);
//
//        // Rysowanie Debug Info
//        if (showDebugInfo) {
//            DrawRectangle(5, 5, 280, 390, Fade(BLACK, 0.7f));
//            int textY = 10; int lineHeight = 20;
//            DrawText(TextFormat("FPS: %d", GetFPS()), 10, textY, lineHeight, WHITE); textY += lineHeight;
//            DrawText(TextFormat("P.Log Pos: (%.1f, %.1f)", playerPos.x, playerPos.y), 10, textY, lineHeight, LIME); textY += lineHeight;
//            DrawText(TextFormat("P.Draw Pos: (%.1f, %.1f)", playerDrawVirtualPos.x, playerDrawVirtualPos.y), 10, textY, lineHeight, LIME); textY += lineHeight;
//            // ... (pozosta³e linie DrawText dla debug info) ...
//            DrawText(TextFormat("NPC In Dialog: %s", isNpcInDialogue ? "Yes" : "No"), 10, textY, lineHeight, PINK); textY += lineHeight;
//            DrawText(TextFormat("Button Can Click: %s", buttonCanBeClicked ? "Yes" : "No"), 10, textY, lineHeight, ORANGE); textY += lineHeight;
//
//            // U¿ywamy globalnych flag zadeklarowanych w Audio.h
//            // Zak³adamy, ¿e g_musicLoaded, g_jumpSoundLoaded, g_npcDialogueSoundLoaded s¹ dostêpne
//            std::string musicStatus = g_musicLoaded ? "Loaded" : "Failed";
//            std::string soundsStatus = (g_jumpSoundLoaded && g_npcDialogueSoundLoaded) ? "Loaded" : "Partial/Failed";
//            DrawText(TextFormat("Audio: M:%s | S:%s", musicStatus.c_str(), soundsStatus.c_str()), 10, textY, lineHeight, SKYBLUE); textY += lineHeight;
//            DrawText("DEBUG (F3) Full (F11)", virtualScreenWidth - MeasureText("DEBUG (F3) Full (F11)", 20) - 10, 10, 20, WHITE);
//        }
//
//        EndTextureMode(); // Koniec rysowania do Render Texture
//
//        // --- RYSOWANIE NA EKRAN ---
//        BeginDrawing();
//        ClearBackground(BLACK);
//
//        Rectangle renderTextureSource = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
//        Rectangle renderTextureDest = { renderTargetOffsetX, renderTargetOffsetY, (float)virtualScreenWidth * finalScale, (float)virtualScreenHeight * finalScale };
//        DrawTexturePro(target.texture, renderTextureSource, renderTextureDest, { 0, 0 }, 0.0f, WHITE);
//
//        EndDrawing();
//    } // Koniec pêtli while
//
//    // --- Zwolnienie Zasobów ---
//    TraceLog(LOG_INFO, "Zwalnianie zasobów...");
//    UnloadRenderTexture(target);
//    // Zwolnienie tekstur (pozostaje w main.cpp)
//    UnloadTexture(bg); UnloadTexture(tile); UnloadTexture(idleRight); UnloadTexture(idleLeft);
//    UnloadTexture(walkRight); UnloadTexture(walkLeft); UnloadTexture(jumpRight); UnloadTexture(jumpLeft);
//    UnloadTexture(buttonVoicesOff); UnloadTexture(buttonVoicesOn);
//    if (npcBodyTexture.id > 0) UnloadTexture(npcBodyTexture);
//    if (npcDialogueBodyTexture.id > 0) UnloadTexture(npcDialogueBodyTexture);
//    TraceLog(LOG_INFO, "Zwalnianie tekstur oczu NPC...");
//    for (int i = 0; i < numEyeSprites; ++i) { if (npcEyeTextures[i].id > 0) UnloadTexture(npcEyeTextures[i]); }
//    TraceLog(LOG_INFO, "Zasoby tekstur NPC zwolnione.");
//
//    // Zwolnienie Zasobów Audio (wywo³anie funkcji z Audio.h)
//    ShutdownAudioSystem(); // <<<--- Wywo³anie funkcji z Audio.h
//
//    CloseWindow(); // Zamknij okno aplikacji
//    TraceLog(LOG_INFO, "Aplikacja zakoñczona.");
//    return 0;
//}
//// --- KONIEC FUNKCJI main ---