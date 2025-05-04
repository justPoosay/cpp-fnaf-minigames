#pragma once

#include "raylib.h"
#include "GameSettings.h"
#include <vector>
#include <string>

struct MotoristGameResources { // Store game resources
	Texture2D car;
    std::vector<Texture2D> carSpinFrames; // Zamiast pojedynczej tekstury
	Texture2D leftCarNPC;
	Texture2D rightCarNPC;

	Texture2D bg;
	Texture2D one;
	Texture2D two;
	Texture2D three;
	Texture2D go;
	Texture2D lap;
	Texture2D goal;

	Music backgroundMusic;
	Sound countdown;
	Sound carCrash;
	Sound lapReached;
	Sound goalReached;

	bool backgroundMusicLoaded = false;
	bool countdownSoundLoaded = false;
	bool carCrashSoundLoaded = false;
	bool lapSoundLoaded = false;
	bool goalSoundLoaded = false;

};

// Loading Resources
MotoristGameResources LoadMotoristResources(GraphicsQuality quality);

// Unloadind Resources
void UnloadMotoristResources(MotoristGameResources& resources);