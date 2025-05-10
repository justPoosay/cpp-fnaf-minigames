#pragma once

#include "raylib.h"
#include <vector>
#include <string>
#include "GameSettings.h"

struct RainbowLandGameResources { // Store game resources

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
    std::vector<Texture2D> rbowEyeTextures;
    const int numEyeSprites = 32;

// Butterfly
    Texture2D butterfly;
    Texture2D bflyLaser;

// Background, Tiles, UI
    Texture2D bg;
    Texture2D tile;
    Texture2D platformTexture;

    Texture2D sunflower;
    std::vector<Texture2D> sunflowerPetals;
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
    Texture2D checkpointAnimTexture;
    Image checkpointAnimImage;
    int checkpointAnimNumFrames;
    float checkpointAnimFrameDelay;

    Texture2D controlKeysInfo;
    Texture2D greatJob;
    Texture2D youSuck;


// Music & FX
    Music backgroundMusic;  // Music
    Sound respawn;          // Respawn
    Sound death;            // Death
    Sound jump;             // Jump 
    Sound eyePop;           // Rainbow Eye Shoot
    Sound checkpointSound;  // Checkpoint
    Sound logs;             // Logs Shoot
    Sound petalShoot;       // Sunflower Petal Shoot
    Sound rbowLaserShoot;   // Rainbow Laser (Single Tick)
    Sound bflyLaserShoot;   // Butterfly Laser (Single Tick)


    Sound rbowVoiceOff;
    Sound rbowGonnaKillYou;
    Sound rbowLastChance;
    Sound rbowHA;

    std::vector<Sound> rbowDialogues;
    const int numRbowVoiceLines = 32;

    bool checkpointAnimLoaded = false;

    bool backgroundMusicLoaded = false;
    bool respawnSoundLoaded = false;
    bool deathSoundLoaded = false;
    bool jumpSoundLoaded = false;
    bool eyePopSoundLoaded = false;
    bool checkpointSoundLoaded = false;
    bool logsSoundLoaded = false;
    bool petalSoundLoaded = false;
    bool bflyLaserShootSoundLoaded = false;
    bool rbowLaserShootSoundLoaded = false;

    bool rbowVoiceOffSoundLoaded = false;
    bool rbowGonnaKillYouSoundLoaded = false;
    bool rbowLastChanceSoundLoaded = false;
    bool rbowHASoundLoaded = false;
    bool rbowDialoguesSoundLoaded = false;

};

// Loading Resources
RainbowLandGameResources LoadRainbowLandResources(GraphicsQuality quality);

// Unloadind Resources
void UnloadRainbowLandResources(RainbowLandGameResources& resources);