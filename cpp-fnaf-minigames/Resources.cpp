#include "Resources.h"
#include "raylib.h"

#include <string>
#include <vector>


GameResources LoadGameResources() { // Loading Resources Function
    TraceLog(LOG_INFO, "Loading game resources...");

    GameResources resources;


// Chica (player)
    resources.idleRight = LoadTexture("resources/textures/sprites/chica/chica_idle_right.png");
    resources.idleLeft = LoadTexture("resources/textures/sprites/chica/chica_idle_left.png");
    resources.walkRight = LoadTexture("resources/textures/sprites/chica/chica_walk_right.png");
    resources.walkLeft = LoadTexture("resources/textures/sprites/chica/chica_walk_left.png");
    resources.jumpRight = LoadTexture("resources/textures/sprites/chica/chica_jump_right.png");
    resources.jumpLeft = LoadTexture("resources/textures/sprites/chica/chica_jump_left.png");


// Rainbow & Eyes
    resources.npcBodyTexture = LoadTexture("resources/textures/sprites/rainbow/chica_rainbow.png");
    resources.npcDialogueBodyTexture = LoadTexture("resources/textures/sprites/rainbow/chica_rainbow_pissed.png");

    resources.npcEyeTextures.resize(resources.numEyeSprites);
    resources.npcEyeTextures[0] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-0 Frame-0.png");
    resources.npcEyeTextures[1] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-1 Frame-0.png");
    resources.npcEyeTextures[2] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-2 Frame-0.png");
    resources.npcEyeTextures[3] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-3 Frame-0.png");
    resources.npcEyeTextures[4] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-4 Frame-0.png");
    resources.npcEyeTextures[5] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-5 Frame-0.png");
    resources.npcEyeTextures[6] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-6 Frame-0.png");
    resources.npcEyeTextures[7] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-7 Frame-0.png");
    resources.npcEyeTextures[8] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-8 Frame-0.png");
    resources.npcEyeTextures[9] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-9 Frame-0.png");
    resources.npcEyeTextures[10] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-10 Frame-0.png");
    resources.npcEyeTextures[11] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-11 Frame-0.png");
    resources.npcEyeTextures[12] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-12 Frame-0.png");
    resources.npcEyeTextures[13] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-13 Frame-0.png");
    resources.npcEyeTextures[14] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-14 Frame-0.png");
    resources.npcEyeTextures[15] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-15 Frame-0.png");
    resources.npcEyeTextures[16] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-16 Frame-0.png");
    resources.npcEyeTextures[17] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-17 Frame-0.png");
    resources.npcEyeTextures[18] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-18 Frame-0.png");
    resources.npcEyeTextures[19] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-19 Frame-0.png");
    resources.npcEyeTextures[20] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-20 Frame-0.png");
    resources.npcEyeTextures[21] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-21 Frame-0.png");
    resources.npcEyeTextures[22] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-22 Frame-0.png");
    resources.npcEyeTextures[23] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-23 Frame-0.png");
    resources.npcEyeTextures[24] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-24 Frame-0.png");
    resources.npcEyeTextures[25] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-25 Frame-0.png");
    resources.npcEyeTextures[26] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-26 Frame-0.png");
    resources.npcEyeTextures[27] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-27 Frame-0.png");
    resources.npcEyeTextures[28] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-28 Frame-0.png");
    resources.npcEyeTextures[29] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-29 Frame-0.png");
    resources.npcEyeTextures[30] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-30 Frame-0.png");
    resources.npcEyeTextures[31] = LoadTexture("resources/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-31 Frame-0.png");


// Background, Tiles & UI
    resources.bg = LoadTexture("resources/background.png");
    resources.tile = LoadTexture("resources/textures/misc/dirt_tile.png");
    resources.platformTexture = LoadTexture("resources/textures/misc/platform_tile.png");
    resources.buttonVoicesOn = LoadTexture("resources/textures/text/voices_on.png");
    resources.buttonVoicesOff = LoadTexture("resources/textures/text/voices_off.png");


// Music & FX
    const char* musicPath = "resources/rainbow.mp3";
    const char* jumpPath = "resources/audio/sfx/jump.wav";
    const char* npcDialoguePath = "resources/audio/voice/voice_off_dialogue.mp3";
    //const char* laserShootPath = "resources/audio/sfx/.mp3"; // Jeœli bêdzie u¿ywany

    resources.backgroundMusic = LoadMusicStream(musicPath);
    resources.jumpSound = LoadSound(jumpPath);
    resources.npcDialogueSound = LoadSound(npcDialoguePath);
    //resources.laserShootSound = LoadSound(laserShootPath);

    resources.musicLoaded = (GetMusicTimeLength(resources.backgroundMusic) > 0); 
    resources.jumpSoundLoaded = (resources.jumpSound.frameCount > 0);
    resources.npcDialogueSoundLoaded = (resources.npcDialogueSound.frameCount > 0); 
    //resources.laserSoundLoaded = (resources.laserShootSound.frameCount > 0); // Jeœli dodasz laser


    TraceLog(LOG_INFO, "Game resources loaded successfully.");
    return resources;
}


