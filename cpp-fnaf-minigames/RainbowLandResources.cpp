#include "RainbowLandResources.h"

RainbowLandGameResources LoadRainbowLandResources(GraphicsQuality quality) {
    cout << "Loading game resources..." << endl;
    RainbowLandGameResources resources;

// Chica (player)
    resources.idleRight = LoadTexture("resources/ChicasMagicRainbowLand/Textures/chica_idle_right.png");
    resources.idleLeft = LoadTexture("resources/ChicasMagicRainbowLand/Textures/chica_idle_left.png");
    resources.walkRight = LoadTexture("resources/ChicasMagicRainbowLand/Textures/chica_walk_right.png");
    resources.walkLeft = LoadTexture("resources/ChicasMagicRainbowLand/Textures/chica_walk_left.png");
    resources.jumpRight = LoadTexture("resources/ChicasMagicRainbowLand/Textures/chica_jump_right.png");
    resources.jumpLeft = LoadTexture("resources/ChicasMagicRainbowLand/Textures/chica_jump_left.png");


// Rainbow & Eyes
    resources.rbowBodyTexture = LoadTexture("resources/ChicasMagicRainbowLand/Textures/chica_rainbow.png");
    resources.rbowVoiceOffBodyTexture = LoadTexture("resources/ChicasMagicRainbowLand/Textures/chica_rainbow_pissed.png");

    resources.rbowEyeTextures.resize(resources.numEyeSprites);
    for (int i = 0; i < resources.numEyeSprites; i++) {
        string filePath = "resources/ChicasMagicRainbowLand/Textures/eye";
        if (i < 9) filePath += "0";
        
        filePath += to_string(i + 1);
        filePath += ".png";
        resources.rbowEyeTextures[i] = LoadTexture(filePath.c_str());
    }

    resources.laserBeamFrame = LoadTexture("resources/ChicasMagicRainbowLand/Textures/bflyBeam.png");

// Butterfly
    resources.butterfly = LoadTexture("resources/ChicasMagicRainbowLand/Textures/butterfly.png");

// Background, Tiles & UI
    resources.cursor = LoadTexture("resources/cursor.png");
    resources.bg = LoadTexture("resources/ChicasMagicRainbowLand/Textures/background.png");
    resources.tile = LoadTexture("resources/ChicasMagicRainbowLand/Textures/dirt_tile.png");
    resources.platformTexture = LoadTexture("resources/ChicasMagicRainbowLand/Textures/platform_tile.png");

    resources.sunflower = LoadTexture("resources/ChicasMagicRainbowLand/Textures/deadly_flower.png");

    resources.sunflowerPetals.resize(resources.numPetals);
    resources.sunflowerPetals[0] = LoadTexture("resources/ChicasMagicRainbowLand/Textures/pettleN.png");
    resources.sunflowerPetals[1] = LoadTexture("resources/ChicasMagicRainbowLand/Textures/pettleNE.png");
    resources.sunflowerPetals[2] = LoadTexture("resources/ChicasMagicRainbowLand/Textures/pettleE.png");
    resources.sunflowerPetals[3] = LoadTexture("resources/ChicasMagicRainbowLand/Textures/pettleSE.png");
    resources.sunflowerPetals[4] = LoadTexture("resources/ChicasMagicRainbowLand/Textures/pettleS.png");
    resources.sunflowerPetals[5] = LoadTexture("resources/ChicasMagicRainbowLand/Textures/pettleSW.png");
    resources.sunflowerPetals[6] = LoadTexture("resources/ChicasMagicRainbowLand/Textures/pettleW.png");
    resources.sunflowerPetals[7] = LoadTexture("resources/ChicasMagicRainbowLand/Textures/pettleNW.png");

    resources.fenceProp = LoadTexture("resources/ChicasMagicRainbowLand/Textures/fence.png");
    resources.flowerBigProp = LoadTexture("resources/ChicasMagicRainbowLand/Textures/decoration_flower_big.png");
    resources.flowerSmallProp = LoadTexture("resources/ChicasMagicRainbowLand/Textures/decoration_flower_small.png");
    resources.checkpointFlag = LoadTexture("resources/ChicasMagicRainbowLand/Textures/checkpoint_flag.png");

    resources.log = LoadTexture("resources/ChicasMagicRainbowLand/Textures/log.png");
    resources.spikesUp = LoadTexture("resources/ChicasMagicRainbowLand/Textures/spikes_up.png");
    resources.spikesDown = LoadTexture("resources/ChicasMagicRainbowLand/Textures/spikes_down.png");

    resources.bloodParticle = LoadTexture("resources/ChicasMagicRainbowLand/Textures/blood_particle.png");
    resources.buttonVoicesOn = LoadTexture("resources/ChicasMagicRainbowLand/Textures/voices_on.png");
    resources.buttonVoicesOff = LoadTexture("resources/ChicasMagicRainbowLand/Textures/voices_off.png");
    resources.checkpoint = LoadTexture("resources/ChicasMagicRainbowLand/Textures/checkpoint.png");

    resources.controlKeysInfo = LoadTexture("resources/ChicasMagicRainbowLand/Textures/control_keys_tutorial.png");
    resources.greatJob = LoadTexture("resources/ChicasMagicRainbowLand/Textures/great_job.png");
    resources.youSuck = LoadTexture("resources/ChicasMagicRainbowLand/Textures/you_suck.png");


// Music & FX
    resources.rbowDialogues.resize(resources.numRbowVoiceLines);
    for (int i = 0; i < resources.numRbowVoiceLines; i++) {
        string filePath = "resources/ChicasMagicRainbowLand/Audio/voice";
        if (i < 9) filePath += "0";
        
        filePath += to_string(i + 1);
        filePath += ".mp3";
        resources.rbowDialogues[i] = LoadSound(filePath.c_str());
    }

    resources.backgroundMusic = LoadMusicStream("resources/ChicasMagicRainbowLand/Audio/rainbow.mp3");
    resources.respawn = LoadSound("resources/ChicasMagicRainbowLand/Audio/respawn.wav");
    resources.death = LoadSound("resources/ChicasMagicRainbowLand/Audio/death.wav");
    resources.jump = LoadSound("resources/ChicasMagicRainbowLand/Audio/jump.wav");
    resources.eyePop = LoadSound("resources/ChicasMagicRainbowLand/Audio/eye.wav");
    resources.checkpointSound = LoadSound("resources/ChicasMagicRainbowLand/Audio/checkpoint.wav");
    resources.logs = LoadSound("resources/ChicasMagicRainbowLand/Audio/logs.wav");
    resources.spikesPush = LoadSound("resources/ChicasMagicRainbowLand/Audio/spikes.wav");
    resources.petalShoot = LoadSound("resources/ChicasMagicRainbowLand/Audio/petal.wav");
    resources.rbowLaserShoot = LoadSound("resources/ChicasMagicRainbowLand/Audio/rlaser.wav");
    resources.bflyLaserShoot = LoadSound("resources/ChicasMagicRainbowLand/Audio/blaser.wav");

    resources.rbowVoiceOff = LoadSound("resources/ChicasMagicRainbowLand/Audio/voiceOff.mp3");
    resources.rbowGonnaKillYou = LoadSound("resources/ChicasMagicRainbowLand/Audio/imgonnakillyou.mp3");
    resources.rbowYouStillSuck = LoadSound("resources/ChicasMagicRainbowLand/Audio/youStillSuck.mp3");

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

    cout << "Game resources loaded successfully." << endl;

    return resources;
}

