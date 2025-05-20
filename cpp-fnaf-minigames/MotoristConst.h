#pragma once

#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <iomanip>

using namespace std;


const float defaultBackgroundWidth = 1920;
const float defaultBackgroundHeight = 1080;

const float virtualScreenWidth = 960;
const float virtualScreenHeight = 540;
const float motoristRoadMargin = 25;
const float motoristRoadYTop = motoristRoadMargin;
const float motoristRoadYBottom = virtualScreenHeight - motoristRoadMargin;
const float lapDistance = 12500;

const float playerMoveSpeed = 250;
const float virtualSpeedInitial = 0;
const float virtualSpeedMax = 1690;
const float virtualSpeedAcceleration = 47.5;
const float bgScrollMultiplier = 1.75;
const float spinFrameDuration = 0.05;


const float minNpcSpeed = 200;
const float maxNpcSpeed = 350;

const float npcSpawnRate = 3.25;
const float minNpcSpacing = 30;

const int lapAnimBlinks = 16;
const float lapAnimBlinkDuration = 0.025;
const float lapAnimScale = 0.35;

const float laneCenterY1Ratio = 95 / defaultBackgroundHeight;
const float laneCenterY2Ratio = 191 / defaultBackgroundHeight;
const float laneCenterY3Ratio = 349 / defaultBackgroundHeight;
const float laneCenterY4Ratio = 455 / defaultBackgroundHeight;
const float laneCenterY5Ratio = 611 / defaultBackgroundHeight;
const float laneCenterY6Ratio = 717 / defaultBackgroundHeight;
const float laneCenterY7Ratio = 877 / defaultBackgroundHeight;
const float laneCenterY8Ratio = 981 / defaultBackgroundHeight;

const float uiTextSize = 60;
const float uiTextSpacing = 1.5;
const float uiMargin = 10;

const float playerScaleFactor = 0.75;
const float defaultPlayerWidth = 160;
const float defaultPlayerHeight = 131;
const float baseScaleY = virtualScreenHeight / defaultBackgroundHeight;
const float baseScaleX = baseScaleY;
const float playerTextureWidth = defaultPlayerWidth * baseScaleX * playerScaleFactor;
const float playerTextureHeight = defaultPlayerHeight * baseScaleY * playerScaleFactor;
const float playerHalfWidth = playerTextureWidth / 2;
const float playerHalfHeight = playerTextureHeight / 2;

const float npcScaleFactor = 0.75;
const float defaultNpcWidth = 160;
const float defaultNpcHeight = 131;
const float npcTextureWidth = defaultNpcWidth * baseScaleX * npcScaleFactor;
const float npcTextureHeight = defaultNpcHeight * baseScaleY * npcScaleFactor;
const float npcHalfWidth = npcTextureWidth / 2;
const float npcHalfHeight = npcTextureHeight / 2;

const float npcCollisionMarginX_raw = 10;
const float npcCollisionMarginY_raw = 5;
const float npcCollisionMarginX = npcCollisionMarginX_raw * baseScaleX * npcScaleFactor;
const float npcCollisionMarginY = npcCollisionMarginY_raw * baseScaleY * npcScaleFactor;