#pragma once

#include "raylib.h"
#include "GameSettings.h"

#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

struct RainbowLandGameResources { 

// Chica (player)
    Texture2D idleRight;
    Texture2D idleLeft;
    Texture2D walkRight;
    Texture2D walkLeft;
    Texture2D jumpRight;
    Texture2D jumpLeft;

// Rainbow
    Texture2D rbowBodyTexture;
    Texture2D rbowVoiceOffBodyTexture;

// Rainbow (eyes)
    vector<Texture2D> rbowEyeTextures;
    const int numEyeSprites = 32;

// Butterfly
    Texture2D butterfly;
    Texture2D laserBeamFrame;

// Background, Tiles, UI
    Texture2D cursor;
    Texture2D bg;
    Texture2D tile;
    Texture2D platformTexture;

    Texture2D sunflower;
    vector<Texture2D> sunflowerPetals;
    const int numPetals = 8;

    Texture2D fenceProp;
    Texture2D flowerBigProp;
    Texture2D flowerSmallProp;
    Texture2D checkpointFlag;

    Texture2D log;
    Texture2D spikesUp;
    Texture2D spikesDown;

    Texture2D bloodParticle;
    Texture2D buttonVoicesOn;
    Texture2D buttonVoicesOff;
    Texture2D checkpoint;

    Texture2D controlKeysInfo;
    Texture2D greatJob;
    Texture2D youSuck;


// Music & FX
    Music backgroundMusic;
    Sound respawn;
    Sound death;
    Sound jump;
    Sound eyePop;
    Sound checkpointSound;
    Sound logs;
    Sound spikesPush;
    Sound petalShoot;
    Sound rbowLaserShoot;
    Sound bflyLaserShoot;


    Sound rbowVoiceOff;
    Sound rbowGonnaKillYou;
    Sound rbowYouStillSuck;

    vector<Sound> rbowDialogues;
    const int numRbowVoiceLines = 33;

    bool checkpointAnimLoaded;

    bool backgroundMusicLoaded;
    bool respawnSoundLoaded;
    bool deathSoundLoaded;
    bool jumpSoundLoaded;
    bool eyePopSoundLoaded;
    bool checkpointSoundLoaded;
    bool logsSoundLoaded;
    bool spikesSoundLoaded;
    bool petalSoundLoaded;
    bool bflyLaserShootSoundLoaded;
    bool rbowLaserShootSoundLoaded;

    bool rbowVoiceOffSoundLoaded;
    bool rbowGonnaKillYouSoundLoaded;
    bool rbowYouStillSuckSoundLoaded;
    bool rbowDialoguesSoundLoaded;
};

RainbowLandGameResources LoadRainbowLandResources(GraphicsQuality quality);
void UnloadRainbowLandResources(RainbowLandGameResources& resources);
bool CheckRainbowLandResourcesLoaded(RainbowLandGameResources& res);