// Unloadind Resources
void UnloadGameResources(GameResources& resources) {
    TraceLog(LOG_INFO, "Unloading game resources...");

    if (resources.idleRight.id > 0) UnloadTexture(resources.idleRight);
    if (resources.idleLeft.id > 0) UnloadTexture(resources.idleLeft);
    if (resources.walkRight.id > 0) UnloadTexture(resources.walkRight);
    if (resources.walkLeft.id > 0) UnloadTexture(resources.walkLeft);
    if (resources.jumpRight.id > 0) UnloadTexture(resources.jumpRight);
    if (resources.jumpLeft.id > 0) UnloadTexture(resources.jumpLeft);
    if (resources.npcBodyTexture.id > 0) UnloadTexture(resources.npcBodyTexture);
    if (resources.npcDialogueBodyTexture.id > 0) UnloadTexture(resources.npcDialogueBodyTexture);
    if (resources.bg.id > 0) UnloadTexture(resources.bg);
    if (resources.tile.id > 0) UnloadTexture(resources.tile);
    if (resources.platformTexture.id > 0) UnloadTexture(resources.platformTexture);
    if (resources.buttonVoicesOn.id > 0) UnloadTexture(resources.buttonVoicesOn);
    if (resources.buttonVoicesOff.id > 0) UnloadTexture(resources.buttonVoicesOff);
    if (resources.npcEyeTextures[0].id > 0) UnloadTexture(resources.npcEyeTextures[0]);
    if (resources.npcEyeTextures[1].id > 0) UnloadTexture(resources.npcEyeTextures[1]);
    if (resources.npcEyeTextures[2].id > 0) UnloadTexture(resources.npcEyeTextures[2]);
    if (resources.npcEyeTextures[3].id > 0) UnloadTexture(resources.npcEyeTextures[3]);
    if (resources.npcEyeTextures[4].id > 0) UnloadTexture(resources.npcEyeTextures[4]);
    if (resources.npcEyeTextures[5].id > 0) UnloadTexture(resources.npcEyeTextures[5]);
    if (resources.npcEyeTextures[6].id > 0) UnloadTexture(resources.npcEyeTextures[6]);
    if (resources.npcEyeTextures[7].id > 0) UnloadTexture(resources.npcEyeTextures[7]);
    if (resources.npcEyeTextures[8].id > 0) UnloadTexture(resources.npcEyeTextures[8]);
    if (resources.npcEyeTextures[9].id > 0) UnloadTexture(resources.npcEyeTextures[9]);
    if (resources.npcEyeTextures[10].id > 0) UnloadTexture(resources.npcEyeTextures[10]);
    if (resources.npcEyeTextures[11].id > 0) UnloadTexture(resources.npcEyeTextures[11]);
    if (resources.npcEyeTextures[12].id > 0) UnloadTexture(resources.npcEyeTextures[12]);
    if (resources.npcEyeTextures[13].id > 0) UnloadTexture(resources.npcEyeTextures[13]);
    if (resources.npcEyeTextures[14].id > 0) UnloadTexture(resources.npcEyeTextures[14]);
    if (resources.npcEyeTextures[15].id > 0) UnloadTexture(resources.npcEyeTextures[15]);
    if (resources.npcEyeTextures[16].id > 0) UnloadTexture(resources.npcEyeTextures[16]);
    if (resources.npcEyeTextures[17].id > 0) UnloadTexture(resources.npcEyeTextures[17]);
    if (resources.npcEyeTextures[18].id > 0) UnloadTexture(resources.npcEyeTextures[18]);
    if (resources.npcEyeTextures[19].id > 0) UnloadTexture(resources.npcEyeTextures[19]);
    if (resources.npcEyeTextures[20].id > 0) UnloadTexture(resources.npcEyeTextures[20]);
    if (resources.npcEyeTextures[21].id > 0) UnloadTexture(resources.npcEyeTextures[21]);
    if (resources.npcEyeTextures[22].id > 0) UnloadTexture(resources.npcEyeTextures[22]);
    if (resources.npcEyeTextures[23].id > 0) UnloadTexture(resources.npcEyeTextures[23]);
    if (resources.npcEyeTextures[24].id > 0) UnloadTexture(resources.npcEyeTextures[24]);
    if (resources.npcEyeTextures[25].id > 0) UnloadTexture(resources.npcEyeTextures[25]);
    if (resources.npcEyeTextures[26].id > 0) UnloadTexture(resources.npcEyeTextures[26]);
    if (resources.npcEyeTextures[27].id > 0) UnloadTexture(resources.npcEyeTextures[27]);
    if (resources.npcEyeTextures[28].id > 0) UnloadTexture(resources.npcEyeTextures[28]);
    if (resources.npcEyeTextures[29].id > 0) UnloadTexture(resources.npcEyeTextures[29]);
    if (resources.npcEyeTextures[30].id > 0) UnloadTexture(resources.npcEyeTextures[30]);
    if (resources.npcEyeTextures[31].id > 0) UnloadTexture(resources.npcEyeTextures[31]);
    if (resources.musicLoaded) UnloadMusicStream(resources.backgroundMusic);
    if (resources.jumpSoundLoaded) UnloadSound(resources.jumpSound);
    if (resources.npcDialogueSoundLoaded) UnloadSound(resources.npcDialogueSound);
    //if (resources.laserSoundLoaded) UnloadSound(resources.laserShootSound);
}