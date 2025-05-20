#include "MotoristResources.h"

MotoristGameResources LoadMotoristResources(GraphicsQuality quality) {
    cout << "Loading game resources..." << endl;
    MotoristGameResources resources;

    // Car (player & NPC)
    resources.car = LoadTexture("resources/mm/textures/car.png");

    resources.carSpinFrames.resize(19);
    for (int i = 0; i < 19; i++) {
        string filename = TextFormat("resources/mm/textures/carSpin_%02d.png", i + 1);
        resources.carSpinFrames[i] = LoadTexture(filename.c_str());
    }

    resources.rightCarNPC = LoadTexture("resources/mm/textures/leftCarNPC.png");
    resources.leftCarNPC = LoadTexture("resources/mm/textures/rightCarNPC.png");

    // Background & UI
    resources.bg = LoadTexture("resources/mm/textures/bg.png");
    resources.one = LoadTexture("resources/mm/textures/1.png");
    resources.two = LoadTexture("resources/mm/textures/2.png");
    resources.three = LoadTexture("resources/mm/textures/3.png");
    resources.go = LoadTexture("resources/mm/textures/go.png");
    resources.wasd = LoadTexture("resources/mm/textures/wasd.png");
    resources.lap = LoadTexture("resources/mm/textures/lap.png");
    resources.lapMark = LoadTexture("resources/mm/textures/lapMark.png");
    resources.goal = LoadTexture("resources/mm/textures/goal.png");

    resources.gameFont = LoadFont("resources/ARCADECLASSIC.TTF");


    // Music & FX
    resources.backgroundMusic = LoadMusicStream("resources/mm/audio/motorist.mp3");
    resources.bgNoise = LoadMusicStream("resources/mm/audio/bgNoise.wav");
    resources.countdown = LoadSound("resources/mm/audio/countdown.wav");
    resources.ctdwnGo = LoadSound("resources/mm/audio/go.wav");
    resources.carCrash = LoadSound("resources/mm/audio/crash.wav");
    resources.lapReached = LoadSound("resources/mm/audio/lap.wav");
    resources.goalReached = LoadSound("resources/mm/audio/goal.wav");

    resources.backgroundMusicLoaded = (GetMusicTimeLength(resources.backgroundMusic) > 0);
    resources.bgNoiseLoaded = (GetMusicTimeLength(resources.bgNoise) > 0);
    resources.countdownSoundLoaded = (resources.countdown.frameCount > 0);
    resources.goSoundLoaded = (resources.ctdwnGo.frameCount > 0);
    resources.carCrashSoundLoaded = (resources.carCrash.frameCount > 0);
    resources.lapSoundLoaded = (resources.lapReached.frameCount > 0);
    resources.goalSoundLoaded = (resources.goalReached.frameCount > 0);

    cout << "Game resources loaded successfully." << endl;

    return resources;
}


// Unloadind Resources
void UnloadMotoristResources(MotoristGameResources& resources) {
    cout << "Unloading game resources..." << endl;

    if (resources.car.id > 0) UnloadTexture(resources.car);
    for (const auto& frame : resources.carSpinFrames) UnloadTexture(frame);

    if (resources.leftCarNPC.id > 0) UnloadTexture(resources.leftCarNPC);
    if (resources.rightCarNPC.id > 0) UnloadTexture(resources.rightCarNPC);

    if (resources.bg.id > 0) UnloadTexture(resources.bg);
    if (resources.one.id > 0) UnloadTexture(resources.one);
    if (resources.two.id > 0) UnloadTexture(resources.two);
    if (resources.three.id > 0) UnloadTexture(resources.three);
    if (resources.go.id > 0) UnloadTexture(resources.go);
    if (resources.wasd.id > 0) UnloadTexture(resources.wasd);
    if (resources.lap.id > 0) UnloadTexture(resources.lap);
    if (resources.lapMark.id > 0) UnloadTexture(resources.lapMark);
    if (resources.goal.id > 0) UnloadTexture(resources.goal);

    if (resources.backgroundMusicLoaded) UnloadMusicStream(resources.backgroundMusic);
    if (resources.bgNoiseLoaded) UnloadMusicStream(resources.bgNoise);
    if (resources.countdownSoundLoaded) UnloadSound(resources.countdown);
    if (resources.carCrashSoundLoaded) UnloadSound(resources.goalReached);
    if (resources.lapSoundLoaded) UnloadSound(resources.lapReached);

    UnloadFont(resources.gameFont);
}

bool CheckMotoristResourcesLoaded(MotoristGameResources& res) {
    vector<string> missingResources;

    if (res.car.id == 0) missingResources.push_back("car");

    for (int i = 0; i < res.carSpinFrames.size(); i++)
        if (res.carSpinFrames[i].id == 0)
            missingResources.push_back("carSpinFrames[" + to_string(i) + "]");

    if (res.leftCarNPC.id == 0) missingResources.push_back("leftCarNPC");
    if (res.rightCarNPC.id == 0) missingResources.push_back("rightCarNPC");
    if (res.bg.id == 0) missingResources.push_back("bg");
    if (res.one.id == 0) missingResources.push_back("one");
    if (res.two.id == 0) missingResources.push_back("two");
    if (res.three.id == 0) missingResources.push_back("three");
    if (res.go.id == 0) missingResources.push_back("go");
    if (res.wasd.id == 0) missingResources.push_back("wasd");
    if (res.lap.id == 0) missingResources.push_back("lap");
    if (res.goal.id == 0) missingResources.push_back("goal");
    if (!res.backgroundMusicLoaded) missingResources.push_back("backgroundMusic");
    if (!res.bgNoiseLoaded) missingResources.push_back("FX: bgNoise");
    if (!res.countdownSoundLoaded) missingResources.push_back("FX: countdown");
    if (!res.goSoundLoaded) missingResources.push_back("FX: ctdwnGo");
    if (!res.carCrashSoundLoaded) missingResources.push_back("FX: carCrash");
    if (!res.lapSoundLoaded) missingResources.push_back("FX: lapReached");
    if (!res.goalSoundLoaded) missingResources.push_back("FX: goalReached");


    if (!missingResources.empty()) {
        cout << "MOTORIST: Critical error - failed to load resources:" << endl;
        for (const string& resourceName : missingResources)
            cout << resourceName << endl;

        return false;
    }

    return true;
}