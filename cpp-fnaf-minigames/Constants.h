#pragma once

#ifndef CONSTANTS_H
#define CONSTANTS_H

// --- Wirtualna/Logiczna Rozdzielczoœæ Gry ---
const int VIRTUAL_SCREEN_WIDTH = 800;
const int VIRTUAL_SCREEN_HEIGHT = 450;
const float VIRTUAL_ASPECT_RATIO = (float)VIRTUAL_SCREEN_WIDTH / (float)VIRTUAL_SCREEN_HEIGHT;

// --- Œcie¿ki do Zasobów ---
const char* TILE_PATH = "resources/dirt_tile.png";
const char* BG_PATH = "resources/background.png";
const char* IDLE_RIGHT_PATH = "resources/chica_idle_right.png";
const char* IDLE_LEFT_PATH = "resources/chica_idle_left.png";
const char* WALK_RIGHT_PATH = "resources/chica_walk_right.png";
const char* WALK_LEFT_PATH = "resources/chica_walk_left.png";
const char* JUMP_RIGHT_PATH = "resources/chica_jump_right.png";
const char* JUMP_LEFT_PATH = "resources/chica_jump_left.png";
const char* BUTTON_VOICES_OFF_PATH = "resources/voices_off.png";
const char* BUTTON_VOICES_ON_PATH = "resources/voices_on.png";
const char* MUSIC_PATH = "resources/rainbow.mp3";
const char* JUMP_SOUND_PATH = "resources/jump.wav";

// --- Sta³e Gracza ---
const float PLAYER_TEXTURE_HEIGHT = 70.0f;
const float PLAYER_TEXTURE_WIDTH = 70.0f;
const float PLAYER_SPEED = 4.0f;
const float JUMP_SPEED = -5.0f;
const float FALL_SPEED = 5.0f;
const float MAX_JUMP_HEIGHT = 300.0f;
const int ANIM_UPDATE_RATE = 3;
const int IDLE_FRAMES = 16;
const int WALK_FRAMES = 16;
const int JUMP_FRAMES = 1;

// --- Sta³e Poziomu ---
// groundLevelY zale¿y od za³adowanej tekstury kafelka, wiêc obliczymy go w Level.cpp
const float BG_SCROLL_FACTOR = 0.5f;

// --- Sta³e UI ---
const float BUTTON_PADDING = 5.0f;

#endif // CONSTANTS_H