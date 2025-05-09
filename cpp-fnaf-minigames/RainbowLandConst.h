#pragma once

#include "raylib.h"
#include "raymath.h"
#include <cmath>


const float PI_F = PI; // Use the PI defined by Raylib/raymath.h

const float radiansToDegrees = PI_F / 180.0f;

const int virtualScreenWidth = 800;
const int virtualScreenHeight = 450;

const float levelLogicalWidth = 5000.0f;
const float bgScrollFactor = 0.3f;

const float virtualAspectRatio = (float)virtualScreenWidth / (float)virtualScreenHeight;
const float maxScrollX = fmaxf(0.0f, levelLogicalWidth - virtualScreenWidth);
const float groundLevelY = virtualScreenHeight - 25.0f;



// --------- REGULACJA LOGIKI RUCHU ---------

const float playerTextureHeight = 70.0f;
const float playerTextureWidth = 70.0f;

// dostosowanie rozmiary hitboxa playera
const float playerHitboxWidth = playerTextureWidth * 0.6f;
const float playerHitboxHeight = playerTextureHeight * 0.8f;
const float playerHitboxOffsetX = (playerTextureWidth - playerHitboxWidth) / 2.0f;
const float playerHitboxOffsetY = (playerTextureHeight - playerHitboxHeight);

const float playerVirtualScreenX = virtualScreenWidth / 2.0f - playerTextureWidth / 2.0f;

// --------- DOSTOSUJ FIZYKE RUCHU CHICI!! ---------
const float playerSpeed = 3.8f;
const float jumpSpeed = -5.0f;
const float fallSpeed = 5.0f;
const float maxJumpHeight = 270.0f;
const int animUpdateRate = 1;
const int idleFrames = 16;
const int walkFrames = 16;
const int jumpFrames = 1;


// --------- DOSTOSUJ FIZYKE RUCHU RAINBOWA!! ---------
const float rainbowTargetY = 5.0f;
const float rainbowPaddingX = 60.0f;
const float rainbowSpeed = playerSpeed / 1.75f;
const float rainbowLeftEyeOffsetX = 90.0f;
const float rainbowLeftEyeOffsetY = 18.0f;
const float rainbowRightEyeOffsetX = 152.0f;
const float rainbowRightEyeOffsetY = 18.0f;


// ------- BUTTERFLY SETUP -------
const float butterflyWidth = 32.0f;
const float butterflyHeight = 32.0f;
const int BUTTERFLY_TOTAL_FRAMES = 6;
const int BUTTERFLY_ANIM_UPDATE_RATE = 3;
const float BUTTERFLY_MOVEMENT_RADIUS_X = 40.0f;
const float BUTTERFLY_MOVEMENT_RADIUS_Y = 20.0f;
const float BUTTERFLY_BASE_MOVEMENT_SPEED = 0.8f;

// fences
const float fenceWidth = 159.0f;
const float fenceHeight = 43.0f;

const float fence1 = 275.0f;
const float fence2 = 434.0f;
const float fence3 = 593.0f;
const float fence4 = 752.0f;
const float fence5 = 911.0f;

// checkpoint flags
const float checkpointFlagWidth = 49.0f;
const float checkpointFlagHeight = 51.0f;

const float flag1 = 100.0f;
const float flag2 = 850.0f;

// small flowers
const float flowerSmallWidth = 60.0f;
const float flowerSmallHeight = 60.0f;

const float smalFlow1 = 600.0f;
const float smalFlow2 = 700.0f;

// big flowers
const float flowerBigWidth = 100.0f;
const float flowerBigHeight = 100.0f;

const float bigFlow1 = 1000.0f;

// sunflowers
const float sunflowerWidth = 136.0f;
const float sunflowerHeight = 296.0f;

const float sunfl1 = 920.0f;
const float sunfl2 = 1220.0f;
const float sunfl3 = 1570.0f;

// sunflower pettles
const float petalSpeed = 1000.0f;
const float petalLifespan = 0.75f;

const float sunflowerDiscCenterX = 68.0f;
const float sunflowerDiscCenterY = 67.0f;


//Vector2 petalDirections[] = {
//    {0.0f, -1.0f},          // 0: N
//    {0.7071f, -0.7071f},    // 1: NE
//    {1.0f, 0.0f},           // 2: E
//    {0.7071f, 0.7071f},     // 3: SE
//    {0.0f, 1.0f},           // 4: S
//    {-0.7071f, 0.7071f},    // 5: SW
//    {-1.0f, 0.0f},          // 6: W
//    {-0.7071f, -0.7071f}    // 7: NW
//};

Vector2 petalDirections[] = {
// N: Points towards -Y. Angle of -90 degrees (or 270 degrees)
    {cosf(-90.0f * radiansToDegrees), sinf(-90.0f * radiansToDegrees)},     // (0, -1)
// NE: Angle of -45 degrees (or 315 degrees)
    {cosf(-45.0f * radiansToDegrees), sinf(-45.0f * radiansToDegrees)},     // (0.7071, -0.7071)
// E: Points towards +X. Angle of 0 degrees
    {cosf(0.0f * radiansToDegrees),   sinf(0.0f * radiansToDegrees)},       // (1, 0)
// SE: Angle of 45 degrees
    {cosf(45.0f * radiansToDegrees),  sinf(45.0f * radiansToDegrees)},      // (0.7071, 0.7071)
// S: Points towards +Y. Angle of 90 degrees
    {cosf(90.0f * radiansToDegrees),  sinf(90.0f * radiansToDegrees)},      // (0, 1)
// SW: Angle of 135 degrees
    {cosf(135.0f * radiansToDegrees), sinf(135.0f * radiansToDegrees)},     // (-0.7071, 0.7071)
// W: Points towards -X. Angle of 180 degrees
    {cosf(180.0f * radiansToDegrees), sinf(180.0f * radiansToDegrees)},     // (-1, 0)
// NW: Angle of 225 degrees (or -135 degrees)
    {cosf(225.0f * radiansToDegrees), sinf(225.0f * radiansToDegrees)}      // (-0.7071, -0.7071)
};


// platform specs
const float platformWidth = 80.0f;
const float platformHeight = 21.0f;

// voices button specs
const float buttonPadding = 5.0f;
const float buttonTextureWidth = 100.0f;
const float buttonTextureHeight = 30.0f;
Vector2 buttonPos = { buttonPadding, (float)virtualScreenHeight - buttonTextureHeight - buttonPadding };


