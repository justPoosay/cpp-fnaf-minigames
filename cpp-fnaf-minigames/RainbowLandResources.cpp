#include "RainbowLandResources.h"
#include "raylib.h"

#include <string>
#include <vector>


RainbowLandGameResources LoadRainbowLandResources(GraphicsQuality quality) { // Loading Resources Function
    TraceLog(LOG_INFO, "Loading game resources...");

    RainbowLandGameResources resources;


// Chica (player)
    resources.idleRight = LoadTexture("resources/cmrl/textures/sprites/chica/chica_idle_right.png");
    resources.idleLeft = LoadTexture("resources/cmrl/textures/sprites/chica/chica_idle_left.png");
    resources.walkRight = LoadTexture("resources/cmrl/textures/sprites/chica/chica_walk_right.png");
    resources.walkLeft = LoadTexture("resources/cmrl/textures/sprites/chica/chica_walk_left.png");
    resources.jumpRight = LoadTexture("resources/cmrl/textures/sprites/chica/chica_jump_right.png");
    resources.jumpLeft = LoadTexture("resources/cmrl/textures/sprites/chica/chica_jump_left.png");


// Rainbow & Eyes
    resources.rbowBodyTexture = LoadTexture("resources/cmrl/textures/sprites/rainbow/chica_rainbow.png");
    resources.rbowVoiceOffBodyTexture = LoadTexture("resources/cmrl/textures/sprites/rainbow/chica_rainbow_pissed.png");

    resources.rbowEyeTextures.resize(resources.numEyeSprites);
    resources.rbowEyeTextures[0] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-0 Frame-0.png");
    resources.rbowEyeTextures[1] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-1 Frame-0.png");
    resources.rbowEyeTextures[2] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-2 Frame-0.png");
    resources.rbowEyeTextures[3] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-3 Frame-0.png");
    resources.rbowEyeTextures[4] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-4 Frame-0.png");
    resources.rbowEyeTextures[5] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-5 Frame-0.png");
    resources.rbowEyeTextures[6] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-6 Frame-0.png");
    resources.rbowEyeTextures[7] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-7 Frame-0.png");
    resources.rbowEyeTextures[8] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-8 Frame-0.png");
    resources.rbowEyeTextures[9] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-9 Frame-0.png");
    resources.rbowEyeTextures[10] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-10 Frame-0.png");
    resources.rbowEyeTextures[11] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-11 Frame-0.png");
    resources.rbowEyeTextures[12] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-12 Frame-0.png");
    resources.rbowEyeTextures[13] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-13 Frame-0.png");
    resources.rbowEyeTextures[14] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-14 Frame-0.png");
    resources.rbowEyeTextures[15] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-15 Frame-0.png");
    resources.rbowEyeTextures[16] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-16 Frame-0.png");
    resources.rbowEyeTextures[17] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-17 Frame-0.png");
    resources.rbowEyeTextures[18] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-18 Frame-0.png");
    resources.rbowEyeTextures[19] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-19 Frame-0.png");
    resources.rbowEyeTextures[20] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-20 Frame-0.png");
    resources.rbowEyeTextures[21] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-21 Frame-0.png");
    resources.rbowEyeTextures[22] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-22 Frame-0.png");
    resources.rbowEyeTextures[23] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-23 Frame-0.png");
    resources.rbowEyeTextures[24] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-24 Frame-0.png");
    resources.rbowEyeTextures[25] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-25 Frame-0.png");
    resources.rbowEyeTextures[26] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-26 Frame-0.png");
    resources.rbowEyeTextures[27] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-27 Frame-0.png");
    resources.rbowEyeTextures[28] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-28 Frame-0.png");
    resources.rbowEyeTextures[29] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-29 Frame-0.png");
    resources.rbowEyeTextures[30] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-30 Frame-0.png");
    resources.rbowEyeTextures[31] = LoadTexture("resources/cmrl/textures/sprites/rainbow/rainbow_eyes/Animation-0 Direction-31 Frame-0.png");


// Butterfly
    resources.butterfly = LoadTexture("resources/cmrl/textures/butterfly/butterfly.png");
    resources.bflyLaser = LoadTexture("resources/cmrl/textures/laser/bflyBeam.png");

// Background, Tiles & UI
    resources.bg = LoadTexture("resources/cmrl/background.png");
    resources.tile = LoadTexture("resources/cmrl/textures/misc/dirt_tile.png");
    resources.platformTexture = LoadTexture("resources/cmrl/textures/misc/platform_tile.png");

    resources.sunflower = LoadTexture("resources/cmrl/textures/sunflower/deadly_flower.png");

    resources.sunflowerPetals.resize(resources.numPetals);
    resources.sunflowerPetals[0] = LoadTexture("resources/cmrl/textures/sunflower/pettleN.png");
    resources.sunflowerPetals[1] = LoadTexture("resources/cmrl/textures/sunflower/pettleNE.png");
    resources.sunflowerPetals[2] = LoadTexture("resources/cmrl/textures/sunflower/pettleE.png");
    resources.sunflowerPetals[3] = LoadTexture("resources/cmrl/textures/sunflower/pettleSE.png");
    resources.sunflowerPetals[4] = LoadTexture("resources/cmrl/textures/sunflower/pettleS.png");
    resources.sunflowerPetals[5] = LoadTexture("resources/cmrl/textures/sunflower/pettleSW.png");
    resources.sunflowerPetals[6] = LoadTexture("resources/cmrl/textures/sunflower/pettleW.png");
    resources.sunflowerPetals[7] = LoadTexture("resources/cmrl/textures/sunflower/pettleNW.png");

    resources.fenceProp = LoadTexture("resources/cmrl/textures/misc/fence.png");
    resources.flowerBigProp = LoadTexture("resources/cmrl/textures/misc/decoration_flower_big.png");
    resources.flowerSmallProp = LoadTexture("resources/cmrl/textures/misc/decoration_flower_small.png");
    resources.checkpointFlag = LoadTexture("resources/cmrl/textures/misc/checkpoint_flag.png");

    resources.log = LoadTexture("resources/cmrl/textures/misc/log.png");
    resources.spikesUp = LoadTexture("resources/cmrl/textures/misc/spikes_up.png");
    resources.spikesDown = LoadTexture("resources/cmrl/textures/misc/spikes_down.png");

    resources.bloodParticle = LoadTexture("resources/cmrl/textures/misc/blood_particle.png");
    resources.buttonVoicesOn = LoadTexture("resources/cmrl/textures/text/voices_on.png");
    resources.buttonVoicesOff = LoadTexture("resources/cmrl/textures/text/voices_off.png");

    int animFrames = 0;
    resources.checkpointAnimImage = LoadImageAnim("resources/textures/checkpoint_anim.gif", &animFrames);

    resources.controlKeysInfo = LoadTexture("resources/cmrl/textures/text/control_keys_tutorial.png");
    resources.greatJob = LoadTexture("resources/cmrl/textures/text/great_job.png");
    resources.youSuck = LoadTexture("resources/cmrl/textures/text/you_suck.png");


// Music & FX
    const char* musicPath = "resources/cmrl/rainbow.mp3";
    const char* respawnPath = "resources/cmrl/audio/sfx/respawn.wav";
    const char* deathPath = "resources/cmrl/audio/sfx/death.wav";
    const char* jumpPath = "resources/cmrl/audio/sfx/jump.wav";
    const char* eyePopPath = "resources/cmrl/audio/sfx/eye.wav";
    const char* checkpointPath = "resources/cmrl/audio/sfx/checkpoint.wav";
    const char* logsPath = "resources/cmrl/audio/sfx/logs.wav";
    const char* petalPath = "resources/cmrl/audio/sfx/petal.wav";
    const char* rbowLaserShootPath = "resources/cmrl/audio/sfx/rlaser.wav";
    const char* bflyLaserShootPath = "resources/cmrl/audio/sfx/blaser.wav";

    const char* rbowVoiceOffPath = "resources/cmrl/audio/voice/voiceOff.mp3";
    const char* rbowGonnaKillYouPath = "resources/cmrl/audio/voice/imgonnakillyou.mp3";
    const char* rbowLastChancePath = "resources/cmrl/audio/voice/lastchance.mp3";
    const char* rbowHaPath = "resources/cmrl/audio/voice/HA.wav";




    resources.rbowDialogues.resize(resources.numRbowVoiceLines);
    for (int i = 0; i < resources.numRbowVoiceLines; i++) {
        std::string filePath = "resources/cmrl/audio/voice/voice";
        if (i < 9) {
            filePath += "0";
        }
        filePath += std::to_string(i + 1);
        filePath += ".mp3";
        resources.rbowDialogues[i] = LoadSound(filePath.c_str());
    }

    resources.backgroundMusic = LoadMusicStream(musicPath);
    resources.respawn = LoadSound(respawnPath);
    resources.death = LoadSound(deathPath);
    resources.jump = LoadSound(jumpPath);
    resources.eyePop = LoadSound(eyePopPath);
    resources.checkpointSound = LoadSound(checkpointPath);
    resources.logs = LoadSound(jumpPath);
    resources.petalShoot = LoadSound(petalPath);
    resources.rbowLaserShoot = LoadSound(rbowLaserShootPath);
    resources.bflyLaserShoot = LoadSound(bflyLaserShootPath);

    resources.rbowVoiceOff = LoadSound(rbowVoiceOffPath);
    resources.rbowGonnaKillYou = LoadSound(rbowGonnaKillYouPath);
    resources.rbowHA = LoadSound(rbowHaPath);

    resources.backgroundMusicLoaded = (GetMusicTimeLength(resources.backgroundMusic) > 0); 
    resources.respawnSoundLoaded = (resources.respawn.frameCount > 0);
    resources.deathSoundLoaded = (resources.death.frameCount > 0);
    resources.jumpSoundLoaded = (resources.jump.frameCount > 0);
    resources.eyePopSoundLoaded = (resources.eyePop.frameCount > 0);
    resources.checkpointSoundLoaded = (resources.checkpointSound.frameCount > 0);
    resources.logsSoundLoaded = (resources.logs.frameCount > 0);
    resources.petalSoundLoaded = (resources.petalShoot.frameCount > 0);
    resources.bflyLaserShootSoundLoaded = (resources.bflyLaserShoot.frameCount > 0);
    resources.rbowLaserShootSoundLoaded = (resources.rbowLaserShoot.frameCount > 0);

    resources.rbowVoiceOffSoundLoaded = (resources.rbowVoiceOff.frameCount > 0);
    resources.rbowGonnaKillYouSoundLoaded = (resources.rbowGonnaKillYou.frameCount > 0);
    resources.rbowLastChanceSoundLoaded = (resources.rbowLastChance.frameCount > 0);
    resources.rbowHASoundLoaded = (resources.rbowHA.frameCount > 0);
    resources.rbowDialoguesSoundLoaded = (resources.rbowDialogues.size() > 0);


    resources.checkpointAnimLoaded = false; // Initialize
    resources.checkpointSoundLoaded = false; // Initialize
    if (resources.checkpointAnimImage.data != NULL && animFrames > 0) {
        resources.checkpointAnimTexture = LoadTextureFromImage(resources.checkpointAnimImage);

        if (resources.checkpointAnimTexture.id > 0) {
            resources.checkpointAnimNumFrames = animFrames;
            resources.checkpointAnimFrameDelay = 0.15f; // Adjust as needed (seconds per frame)
            resources.checkpointAnimLoaded = true;
            TraceLog(LOG_INFO, "Loaded checkpoint animation (%d frames). Image W:%d H:%d. Texture W:%d H:%d",
                animFrames, resources.checkpointAnimImage.width, resources.checkpointAnimImage.height,
                resources.checkpointAnimTexture.width, resources.checkpointAnimTexture.height);
        }
    }

    TraceLog(LOG_INFO, "Game resources loaded successfully.");

    return resources;
}


