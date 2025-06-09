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
    int frameWidth;
    int frameHeight;
    float frameTime;        // Time per frame (e.g., 0.1f for 10 FPS)
    float currentTime;      // Current animation time
    int currentFrame;       // Current frame index
    bool loop;              // Whether animation should loop
    bool isPlaying;         // Whether animation is currently playing
};

struct FruityMazeGameResources {
    // Map data
    int mapWidth;
    int mapHeight;
    Texture2D minimapTexture;
    Texture2D cubeWallTexture;

    Vector3 cubicMapSize;
    Model mazeModel;
    Color* mapPixels;
    Vector3 mapModelPosition;

    // **MODIFIED: Changed from individual textures to animation data**
    AnimationData amazingAnim;
    AnimationData timeIsUpAnim;
    AnimationData timeExtendedAnim;
    Texture2D outOfBounds;  // Keep this as static image

    // 3D Models
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
void UnloadAnimationData(AnimationData& anim);

FruityMazeGameResources LoadFruityMazeResources(GraphicsQuality quality);
void UnloadFruityMazeResources(FruityMazeGameResources& resources);
bool CheckFruityMazeResourcesLoaded(FruityMazeGameResources& res);