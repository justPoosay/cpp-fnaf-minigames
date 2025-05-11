#pragma once

#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <iomanip>

using namespace std;


constexpr float defaultBackgroundWidth = 1920;
constexpr float defaultBackgroundHeight = 1080;

constexpr float virtualScreenWidth = 960;
constexpr float virtualScreenHeight = 540;
constexpr float motoristRoadMargin = 25;
constexpr float motoristRoadYTop = motoristRoadMargin;
constexpr float motoristRoadYBottom = virtualScreenHeight - motoristRoadMargin;
constexpr float LAP_DISTANCE = 10000;

constexpr float PLAYER_MOVE_SPEED = 250;
constexpr float VIRTUAL_SPEED_INITIAL = 0;
constexpr float VIRTUAL_SPEED_MAX = 1690;
constexpr float VIRTUAL_SPEED_ACCELERATION = 45;
constexpr float bgScrollMultiplier = 1.5;
constexpr float spinDurationPerFrame = 0.05;

constexpr float MIN_NPC_SPEED = 150;
constexpr float MAX_NPC_SPEED = 300;

constexpr float npcSpawnRate = 2.5;
constexpr float minNpcSpacing = 25;

constexpr int LAP_ANIM_TOTAL_BLINKS = 16;
constexpr float LAP_ANIM_BLINK_DURATION = 0.025;
constexpr float LAP_ANIM_SCALE = 0.35;

constexpr float laneCenterY1Ratio = 95 / defaultBackgroundHeight;
constexpr float laneCenterY2Ratio = 191 / defaultBackgroundHeight;
constexpr float laneCenterY3Ratio = 349 / defaultBackgroundHeight;
constexpr float laneCenterY4Ratio = 455 / defaultBackgroundHeight;
constexpr float laneCenterY5Ratio = 611 / defaultBackgroundHeight;
constexpr float laneCenterY6Ratio = 717 / defaultBackgroundHeight;
constexpr float laneCenterY7Ratio = 877 / defaultBackgroundHeight;
constexpr float laneCenterY8Ratio = 981 / defaultBackgroundHeight;

constexpr float UI_TEXT_SIZE_INFO = 60;
constexpr float UI_TEXT_SPACING = 1.5;
constexpr float UI_MARGIN = 10;

constexpr float playerScaleFactor = 0.75;
constexpr float defaultPlayerWidth = 160;
constexpr float defaultPlayerHeight = 131;
constexpr float baseScaleY = virtualScreenHeight / defaultBackgroundHeight;
constexpr float baseScaleX = baseScaleY;
constexpr float playerTextureWidth = defaultPlayerWidth * baseScaleX * playerScaleFactor;
constexpr float playerTextureHeight = defaultPlayerHeight * baseScaleY * playerScaleFactor;
constexpr float playerHalfWidth = playerTextureWidth / 2;
constexpr float playerHalfHeight = playerTextureHeight / 2;

constexpr float npcScaleFactor = 0.75;
constexpr float defaultNpcWidth = 160;
constexpr float defaultNpcHeight = 131;
constexpr float npcTextureWidth = defaultNpcWidth * baseScaleX * npcScaleFactor;
constexpr float npcTextureHeight = defaultNpcHeight * baseScaleY * npcScaleFactor;
constexpr float npcHalfWidth = npcTextureWidth / 2;
constexpr float npcHalfHeight = npcTextureHeight / 2;

constexpr float npcCollisionMarginX_raw = 10;
constexpr float npcCollisionMarginY_raw = 5;
constexpr float npcCollisionMarginX = npcCollisionMarginX_raw * baseScaleX * npcScaleFactor;
constexpr float npcCollisionMarginY = npcCollisionMarginY_raw * baseScaleY * npcScaleFactor;