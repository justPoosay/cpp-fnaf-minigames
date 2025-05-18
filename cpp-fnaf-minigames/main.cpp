#include "raylib.h"

#include <sstream>
#include <algorithm>
#include <map>

#include "GameSettings.h"

#include "MainMenuResources.h"
#include "MotoristResources.h"
#include "RainbowLandResources.h"

using namespace std;

// --- Game states ---
typedef enum GameScreen {
    LOADING,
    MAIN_MENU,
    SETTINGS,
    PLAYING_MOTORIST,
    PLAYING_RAINBOW,
    EXITING
} GameScreen;

// Definicja argumentu
struct ArgumentDetail {
    string name; // np. "/?", "/T"
    string description; // Co robi ten argument
};

// Definicja komendy
struct CommandDefinition {
    string name;                     // Nazwa komendy, np. "TASKKILL"
    string shortDescription;         // Krótki opis dla ogólnego HELP
    string helpMessage;              // Pe³ny tekst pomocy (wyœwietlany dla komenda /?)
    // Mo¿e zawieraæ "Usage: ..."
    map<string, ArgumentDetail> acceptedArguments; // Mapa argumentów, klucz to nazwa argumentu
    // Funkcja (wskaŸnik lub function) do wykonania komendy - to bardziej zaawansowane,
    // na razie zostawimy logikê w if/else if, ale to jest kierunek na przysz³oœæ.
};

// --- Console Variables ---
bool consoleActive = false;
char consoleInputBuffer[consoleMaxChars + 1] = { 0 };
int consoleInputCursorPos = 0;
int consoleScrollOffset = 0;
Rectangle consoleRect = {(logicalWidth - (logicalWidth * 0.9)) / 2, 20, logicalWidth * 0.9, logicalHeight * 0.4};
vector<string> consoleOutputLines;
vector<string> runningProcesses = { "helpy.ini", "fazbOS.exe" };
vector<CommandDefinition> definedCommands = {
    { 
        "HELP",
        "Displays help information for console commands.",
        "Provides help information for console commands.\n\nHELP [command]\n\n  command - displays help information on that command.",
        {{"/?", {"/?", "Displays this help message."}}}
    },
    {
        "CLEAR",
        "Clears the console output.",
        "Clears all text from the console window.\n\nCLEAR [/?]",
        {{"/?", {"/?", "Displays this help message."}}}
    },
    //{
    //    "HISTORY",
    //    "Displays command history (not yet implemented).",
    //    "Displays a list of previously entered commands.\n\nHISTORY [/?]\n\n(Feature not fully implemented)",
    //    {{"/?", {"/?", "Displays this help message."}}}
    //},
    {
        "TASKLIST",
        "Displays a list of simulated running processes.",
        "Displays a list of all currently running simulated processes.\n\nTASKLIST [/?]",
        {{"/?", {"/?", "Displays this help message."}}}
    },
    {
        "TASKKILL",
        "Terminates a simulated process.",
        "Terminates a specified process.\n\nTASKKILL <process_name> [/T] [/F] [/?]\n\n"
        "  <process_name>  Specifies the process to be terminated.\n"
        "  /T              Terminates the specified process and any child processes (simulated).\n"
        "  /F              Specifies to forcefully terminate the process(es).",
        {
            {"/?", {"/?", "Displays this help message."}},
            {"/T", {"/T", "Terminates the specified process gracefully (simulated)."}},
            {"/F", {"/F", "Forcefully terminates the specified process."}}
        }
    }
};


// --- Helpy Animation Variables ---
bool helpyKilled = false;

int helpyCurrentAnimFrame = 0;
float helpyFrameDelay = 0.73;
float helpyFrameTimer = 0;

int helpyCrackedCurrentPlayingFrame = 0;
float helpyCrackedAnimInternalTimer = 0;
float helpyCrackedAnimFrameDelay = 0.25;

float musicRestartKillTimer = 0;

// --- UI Data  ---
const char* resolutionOptions[] = { "1280x720", "1920x1080" };
const ScreenResolution resolutionValues[] = { RES_1280x720, RES_1920x1080 };
const int numResolutionOptions = sizeof(resolutionOptions) / sizeof(resolutionOptions[0]);
int currentResolutionIndex = 0;

const char* qualityOptions[] = { "Low", "Medium", "High" };
const GraphicsQuality qualityValues[] = { QUALITY_LOW, QUALITY_MEDIUM, QUALITY_HIGH };
const int numQualityOptions = sizeof(qualityOptions) / sizeof(qualityOptions[0]);
int currentQualityIndex = 1;


// --- Music and Fading ---
float currentMenuMusicVolume = 1;
float currentSettingsMusicVolume = 1;


// --- Forward declarations ---
int runMidnightMotorist(GraphicsQuality quality, Shader postProcessingShader, bool applyShader);
int runMagicRainbowLand(GraphicsQuality quality);


// ------- Functions -------
void StopAndUnloadMusic(Music& music, bool& loadedFlag) {
    if (loadedFlag && music.stream.buffer) {
        if (IsMusicStreamPlaying(music))
            StopMusicStream(music);

        UnloadMusicStream(music);
        music = { 0 };
        loadedFlag = false;
    }
    else {
        music = { 0 };
        loadedFlag = false;
    }
}

bool GuiButton(Rectangle bounds, const char* text, Font font, Color bgColor, Color hoverColor, Color pressedColor, Vector2 scaledMousePos) {
    bool clicked = false;
    Color currentBgColor = bgColor;

    Vector2 textSize = MeasureTextEx(font, text, settingsSelectorFontSize, 1);

    if (!consoleActive && CheckCollisionPointRec(scaledMousePos, bounds)) { 
        currentBgColor = hoverColor;
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            currentBgColor = pressedColor;

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            clicked = true;
    }

    DrawRectangleRec(bounds, currentBgColor);
    DrawTextEx(font, text, { bounds.x + (bounds.width - textSize.x) / 2, bounds.y + (bounds.height - textSize.y) / 2 }, settingsSelectorFontSize, 1, BLACK);
    
    return clicked;
}

int GuiSelector(Rectangle bounds, const char* currentOptionText, Font font, Color textColor, Color boxColor, Color arrowColor, Color arrowHoverColor, Color arrowPressedColor, Vector2 scaledMousePos) {
    int clicked_direction = 0;
    float arrowPaddingFromBox = 5;
    float arrowHotspotExtraPadding = 8;
    float fontSize = settingsSelectorFontSize;

    DrawRectangleRec(bounds, boxColor);
    DrawRectangleLinesEx(bounds, 1, Fade(BLACK, 0.3));

    Vector2 optionTextSize = MeasureTextEx(font, currentOptionText, settingsSelectorFontSize, 1);
    DrawTextEx(font, currentOptionText,
        { bounds.x + (bounds.width - optionTextSize.x) / 2,
          bounds.y + (bounds.height - optionTextSize.y) / 2 },
        settingsSelectorFontSize, 1, textColor);

    float arrowFontSize = settingsSelectorFontSize * 2;

    // Left Arrow
    const char* leftArrowChar = "<";
    Vector2 leftArrowTextSize = MeasureTextEx(font, leftArrowChar, arrowFontSize, 1);
    Rectangle leftArrowHotspot = {
        bounds.x - leftArrowTextSize.x - arrowPaddingFromBox - arrowHotspotExtraPadding,
        bounds.y + (bounds.height - leftArrowTextSize.y) / 2 - arrowHotspotExtraPadding / 2,
        leftArrowTextSize.x + arrowHotspotExtraPadding * 2,
        leftArrowTextSize.y + arrowHotspotExtraPadding
    };
    Color currentLeftArrowColor = arrowColor;
    if (!consoleActive && CheckCollisionPointRec(scaledMousePos, leftArrowHotspot)) {
        currentLeftArrowColor = arrowHoverColor;
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) currentLeftArrowColor = arrowPressedColor;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) clicked_direction = -1;
    }
    DrawTextEx(font, leftArrowChar, { leftArrowHotspot.x + arrowHotspotExtraPadding, leftArrowHotspot.y + arrowHotspotExtraPadding / 2 }, arrowFontSize, 1, currentLeftArrowColor);

    // Right Arrow
    const char* rightArrowChar = ">";
    Vector2 rightArrowTextSize = MeasureTextEx(font, rightArrowChar, arrowFontSize, 1);
    Rectangle rightArrowHotspot = {
       bounds.x + bounds.width + arrowPaddingFromBox - arrowHotspotExtraPadding,
       bounds.y + (bounds.height - rightArrowTextSize.y) / 2 - arrowHotspotExtraPadding / 2,
       rightArrowTextSize.x + arrowHotspotExtraPadding * 2,
       rightArrowTextSize.y + arrowHotspotExtraPadding
    };
    Color currentRightArrowColor = arrowColor;
    if (!consoleActive && CheckCollisionPointRec(scaledMousePos, rightArrowHotspot)) {
        currentRightArrowColor = arrowHoverColor;
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) currentRightArrowColor = arrowPressedColor;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && clicked_direction == 0) clicked_direction = 1;
    }
    DrawTextEx(font, rightArrowChar, { rightArrowHotspot.x + arrowHotspotExtraPadding, rightArrowHotspot.y + arrowHotspotExtraPadding / 2 }, arrowFontSize, 1, currentRightArrowColor);

    return clicked_direction;
}