void UnloadRainbowLandResources(RainbowLandGameResources& resources) {
    cout << "Unloading game resources..." << endl;

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
        for (int i = 0; i < resources.rbowDialogues.size(); i++) 
            UnloadSound(resources.rbowDialogues[i]);
}

bool CheckRainbowLandResourcesLoaded(RainbowLandGameResources& res) {
    vector<string> missingResources;

    if (res.idleRight.id == 0) missingResources.push_back("idleRight");
    if (res.idleLeft.id == 0) missingResources.push_back("idleLeft");
    if (res.walkRight.id == 0) missingResources.push_back("walkRight");
    if (res.walkLeft.id == 0) missingResources.push_back("walkLeft");
    if (res.jumpRight.id == 0) missingResources.push_back("jumpRight");
    if (res.jumpLeft.id == 0) missingResources.push_back("jumpLeft");
    if (res.bg.id == 0) missingResources.push_back("bg");
    if (res.tile.id == 0) missingResources.push_back("tile");
    if (res.platformTexture.id == 0) missingResources.push_back("platformTexture");
    if (res.butterfly.id == 0) missingResources.push_back("butterfly");
    if (res.spikesUp.id == 0) missingResources.push_back("spikesUp");
    if (res.spikesDown.id == 0) missingResources.push_back("spikesDown");
    if (res.controlKeysInfo.id == 0) missingResources.push_back("controlKeysInfo");
    if (res.fenceProp.id == 0) missingResources.push_back("fenceProp");
    if (res.flowerSmallProp.id == 0) missingResources.push_back("flowerSmallProp");
    if (res.flowerBigProp.id == 0) missingResources.push_back("flowerBigProp");
    if (res.checkpointFlag.id == 0) missingResources.push_back("checkpointFlag");
    if (res.sunflower.id == 0) missingResources.push_back("sunflower");
    if (res.checkpoint.id == 0) missingResources.push_back("checkpoint");
    if (res.rbowBodyTexture.id == 0) missingResources.push_back("rbowBodyTexture");

    for (int i = 0; i < res.rbowEyeTextures.size(); i++)
        if (res.rbowEyeTextures[i].id == 0)
            missingResources.push_back("rbowEyeTextures[" + to_string(i) + "]");

    for (int i = 0; i < res.sunflowerPetals.size(); i++)
        if (res.sunflowerPetals[i].id == 0)
            missingResources.push_back("sunflowerPetals[" + to_string(i) + "]");

    if (res.buttonVoicesOn.id == 0) missingResources.push_back("buttonVoicesOn");
    if (res.buttonVoicesOff.id == 0) missingResources.push_back("buttonVoicesOff");
    if (!res.backgroundMusicLoaded) missingResources.push_back("backgroundMusic");
    if (!res.jumpSoundLoaded) missingResources.push_back("FX: jump");
    if (!res.petalSoundLoaded) missingResources.push_back("FX: petalShoot");
    if (!res.spikesSoundLoaded) missingResources.push_back("FX: spikesPush");
    if (!res.deathSoundLoaded) missingResources.push_back("FX: death");
    if (!res.checkpointSoundLoaded) missingResources.push_back("FX: checkpoint");

    for (int i = 0; i < res.rbowDialogues.size(); i++)
        if (res.rbowDialogues[i].frameCount == 0)
            missingResources.push_back("rbowDialogues[" + to_string(i) + "]");

    if (!res.rbowVoiceOffSoundLoaded) missingResources.push_back("rbowVoiceOff");
    if (!res.rbowYouStillSuckSoundLoaded) missingResources.push_back("rbowYouStillSuck");


    if (!missingResources.empty()) {
        cout << "RAINBOW LAND: Critical error - failed to load resources:" << endl;

        for (const string& resourceName : missingResources)
            cout << resourceName << endl;

        return false;
    }

    return true;
}