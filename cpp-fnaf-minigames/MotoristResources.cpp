#include "MotoristResources.h"

MotoristGameResources LoadMotoristResources(GraphicsQuality quality) { // Loading Resources Function
    TraceLog(LOG_INFO, "Loading game resources...");

    MotoristGameResources resources;

    // Car (player & NPC)
    resources.car = LoadTexture("resources/mm/car.png");
    resources.carSpinFrames.resize(19);
    for (int i = 0; i < 19; i++) {
        string filename = TextFormat("resources/mm/carSpin_%02d.png", i + 1);
        resources.carSpinFrames[i] = LoadTexture(filename.c_str());
    }

    resources.rightCarNPC = LoadTexture("resources/mm/leftCarNPC.png");
    resources.leftCarNPC = LoadTexture("resources/mm/rightCarNPC.png");

    // Background & UI
    resources.bg = LoadTexture("resources/mm/bg.png");
    resources.one = LoadTexture("resources/mm/1.png");
    resources.two = LoadTexture("resources/mm/2.png");
    resources.three = LoadTexture("resources/mm/3.png");
    resources.go = LoadTexture("resources/mm/go.png");
    resources.wasd = LoadTexture("resources/mm/wasd.png");
    resources.lap = LoadTexture("resources/mm/lap.png");
    resources.lapMark = LoadTexture("resources/mm/lapMark.png");
    resources.goal = LoadTexture("resources/mm/goal.png");
    resources.gameFont = LoadFont("resources/ARCADECLASSIC.TTF");


    // Music & FX
    resources.backgroundMusic = LoadMusicStream("resources/mm/motorist.mp3");
    resources.bgNoise = LoadMusicStream("resources/mm/bgNoise.wav");
    resources.countdown = LoadSound("resources/mm/countdown.wav");
    resources.ctdwnGo = LoadSound("resources/mm/go.wav");
    resources.carCrash = LoadSound("resources/mm/crash.wav");
    resources.lapReached = LoadSound("resources/mm/lap.wav");
    resources.goalReached = LoadSound("resources/mm/goal.wav");

    resources.backgroundMusicLoaded = (GetMusicTimeLength(resources.backgroundMusic) > 0);
    resources.bgNoiseLoaded = (GetMusicTimeLength(resources.bgNoise) > 0);
    resources.countdownSoundLoaded = (resources.countdown.frameCount > 0);
    resources.goSoundLoaded = (resources.ctdwnGo.frameCount > 0);
    resources.carCrashSoundLoaded = (resources.carCrash.frameCount > 0);
    resources.lapSoundLoaded = (resources.lapReached.frameCount > 0);
    resources.goalSoundLoaded = (resources.goalReached.frameCount > 0);

    SetTextureFilter(resources.gameFont.texture, TEXTURE_FILTER_BILINEAR);
    TraceLog(LOG_INFO, "Game resources loaded successfully.");

    return resources;
}


// Unloadind Resources
void UnloadMotoristResources(MotoristGameResources& resources) {
    TraceLog(LOG_INFO, "Unloading game resources...");

    if (resources.car.id > 0) UnloadTexture(resources.car);
    for (const auto& frame : resources.carSpinFrames) {
        UnloadTexture(frame);
    }
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