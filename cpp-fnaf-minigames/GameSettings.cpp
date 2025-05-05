#include "GameSettings.h" // Include the header file
#include "raylib.h"       // Needed for GetMonitorWidth/Height etc.

// --- Global Settings Instance Definition ---
// Define the actual global variable here.
GameSettingsData g_settings;

// --- Function Definitions ---

void UpdateScreenDimensionsFromSettings() {
    switch (g_settings.currentResolution) {
    case RES_640x360:
        g_settings.screenWidth = 640;
        g_settings.screenHeight = 360;
        break;
    case RES_854x480:
        g_settings.screenWidth = 854;
        g_settings.screenHeight = 480;
        break;
    case RES_1280x720:
        g_settings.screenWidth = 1280;
        g_settings.screenHeight = 720;
        break;
    case RES_1920x1080:
        g_settings.screenWidth = 1920;
        g_settings.screenHeight = 1080;
        break;
    default: // Default to a known resolution if something goes wrong
        g_settings.currentResolution = RES_1280x720;
        g_settings.screenWidth = 1280;
        g_settings.screenHeight = 720;
        break;
    }
}

void InitializeGameSettings() {
    // Set default values
    g_settings.currentResolution = RES_1280x720;
    g_settings.quality = QUALITY_HIGH;
    g_settings.masterVolume = 1.0f;
    g_settings.useCRTShader = true; // Default CRT for Motorist to ON

    // Set initial screen dimensions based on default resolution
    UpdateScreenDimensionsFromSettings();

    // Initialize other settings if added to the struct...
    // g_settings.menuBackgroundColor = RAYWHITE;
    // g_settings.settingsBackgroundColor = LIGHTGRAY;
}

// Optional helper implementation

//Vector2 GetResolutionDimensions(ScreenResolution res) {
//    switch (res) {
//        case RES_640x360: return (Vector2){ 640, 360 };
//        case RES_854x480: return (Vector2){ 854, 480 };
//        case RES_1280x720: return (Vector2){ 1280, 720 };
//        case RES_1920x1080: return (Vector2){ 1920, 1080 };
//        default: return (Vector2){ 1280, 720 };
//    }
//}