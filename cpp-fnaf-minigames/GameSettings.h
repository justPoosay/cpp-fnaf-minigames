#pragma once

#include "raylib.h" // Include raylib here for Color, maybe Vector2 later?

// --- Enums for Settings ---
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

// --- Struct to Hold All Game Settings ---
typedef struct GameSettingsData {
    // Display
    ScreenResolution currentResolution;
    int screenWidth;
    int screenHeight;

    // Graphics
    GraphicsQuality quality;
    bool useCRTShader; // Toggle specifically for Motorist

    // Audio
    float masterVolume;

    // Add other settings here as needed...
    // Color menuBackgroundColor;
    // Color settingsBackgroundColor;

} GameSettingsData;

// --- Global Settings Instance Declaration ---
// This makes the settings accessible globally after initialization.
// The actual definition will be in GameSettings.cpp
extern GameSettingsData g_settings;



const int logicalWidth = 1280;
const int logicalHeight = 720;


const int settingsTextFontSize = 25;
const int settingsSelectorFontSize = 23;

const float selectorWidth = 125;
const float selectorHeight = 30;
const float selectorX = 250;



// --- Function Declarations ---

// Call this once at the very beginning of main()
void InitializeGameSettings();

// Call this whenever currentResolution changes to update screenWidth/Height
void UpdateScreenDimensionsFromSettings();