void UpdateMainMenuScreen(MainMenuResources& res, Vector2 scaledMousePos, GameScreen& currentScreen, GameScreen& previousScreen, GameScreen& fadeTargetScreen, bool& isFadingOut, float& fadeAlpha, float dt,
    int& mainMenuBgCurrentFrame, float& mainMenuBgFrameTimer, float mainMenuBgFrameDelay) {
    if (res.menuMusicLoaded && !IsMusicStreamPlaying(res.menuMusic)) {
        PlayMusicStream(res.menuMusic);
        SetMusicVolume(res.menuMusic, currentMenuMusicVolume * g_settings.masterVolume);
    }
    if (res.menuMusicLoaded && IsMusicStreamPlaying(res.menuMusic)) {
        UpdateMusicStream(res.menuMusic);
        if (GetMusicTimePlayed(res.menuMusic) >= GetMusicTimeLength(res.menuMusic))
            SeekMusicStream(res.menuMusic, 0);
    }

    if (res.gifLoaded && res.bgTexture.id > 0 && res.bgGifImage.data) {
        mainMenuBgFrameTimer += dt;
        bool bgFrameUpdated = false;
        while (mainMenuBgFrameTimer >= mainMenuBgFrameDelay) {
            mainMenuBgFrameTimer -= mainMenuBgFrameDelay;
            mainMenuBgCurrentFrame = (mainMenuBgCurrentFrame + 1) % res.animFrames;
            bgFrameUpdated = true;
        }
        if (bgFrameUpdated) {
            int frameDataSize = GetPixelDataSize(res.bgGifImage.width, res.bgGifImage.height, res.bgGifImage.format);
            unsigned char* frameDataOffset = (unsigned char*)res.bgGifImage.data + (mainMenuBgCurrentFrame * frameDataSize);
            UpdateTexture(res.bgTexture, frameDataOffset);
        }
    }

    if (IsWindowResized() && !IsWindowFullscreen())
        SetWindowPosition((GetMonitorWidth(GetCurrentMonitor()) - GetScreenWidth()) / 2, (GetMonitorHeight(GetCurrentMonitor()) - GetScreenHeight()) / 2);

}

void DrawMainMenuScreen(MainMenuResources& res, Vector2 scaledMousePos, int mainMenuBgCurrentFrame,
    GameScreen& currentScreen, GameScreen& previousScreen, GameScreen& fadeTargetScreen, bool& isFadingOut, float& fadeAlpha) {
    if (res.gifLoaded && res.bgTexture.id > 0) 
        DrawTexturePro(res.bgTexture, { 0, 0, (float)res.bgTexture.width, (float)res.bgTexture.height }, { 0, 0, (float)logicalWidth, (float)logicalHeight }, { 0, 0 }, 0, WHITE);
    else ClearBackground(DARKBLUE);
    
    Vector2 titleSize = MeasureTextEx(res.bytesFont, "FNaF Minigames Port", 75, 1);
    DrawRectangleRec({ (logicalWidth - 400) / 2, 160, 400, 320 }, Fade(BLACK, 0.7));
    DrawTextEx(res.bytesFont, "FNaF Minigames Port", { logicalWidth / 2 - titleSize.x / 2, 40 }, 75, 1, WHITE);

    float buttonX = logicalWidth / 2 - 150;
    float buttonYStart = 190;
    float buttonSpacing = 70;
    float buttonWidth = 300;
    float buttonHeight = 50;

    if (GuiButton({ buttonX, buttonYStart, buttonWidth, buttonHeight }, "Midnight Motorist", res.arcadeClassicFont, WHITE, BLUE, PURPLE, scaledMousePos)) {
        previousScreen = currentScreen;
        fadeTargetScreen = PLAYING_MOTORIST;
        PlaySound(res.buttonSelect);
        isFadingOut = true;
        fadeAlpha = 0;
    }
    if (GuiButton({ buttonX, buttonYStart + buttonSpacing, buttonWidth, buttonHeight }, "Chica's Magic Rainbow Land", res.arcadeClassicFont, WHITE, YELLOW, RED, scaledMousePos)) {
        previousScreen = currentScreen;
        fadeTargetScreen = PLAYING_RAINBOW;
        PlaySound(res.buttonSelect);
        isFadingOut = true;
        fadeAlpha = 0;
    }
    if (GuiButton({ buttonX, buttonYStart + buttonSpacing * 2, buttonWidth, buttonHeight }, "Settings", res.arcadeClassicFont, WHITE, LIGHTGRAY, GRAY, scaledMousePos)) {
        previousScreen = MAIN_MENU;
        fadeTargetScreen = SETTINGS;
        PlaySound(res.buttonSelect);
        isFadingOut = true;
        fadeAlpha = 0;
    }
    if (GuiButton({ buttonX, buttonYStart + buttonSpacing * 3, buttonWidth, buttonHeight }, "Exit", res.arcadeClassicFont, WHITE, LIGHTGRAY, GRAY, scaledMousePos)) {
        previousScreen = currentScreen;
        currentScreen = EXITING;
    }
}


