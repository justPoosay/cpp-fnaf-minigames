#include "MotoristResources.h"
#include "raylib.h"
#include "GameSettings.h"

#include <string>
#include <vector>


MotoristGameResources LoadMotoristResources(GraphicsQuality quality) { // Loading Resources Function
    TraceLog(LOG_INFO, "Loading game resources...");

    MotoristGameResources resources;

    // Car (player & NPC)
    resources.car = LoadTexture("resources/mm/car.png");
    resources.carSpinFrames.resize(19);
    for (int i = 0; i < 19; i++) {
        std::string filename = TextFormat("resources/mm/carSpin_%02d.png", i + 1);
        resources.carSpinFrames[i] = LoadTexture(filename.c_str());
        if (resources.carSpinFrames[i].id == 0) {
            TraceLog(LOG_ERROR, "Blad ladowania tekstury animacji: %s", filename.c_str());
        }
    }

    resources.rightCarNPC = LoadTexture("resources/mm/leftCarNPC.png");
    resources.leftCarNPC = LoadTexture("resources/mm/rightCarNPC.png");

    // Background & UI
    resources.bg = LoadTexture("resources/mm/bg.png");
    resources.one = LoadTexture("resources/mm/1.png");
    resources.two = LoadTexture("resources/mm/2.png");
    resources.three = LoadTexture("resources/mm/3.png");
    resources.go = LoadTexture("resources/mm/go.png");
    resources.lap = LoadTexture("resources/mm/lap.png");
    resources.goal = LoadTexture("resources/mm/goal.png");


    // Music & FX
    const char* musicPath = "resources/mm/motorist.mp3";
    const char* noisePath = "resources/mm/bgNoise.wav";
    const char* countdownPath = "resources/mm/countdown.wav";
    const char* goPath = "resources/mm/go.wav";
    const char* carCrashPath = "resources/mm/crash.wav";
    const char* lapPath = "resources/mm/lap.wav";
    const char* goalPath = "resources/mm/goal.wav";

    resources.backgroundMusic = LoadMusicStream(musicPath);
    resources.bgNoise = LoadMusicStream(noisePath);
    resources.countdown = LoadSound(countdownPath);
    resources.ctdwnGo = LoadSound(goPath);
    resources.carCrash = LoadSound(carCrashPath);
    resources.lapReached = LoadSound(lapPath);
    resources.goalReached = LoadSound(goalPath);

    resources.backgroundMusicLoaded = (GetMusicTimeLength(resources.backgroundMusic) > 0);
    resources.bgNoiseLoaded = (GetMusicTimeLength(resources.bgNoise) > 0);
    resources.countdownSoundLoaded = (resources.countdown.frameCount > 0);
    resources.goSoundLoaded = (resources.ctdwnGo.frameCount > 0);
    resources.carCrashSoundLoaded = (resources.carCrash.frameCount > 0);
    resources.lapSoundLoaded = (resources.lapReached.frameCount > 0);
    resources.goalSoundLoaded = (resources.goalReached.frameCount > 0);

    resources.gameFont = LoadFont("resources/BRLNSDB.TTF");

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
    if (resources.lap.id > 0) UnloadTexture(resources.lap);
    if (resources.goal.id > 0) UnloadTexture(resources.goal);

    if (resources.backgroundMusicLoaded) UnloadMusicStream(resources.backgroundMusic);
    if (resources.bgNoiseLoaded) UnloadMusicStream(resources.bgNoise);
    if (resources.countdownSoundLoaded) UnloadSound(resources.countdown);
    if (resources.carCrashSoundLoaded) UnloadSound(resources.goalReached);
    if (resources.lapSoundLoaded) UnloadSound(resources.lapReached);

    UnloadFont(resources.gameFont);
}