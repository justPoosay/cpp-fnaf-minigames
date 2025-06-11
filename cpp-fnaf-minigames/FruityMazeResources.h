#pragma once

#include "GameSettings.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <cmath>

using namespace std;

struct AnimationData {
    Texture2D spriteSheet;

    int frameCount;
    int loopCount;
    int maxLoops;
    int currentFrame;

    float frameWidth;
    float frameHeight;
    float frameTime;
    float currentTime;

    bool isPlaying;
    bool loop;
};

struct FruityMazeGameResources {
    // Map data
    int mapWidth;
    int mapHeight;
    Vector3 cubicMapSize;
    Vector3 mapModelPosition;
    Color* mapPixels;

	// Textures
    Texture2D minimapTexture;
    Texture2D cubeWallTexture;
    Texture2D outOfBounds;
    AnimationData amazingAnim;
    AnimationData timeIsUpAnim;
    AnimationData timeExtendedAnim;

    // 3D Models
    Model mazeModel;
    Model playerModel;
    Model cherryModel;
    Model orangeModel;
    Model grapesModel;
    Model lightningModel;
    Model gummybearModel;
    Model magnetModel;

    // Audio
    Music backgroundMusic;
    Sound fruit1Sound;
    Sound fruit2Sound;
    Sound fruit3Sound;
    Sound boundsSound;
    Sound powerUpSound;
    Sound timerPingSound;
    Sound timeExtendedSound;

    // Shaders
    Shader lightingShader;
};

void InitializeAnimation(AnimationData& anim, const char* spritesheetPath, int frameCount, int frameWidth, int frameHeight, float frameTime = 0.1, bool loop = true);
void UpdateAnimation(AnimationData& anim, float deltaTime);
void StartAnimation(AnimationData& anim);
void StopAnimation(AnimationData& anim);
Rectangle GetCurrentAnimationFrame(const AnimationData& anim);

FruityMazeGameResources LoadFruityMazeResources(GraphicsQuality quality);
void UnloadFruityMazeResources(FruityMazeGameResources& resources);
bool CheckFruityMazeResourcesLoaded(FruityMazeGameResources& res);