void UpdateSettingsScreen(MainMenuResources& res, Vector2 scaledMousePos, GameScreen& currentScreen, GameScreen& previousScreen, GameScreen& fadeTargetScreen, bool& isFadingOut, float& fadeAlpha, float dt,
    int& currentNormalHelpyFrame_param, float& normalHelpyTimer_param, float normalHelpyFrameDelay_param) { // Parameters for normal Helpy

    // --- Settings Music Logic ---
    if (helpyKilled) {
        musicRestartKillTimer += dt; // Increment dedicated music timer

        if (res.settingsMusicLoaded && !IsMusicStreamPlaying(res.settingsMusic)) {
            if (musicRestartKillTimer >= 3) {
                SeekMusicStream(res.settingsMusic, 0);
                PlayMusicStream(res.settingsMusic);
                SetMusicVolume(res.settingsMusic, currentSettingsMusicVolume * g_settings.masterVolume);
                SetMusicPitch(res.settingsMusic, 0.4); // Killed pitch
            }
        }
        else if (res.settingsMusicLoaded && IsMusicStreamPlaying(res.settingsMusic)) {
            UpdateMusicStream(res.settingsMusic);
            
            if (GetMusicTimePlayed(res.settingsMusic) >= GetMusicTimeLength(res.settingsMusic))
                SeekMusicStream(res.settingsMusic, 0); // Loop
        }
    }
    else {
        // Normal music behavior
        if (res.settingsMusicLoaded && !IsMusicStreamPlaying(res.settingsMusic) && currentSettingsMusicVolume > 0.01f) {
            PlayMusicStream(res.settingsMusic);
            SetMusicVolume(res.settingsMusic, currentSettingsMusicVolume * g_settings.masterVolume);
            SetMusicPitch(res.settingsMusic, 1.0f); // Normal pitch
        }
        if (res.settingsMusicLoaded && IsMusicStreamPlaying(res.settingsMusic)) {
            UpdateMusicStream(res.settingsMusic);
            SetMusicPitch(res.settingsMusic, 1.0f);

            if (GetMusicTimePlayed(res.settingsMusic) >= GetMusicTimeLength(res.settingsMusic)) {
                SeekMusicStream(res.settingsMusic, 0);
            }
        }
    }

    // --- Helpy Animation Logic ---
    if (helpyKilled) {
        // Helpy IS KILLED - Animate cracked Helpy
        if (res.helpyCrackedGifLoaded && res.helpyCrackedGifImage.data && (res.helpyCrackedAnimFrames == 0 || helpyCrackedCurrentPlayingFrame < res.helpyCrackedAnimFrames - 1)) {
            helpyCrackedAnimInternalTimer += dt; // Use the dedicated timer for THIS animation
            bool frameUpdated = false;

            while (helpyCrackedAnimInternalTimer >= helpyCrackedAnimFrameDelay) {
                helpyCrackedAnimInternalTimer -= helpyCrackedAnimFrameDelay; // Consume time

                if (helpyCrackedCurrentPlayingFrame < res.helpyCrackedAnimFrames - 1) {
                    helpyCrackedCurrentPlayingFrame++;
                    frameUpdated = true;
                }
                else {
                    if (res.helpyCrackedAnimFrames > 0) 
                        helpyCrackedCurrentPlayingFrame = res.helpyCrackedAnimFrames - 1;
                    else 
                        helpyCrackedCurrentPlayingFrame = 0;
                    
                    frameUpdated = true; // Ensure last frame is updated to texture
                    break; // Exit while loop, animation is done
                }
            }

            if (frameUpdated) {
                int frameToUpdate = helpyCrackedCurrentPlayingFrame;

                if (frameToUpdate >= res.helpyCrackedAnimFrames)
                    frameToUpdate = res.helpyCrackedAnimFrames - 1;

                if (frameToUpdate < 0) 
                    frameToUpdate = 0;

                if (res.helpyCrackedAnimFrames > 0) { // Proceed only if there are frames
                    int frameDataSize = GetPixelDataSize(res.helpyCrackedGifImage.width, res.helpyCrackedGifImage.height, res.helpyCrackedGifImage.format);
                    unsigned char* frameDataOffset = (unsigned char*)res.helpyCrackedGifImage.data + (frameToUpdate * frameDataSize);
                    UpdateTexture(res.helpyCrackedTexture, frameDataOffset);
                }
            }
        }
    }
    else {
        // Helpy is NOT KILLED - Animate normal Helpy
        if (res.helpyGifLoaded && res.helpyTexture.id > 0 && res.helpyGifImage.data) {
            normalHelpyTimer_param += dt; // Use the parameters for normal Helpy's animation
            bool normalFrameUpdated = false;
            while (normalHelpyTimer_param >= normalHelpyFrameDelay_param) { // Use parameters
                normalHelpyTimer_param -= normalHelpyFrameDelay_param;
                currentNormalHelpyFrame_param = (currentNormalHelpyFrame_param + 1) % res.helpyAnimFrames; // Use res.helpyAnimFrames
                normalFrameUpdated = true;
            }
            if (normalFrameUpdated) {
                int frameDataSize = GetPixelDataSize(res.helpyGifImage.width, res.helpyGifImage.height, res.helpyGifImage.format);
                unsigned char* frameDataOffset = (unsigned char*)res.helpyGifImage.data + (currentNormalHelpyFrame_param * frameDataSize);
                UpdateTexture(res.helpyTexture, frameDataOffset);
            }
        }
    }

// --- Console Toggle ---
    if (IsKeyPressed(KEY_GRAVE)) {
        consoleActive = !consoleActive;

        if (consoleActive) {
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
            memset(consoleInputBuffer, 0, 256);
            consoleInputCursorPos = 0;
        }
        else SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }
}

