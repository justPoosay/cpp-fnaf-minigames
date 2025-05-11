#include "GameSettings.h"

GameSettingsData g_settings;

void UpdateScreenDimensionsFromSettings() {
    switch (g_settings.currentResolution) {
    //case RES_640x360:
    //    g_settings.screenWidth = 640;
    //    g_settings.screenHeight = 360;
    //    break;
    //case RES_854x480:
    //    g_settings.screenWidth = 854;
    //    g_settings.screenHeight = 480;
    //    break;
    case RES_1280x720:
        g_settings.screenWidth = 1280;
        g_settings.screenHeight = 720;
        break;
    case RES_1920x1080:
        g_settings.screenWidth = 1920;
        g_settings.screenHeight = 1080;
        break;
    default:
        g_settings.currentResolution = RES_1280x720;
        g_settings.screenWidth = 1280;
        g_settings.screenHeight = 720;
        break;
    }
}

void InitializeGameSettings() {
    g_settings.currentResolution = RES_1280x720;
    g_settings.quality = QUALITY_HIGH;
    g_settings.masterVolume = 1;
    g_settings.useCRTShader = true;

    UpdateScreenDimensionsFromSettings();
}