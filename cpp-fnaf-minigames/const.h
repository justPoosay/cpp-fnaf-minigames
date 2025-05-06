#pragma once

// Definicja PI
#ifndef PI
#define PI 3.14159265358979323846f
#endif

// --- Domyœlne wymiary t³a ---
constexpr float defaultBackgroundWidth = 1920.0f;
constexpr float defaultBackgroundHeight = 1080.0f;

// --- Konfiguracja poziomu Midnight Motorist ---
constexpr float virtualScreenWidth = 960.0f;
constexpr float virtualScreenHeight = 540.0f;
constexpr float motoristRoadMargin = 25.0f;
constexpr float motoristRoadYTop = motoristRoadMargin;
constexpr float motoristRoadYBottom = virtualScreenHeight - motoristRoadMargin;

// --- Prêdkoœci gracza i œwiata ---
constexpr float PLAYER_MOVE_SPEED = 250.0f;
constexpr float VIRTUAL_SPEED_INITIAL = 0.0f;
constexpr float VIRTUAL_SPEED_MAX = 1690.0f;
constexpr float VIRTUAL_SPEED_ACCELERATION = 45.0f;
constexpr float spinDurationPerFrame = 0.05f;

// --- Prêdkoœci NPC ---
constexpr float MIN_NPC_SPEED = 150.0f;
constexpr float MAX_NPC_SPEED = 300.0f;

// --- Spawnowanie NPC ---
constexpr float npcSpawnRate = 2.5f;
constexpr float minNpcSpacing = 25.0f;

// --- LAP ANIMATION CONSTANTS ---
constexpr int LAP_ANIM_TOTAL_BLINKS = 16;
constexpr float LAP_ANIM_BLINK_DURATION = 0.025f;
constexpr float LAP_ANIM_SCALE = 0.35f;

// --- 8 Lane Definitions (Ratio) ---
constexpr float laneCenterY1Ratio = 95.0f / defaultBackgroundHeight;
constexpr float laneCenterY2Ratio = 191.0f / defaultBackgroundHeight;
constexpr float laneCenterY3Ratio = 349.0f / defaultBackgroundHeight;
constexpr float laneCenterY4Ratio = 455.0f / defaultBackgroundHeight;
constexpr float laneCenterY5Ratio = 611.0f / defaultBackgroundHeight;
constexpr float laneCenterY6Ratio = 717.0f / defaultBackgroundHeight;
constexpr float laneCenterY7Ratio = 877.0f / defaultBackgroundHeight;
constexpr float laneCenterY8Ratio = 981.0f / defaultBackgroundHeight;

// --- Dystans okr¹¿enia i UI (dodane z poprzedniej wersji, jeœli by³y potrzebne) ---
constexpr float LAP_DISTANCE = 10000.0f;
constexpr float UI_TEXT_SIZE_INFO = 30.0f;
constexpr float UI_TEXT_SPACING = 1.5f;
constexpr float UI_MARGIN = 15.0f;
constexpr float UI_LINE_SPACING = 5.0f;

// --- Sta³e obliczeniowe dla wymiarów gracza ---
constexpr float playerScaleFactor = 0.75f;
constexpr float defaultPlayerWidth = 160.0f;
constexpr float defaultPlayerHeight = 131.0f;
constexpr float baseScaleY = virtualScreenHeight / defaultBackgroundHeight;
constexpr float baseScaleX = baseScaleY; // Zak³adamy kwadratowe piksele
constexpr float playerTextureWidth = defaultPlayerWidth * baseScaleX * playerScaleFactor;
constexpr float playerTextureHeight = defaultPlayerHeight * baseScaleY * playerScaleFactor;
constexpr float playerHalfWidth = playerTextureWidth / 2.0f;
constexpr float playerHalfHeight = playerTextureHeight / 2.0f;

// --- Sta³e obliczeniowe dla wymiarów NPC ---
constexpr float npcScaleFactor = 0.75f;
constexpr float defaultNpcWidth = 160.0f;
constexpr float defaultNpcHeight = 131.0f;
constexpr float npcTextureWidth = defaultNpcWidth * baseScaleX * npcScaleFactor;
constexpr float npcTextureHeight = defaultNpcHeight * baseScaleY * npcScaleFactor;
constexpr float npcHalfWidth = npcTextureWidth / 2.0f;
constexpr float npcHalfHeight = npcTextureHeight / 2.0f;

constexpr float npcCollisionMarginX_raw = 10.0f;
constexpr float npcCollisionMarginY_raw = 5.0f;
constexpr float npcCollisionMarginX = npcCollisionMarginX_raw * baseScaleX * npcScaleFactor;
constexpr float npcCollisionMarginY = npcCollisionMarginY_raw * baseScaleY * npcScaleFactor;