void DrawSettingsScreen(MainMenuResources& res, Vector2 scaledMousePos, int helpyCurrentAnimFrame, GameScreen& currentScreen, GameScreen& previousScreen, GameScreen& fadeTargetScreen, bool& isFadingOut, float& fadeAlpha) {
    if (res.settingsBgTexture.id > 0) 
        DrawTexturePro(res.settingsBgTexture, { 0, 0, (float)res.settingsBgTexture.width, (float)res.settingsBgTexture.height }, { 0, 0, logicalWidth, logicalHeight }, { 0, 0 }, 0, WHITE);
    else ClearBackground(DARKGRAY);
    

    const char* settingsTitleText = "SETTINGS";
    Vector2 settingsTitleSize = MeasureTextEx(res.bytesFont, settingsTitleText, 65, 1);
    DrawTextEx(res.bytesFont, settingsTitleText, { logicalWidth / 2 - settingsTitleSize.x / 2, 80 }, 65, 1, WHITE);

    Font uiFont = res.defaultGuiFont;

    //DrawTextEx(uiFont, "Volume:", { 100, 180 }, settingsTextFontSize, 1, WHITE);
    //Rectangle volumeSliderBaseRect = { 250, 175, 300, 30 };
    //DrawRectangleRec(volumeSliderBaseRect, WHITE);
    //if (!consoleActive && CheckCollisionPointRec(scaledMousePos, volumeSliderBaseRect) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    //    g_settings.masterVolume = Clamp((scaledMousePos.x - volumeSliderBaseRect.x) / volumeSliderBaseRect.width, 0, 1);
    //    SetMasterVolume(g_settings.masterVolume);
    //    if (res.settingsMusicLoaded && IsMusicStreamPlaying(res.settingsMusic)) {
    //        SetMusicVolume(res.settingsMusic, currentSettingsMusicVolume * g_settings.masterVolume);
    //    }
    //}
    //DrawRectangle(volumeSliderBaseRect.x, volumeSliderBaseRect.y, volumeSliderBaseRect.width * g_settings.masterVolume, volumeSliderBaseRect.height, VIOLET);
    //DrawTextEx(uiFont, TextFormat("%.0f %", g_settings.masterVolume * 100), { volumeSliderBaseRect.x + volumeSliderBaseRect.width + 15, 180 }, settingsTextFontSize, 1, WHITE);


    // Resolution Selector
    DrawTextEx(uiFont, "Resolution:", { 100, 240 }, settingsTextFontSize, 1, WHITE);
    Rectangle resolutionSelectorBounds = { selectorX, 235, selectorWidth, selectorHeight }; 
    int resolutionChange = GuiSelector(resolutionSelectorBounds, resolutionOptions[currentResolutionIndex], uiFont, BLACK, WHITE, DARKGRAY, GRAY, BLACK, scaledMousePos);
    if (resolutionChange != 0) {
        PlaySound(res.buttonClick);
        currentResolutionIndex = (currentResolutionIndex + resolutionChange + numResolutionOptions) % numResolutionOptions;
        g_settings.currentResolution = resolutionValues[currentResolutionIndex];
        UpdateScreenDimensionsFromSettings();
        SetWindowSize(g_settings.screenWidth, g_settings.screenHeight);
        if (!IsWindowFullscreen())
            SetWindowPosition((GetMonitorWidth(GetCurrentMonitor()) - g_settings.screenWidth) / 2, (GetMonitorHeight(GetCurrentMonitor()) - g_settings.screenHeight) / 2);
    }

    // Quality Selector
    DrawTextEx(uiFont, "Quality:", { 100, 300 }, settingsTextFontSize, 1, WHITE);
    Rectangle qualitySelectorBounds = { selectorX, 295, selectorWidth, selectorHeight };
    int qualityChange = GuiSelector(qualitySelectorBounds, qualityOptions[currentQualityIndex], uiFont, BLACK, WHITE, DARKGRAY, GRAY, BLACK, scaledMousePos);
    if (qualityChange != 0) {
        PlaySound(res.buttonClick);
        currentQualityIndex = (currentQualityIndex + qualityChange + numQualityOptions) % numQualityOptions;
        g_settings.quality = qualityValues[currentQualityIndex];
    }

    // Shaders Toggle
    DrawTextEx(uiFont, "Shaders:", { 100, 360 }, settingsTextFontSize, 1, WHITE);
    Rectangle crtToggleButtonBounds = { selectorX, 355, 100, 30 };
    if (GuiButton(crtToggleButtonBounds, g_settings.useCRTShader ? "ON" : "OFF", uiFont, WHITE, LIGHTGRAY, GRAY, scaledMousePos)) {
        if (res.shaderLoadedSuccessfully) {
            g_settings.useCRTShader = !g_settings.useCRTShader;
            if (g_settings.useCRTShader) PlaySound(res.buttonClick);
            else PlaySound(res.buttonPoof);
        }
        else PlaySound(res.buttonPoof);
    }
    if (!res.shaderLoadedSuccessfully)
        DrawTextEx(uiFont, "(Shader N/A)", { crtToggleButtonBounds.x + crtToggleButtonBounds.width + 10, 360 }, 18, 1, LIGHTGRAY);

    // Back Button
    if (GuiButton({ logicalWidth / 2 - 100, logicalHeight - 100, 200, 50 }, "Back", uiFont, WHITE, LIGHTGRAY, GRAY, scaledMousePos)) {
        PlaySound(res.buttonSelect);
        previousScreen = SETTINGS;
        fadeTargetScreen = MAIN_MENU;
        isFadingOut = true;
        fadeAlpha = 0;
        if (consoleActive) {
            consoleActive = false;
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }
    }

    // --- Helpy Drawing Logic ---
    Texture2D* textureToDrawForHelpy = nullptr;
    Image* imageForHelpyDimensions = nullptr;

    if (helpyKilled) {
        // Helpy is killed, so select the cracked Helpy texture and its original image dimensions
        if (res.helpyCrackedGifLoaded && res.helpyCrackedTexture.id > 0) {
            textureToDrawForHelpy = &res.helpyCrackedTexture;
            imageForHelpyDimensions = &res.helpyCrackedGifImage;
        }
    }
    else {
        // Helpy is NOT killed, select the normal dancing Helpy texture and its dimensions
        if (res.helpyGifLoaded && res.helpyTexture.id > 0) {
            textureToDrawForHelpy = &res.helpyTexture;
            imageForHelpyDimensions = &res.helpyGifImage;
        }
    }

    // Common drawing code if a Helpy texture has been selected
    if (textureToDrawForHelpy != nullptr && textureToDrawForHelpy->id > 0 &&
        imageForHelpyDimensions != nullptr && imageForHelpyDimensions->data != nullptr) {

        float helpyDrawWidthOnScreen = 200; // Your desired draw size
        float helpyDrawHeightOnScreen = 200;
        float finalDrawWidth, finalDrawHeight;

        float helpyOriginalAspectRatio = (float)imageForHelpyDimensions->width / imageForHelpyDimensions->height;

        if (helpyOriginalAspectRatio > 0) {
            if (imageForHelpyDimensions->width > imageForHelpyDimensions->height) {
                finalDrawWidth = helpyDrawWidthOnScreen;
                finalDrawHeight = helpyDrawWidthOnScreen / helpyOriginalAspectRatio;
            }
            else {
                finalDrawHeight = helpyDrawHeightOnScreen;
                finalDrawWidth = helpyDrawHeightOnScreen * helpyOriginalAspectRatio;
            }
            if (finalDrawHeight > helpyDrawHeightOnScreen) {
                finalDrawHeight = helpyDrawHeightOnScreen;
                finalDrawWidth = finalDrawHeight * helpyOriginalAspectRatio;
            }
            if (finalDrawWidth > helpyDrawWidthOnScreen) {
                finalDrawWidth = helpyDrawWidthOnScreen;
                finalDrawHeight = finalDrawWidth / helpyOriginalAspectRatio;
            }
        }
        else {
            finalDrawWidth = helpyDrawWidthOnScreen; // Fallback
            finalDrawHeight = helpyDrawHeightOnScreen;
        }

        Rectangle helpyDestinationRect = {
            logicalWidth - finalDrawWidth + 10,
            logicalHeight - finalDrawHeight - 15,
            finalDrawWidth,
            finalDrawHeight
        };
        Rectangle helpySourceRect = { 0, 0, (float)textureToDrawForHelpy->width, (float)textureToDrawForHelpy->height };
        DrawTexturePro(*textureToDrawForHelpy, helpySourceRect, helpyDestinationRect, { 0,0 }, 0, WHITE);
    }
}

