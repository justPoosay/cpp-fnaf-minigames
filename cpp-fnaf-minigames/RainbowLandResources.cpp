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
    for (int i = 0; i < resources.numEyeSprites; i++) {
        std::string filePath = "resources/cmrl/textures/sprites/rainbow/eyes/eye";
        if (i < 9) {
            filePath += "0";
        }
        filePath += std::to_string(i + 1);
        filePath += ".png";
        resources.rbowEyeTextures[i] = LoadTexture(filePath.c_str());
    }

    resources.laserBeamFrame = LoadTexture("resources/cmrl/textures/laser/bflyBeam.png");

// Butterfly
    resources.butterfly = LoadTexture("resources/cmrl/textures/butterfly/butterfly.png");

// Background, Tiles & UI
    resources.cursor = LoadTexture("resources/cursor.png");
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
    resources.checkpoint = LoadTexture("resources/cmrl/textures/text/checkpoint.png");

    resources.controlKeysInfo = LoadTexture("resources/cmrl/textures/text/control_keys_tutorial.png");
    resources.greatJob = LoadTexture("resources/cmrl/textures/text/great_job.png");
    resources.youSuck = LoadTexture("resources/cmrl/textures/text/you_suck.png");


// Music & FX
    resources.rbowDialogues.resize(resources.numRbowVoiceLines);
    for (int i = 0; i < resources.numRbowVoiceLines; i++) {
        std::string filePath = "resources/cmrl/audio/rVoice/voice";
        if (i < 9) {
            filePath += "0";
        }
        filePath += std::to_string(i + 1);
        filePath += ".mp3";
        resources.rbowDialogues[i] = LoadSound(filePath.c_str());
    }

    resources.backgroundMusic = LoadMusicStream("resources/cmrl/rainbow.mp3");
    resources.respawn = LoadSound("resources/cmrl/audio/sfx/respawn.wav");
    resources.death = LoadSound("resources/cmrl/audio/sfx/death.wav");
    resources.jump = LoadSound("resources/cmrl/audio/sfx/jump.wav");
    resources.eyePop = LoadSound("resources/cmrl/audio/sfx/eye.wav");
    resources.checkpointSound = LoadSound("resources/cmrl/audio/sfx/checkpoint.wav");
    resources.logs = LoadSound("resources/cmrl/audio/sfx/logs.wav");
    resources.spikesPush = LoadSound("resources/cmrl/audio/sfx/spikes.wav");
    resources.petalShoot = LoadSound("resources/cmrl/audio/sfx/petal.wav");
    resources.rbowLaserShoot = LoadSound("resources/cmrl/audio/sfx/rlaser.wav");
    resources.bflyLaserShoot = LoadSound("resources/cmrl/audio/sfx/blaser.wav");

    resources.rbowVoiceOff = LoadSound("resources/cmrl/audio/rVoice/voiceOff.mp3");
    resources.rbowGonnaKillYou = LoadSound("resources/cmrl/audio/rVoice/imgonnakillyou.mp3");
    resources.rbowYouStillSuck = LoadSound("resources/cmrl/audio/rVoice/youStillSuck.mp3");

    resources.backgroundMusicLoaded = (GetMusicTimeLength(resources.backgroundMusic) > 0); 

    resources.respawnSoundLoaded = (resources.respawn.frameCount > 0);
    resources.deathSoundLoaded = (resources.death.frameCount > 0);
    resources.jumpSoundLoaded = (resources.jump.frameCount > 0);
    resources.eyePopSoundLoaded = (resources.eyePop.frameCount > 0);
    resources.checkpointSoundLoaded = (resources.checkpointSound.frameCount > 0);
    resources.logsSoundLoaded = (resources.logs.frameCount > 0);
    resources.spikesSoundLoaded = (resources.spikesPush.frameCount > 0);
    resources.petalSoundLoaded = (resources.petalShoot.frameCount > 0);
    resources.bflyLaserShootSoundLoaded = (resources.bflyLaserShoot.frameCount > 0);
    resources.rbowLaserShootSoundLoaded = (resources.rbowLaserShoot.frameCount > 0);

    resources.rbowVoiceOffSoundLoaded = (resources.rbowVoiceOff.frameCount > 0);
    resources.rbowGonnaKillYouSoundLoaded = (resources.rbowGonnaKillYou.frameCount > 0);
    resources.rbowYouStillSuckSoundLoaded = (resources.rbowYouStillSuck.frameCount > 0);
    resources.rbowDialoguesSoundLoaded = (resources.rbowDialogues.size() > 0);

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
    if (resources.laserBeamFrame.id > 0) UnloadTexture(resources.laserBeamFrame);

    if (resources.cursor.id > 0) UnloadTexture(resources.cursor);
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

    if (resources.rbowEyeTextures[1].id > 0) 
        for (int i = 0; i < resources.rbowEyeTextures.size(); i++) 
            UnloadTexture(resources.rbowEyeTextures[i]);
        
    if (resources.backgroundMusicLoaded) UnloadMusicStream(resources.backgroundMusic);
    if (resources.respawnSoundLoaded) UnloadSound(resources.respawn);
    if (resources.deathSoundLoaded) UnloadSound(resources.death);
    if (resources.jumpSoundLoaded) UnloadSound(resources.jump);
    if (resources.eyePopSoundLoaded) UnloadSound(resources.eyePop);
    if (resources.checkpointSoundLoaded) UnloadSound(resources.checkpointSound);
    if (resources.logsSoundLoaded) UnloadSound(resources.logs);
    if (resources.spikesSoundLoaded) UnloadSound(resources.spikesPush);
    if (resources.petalSoundLoaded) UnloadSound(resources.petalShoot);
    if (resources.rbowLaserShootSoundLoaded) UnloadSound(resources.rbowLaserShoot);
    if (resources.bflyLaserShootSoundLoaded) UnloadSound(resources.bflyLaserShoot);


    if (resources.rbowVoiceOffSoundLoaded) UnloadSound(resources.rbowVoiceOff);
    if (resources.rbowGonnaKillYouSoundLoaded) UnloadSound(resources.rbowGonnaKillYou);
    if (resources.rbowYouStillSuckSoundLoaded) UnloadSound(resources.rbowYouStillSuck);

    if (resources.rbowDialoguesSoundLoaded) 
        for (size_t i = 0; i < resources.rbowDialogues.size(); i++) 
            UnloadSound(resources.rbowDialogues[i]);

}