#pragma once

#include "raylib.h"
#include "raymath.h"
#include "GameSettings.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;

struct MotoristGameResources {
	Texture2D car;
    vector<Texture2D> carSpinFrames;
	Texture2D leftCarNPC;
	Texture2D rightCarNPC;

	Texture2D bg;
	Texture2D one;
	Texture2D two;
	Texture2D three;
	Texture2D go;
	Texture2D wasd;
	Texture2D lap;
	Texture2D lapMark;
	Texture2D goal;

	Music backgroundMusic;
	Music bgNoise;
	Sound countdown;
	Sound ctdwnGo;
	Sound carCrash;
	Sound lapReached;
	Sound goalReached;

	bool backgroundMusicLoaded = false;
	bool bgNoiseLoaded = false;
	bool countdownSoundLoaded = false;
	bool goSoundLoaded = false;
	bool carCrashSoundLoaded = false;
	bool lapSoundLoaded = false;
	bool goalSoundLoaded = false;

	Font gameFont;
};

MotoristGameResources LoadMotoristResources(GraphicsQuality quality);
void UnloadMotoristResources(MotoristGameResources& resources);
bool CheckMotoristResourcesLoaded(MotoristGameResources& res);