void UpdateDrawConsole(MainMenuResources& res, Font font, Vector2 scaledMousePos, float dt) {
    if (!consoleActive) return;
    
    int keyPressedUnicode = GetCharPressed();
    while (keyPressedUnicode > 0) {
        char actualChar = (char)keyPressedUnicode;

        if (actualChar == '`' || actualChar == '~') {
            keyPressedUnicode = GetCharPressed();
            continue;
        }

        if ((actualChar >= 32) && (actualChar <= 126) && (strlen(consoleInputBuffer) < consoleMaxChars)) {
            int currentTextLength = strlen(consoleInputBuffer);

            for (int i = currentTextLength; i >= consoleInputCursorPos; i--)
                if (i + 1 <= consoleMaxChars)
                    consoleInputBuffer[i + 1] = consoleInputBuffer[i];

            consoleInputBuffer[consoleInputCursorPos] = actualChar;
            consoleInputCursorPos++;            
        }

        keyPressedUnicode = GetCharPressed();
    }

    if (consoleInputCursorPos > 0)
        if ((IsKeyPressed(KEY_BACKSPACE)) || IsKeyPressed(KEY_LEFT))
            consoleInputCursorPos--;
    

    if (consoleInputCursorPos < strlen(consoleInputBuffer)) {
        if (IsKeyPressed(KEY_RIGHT))
            consoleInputCursorPos++;

        if (IsKeyPressed(KEY_BACKSPACE))
            for (int i = consoleInputCursorPos; i < strlen(consoleInputBuffer); i++)
                consoleInputBuffer[i] = consoleInputBuffer[i + 1];
    }
    
    

    if (IsKeyPressed(KEY_ENTER)) {
        if (strlen(consoleInputBuffer) > 0) {
            string input = consoleInputBuffer;
            consoleOutputLines.push_back("> " + input);

            vector<string> tokens;
            string currentToken = "";
            for (char ch : input) {
                if (ch == ' ') { // Spacja jest separatorem
                    if (!currentToken.empty()) {
                        tokens.push_back(currentToken);
                        currentToken = ""; // Resetuj token
                    }
                }
                else currentToken += ch; // Dodaj znak do bie¿¹cego tokenu
                
            }

            if (!currentToken.empty()) tokens.push_back(currentToken);
            bool commandExecutedSuccessfully = false;

            if (!tokens.empty()) {
                string commandNameInput = tokens[0];
                // Konwersja na wielkie litery dla spójnego porównywania z definicjami komend
                string commandNameNormalized = commandNameInput;
                transform(commandNameNormalized.begin(), commandNameNormalized.end(), commandNameNormalized.begin(), ::toupper);

                const CommandDefinition* foundCmdDef = nullptr;
                for (const auto& cmdDef : definedCommands) {
                    if (cmdDef.name == commandNameNormalized) {
                        foundCmdDef = &cmdDef;
                        break;
                    }
                }

                if (foundCmdDef) {
                    // SprawdŸ, czy jednym z argumentów jest "/?"
                    bool helpRequestedForThisCommand = false;
                    for (size_t i = 1; i < tokens.size(); ++i) {
                        if (tokens[i] == "/?") {
                            helpRequestedForThisCommand = true;
                            break;
                        }
                    }

                    if (helpRequestedForThisCommand) {
                        // Dzielenie helpMessage na linie, jeœli zawiera '\n'
                        string line;
                        size_t start = 0;
                        size_t end = foundCmdDef->helpMessage.find('\n');
                        while (end != string::npos) {
                            consoleOutputLines.push_back(foundCmdDef->helpMessage.substr(start, end - start));
                            start = end + 1;
                            end = foundCmdDef->helpMessage.find('\n', start);
                        }
                        consoleOutputLines.push_back(foundCmdDef->helpMessage.substr(start)); // Ostatnia linia
                        commandExecutedSuccessfully = true;
                    }
                    else {
                        // --- Logika specyficzna dla komendy ---
                        if (foundCmdDef->name == "HELP") {
                            if (tokens.size() > 1) { // HELP <command_name_target>
                                string helpTargetName = tokens[1];
                                string helpTargetNameNormalized = helpTargetName;
                                transform(helpTargetNameNormalized.begin(), helpTargetNameNormalized.end(), helpTargetNameNormalized.begin(), ::toupper);

                                const CommandDefinition* targetCmdHelpDef = nullptr;
                                for (const auto& cmdDefTarget : definedCommands) {
                                    if (cmdDefTarget.name == helpTargetNameNormalized) {
                                        targetCmdHelpDef = &cmdDefTarget;
                                        break;
                                    }
                                }
                                if (targetCmdHelpDef) {
                                    // Dzielenie helpMessage na linie
                                    string line;
                                    size_t start = 0;
                                    size_t end = targetCmdHelpDef->helpMessage.find('\n');
                                    while (end != string::npos) {
                                        consoleOutputLines.push_back(targetCmdHelpDef->helpMessage.substr(start, end - start));
                                        start = end + 1;
                                        end = targetCmdHelpDef->helpMessage.find('\n', start);
                                    }
                                    consoleOutputLines.push_back(targetCmdHelpDef->helpMessage.substr(start));
                                }
                                else {
                                    consoleOutputLines.push_back("No help topic found for '" + helpTargetName + "'.");
                                }
                            }
                            else { // HELP (ogólne)
                                consoleOutputLines.push_back("For more information on a specific command, type HELP [command-name].");
                                consoleOutputLines.push_back("");
                                for (const auto& cmdDefList : definedCommands) {
                                    consoleOutputLines.push_back(cmdDefList.name + " - " + cmdDefList.shortDescription);
                                }
                            }
                            commandExecutedSuccessfully = true;
                        }
                        else if (foundCmdDef->name == "CLEAR") {
                            consoleOutputLines.clear();
                            consoleScrollOffset = 0;
                            commandExecutedSuccessfully = true;
                        }
                        //else if (foundCmdDef->name == "HISTORY") {
                        //    consoleOutputLines.push_back("Command history");
                        //    commandExecutedSuccessfully = true;
                        //}
                        else if (foundCmdDef->name == "TASKLIST") {
                            if (runningProcesses.empty())
                                consoleOutputLines.push_back("INFO: No tasks are running which match the specified criteria.");
                            else {
                                consoleOutputLines.push_back("Image Name");
                                consoleOutputLines.push_back("=========================");

                                for (const auto& proc : runningProcesses)
                                    consoleOutputLines.push_back(proc);
                                
                            }
                            commandExecutedSuccessfully = true;
                        }
                        else if (foundCmdDef->name == "TASKKILL") {
                            string targetProcess = "";
                            bool argT = false;
                            bool argF = false;

                            // Przetwarzanie argumentów dla TASKKILL
                            for (size_t i = 1; i < tokens.size(); ++i) {
                                string currentArg = tokens[i];
                                // Normalizuj argumenty do wielkich liter, jeœli prze³¹czniki s¹ zdefiniowane jako wielkie
                                string normalizedArg = currentArg;
                                transform(normalizedArg.begin(), normalizedArg.end(), normalizedArg.begin(), ::toupper); // Normalizuj argument do /T /F

                                if (normalizedArg == "/T") argT = true;
                                else if (normalizedArg == "/F") argF = true;
                                else if (currentArg != "/?" && targetProcess.empty()) {
                                    targetProcess = tokens[i]; // Pierwszy nie-prze³¹cznik to nazwa procesu
                                }
                            }

                            if (targetProcess.empty()) {
                                consoleOutputLines.push_back("ERROR: The syntax of the command is incorrect.");
                                consoleOutputLines.push_back("Type TASKKILL /? for command syntax.");
                            }
                            else {
                                // SprawdŸ, czy proces istnieje w runningProcesses
                                // Normalizuj targetProcess, jeœli nazwy w runningProcesses s¹ w jednym przypadku
                                string normalizedTargetProcess = targetProcess;
                                // transform(normalizedTargetProcess.begin(), normalizedTargetProcess.end(), normalizedTargetProcess.begin(), ::toupper); // Jeœli helpy.ini jest zawsze uppercase

                                auto it = find(runningProcesses.begin(), runningProcesses.end(), targetProcess); // U¿yj oryginalnego targetProcess, jeœli w runningProcesses masz np. "helpy.ini"

                                if (it == runningProcesses.end())
                                    consoleOutputLines.push_back("ERROR: The process \"" + targetProcess + "\" not found.");
                                else {
                                    // ----- Logika dla helpy.ini -----
                                    if (targetProcess == "helpy.ini") { // U¿yj oryginalnego targetProcess lub znormalizowanego, w zale¿noœci od runningProcesses
                                        if (argT || argF) { // Jeœli podano /T lub /F
                                            if (!helpyKilled) {
                                                helpyKilled = true;
                                                consoleOutputLines.push_back("SUCCESS: The process \"" + targetProcess + "\" has been terminated.");
                                                // Twoja logika zabijania Helpy (animacja, dŸwiêk, muzyka)
                                                if (res.helpyCrackedGifLoaded) {
                                                    helpyCrackedCurrentPlayingFrame = 0;
                                                    helpyCrackedAnimInternalTimer = 0.0f;
                                                    musicRestartKillTimer = 0.0f;
                                                    if (res.helpyCrackedGifImage.data && res.helpyCrackedAnimFrames > 0)
                                                        UpdateTexture(res.helpyCrackedTexture, (unsigned char*)res.helpyCrackedGifImage.data);
                                                }
                                                PlaySound(res.helpyCrackSound);

                                                if (res.settingsMusicLoaded && IsMusicStreamPlaying(res.settingsMusic))
                                                    StopMusicStream(res.settingsMusic);

                                                runningProcesses.erase(remove(runningProcesses.begin(), runningProcesses.end(), targetProcess), runningProcesses.end());
                                            }
                                            else consoleOutputLines.push_back("You've already broken him...");
                                            
                                        }
                                        else {
                                            consoleOutputLines.push_back("ERROR: Invalid argument/option -  \'" + targetProcess + "\'.");
                                            consoleOutputLines.push_back("Type \"TASKKILL /?\" for usage.");
                                        }
                                    }
                                    // ----- Logika dla fazbOS.exe -----
                                    else if (targetProcess == "fazbOS.exe") {
                                        if (argF) {
                                            consoleOutputLines.push_back("SUCCESS: The process \"" + targetProcess + "\" has been terminated.");
                                            consoleOutputLines.push_back("...");
                                            consoleOutputLines.push_back("CRITICAL_PROCESS_DIED");
                                            // Tu logika Easter Egga

                                            runningProcesses.erase(remove(runningProcesses.begin(), runningProcesses.end(), targetProcess), runningProcesses.end());
                                        }
                                        else if (argT) {
                                            consoleOutputLines.push_back("ERROR: The process \"" + targetProcess + "\" could not be terminated (graceful termination not supported).");
                                            consoleOutputLines.push_back("Reason: This process can only be terminated forcefully (with /F).");
                                        }
                                        else { // Jeœli dla fazbOS.exe nie podano /F ani /T
                                            consoleOutputLines.push_back("ERROR: Invalid argument/option. No valid termination switch (/F) provided for fazbOS.exe.");
                                            consoleOutputLines.push_back("Type TASKKILL /? for command syntax.");
                                        }
                                    }
                                    // ----- Dla innych procesów z listy runningProcesses, które nie s¹ specjalnie obs³ugiwane -----
                                    else {
                                        consoleOutputLines.push_back("INFO: The process \"" + targetProcess + "\" cannot be terminated by this utility (not a recognized user process for termination).");
                                    }
                                }
                            }
                            commandExecutedSuccessfully = true;

                        }
                    }
                }
                else { // foundCmdDef jest nullptr -> Nie znaleziono komendy
                    consoleOutputLines.push_back("'" + commandNameInput + "' is not recognized as an internal or external command,");
                    consoleOutputLines.push_back("operable program or batch file. Type HELP for a list of commands.");
                }
            }

            // Czyszczenie bufora i przewijanie
            memset(consoleInputBuffer, 0, consoleMaxChars + 1);
            consoleInputCursorPos = 0;
            if (consoleOutputLines.size() > consoleLinesToDraw)
                consoleScrollOffset = consoleOutputLines.size() - consoleLinesToDraw;
            else
                consoleScrollOffset = 0;
        }
    }

    while (consoleOutputLines.size() > 100) {
        consoleOutputLines.erase(consoleOutputLines.begin());
        if (consoleScrollOffset > 0) consoleScrollOffset--;
    }

    float mouseWheelMove = GetMouseWheelMove();
    if (mouseWheelMove != 0) {
        consoleScrollOffset -= (int)(mouseWheelMove * 2);

        if (consoleScrollOffset < 0) consoleScrollOffset = 0;
        int maxPossibleScroll = 0;

        if (consoleOutputLines.size() > (size_t)consoleLinesToDraw)
            maxPossibleScroll = consoleOutputLines.size() - consoleLinesToDraw;

        if (consoleScrollOffset > maxPossibleScroll)
            consoleScrollOffset = maxPossibleScroll;
    }


// --- Console Drawing ---
    Rectangle outputArea = {
        consoleRect.x + 10,
        consoleRect.y + 10,
        consoleRect.width - 20,
        consoleRect.height - 55
    };

    Rectangle inputBgArea = {
        consoleRect.x + 5,
        consoleRect.y + consoleRect.height - 35,
        consoleRect.width - 10,
        consoleRect.height - (consoleRect.height - 30)
    };

    DrawRectangleRec(consoleRect, GRAY);
    DrawRectangleLinesEx(consoleRect, 1, WHITE);

    int outputFontSize = 16;

    int startLineToDraw = consoleScrollOffset;
    if (consoleOutputLines.size() <= consoleLinesToDraw)
        startLineToDraw = 0;
    else if (startLineToDraw > consoleOutputLines.size() - consoleLinesToDraw)
        startLineToDraw = consoleOutputLines.size() - consoleLinesToDraw;
    
    for (int i = 0; i < consoleLinesToDraw; ++i) {
        int currentLineInHistory = startLineToDraw + i;
        if (currentLineInHistory >= 0 && currentLineInHistory < (int)consoleOutputLines.size()) {
            DrawTextEx(font, consoleOutputLines[currentLineInHistory].c_str(),
                { outputArea.x, outputArea.y + (i * 18)},
                outputFontSize, 1, WHITE);
        }
        else break;
        
    }
    float textInputY = inputBgArea.y + (inputBgArea.height - outputFontSize) / 2;
    DrawRectangleRec(inputBgArea, Fade(BLACK, 0.75));
    DrawRectangleLinesEx(inputBgArea, 1, DARKGRAY);
    DrawTextEx(font, TextFormat("> %s", consoleInputBuffer), { inputBgArea.x + 5, textInputY }, outputFontSize, 1, LIME);



    char tempCursorBuffer[consoleMaxChars + 3];
    snprintf(tempCursorBuffer, sizeof(tempCursorBuffer), "> %.*s", consoleInputCursorPos, consoleInputBuffer);
    Vector2 cursorPosTextSize = MeasureTextEx(font, tempCursorBuffer, outputFontSize, 1);

    if (((int)(GetTime() * 2.5)) % 2 == 0) {
        DrawRectangle(inputBgArea.x + 5 + cursorPosTextSize.x, textInputY - 1, 2, outputFontSize + 2, LIME);
    }
}

