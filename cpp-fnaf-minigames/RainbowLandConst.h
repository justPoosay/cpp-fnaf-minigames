#pragma once

#include "raylib.h"
#include "raymath.h"

const float PI_F = PI;

const float radiansToDegrees = PI_F / 180;

const int virtualScreenWidth = 800;
const int virtualScreenHeight = 450;

const float levelLogicalWidth = 5000;
const float bgScrollFactor = 0.3;

const float virtualAspectRatio = virtualScreenWidth / virtualScreenHeight;
const float maxScrollX = fmax(0, levelLogicalWidth - virtualScreenWidth);
const float groundLevelY = virtualScreenHeight - 25;


const float playerTextureHeight = 70;
const float playerTextureWidth = 70;

const float playerHitboxWidth = playerTextureWidth * 0.6;
const float playerHitboxHeight = playerTextureHeight * 0.8;
const float playerHitboxOffsetX = (playerTextureWidth - playerHitboxWidth) / 2;
const float playerHitboxOffsetY = (playerTextureHeight - playerHitboxHeight);

const float playerVirtualScreenX = (virtualScreenWidth - playerTextureWidth) / 2;

const float playerSpeed = 4;
const float jumpSpeed = -5.5;
const float fallSpeed = 5.5;
const float maxJumpHeight = 270;
const int animUpdateRate = 1;
const int idleFrames = 16;
const int walkFrames = 16;
const int jumpFrames = 1;


const float rainbowPaddingX = 60;
const float rainbowSpeed = playerSpeed / 1.75;
const float rainbowLeftEyeOffsetX = 90;
const float rainbowLeftEyeOffsetY = 18;
const float rainbowRightEyeOffsetX = 152;
const float rainbowRightEyeOffsetY = 18;


const float butterflyWidth = 32;
const float butterflyHeight = 32;
const int BUTTERFLY_TOTAL_FRAMES = 6;
const int BUTTERFLY_ANIM_UPDATE_RATE = 3;
const float BUTTERFLY_MOVEMENT_RADIUS_X = 40;
const float BUTTERFLY_MOVEMENT_RADIUS_Y = 20;
const float BUTTERFLY_BASE_MOVEMENT_SPEED = 0.8;

// platforms
const float platformWidth = 80;
const float platformHeight = 21;

const float platform1 = 260;
const float platform2 = 360;
const float platform3 = 460;
const float platform4 = 560;
const float platform5 = 660;

const float platform6 = 1825;
const float platform7 = 2250;

// spikes
const float spikeSpeed = 1500;

const float spike1 = 3000;
const float spike2 = 3300;
const float spike3 = 3450;

// fences
const float fenceWidth = 159;
const float fenceHeight = 43;

const float fence1 = 275;
const float fence2 = 434;
const float fence3 = 593;
const float fence4 = 752;
const float fence5 = 911;

// checkpoint flags
const float checkpointFlagWidth = 49;
const float checkpointFlagHeight = 51;

const float flag1 = 100;
const float flag2 = 850;
const float flag3 = 1775;
const float flag4 = 2800;

// small flowers
const float flowerSmallWidth = 60;
const float flowerSmallHeight = 60;

const float smalFlow1 = 1500;

// big flowers
const float flowerBigWidth = 100;
const float flowerBigHeight = 100;

const float bigFlow1 = 1050;
const float bigFlow2 = 1150;
const float bigFlow3 = 1250;
const float bigFlow4 = 1350;

// sunflowers
const float sunflowerWidth = 136;
const float sunflowerHeight = 296;

const float sunflowerDiscCenterX = 68;
const float sunflowerDiscCenterY = 67;

float CalculateTriggerX(Rectangle sf1, Rectangle sf2) {
    float x1 = sf1.x + sunflowerDiscCenterX;
    float x2 = sf2.x + sunflowerDiscCenterX;
    return (x1 + x2) / 2;
}

const float sunfl1 = 920;
const float sunfl2 = 1270;
const float sunfl3 = 1620;

const float sunfl3_1 = 1987.5;
const float sunfl3_2 = 2437.5;

const float sunfl4 = 1875;
const float sunfl5 = 2100;
const float sunfl6 = 2325;
const float sunfl7 = 2550;

// sunflower petals
const float petalSpeed = 1000;
const float petalLifespan = 0.75;

Vector2 petalDirections[] = {
    {cosf(-90 * radiansToDegrees), sinf(-90 * radiansToDegrees)},     // (0, -1)
    {cosf(-45 * radiansToDegrees), sinf(-45 * radiansToDegrees)},     // (0.7071, -0.7071)
    {cosf(0 * radiansToDegrees),   sinf(0 * radiansToDegrees)},       // (1, 0)
    {cosf(45 * radiansToDegrees),  sinf(45 * radiansToDegrees)},      // (0.7071, 0.7071)
    {cosf(90 * radiansToDegrees),  sinf(90 * radiansToDegrees)},      // (0, 1)
    {cosf(135 * radiansToDegrees), sinf(135 * radiansToDegrees)},     // (-0.7071, 0.7071)
    {cosf(180 * radiansToDegrees), sinf(180 * radiansToDegrees)},     // (-1, 0)
    {cosf(225 * radiansToDegrees), sinf(225 * radiansToDegrees)}      // (-0.7071, -0.7071)
};

// voices button specs
const float buttonPadding = 6;
const float buttonTextureWidth = 126;
const float buttonTextureHeight = 26;
Vector2 buttonPos = { buttonPadding, virtualScreenHeight - (buttonTextureHeight + buttonPadding) / 2 };