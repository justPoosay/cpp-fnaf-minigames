#pragma once

#include "GameSettings.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <cmath>

using namespace std;

struct FruityMazeGameResources {

    // Map and terrain
    Image mapImage;
    Color* mapPixels;
    Texture2D minimapTexture;

    // 3D Models and textures
    Model mazeModel;
    Model playerModel;
    Texture2D cubeWallTexture;

    // Prop models
    Model cherryModel;
    Model orangeModel;
    Model grapesModel;
    Model lightningModel;
    Model gummybearModel;
    Model magnetModel;

    // Audio
    Music backgroundMusic;
    Sound fruit1Sound;  // orange collection sound
    Sound fruit2Sound;  // cherry collection sound
    Sound fruit3Sound;  // grapes collection sound
    Sound powerUpSound; // power-up collection sound
    Sound timerPingSound; // timer tick sound
    Sound boundsSound;  // out of bounds sound
    Sound timeExtendedSound;  // extended time sound

    bool backgroundMusicLoaded;
    bool fruit1SoundLoaded;
    bool fruit2SoundLoaded;
    bool fruit3SoundLoaded;
    bool powerUpSoundLoaded;
    bool timerPingSoundLoaded;
    bool boundsSoundLoaded;
    bool timeExtendedSoundLoaded;

    // Model loading flags
    bool cherryLoaded;
    bool orangeLoaded;
    bool grapesLoaded;
    bool lightningLoaded;
    bool gummybearLoaded;
    bool magnetLoaded;

    // Map processing data
    Vector3 mapModelPosition;
    Vector3 cubicMapSize;
    bool mapLoaded;
    bool playerModelLoaded;
    bool wallTextureLoaded;

    // Map dimensions
    int mapWidth;
    int mapHeight;
    int wallPixelCount;
    int pathPixelCount;

    // Shaders
    Shader lightingShader;
    bool lightingShaderLoaded;
};

FruityMazeGameResources LoadFruityMazeResources(GraphicsQuality quality);
void UnloadFruityMazeResources(FruityMazeGameResources& resources);
bool CheckFruityMazeResourcesLoaded(FruityMazeGameResources& res);