//#include "Game.h"
//#include "Constants.h"
//#include "Animation.h" // Dla UpdatePlayerAnimation
//#include <string>       // Dla TextFormat w C++
//#include <algorithm>    // Dla std::min
//#include <cmath>        // Dla fmaxf itp.
//
//
//Game::Game() : initialized(false) {
//    // Konstruktor mo�e pozosta� pusty lub inicjalizowa� podstawowe warto�ci
//    state.showDebugInfo = false;
//    state.virtualMousePos = { 0, 0 };
//    state.renderScale = 1.0f;
//}
//
//Game::~Game() {
//    // Upewnij si�, �e zasoby s� zwolnione, je�li obiekt zostanie zniszczony
//    // bez wywo�ania Shutdown (chocia� nie powinno si� tak zdarzy�)
//    if (initialized) {
//        Shutdown();
//    }
//}
//
//bool Game::Init() {
//    if (initialized) return true; // Ju� zainicjalizowano
//
//    // --- Inicjalizacja Okna ---
//    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
//    InitWindow(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, "Chica's Magic Rainbow Land - Modular");
//    SetWindowMinSize(VIRTUAL_SCREEN_WIDTH / 2, VIRTUAL_SCREEN_HEIGHT / 2);
//    SetTargetFPS(60);
//
//    // --- Audio ---
//    InitGameAudio(); // Musi by� przed �adowaniem d�wi�k�w
//
//    // --- Render Target ---
//    state.targetRenderTexture = LoadRenderTexture(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);
//    if (state.targetRenderTexture.id == 0) {
//        TraceLog(LOG_ERROR, "GAME: Nie uda�o si� utworzy� Render Texture!");
//        CloseWindow(); // Zamknij okno, bo bez RT nie ma sensu kontynuowa�
//        return false;
//    }
//    SetTextureFilter(state.targetRenderTexture.texture, TEXTURE_FILTER_BILINEAR);
//
//    // --- �adowanie Zasob�w ---
//    LoadResources();
//    if (state.assets.tile.id == 0 || state.assets.bg.id == 0 || state.assets.idleRight.id == 0) {
//        // Podstawowe sprawdzenie, mo�na doda� wi�cej
//        TraceLog(LOG_WARNING, "GAME: Nie uda�o si� za�adowa� podstawowych tekstur.");
//        // Mo�na zdecydowa� o przerwaniu inicjalizacji
//    }
//
//
//    // --- Inicjalizacja Modu��w Gry ---
//    InitLevel(state.level, state.assets.tile, state.assets.bg);
//    InitPlayer(state.player, state.level.groundLevelY);
//    // Ustawienie pocz�tkowej tekstury gracza (teraz w PlayerData)
//    state.player.currentTexture = state.assets.idleRight;
//
//    InitUI(state.ui, state.assets.buttonVoicesOff, state.assets.buttonVoicesOn);
//    LoadGameAudio(state.audio); // Przeka� struktur� audio do za�adowania danych
//
//
//    initialized = true;
//    TraceLog(LOG_INFO, "GAME: Inicjalizacja zako�czona pomy�lnie.");
//    return true;
//}
//
//void Game::LoadResources() {
//    TraceLog(LOG_INFO, "GAME: �adowanie zasob�w...");
//    state.assets.tile = LoadTexture(TILE_PATH);
//    state.assets.bg = LoadTexture(BG_PATH);
//    state.assets.idleRight = LoadTexture(IDLE_RIGHT_PATH);
//    state.assets.idleLeft = LoadTexture(IDLE_LEFT_PATH);
//    state.assets.walkRight = LoadTexture(WALK_RIGHT_PATH);
//    state.assets.walkLeft = LoadTexture(WALK_LEFT_PATH);
//    state.assets.jumpRight = LoadTexture(JUMP_RIGHT_PATH);
//    state.assets.jumpLeft = LoadTexture(JUMP_LEFT_PATH);
//    state.assets.buttonVoicesOff = LoadTexture(BUTTON_VOICES_OFF_PATH);
//    state.assets.buttonVoicesOn = LoadTexture(BUTTON_VOICES_ON_PATH);
//
//    // Podstawowe sprawdzenie b��d�w �adowania
//    if (state.assets.tile.id == 0) TraceLog(LOG_WARNING, "Nie uda�o si� za�adowa�: %s", TILE_PATH);
//    if (state.assets.bg.id == 0) TraceLog(LOG_WARNING, "Nie uda�o si� za�adowa�: %s", BG_PATH);
//    // ... dodaj wi�cej sprawdze� dla innych tekstur
//}
//
//void Game::UnloadResources() {
//    TraceLog(LOG_INFO, "GAME: Zwalnianie zasob�w...");
//    UnloadTexture(state.assets.tile);
//    UnloadTexture(state.assets.bg);
//    UnloadTexture(state.assets.idleRight); UnloadTexture(state.assets.idleLeft);
//    UnloadTexture(state.assets.walkRight); UnloadTexture(state.assets.walkLeft);
//    UnloadTexture(state.assets.jumpRight); UnloadTexture(state.assets.jumpLeft);
//    UnloadTexture(state.assets.buttonVoicesOff); UnloadTexture(state.assets.buttonVoicesOn);
//
//    UnloadGameAudio(state.audio); // Zwolnij zasoby audio
//}
//
//
//void Game::RunLoop() {
//    if (!initialized) {
//        TraceLog(LOG_ERROR, "GAME: Pr�ba uruchomienia p�tli przed inicjalizacj�!");
//        return;
//    }
//
//    while (!WindowShouldClose()) {
//        Update();
//        Draw();
//    }
//}
//
//void Game::Update() {
//    // --- Aktualizacja Strumienia Muzyki ---
//    UpdateGameMusic(state.audio);
//
//    // --- Obs�uga Pe�nego Ekranu ---
//    if (IsKeyPressed(KEY_F11)) {
//        int display = GetCurrentMonitor();
//        if (IsWindowFullscreen()) {
//            ToggleFullscreen();
//            // Po wyj�ciu z FS, przywr�� rozmiar wirtualny jako domy�lny rozmiar okna
//            SetWindowSize(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);
//        }
//        else {
//            // Przed wej�ciem w FS, ustaw rozmiar okna na rozmiar monitora
//            SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
//            ToggleFullscreen();
//        }
//    }
//
//    // --- Przeliczanie pozycji myszy na koordynaty wirtualne ---
//    Vector2 mousePosWindow = GetMousePosition();
//    // Oblicz skal� renderowania (ponownie w ka�dej klatce na wypadek resize)
//    state.renderScale = std::min((float)GetScreenWidth() / VIRTUAL_SCREEN_WIDTH, (float)GetScreenHeight() / VIRTUAL_SCREEN_HEIGHT);
//
//    float virtualMouseX = (mousePosWindow.x - ((float)GetScreenWidth() - (VIRTUAL_SCREEN_WIDTH * state.renderScale)) * 0.5f) / state.renderScale;
//    float virtualMouseY = (mousePosWindow.y - ((float)GetScreenHeight() - (VIRTUAL_SCREEN_HEIGHT * state.renderScale)) * 0.5f) / state.renderScale;
//    state.virtualMousePos = { virtualMouseX, virtualMouseY };
//
//
//    // --- Prze��cznik Debug Info ---
//    if (IsKeyPressed(KEY_F3))
//        state.showDebugInfo = !state.showDebugInfo;
//
//    // --- Aktualizacja Modu��w ---
//    UpdatePlayer(state.player, state.level.groundLevelY, state.level.scrollX, state.level.maxScrollX);
//    UpdateLevelScroll(state.level, state.player);
//    UpdateUI(state.ui, state.virtualMousePos); // Przeka� wirtualn� pozycj� myszy
//
//    // --- Aktualizacja Animacji Gracza ---
//    // Przekazujemy referencj� do PlayerData oraz za�adowane tekstury
//    UpdatePlayerAnimation(state.player, state.assets.idleRight, state.assets.idleLeft,
//        state.assets.walkRight, state.assets.walkLeft,
//        state.assets.jumpRight, state.assets.jumpLeft);
//
//
//    // --- Logika po klikni�ciu przycisku (je�li potrzebna) ---
//    if (state.ui.isButtonClicked) {
//        // Przyk�ad: Mo�na by tu np. zmienia� g�o�no�� muzyki
//        // SetMusicVolume(state.audio.backgroundMusic, state.ui.isButtonClicked ? 0.1f : 0.5f);
//    }
//}
//
//void Game::Draw() {
//    // --- Rysowanie do Wirtualnej Tekstury (Render Texture) ---
//    BeginTextureMode(state.targetRenderTexture);
//    ClearBackground(RAYWHITE); // Czy�� t�o wirtualnego ekranu
//
//    // Rysuj elementy gry u�ywaj�c modu��w
//    DrawLevel(state.level);
//    DrawPlayer(state.player);
//    DrawUI(state.ui);
//
//    // Rysuj Debug Info (je�li w��czone)
//    if (state.showDebugInfo) {
//        DrawDebug();
//    }
//
//    EndTextureMode();
//
//    // --- Rysowanie Wirtualnej Tekstury na Ekranie (skalowanie) ---
//    BeginDrawing();
//    ClearBackground(BLACK); // Czy�� t�o okna (obszar poza render texture)
//
//    // Oblicz pozycj� i rozmiar docelowy render texture na ekranie
//    float destX = ((float)GetScreenWidth() - (VIRTUAL_SCREEN_WIDTH * state.renderScale)) * 0.5f;
//    float destY = ((float)GetScreenHeight() - (VIRTUAL_SCREEN_HEIGHT * state.renderScale)) * 0.5f;
//
//    // Definicja obszaru �r�d�owego (ca�a tekstura, Y odwr�cone w OpenGL)
//    state.renderSourceRect = { 0.0f, 0.0f, (float)state.targetRenderTexture.texture.width, -(float)state.targetRenderTexture.texture.height };
//    // Definicja obszaru docelowego (skalowany i wy�rodkowany)
//    state.renderDestRect = { destX, destY, (float)VIRTUAL_SCREEN_WIDTH * state.renderScale, (float)VIRTUAL_SCREEN_HEIGHT * state.renderScale };
//
//    // Narysuj render texture na ekranie
//    DrawTexturePro(state.targetRenderTexture.texture, state.renderSourceRect, state.renderDestRect, { 0, 0 }, 0.0f, WHITE);
//
//    // Mo�na tu rysowa� dodatkowe elementy UI *ponad* gr�, kt�re nie powinny by� skalowane (np. kursor systemowy, je�li potrzebny)
//
//    EndDrawing();
//}
//
//void Game::DrawDebug() {
//    // Zwi�kszono nieco wysoko�� t�a, aby zmie�ci� info o muzyce
//    DrawRectangle(5, 5, 240, 205, Fade(BLACK, 0.6f)); // T�o dla tekstu
//    DrawText(TextFormat("P.Log X: %.1f", state.player.position.x), 10, 10, 20, LIME);
//    DrawText(TextFormat("P.VDraw X: %.1f", state.player.virtualDrawPos.x), 10, 30, 20, LIME);
//    DrawText(TextFormat("P.Log Y: %.1f", state.player.position.y), 10, 50, 20, LIME);
//    DrawText(TextFormat("ScrollX: %.1f", state.level.scrollX), 10, 70, 20, LIME);
//    DrawText(TextFormat("MaxScrollX: %.1f", state.level.maxScrollX), 10, 90, 20, ORANGE);
//    // DrawText(TextFormat("CamLockPosX: %.1f", cameraLockPosX), 10, 110, 20, ORANGE); // Usuni�to, bo mniej istotne
//    DrawText(TextFormat("Grounded: %s", state.player.isGrounded ? "True" : "False"), 10, 110, 20, LIME); // Przesuni�to
//    DrawText(TextFormat("Button Clicked: %s", state.ui.isButtonClicked ? "Yes" : "No"), 10, 130, 20, YELLOW); // Przesuni�to
//    DrawText(TextFormat("V_Mouse: %.0f,%.0f", state.virtualMousePos.x, state.virtualMousePos.y), 10, 150, 20, SKYBLUE); // Dodano VMouse
//
//
//    // Informacje o muzyce w debug info
//    if (state.audio.musicLoaded) {
//        DrawText(TextFormat("Music: %.2f/%.2f", GetMusicTimePlayed(state.audio.backgroundMusic), GetMusicTimeLength(state.audio.backgroundMusic)), 10, 170, 20, SKYBLUE);
//    }
//    else {
//        DrawText("Music: Not Loaded", 10, 170, 20, RED);
//    }
//    // Informacja o d�wi�ku skoku
//    DrawText(TextFormat("JumpSnd: %s", state.audio.jumpSoundLoaded ? "Loaded" : "Failed"), 10, 190, 20, state.audio.jumpSoundLoaded ? SKYBLUE : RED);
//
//
//    DrawText("DEBUG (F3) Full (F11)", VIRTUAL_SCREEN_WIDTH - MeasureText("DEBUG (F3) Full (F11)", 20) - 10, 10, 20, WHITE);
//}
//
//
//void Game::Shutdown() {
//    if (!initialized) return; // Ju� zamkni�to lub nie zainicjalizowano
//
//    TraceLog(LOG_INFO, "GAME: Rozpoczynanie zamykania...");
//
//    // Zwolnij zasoby w odwrotnej kolejno�ci do �adowania (dobra praktyka)
//    UnloadRenderTexture(state.targetRenderTexture);
//    UnloadResources(); // Zwalnia tekstury i d�wi�ki
//    ShutdownGameAudio(); // Zamyka urz�dzenie audio
//    CloseWindow();        // Zamyka okno Raylib
//
//    initialized = false;
//    TraceLog(LOG_INFO, "GAME: Zamykanie zako�czone.");
//}