static bool CheckRainbowLandResourcesLoaded(const RainbowLandGameResources& res) {
    vector<string> missingResources;
    if (res.idleRight.id == 0) missingResources.push_back("idleRight");
    if (res.idleLeft.id == 0) missingResources.push_back("idleLeft");
    if (res.walkRight.id == 0) missingResources.push_back("walkRight");
    if (res.walkLeft.id == 0) missingResources.push_back("walkLeft");
    if (res.jumpRight.id == 0) missingResources.push_back("jumpRight");
    if (res.jumpLeft.id == 0) missingResources.push_back("jumpLeft");
    if (res.bg.id == 0) missingResources.push_back("bg");
    if (res.tile.id == 0) missingResources.push_back("tile");
    if (res.platformTexture.id == 0) missingResources.push_back("platformTexture");
    if (res.butterfly.id == 0) missingResources.push_back("butterfly");
    if (res.spikesUp.id == 0) missingResources.push_back("spikesUp");
    if (res.spikesDown.id == 0) missingResources.push_back("spikesDown");
    if (res.controlKeysInfo.id == 0) missingResources.push_back("controlKeysInfo");
    if (res.fenceProp.id == 0) missingResources.push_back("fenceProp");
    if (res.flowerSmallProp.id == 0) missingResources.push_back("flowerSmallProp");
    if (res.flowerBigProp.id == 0) missingResources.push_back("flowerBigProp");
    if (res.checkpointFlag.id == 0) missingResources.push_back("checkpointFlag");
    if (res.sunflower.id == 0) missingResources.push_back("sunflower");
    if (res.checkpoint.id == 0) missingResources.push_back("checkpoint");
    if (res.rbowBodyTexture.id == 0) missingResources.push_back("rbowBodyTexture");
    for (size_t i = 0; i < res.rbowEyeTextures.size(); ++i) if (res.rbowEyeTextures[i].id == 0) missingResources.push_back("rbowEyeTextures[" + to_string(i) + "]");
    for (size_t i = 0; i < res.sunflowerPetals.size(); ++i) if (res.sunflowerPetals[i].id == 0) missingResources.push_back("sunflowerPetals[" + to_string(i) + "]");
    if (res.buttonVoicesOn.id == 0) missingResources.push_back("buttonVoicesOn");
    if (res.buttonVoicesOff.id == 0) missingResources.push_back("buttonVoicesOff");
    if (!res.backgroundMusicLoaded) missingResources.push_back("backgroundMusic");
    if (!res.jumpSoundLoaded) missingResources.push_back("FX: jump");
    if (!res.petalSoundLoaded) missingResources.push_back("FX: petalShoot");
    if (!res.spikesSoundLoaded) missingResources.push_back("FX: spikesPush");
    if (!res.deathSoundLoaded) missingResources.push_back("FX: death");
    if (!res.checkpointSoundLoaded) missingResources.push_back("FX: checkpoint");
    for (size_t i = 0; i < res.rbowDialogues.size(); ++i) if (res.rbowDialogues[i].frameCount == 0) missingResources.push_back("rbowDialogues[" + to_string(i) + "]");
    if (!res.rbowVoiceOffSoundLoaded) missingResources.push_back("rbowVoiceOff");
    if (!res.rbowYouStillSuckSoundLoaded) missingResources.push_back("rbowYouStillSuck");


    if (!missingResources.empty()) {
        cout << "RAINBOW LAND: Critical error - failed to load resources:" << endl;
        for (const string& resourceName : missingResources) 
            cout << resourceName << endl;

        return false;
    }
    return true;
}
static bool CheckMotoristResourcesLoaded(const MotoristGameResources& res) {
    vector<string> missingResources;
    if (res.car.id == 0) missingResources.push_back("car");
    for (size_t i = 0; i < res.carSpinFrames.size(); ++i) if (res.carSpinFrames[i].id == 0) missingResources.push_back("carSpinFrames[" + to_string(i) + "]");
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


// --- MAIN FUNCTION ---
int main(void) {
    InitializeGameSettings(); // £aduje ustawienia z pliku lub ustawia domyœlne

    for (int i = 0; i < numResolutionOptions; i++) 
        if (g_settings.currentResolution == resolutionValues[i]) {
            currentResolutionIndex = i;

            break;
        }
    
    for (int i = 0; i < numQualityOptions; i++) 
        if (g_settings.quality == qualityValues[i]) {
            currentQualityIndex = i;

            break;
        }
    

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(g_settings.screenWidth, g_settings.screenHeight, "FNaF Minigames - Loading...");
    InitAudioDevice();
    HideCursor();

    MainMenuResources mainMenuRes;
    Font consoleFont = GetFontDefault(); // TODO: change console font

    bool initialStaticLoadScreenReady = LoadInitialLoadingScreenResources(mainMenuRes);
    float loadingScreenMinVisibleTime = 1; // Skrócony czas dla testów
    float loadingScreenVisibleTimer = 0;

    SetWindowPosition((GetMonitorWidth(GetCurrentMonitor()) - g_settings.screenWidth) / 2, (GetMonitorHeight(GetCurrentMonitor()) - g_settings.screenHeight) / 2);
    SetMasterVolume(g_settings.masterVolume);
    SetTargetFPS(60);

    GameScreen currentScreen = LOADING;
    GameScreen previousScreen = LOADING;
    bool shouldExit = false;

    int mainMenuBgCurrentFrame = 0;
    float mainMenuBgFrameDelay = 0.067;
    float mainMenuBgFrameTimer = 0;

    float fadeAlpha = 0;
    float fadeSpeed = 1.25;
    bool isFadingOut = false;
    bool isFadingIn = false;
    GameScreen fadeTargetScreen = MAIN_MENU;

    bool mainResourcesLoadAttempted = false;
    bool mainMenuResourcesLoaded = false;


    while (!shouldExit && !WindowShouldClose()) {
        float dt = GetFrameTime();
        Vector2 mousePointWindow = GetMousePosition();
        float mouseScaleX = (float)logicalWidth / GetScreenWidth();
        float mouseScaleY = (float)logicalHeight / GetScreenHeight();
        Vector2 scaledMousePos = { mousePointWindow.x * mouseScaleX, mousePointWindow.y * mouseScaleY };

        // --- Fading Logic ---
        if (isFadingOut) {
            fadeAlpha += fadeSpeed * dt;
            if (previousScreen == MAIN_MENU && mainMenuRes.menuMusicLoaded && IsMusicStreamPlaying(mainMenuRes.menuMusic)) {
                currentMenuMusicVolume = fmaxf(0, currentMenuMusicVolume - musicFadeSpeed * dt);
                SetMusicVolume(mainMenuRes.menuMusic, currentMenuMusicVolume * g_settings.masterVolume);
            }
            else if (previousScreen == SETTINGS && mainMenuRes.settingsMusicLoaded && IsMusicStreamPlaying(mainMenuRes.settingsMusic)) {
                currentSettingsMusicVolume = fmaxf(0, currentSettingsMusicVolume - musicFadeSpeed * dt);
                SetMusicVolume(mainMenuRes.settingsMusic, currentSettingsMusicVolume * g_settings.masterVolume);
            }

            if (fadeAlpha >= 1) {
                fadeAlpha = 1;
                isFadingOut = false;

                if (previousScreen == MAIN_MENU) StopAndUnloadMusic(mainMenuRes.menuMusic, mainMenuRes.menuMusicLoaded);
                else if (previousScreen == SETTINGS) StopAndUnloadMusic(mainMenuRes.settingsMusic, mainMenuRes.settingsMusicLoaded);

                currentMenuMusicVolume = 1;
                currentSettingsMusicVolume = 1;

                GameScreen screenAfterGame = MAIN_MENU;

                if (fadeTargetScreen == PLAYING_MOTORIST) {
                    MotoristGameResources motoristRes = LoadMotoristResources(g_settings.quality);
                    if (CheckMotoristResourcesLoaded(motoristRes)) {
                        ShowCursor();
                        runMidnightMotorist(g_settings.quality, mainMenuRes.crtShader, g_settings.useCRTShader && mainMenuRes.shaderLoadedSuccessfully);
                        HideCursor();
                    }
                    else cout << "Failed to load Motorist resources. Returning to Main Menu." << endl;

                    UnloadMotoristResources(motoristRes);
                }
                else if (fadeTargetScreen == PLAYING_RAINBOW) {
                    RainbowLandGameResources rainbowRes = LoadRainbowLandResources(g_settings.quality);
                    if (CheckRainbowLandResourcesLoaded(rainbowRes)) {
                        ShowCursor();
                        runMagicRainbowLand(g_settings.quality);
                        HideCursor();
                    }
                    else cout << "Failed to load Rainbow Land resources. Returning to Main Menu.";
                    
                    UnloadRainbowLandResources(rainbowRes);
                }
                previousScreen = currentScreen;

                if (fadeTargetScreen == PLAYING_MOTORIST || fadeTargetScreen == PLAYING_RAINBOW) currentScreen = screenAfterGame;
                
                else currentScreen = fadeTargetScreen;
                
                if (IsWindowMinimized()) RestoreWindow();
                SetTargetFPS(60);

                if (currentScreen == MAIN_MENU) {
                    if (!mainMenuRes.menuMusicLoaded && FileExists("resources/mainMenu.mp3")) {
                        mainMenuRes.menuMusic = LoadMusicStream("resources/mainMenu.mp3");
                        if (mainMenuRes.menuMusic.stream.buffer)
                            mainMenuRes.menuMusicLoaded = true;
                    }
                    if (mainMenuRes.menuMusicLoaded) {
                        PlayMusicStream(mainMenuRes.menuMusic);

                        if (helpyKilled) SetMusicPitch(mainMenuRes.menuMusic, 0.6);
                        else SetMusicPitch(mainMenuRes.menuMusic, 1);
                    }
                }
                else if (currentScreen == SETTINGS) {
                    if (!mainMenuRes.settingsMusicLoaded && FileExists("resources/SETTINGS.mp3")) {
                        mainMenuRes.settingsMusic = LoadMusicStream("resources/SETTINGS.mp3");
                        if (mainMenuRes.settingsMusic.stream.buffer)
                            mainMenuRes.settingsMusicLoaded = true;
                    }
                    if (mainMenuRes.settingsMusicLoaded) {
                        PlayMusicStream(mainMenuRes.settingsMusic);
                        if (helpyKilled) SetMusicPitch(mainMenuRes.settingsMusic, 0.4); // Your pitch
                        else SetMusicPitch(mainMenuRes.settingsMusic, 1);
                        
                        // Reset NORMAL Helpy animation state
                        helpyCurrentAnimFrame = 0; 
                        helpyFrameTimer = 0;

                        if (helpyKilled && mainMenuRes.helpyCrackedGifLoaded) {
                            if (mainMenuRes.helpyCrackedAnimFrames > 0 &&
                                helpyCrackedCurrentPlayingFrame >= mainMenuRes.helpyCrackedAnimFrames - 1) { // Inferring completion

                                int lastFrameIndex = mainMenuRes.helpyCrackedAnimFrames - 1;
                                helpyCrackedCurrentPlayingFrame = lastFrameIndex; // Lock to last frame

                                int frameDataSize = GetPixelDataSize(mainMenuRes.helpyCrackedGifImage.width, mainMenuRes.helpyCrackedGifImage.height, mainMenuRes.helpyCrackedGifImage.format);
                                unsigned char* frameDataOffset = (unsigned char*)mainMenuRes.helpyCrackedGifImage.data + (lastFrameIndex * frameDataSize);
                                UpdateTexture(mainMenuRes.helpyCrackedTexture, frameDataOffset);
                            }
                        }
                    }
                }
                isFadingIn = true;
            }
        }
        else if (isFadingIn) {
            fadeAlpha -= fadeSpeed * dt;
            if (currentScreen == MAIN_MENU && mainMenuRes.menuMusicLoaded && IsMusicStreamPlaying(mainMenuRes.menuMusic)) {
                currentMenuMusicVolume = fminf(1, currentMenuMusicVolume + musicFadeSpeed * dt);
                SetMusicVolume(mainMenuRes.menuMusic, currentMenuMusicVolume * g_settings.masterVolume);
            }
            else if (currentScreen == SETTINGS && mainMenuRes.settingsMusicLoaded && IsMusicStreamPlaying(mainMenuRes.settingsMusic)) {
                currentSettingsMusicVolume = fminf(1, currentSettingsMusicVolume + musicFadeSpeed * dt);
                SetMusicVolume(mainMenuRes.settingsMusic, currentSettingsMusicVolume * g_settings.masterVolume);
            }

            if (fadeAlpha <= 0) {
                fadeAlpha = 0;
                isFadingIn = false;
                if (currentScreen == MAIN_MENU && mainMenuRes.menuMusicLoaded) 
                    SetMusicVolume(mainMenuRes.menuMusic, g_settings.masterVolume);
                else if (currentScreen == SETTINGS && mainMenuRes.settingsMusicLoaded) 
                    SetMusicVolume(mainMenuRes.settingsMusic, g_settings.masterVolume);
            }
        }

    // --- Screen Logic (when not fading) ---
        if (!isFadingOut && !isFadingIn) {
            switch (currentScreen) {
                case LOADING: {
                    loadingScreenVisibleTimer += dt;
                    if (!mainResourcesLoadAttempted && loadingScreenVisibleTimer > 0.1) {
                        mainResourcesLoadAttempted = true;
                        mainMenuResourcesLoaded = LoadMainMenuResources(mainMenuRes, logicalWidth, logicalHeight);

                        if (!mainMenuRes.shaderLoadedSuccessfully) g_settings.useCRTShader = false;
                        SetWindowTitle("FNaF Minigames Port"); 
                        consoleFont = mainMenuRes.defaultGuiFont; // TODO: change default console font
                    }

                    if (mainMenuResourcesLoaded && loadingScreenVisibleTimer >= loadingScreenMinVisibleTime) {
                        previousScreen = LOADING;
                        currentScreen = MAIN_MENU;
                        fadeTargetScreen = MAIN_MENU;
                        isFadingIn = true;
                        fadeAlpha = 1;

                        if (!mainMenuRes.menuMusicLoaded && FileExists("resources/mainMenu.mp3")) {
                            mainMenuRes.menuMusic = LoadMusicStream("resources/mainMenu.mp3");
                            if (mainMenuRes.menuMusic.stream.buffer) mainMenuRes.menuMusicLoaded = true;
                        }

                        if (mainMenuRes.menuMusicLoaded) PlayMusicStream(mainMenuRes.menuMusic);
                    }
                } break;
                case MAIN_MENU: {
                    UpdateMainMenuScreen(mainMenuRes, scaledMousePos, currentScreen, previousScreen, fadeTargetScreen, isFadingOut, fadeAlpha, dt, mainMenuBgCurrentFrame, mainMenuBgFrameTimer, mainMenuBgFrameDelay);
                } break;
                case SETTINGS: {
                    UpdateSettingsScreen(mainMenuRes, scaledMousePos, currentScreen, previousScreen, fadeTargetScreen, isFadingOut, fadeAlpha, dt, helpyCurrentAnimFrame, helpyFrameTimer, helpyFrameDelay);
                } break;
                case EXITING: {
                    shouldExit = true;
                } break;
                default: break;
            }
        }

    // ------ DRAWING SECTION ------
        BeginDrawing();
        ClearBackground(BLACK);

        if (currentScreen == LOADING && !isFadingOut && !isFadingIn) {
            if (mainMenuRes.helpyLoadingTextureLoaded && mainMenuRes.helpyLoadingScreenTexture.id > 0) {
                float imgDrawWidth = (float)mainMenuRes.helpyLoadingScreenTexture.width;
                float imgDrawHeight = (float)mainMenuRes.helpyLoadingScreenTexture.height;
                DrawTexture(mainMenuRes.helpyLoadingScreenTexture, (GetScreenWidth() - imgDrawWidth) / 2, (GetScreenHeight() - imgDrawHeight) / 2 - 60, WHITE);
            }
            else DrawText("Loading...", GetScreenWidth() / 2 - MeasureText("Loading...", 40) / 2, GetScreenHeight() / 2 - 20, 40, LIGHTGRAY);
        }
        else if (mainMenuRes.targetRenderTexture.id > 0) {
            BeginTextureMode(mainMenuRes.targetRenderTexture);
            ClearBackground(BLANK);

            GameScreen screenToDrawForContent = currentScreen;
            if (isFadingOut) screenToDrawForContent = previousScreen;
            switch (screenToDrawForContent) {
                case MAIN_MENU:
                    DrawMainMenuScreen(mainMenuRes, scaledMousePos, mainMenuBgCurrentFrame, currentScreen, previousScreen, fadeTargetScreen, isFadingOut, fadeAlpha);
                break;
                case SETTINGS: {
                    DrawSettingsScreen(mainMenuRes, scaledMousePos, helpyCurrentAnimFrame, currentScreen, previousScreen, fadeTargetScreen, isFadingOut, fadeAlpha);
                    if (consoleActive) UpdateDrawConsole(mainMenuRes, consoleFont, scaledMousePos, dt);
                } break;
                case PLAYING_MOTORIST:
                case PLAYING_RAINBOW:
                    ClearBackground(BLACK);
                    DrawText("IN GAME...", logicalWidth / 2 - MeasureText("IN GAME...", 20) / 2, logicalHeight / 2 - 10, 20, WHITE);
                    break;
                default: break;
            }
            EndTextureMode();

            float finalScale = fminf((float)GetScreenWidth() / logicalWidth, (float)GetScreenHeight() / logicalHeight);
            float finalOffsetX = (GetScreenWidth() - (logicalWidth * finalScale)) * 0.5;
            float finalOffsetY = (GetScreenHeight() - (logicalHeight * finalScale)) * 0.5;
            Rectangle srcRect = { 0, 0, (float)mainMenuRes.targetRenderTexture.texture.width, -(float)mainMenuRes.targetRenderTexture.texture.height };
            Rectangle destRect = { finalOffsetX, finalOffsetY, logicalWidth * finalScale, logicalHeight * finalScale };
            DrawTexturePro(mainMenuRes.targetRenderTexture.texture, srcRect, destRect, { 0, 0 }, 0, WHITE);

        }
        else if (currentScreen != LOADING) DrawText("Error: Render Target Failed", 100, 100, 20, RED);
        
        if ((isFadingOut || isFadingIn) && fadeAlpha > 0)
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, fadeAlpha));
        
        if (mainMenuRes.cursor.id > 0 && (currentScreen == MAIN_MENU || currentScreen == SETTINGS) && !(isFadingIn || isFadingOut && fadeAlpha > 0.9))
            DrawTexture(mainMenuRes.cursor, (int)mousePointWindow.x - mainMenuRes.cursor.width / 2, (int)mousePointWindow.y - mainMenuRes.cursor.height / 2, WHITE);
        
        DrawFPS(10, 10);
        EndDrawing();
    }

    cout << "CLEANUP: Starting main cleanup..." << endl;

    UnloadMainMenuResources(mainMenuRes);
    CloseAudioDevice();
    CloseWindow();

    cout << "CLEANUP: Finished. Exiting application." << endl;

    return 0;
}