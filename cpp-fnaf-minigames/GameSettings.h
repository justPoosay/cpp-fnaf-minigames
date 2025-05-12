#pragma once

#include "raylib.h"

typedef enum ScreenResolution {
    //RES_640x360,
    //RES_854x480,
    RES_1280x720,
    RES_1920x1080
} ScreenResolution;


typedef enum GraphicsQuality {
    QUALITY_LOW,
    QUALITY_MEDIUM,
    QUALITY_HIGH
} GraphicsQuality;


typedef struct GameSettingsData {
    // Display
    ScreenResolution currentResolution;
    int screenWidth;
    int screenHeight;

    // Graphics
    GraphicsQuality quality;
    bool useCRTShader; 

    // Audio
    float masterVolume;

} GameSettingsData;


// --- Global Settings Instance ---
extern GameSettingsData g_settings;


// --- Constants ---
const int logicalWidth = 1280;
const int logicalHeight = 720;

const int settingsTextFontSize = 25;
const int settingsSelectorFontSize = 23;

const float selectorWidth = 125;
const float selectorHeight = 30;
const float selectorX = 250;

const float musicFadeSpeed = 2;


// --- Function Declarations ---
void InitializeGameSettings();
void UpdateScreenDimensionsFromSettings();