// Unloadind Resources
void UnloadRainbowLandResources(RainbowLandGameResources& resources) {
    TraceLog(LOG_INFO, "Unloading game resources...");

    if (resources.idleRight.id > 0) UnloadTexture(resources.idleRight);
    if (resources.idleLeft.id > 0) UnloadTexture(resources.idleLeft);
    if (resources.walkRight.id > 0) UnloadTexture(resources.walkRight);
    if (resources.walkLeft.id > 0) UnloadTexture(resources.walkLeft);
    if (resources.jumpRight.id > 0) UnloadTexture(resources.jumpRight);
    if (resources.jumpLeft.id > 0) UnloadTexture(resources.jumpLeft);

    if (resources.rbowBodyTexture.id > 0) UnloadTexture(resources.rbowBodyTexture);
    if (resources.rbowVoiceOffBodyTexture.id > 0) UnloadTexture(resources.rbowVoiceOffBodyTexture);

    if (resources.butterfly.id > 0) UnloadTexture(resources.butterfly);
    if (resources.bflyLaser.id > 0) UnloadTexture(resources.bflyLaser);

    if (resources.bg.id > 0) UnloadTexture(resources.bg);
    if (resources.tile.id > 0) UnloadTexture(resources.tile);
    if (resources.platformTexture.id > 0) UnloadTexture(resources.platformTexture);

    if (resources.sunflower.id > 0) UnloadTexture(resources.sunflower);

    if (resources.sunflowerPetals[0].id > 0) UnloadTexture(resources.sunflowerPetals[0]);
    if (resources.sunflowerPetals[1].id > 0) UnloadTexture(resources.sunflowerPetals[1]);
    if (resources.sunflowerPetals[2].id > 0) UnloadTexture(resources.sunflowerPetals[2]);
    if (resources.sunflowerPetals[3].id > 0) UnloadTexture(resources.sunflowerPetals[3]);
    if (resources.sunflowerPetals[4].id > 0) UnloadTexture(resources.sunflowerPetals[4]);
    if (resources.sunflowerPetals[5].id > 0) UnloadTexture(resources.sunflowerPetals[5]);
    if (resources.sunflowerPetals[6].id > 0) UnloadTexture(resources.sunflowerPetals[6]);
    if (resources.sunflowerPetals[7].id > 0) UnloadTexture(resources.sunflowerPetals[7]);

    if (resources.fenceProp.id > 0) UnloadTexture(resources.fenceProp);
    if (resources.flowerBigProp.id > 0) UnloadTexture(resources.flowerBigProp);
    if (resources.flowerSmallProp.id > 0) UnloadTexture(resources.flowerSmallProp);
    if (resources.checkpointFlag.id > 0) UnloadTexture(resources.checkpointFlag);
    if (resources.log.id > 0) UnloadTexture(resources.log);
    if (resources.spikesUp.id > 0) UnloadTexture(resources.spikesUp);
    if (resources.spikesDown.id > 0) UnloadTexture(resources.spikesDown);

    if (resources.bloodParticle.id > 0) UnloadTexture(resources.bloodParticle);
    if (resources.buttonVoicesOn.id > 0) UnloadTexture(resources.buttonVoicesOn);
    if (resources.buttonVoicesOff.id > 0) UnloadTexture(resources.buttonVoicesOff);
    if (resources.checkpoint.id > 0) UnloadTexture(resources.checkpoint);
    if (resources.controlKeysInfo.id > 0) UnloadTexture(resources.controlKeysInfo);
    if (resources.greatJob.id > 0) UnloadTexture(resources.greatJob);
    if (resources.youSuck.id > 0) UnloadTexture(resources.youSuck);

    if (resources.checkpointAnimLoaded) {
        UnloadTexture(resources.checkpointAnimTexture);
        UnloadImage(resources.checkpointAnimImage);
    }
        

    if (resources.rbowEyeTextures[0].id > 0) UnloadTexture(resources.rbowEyeTextures[0]);
    if (resources.rbowEyeTextures[1].id > 0) UnloadTexture(resources.rbowEyeTextures[1]);
    if (resources.rbowEyeTextures[2].id > 0) UnloadTexture(resources.rbowEyeTextures[2]);
    if (resources.rbowEyeTextures[3].id > 0) UnloadTexture(resources.rbowEyeTextures[3]);
    if (resources.rbowEyeTextures[4].id > 0) UnloadTexture(resources.rbowEyeTextures[4]);
    if (resources.rbowEyeTextures[5].id > 0) UnloadTexture(resources.rbowEyeTextures[5]);
    if (resources.rbowEyeTextures[6].id > 0) UnloadTexture(resources.rbowEyeTextures[6]);
    if (resources.rbowEyeTextures[7].id > 0) UnloadTexture(resources.rbowEyeTextures[7]);
    if (resources.rbowEyeTextures[8].id > 0) UnloadTexture(resources.rbowEyeTextures[8]);
    if (resources.rbowEyeTextures[9].id > 0) UnloadTexture(resources.rbowEyeTextures[9]);
    if (resources.rbowEyeTextures[10].id > 0) UnloadTexture(resources.rbowEyeTextures[10]);
    if (resources.rbowEyeTextures[11].id > 0) UnloadTexture(resources.rbowEyeTextures[11]);
    if (resources.rbowEyeTextures[12].id > 0) UnloadTexture(resources.rbowEyeTextures[12]);
    if (resources.rbowEyeTextures[13].id > 0) UnloadTexture(resources.rbowEyeTextures[13]);
    if (resources.rbowEyeTextures[14].id > 0) UnloadTexture(resources.rbowEyeTextures[14]);
    if (resources.rbowEyeTextures[15].id > 0) UnloadTexture(resources.rbowEyeTextures[15]);
    if (resources.rbowEyeTextures[16].id > 0) UnloadTexture(resources.rbowEyeTextures[16]);
    if (resources.rbowEyeTextures[17].id > 0) UnloadTexture(resources.rbowEyeTextures[17]);
    if (resources.rbowEyeTextures[18].id > 0) UnloadTexture(resources.rbowEyeTextures[18]);
    if (resources.rbowEyeTextures[19].id > 0) UnloadTexture(resources.rbowEyeTextures[19]);
    if (resources.rbowEyeTextures[20].id > 0) UnloadTexture(resources.rbowEyeTextures[20]);
    if (resources.rbowEyeTextures[21].id > 0) UnloadTexture(resources.rbowEyeTextures[21]);
    if (resources.rbowEyeTextures[22].id > 0) UnloadTexture(resources.rbowEyeTextures[22]);
    if (resources.rbowEyeTextures[23].id > 0) UnloadTexture(resources.rbowEyeTextures[23]);
    if (resources.rbowEyeTextures[24].id > 0) UnloadTexture(resources.rbowEyeTextures[24]);
    if (resources.rbowEyeTextures[25].id > 0) UnloadTexture(resources.rbowEyeTextures[25]);
    if (resources.rbowEyeTextures[26].id > 0) UnloadTexture(resources.rbowEyeTextures[26]);
    if (resources.rbowEyeTextures[27].id > 0) UnloadTexture(resources.rbowEyeTextures[27]);
    if (resources.rbowEyeTextures[28].id > 0) UnloadTexture(resources.rbowEyeTextures[28]);
    if (resources.rbowEyeTextures[29].id > 0) UnloadTexture(resources.rbowEyeTextures[29]);
    if (resources.rbowEyeTextures[30].id > 0) UnloadTexture(resources.rbowEyeTextures[30]);
    if (resources.rbowEyeTextures[31].id > 0) UnloadTexture(resources.rbowEyeTextures[31]);

    if (resources.backgroundMusicLoaded) UnloadMusicStream(resources.backgroundMusic);
    if (resources.respawnSoundLoaded) UnloadSound(resources.respawn);
    if (resources.deathSoundLoaded) UnloadSound(resources.death);
    if (resources.jumpSoundLoaded) UnloadSound(resources.jump);
    if (resources.eyePopSoundLoaded) UnloadSound(resources.eyePop);
    if (resources.checkpointSoundLoaded) UnloadSound(resources.checkpointSound);
    if (resources.logsSoundLoaded) UnloadSound(resources.logs);
    if (resources.petalSoundLoaded) UnloadSound(resources.petalShoot);
    if (resources.rbowLaserShootSoundLoaded) UnloadSound(resources.rbowLaserShoot);
    if (resources.bflyLaserShootSoundLoaded) UnloadSound(resources.bflyLaserShoot);


    if (resources.rbowVoiceOffSoundLoaded) UnloadSound(resources.rbowVoiceOff);
    if (resources.rbowGonnaKillYouSoundLoaded) UnloadSound(resources.rbowGonnaKillYou);
    if (resources.rbowHASoundLoaded) UnloadSound(resources.rbowHA);

    if (resources.rbowDialoguesSoundLoaded) {
        for (size_t i = 0; i < resources.rbowDialogues.size(); i++) {
            UnloadSound(resources.rbowDialogues[i]);
        }
    }
}