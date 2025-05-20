#ifndef _DEBUG
#pragma comment(linker, "/ENTRY:mainCRTStartup")
#endif

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


struct ArgumentDetail {
    string name;
    string description;
};

struct CommandDefinition {
    string name;
    string helpMessage;
    map<string, ArgumentDetail> acceptedArguments;
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
        "Provides help information for console commands.\n\nHELP [command]\n\n  command - displays help information on that command.",
        {{"/?", {"/?", "Displays this help message."}}}
    },
    {
        "CLEAR",
        "Clears all text from the console window.\n\nCLEAR [/?]",
        {{"/?", {"/?", "Displays this help message."}}}
    },
    {
        "TASKLIST",
        "Displays a list of all currently running simulated processes.\n\nTASKLIST [/?]",
        {{"/?", {"/?", "Displays this help message."}}}
    },
    {
        "TASKKILL",
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
float helpyFrameDelay = 0.725;
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

bool GuiButton(Rectangle bounds, const char* text, Font font, Color bgColor, Color hoverColor, Color pressedColor, Vector2 mousePos) {
    bool clicked = false;
    Color currentBgColor = bgColor;

    Vector2 textSize = MeasureTextEx(font, text, settingsSelectorFontSize, 1);

    if (!consoleActive && CheckCollisionPointRec(mousePos, bounds)) { 
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

int GuiSelector(Rectangle bounds, const char* currentOptionText, Font font, Color textColor, Color boxColor, Color arrowColor, Vector2 mousePos) {
    int clicked_direction = 0;

    float arrowSize = 15;
    float arrowPaddingFromBox = 8;

    DrawRectangleRec(bounds, boxColor);
    DrawRectangleLinesEx(bounds, 1, Fade(BLACK, 0.3));

    Vector2 optionTextSize = MeasureTextEx(font, currentOptionText, settingsSelectorFontSize, 1);
    DrawTextEx(font, currentOptionText, { bounds.x + (bounds.width - optionTextSize.x) / 2, bounds.y + (bounds.height - optionTextSize.y) / 2 }, settingsSelectorFontSize, 1, textColor);

    // Left arrow
    Vector2 leftArrowPoints[3];
    leftArrowPoints[1] = { bounds.x - arrowPaddingFromBox - arrowSize, bounds.y + bounds.height / 2 };
    leftArrowPoints[0] = { bounds.x - arrowPaddingFromBox, bounds.y + bounds.height / 2 - arrowSize};
    leftArrowPoints[2] = { bounds.x - arrowPaddingFromBox, bounds.y + bounds.height / 2 + arrowSize };

    Rectangle leftArrowHotspot = {
        leftArrowPoints[0].x,
        bounds.y + bounds.height / 2 - arrowSize,
        arrowSize + 2,
        arrowSize * 2 + 2
    };

    if (!consoleActive && CheckCollisionPointRec(mousePos, leftArrowHotspot))
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) 
            clicked_direction = -1;
    
    DrawTriangle(leftArrowPoints[0], leftArrowPoints[1], leftArrowPoints[2], arrowColor);

    // --- PRAWA STRZA£KA (TRÓJK¥T) ---
    Vector2 rightArrowPoints[3];
    rightArrowPoints[0] = { bounds.x + bounds.width + arrowPaddingFromBox + arrowSize, bounds.y + bounds.height / 2 };
    rightArrowPoints[1] = { bounds.x + bounds.width + arrowPaddingFromBox, bounds.y + bounds.height / 2 - arrowSize / 1.5f };
    rightArrowPoints[2] = { bounds.x + bounds.width + arrowPaddingFromBox, bounds.y + bounds.height / 2 + arrowSize / 1.5f };

    Rectangle rightArrowHotspot = {
        bounds.x + bounds.width + arrowPaddingFromBox,
        bounds.y + bounds.height / 2 - arrowSize,
        arrowSize + 2,
        arrowSize * 2 + 2
    };
    
    if (!consoleActive && CheckCollisionPointRec(mousePos, rightArrowHotspot)) {
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && clicked_direction == 0) clicked_direction = 1;
    }
    DrawTriangle(rightArrowPoints[0], rightArrowPoints[1], rightArrowPoints[2], arrowColor);
    DrawTriangleLines(rightArrowPoints[0], rightArrowPoints[1], rightArrowPoints[2], Fade(BLACK, 0.5f));

    return clicked_direction;
}

void UpdateMainMenuScreen(MainMenuResources& res, Vector2 mousePos, GameScreen& currentScreen, GameScreen& previousScreen, GameScreen& fadeTargetScreen, bool& isFadingOut, float& fadeAlpha, float dt,
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
            char* frameDataOffset = (char*)res.bgGifImage.data + (mainMenuBgCurrentFrame * frameDataSize);
            UpdateTexture(res.bgTexture, frameDataOffset);
        }
    }

    if (IsWindowResized() && !IsWindowFullscreen())
        SetWindowPosition((GetMonitorWidth(GetCurrentMonitor()) - GetScreenWidth()) / 2, (GetMonitorHeight(GetCurrentMonitor()) - GetScreenHeight()) / 2);
}

void DrawMainMenuScreen(MainMenuResources& res, Vector2 mousePos, int mainMenuBgCurrentFrame, GameScreen& currentScreen, GameScreen& previousScreen, GameScreen& fadeTargetScreen, bool& isFadingOut, float& fadeAlpha) {
    if (res.gifLoaded && res.bgTexture.id > 0) 
        DrawTexturePro(res.bgTexture, { 0, 0, (float)res.bgTexture.width, (float)res.bgTexture.height }, { 0, 0, logicalWidth, logicalHeight }, { 0, 0 }, 0, WHITE);
    else ClearBackground(DARKBLUE);
    
    Vector2 titleSize = MeasureTextEx(res.bytesFont, "FNaF Minigames Port", 100, 1);
    DrawRectangleRec({ (logicalWidth - 400) / 2, 160, 400, 320 }, Fade(BLACK, 0.7));
    DrawTextEx(res.bytesFont, "FNaF Minigames Port", { (logicalWidth - titleSize.x) / 2, 40 }, 100, 1, WHITE);

    float buttonWidth = 300;
    float buttonHeight = 60;
    float buttonX = (logicalWidth - buttonWidth) / 2;
    float buttonYStart = 180;
    float buttonSpacing = 72;

    if (GuiButton({ buttonX, buttonYStart, buttonWidth, buttonHeight }, "Midnight Motorist", res.arcadeClassicFont, WHITE, BLUE, PURPLE, mousePos)) {
        previousScreen = currentScreen;
        fadeTargetScreen = PLAYING_MOTORIST;
        PlaySound(res.buttonSelect);
        isFadingOut = true;
        fadeAlpha = 0;
    }
    if (GuiButton({ buttonX, buttonYStart + buttonSpacing, buttonWidth, buttonHeight }, "Magic Rainbow Land", res.arcadeClassicFont, WHITE, YELLOW, RED, mousePos)) {
        previousScreen = currentScreen;
        fadeTargetScreen = PLAYING_RAINBOW;
        PlaySound(res.buttonSelect);
        isFadingOut = true;
        fadeAlpha = 0;
    }
    if (GuiButton({ buttonX, buttonYStart + buttonSpacing * 2, buttonWidth, buttonHeight }, "Settings", res.arcadeClassicFont, WHITE, LIGHTGRAY, GRAY, mousePos)) {
        previousScreen = MAIN_MENU;
        fadeTargetScreen = SETTINGS;
        PlaySound(res.buttonSelect);
        isFadingOut = true;
        fadeAlpha = 0;
    }
    if (GuiButton({ buttonX, buttonYStart + buttonSpacing * 3, buttonWidth, buttonHeight }, "Exit", res.arcadeClassicFont, WHITE, LIGHTGRAY, GRAY, mousePos)) {
        previousScreen = currentScreen;
        currentScreen = EXITING;
    }
}


void UpdateSettingsScreen(MainMenuResources& res, Vector2 mousePos, GameScreen& currentScreen, GameScreen& previousScreen, GameScreen& fadeTargetScreen, bool& isFadingOut, float& fadeAlpha, float dt, int& currentNormalHelpyFrame_param, float& normalHelpyTimer_param, float normalHelpyFrameDelay_param) {

    // --- Settings Music Logic ---
    if (helpyKilled) {
        musicRestartKillTimer += dt;

        if (res.settingsMusicLoaded && !IsMusicStreamPlaying(res.settingsMusic)) {
            if (musicRestartKillTimer >= 3) {
                SeekMusicStream(res.settingsMusic, 0);
                PlayMusicStream(res.settingsMusic);
                SetMusicVolume(res.settingsMusic, currentSettingsMusicVolume * g_settings.masterVolume);
                SetMusicPitch(res.settingsMusic, 0.4);
            }
        }
        else if (res.settingsMusicLoaded && IsMusicStreamPlaying(res.settingsMusic)) {
            UpdateMusicStream(res.settingsMusic);
            
            if (GetMusicTimePlayed(res.settingsMusic) >= GetMusicTimeLength(res.settingsMusic))
                SeekMusicStream(res.settingsMusic, 0);
        }
    }
    else {
        // Normal music behavior
        if (res.settingsMusicLoaded && !IsMusicStreamPlaying(res.settingsMusic) && currentSettingsMusicVolume > 0) {
            PlayMusicStream(res.settingsMusic);
            SetMusicVolume(res.settingsMusic, currentSettingsMusicVolume * g_settings.masterVolume);
            SetMusicPitch(res.settingsMusic, 1);
        }
        if (res.settingsMusicLoaded && IsMusicStreamPlaying(res.settingsMusic)) {
            UpdateMusicStream(res.settingsMusic);
            SetMusicPitch(res.settingsMusic, 1);

            if (GetMusicTimePlayed(res.settingsMusic) >= GetMusicTimeLength(res.settingsMusic)) {
                SeekMusicStream(res.settingsMusic, 0);
            }
        }
    }

    // --- Helpy Animation Logic ---
    if (helpyKilled) {
        if (res.helpyCrackedGifLoaded && res.helpyCrackedGifImage.data && (res.helpyCrackedAnimFrames == 0 || helpyCrackedCurrentPlayingFrame < res.helpyCrackedAnimFrames - 1)) {
            helpyCrackedAnimInternalTimer += dt;
            bool frameUpdated = false;

            while (helpyCrackedAnimInternalTimer >= helpyCrackedAnimFrameDelay) {
                helpyCrackedAnimInternalTimer -= helpyCrackedAnimFrameDelay;

                if (helpyCrackedCurrentPlayingFrame < res.helpyCrackedAnimFrames - 1) {
                    helpyCrackedCurrentPlayingFrame++;
                    frameUpdated = true;
                }
                else {
                    if (res.helpyCrackedAnimFrames > 0) 
                        helpyCrackedCurrentPlayingFrame = res.helpyCrackedAnimFrames - 1;
                    else 
                        helpyCrackedCurrentPlayingFrame = 0;
                    
                    frameUpdated = true;
                    break;
                }
            }

            if (frameUpdated) {
                int frameToUpdate = helpyCrackedCurrentPlayingFrame;

                if (frameToUpdate >= res.helpyCrackedAnimFrames)
                    frameToUpdate = res.helpyCrackedAnimFrames - 1;

                if (frameToUpdate < 0) 
                    frameToUpdate = 0;

                if (res.helpyCrackedAnimFrames > 0) {
                    int frameDataSize = GetPixelDataSize(res.helpyCrackedGifImage.width, res.helpyCrackedGifImage.height, res.helpyCrackedGifImage.format);
                    char* frameDataOffset = (char*)res.helpyCrackedGifImage.data + (frameToUpdate * frameDataSize);
                    UpdateTexture(res.helpyCrackedTexture, frameDataOffset);
                }
            }
        }
    }
    else {
        if (res.helpyGifLoaded && res.helpyTexture.id > 0 && res.helpyGifImage.data) {
            normalHelpyTimer_param += dt;
            bool normalFrameUpdated = false;
            while (normalHelpyTimer_param >= normalHelpyFrameDelay_param) {
                normalHelpyTimer_param -= normalHelpyFrameDelay_param;
                currentNormalHelpyFrame_param = (currentNormalHelpyFrame_param + 1) % res.helpyAnimFrames;
                normalFrameUpdated = true;
            }
            if (normalFrameUpdated) {
                int frameDataSize = GetPixelDataSize(res.helpyGifImage.width, res.helpyGifImage.height, res.helpyGifImage.format);
                char* frameDataOffset = (char*)res.helpyGifImage.data + (currentNormalHelpyFrame_param * frameDataSize);
                UpdateTexture(res.helpyTexture, frameDataOffset);
            }
        }
    }

// --- Console Toggle ---
    if (IsKeyPressed(KEY_GRAVE)) {
        consoleActive = !consoleActive;

        if (consoleActive) {
            memset(consoleInputBuffer, 0, 256);
            consoleInputCursorPos = 0;
        }
    }
}

void DrawSettingsScreen(MainMenuResources& res, Vector2 mousePos, int helpyCurrentAnimFrame, GameScreen& currentScreen, GameScreen& previousScreen, GameScreen& fadeTargetScreen, bool& isFadingOut, float& fadeAlpha) {
    if (res.settingsBgTexture.id > 0) DrawTexturePro(res.settingsBgTexture, { 0, 0, (float)res.settingsBgTexture.width, (float)res.settingsBgTexture.height }, { 0, 0, logicalWidth, logicalHeight }, { 0, 0 }, 0, WHITE);
    else ClearBackground(DARKGRAY);
    
    Vector2 settingsTitleSize = MeasureTextEx(res.bytesFont, "SETTINGS", 65, 1);
    DrawTextEx(res.bytesFont, "SETTINGS", { (logicalWidth - settingsTitleSize.x) / 2, 80 }, 65, 1, WHITE);


    Font uiFont = res.arcadeClassicFont;

    //DrawTextEx(uiFont, "Volume:", { 100, 180 }, settingsTextFontSize, 1, WHITE);
    //Rectangle volumeSliderBaseRect = { 250, 175, 300, 30 };
    //DrawRectangleRec(volumeSliderBaseRect, WHITE);
    //if (!consoleActive && CheckCollisionPointRec(mousePos, volumeSliderBaseRect) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    //    g_settings.masterVolume = Clamp((mousePos.x - volumeSliderBaseRect.x) / volumeSliderBaseRect.width, 0, 1);
    //    SetMasterVolume(g_settings.masterVolume);
    //    if (res.settingsMusicLoaded && IsMusicStreamPlaying(res.settingsMusic)) {
    //        SetMusicVolume(res.settingsMusic, currentSettingsMusicVolume * g_settings.masterVolume);
    //    }
    //}
    //DrawRectangle(volumeSliderBaseRect.x, volumeSliderBaseRect.y, volumeSliderBaseRect.width * g_settings.masterVolume, volumeSliderBaseRect.height, VIOLET);
    //DrawTextEx(uiFont, TextFormat("% %", g_settings.masterVolume * 100), { volumeSliderBaseRect.x + volumeSliderBaseRect.width + 15, 180 }, settingsTextFontSize, 1, WHITE);


    // Resolution Selector
    DrawTextEx(uiFont, "Resolution:", { 100, 240 }, settingsTextFontSize, 1, WHITE);
    Rectangle resolutionSelectorBounds = { selectorX, 235, selectorWidth, selectorHeight };
    int resolutionChange = GuiSelector(resolutionSelectorBounds, resolutionOptions[currentResolutionIndex], uiFont, BLACK, WHITE, DARKGRAY, mousePos);
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
    int qualityChange = GuiSelector(qualitySelectorBounds, qualityOptions[currentQualityIndex], uiFont, BLACK, WHITE, DARKGRAY, mousePos);
    if (qualityChange != 0) {
        PlaySound(res.buttonClick);
        currentQualityIndex = (currentQualityIndex + qualityChange + numQualityOptions) % numQualityOptions;
        g_settings.quality = qualityValues[currentQualityIndex];
    }

    // Shaders Toggle
    DrawTextEx(uiFont, "Shaders:", { 100, 360 }, settingsTextFontSize, 1, WHITE);
    Rectangle crtToggleButtonBounds = { selectorX, 355, 100, 30 };
    if (GuiButton(crtToggleButtonBounds, g_settings.useCRTShader ? "ON" : "OFF", uiFont, WHITE, LIGHTGRAY, GRAY, mousePos)) {
        if (res.shaderLoadedSuccessfully) {
            g_settings.useCRTShader = !g_settings.useCRTShader;
            if (g_settings.useCRTShader) PlaySound(res.buttonClick);
            else PlaySound(res.buttonPoof);
        }
        else { 
            PlaySound(res.buttonPoof); 
            DrawTextEx(uiFont, "(Shader N/A)", { crtToggleButtonBounds.x + crtToggleButtonBounds.width + 10, 360 }, 18, 1, LIGHTGRAY);
        }
    }

    // Back Button
    if (GuiButton({ logicalWidth / 2 - 100, logicalHeight - 100, 200, 50 }, "Back", uiFont, WHITE, LIGHTGRAY, GRAY, mousePos)) {
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
    Texture2D textureToDrawForHelpy = { 0 };
    Image imageForHelpyDimensions = { 0 };

    if (helpyKilled) {
        if (res.helpyCrackedGifLoaded && res.helpyCrackedTexture.id > 0) {
            textureToDrawForHelpy = res.helpyCrackedTexture;
            imageForHelpyDimensions = res.helpyCrackedGifImage;
        }
    }
    else {
        if (res.helpyGifLoaded && res.helpyTexture.id > 0) {
            textureToDrawForHelpy = res.helpyTexture;
            imageForHelpyDimensions = res.helpyGifImage;
        }
    }


    if (textureToDrawForHelpy.id > 0 && imageForHelpyDimensions.data) {
        Rectangle helpyDestinationRect = {
            logicalWidth - (imageForHelpyDimensions.width / 1.5) - 10,
            logicalHeight - (imageForHelpyDimensions.height / 1.5) - 15,
            imageForHelpyDimensions.width / 1.5,
            imageForHelpyDimensions.height / 1.5
        };
        Rectangle helpySourceRect = { 0, 0, textureToDrawForHelpy.width, textureToDrawForHelpy.height };
        DrawTexturePro(textureToDrawForHelpy, helpySourceRect, helpyDestinationRect, { 0,0 }, 0, WHITE);
    }
}

void UpdateDrawConsole(MainMenuResources& res, Font font, Vector2 mousePos, float dt) {
    if (!consoleActive) return;
    
    int keyPressedUnicode = GetCharPressed();
    bool inputChangedByTyping = false;
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

            inputChangedByTyping = true;
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
        Rectangle tempOutputArea = {
            consoleRect.x + 10,
            consoleRect.y + 10,
            consoleRect.width - 20,
            consoleRect.height - 58
        };

        if (strlen(consoleInputBuffer) > 0) {
            string input = consoleInputBuffer;
            consoleOutputLines.push_back("> " + input);

            vector<string> tokens;
            string currentToken = "";
            for (char ch : input) {
                if (ch == ' ') {
                    if (!currentToken.empty()) {
                        tokens.push_back(currentToken);
                        currentToken = "";
                    }
                }
                else currentToken += ch;
            }
            if (!currentToken.empty())
                tokens.push_back(currentToken);

            bool commandExecutedSuccessfully = false;
            if (!tokens.empty()) {
                string commandNameInput = tokens[0];
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
                    bool helpRequestedForThisCommand = false;
                    for (int i = 1; i < tokens.size(); i++) {
                        if (tokens[i] == "/?") {
                            helpRequestedForThisCommand = true;
                            break;
                        }
                    }

                    if (helpRequestedForThisCommand) {
                        string line;
                        int start = 0;
                        int end = foundCmdDef->helpMessage.find('\n');
                        while (end != string::npos) {
                            consoleOutputLines.push_back(foundCmdDef->helpMessage.substr(start, end - start));
                            start = end + 1;
                            end = foundCmdDef->helpMessage.find('\n', start);
                        }
                        consoleOutputLines.push_back(foundCmdDef->helpMessage.substr(start));
                        commandExecutedSuccessfully = true;
                    }
                    else {
                        if (foundCmdDef->name == "HELP") {
                            if (tokens.size() > 1) {
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
                                    string line_h;
                                    int start_h = 0;
                                    int end_h = targetCmdHelpDef->helpMessage.find('\n');
                                    while (end_h != string::npos) {
                                        consoleOutputLines.push_back(targetCmdHelpDef->helpMessage.substr(start_h, end_h - start_h));
                                        start_h = end_h + 1;
                                        end_h = targetCmdHelpDef->helpMessage.find('\n', start_h);
                                    }
                                    consoleOutputLines.push_back(targetCmdHelpDef->helpMessage.substr(start_h));
                                }
                                else {
                                    consoleOutputLines.push_back("No help topic found for '" + helpTargetName + "'.");
                                }
                            }
                            else {
                                consoleOutputLines.push_back("For more information on a specific command, type HELP [command-name].");
                                consoleOutputLines.push_back("");

                                for (const auto& cmdDefList : definedCommands)
                                    consoleOutputLines.push_back(cmdDefList.name);
                            }
                            commandExecutedSuccessfully = true;

                        }
                        else if (foundCmdDef->name == "CLEAR") {
                            consoleOutputLines.clear();
                            consoleScrollOffset = 0;
                            commandExecutedSuccessfully = true;
                        }
                        else if (foundCmdDef->name == "TASKLIST") {
                            consoleOutputLines.push_back("Image Name");
                            consoleOutputLines.push_back("=========================");

                            for (const auto& proc : runningProcesses)
                                consoleOutputLines.push_back(proc);

                            commandExecutedSuccessfully = true;
                        }
                        else if (foundCmdDef->name == "TASKKILL") {
                            string targetProcess = "";
                            bool argT = false;
                            bool argF = false;

                            for (int i = 1; i < tokens.size(); i++) {
                                string currentArg = tokens[i];
                                string normalizedArg = currentArg;
                                transform(normalizedArg.begin(), normalizedArg.end(), normalizedArg.begin(), ::toupper);
                                if (normalizedArg == "/T") argT = true;
                                else if (normalizedArg == "/F") argF = true;
                                else if (currentArg != "/?" && targetProcess.empty()) targetProcess = tokens[i];
                            }

                            if (targetProcess.empty()) {
                                consoleOutputLines.push_back("ERROR: The syntax of the command is incorrect.");
                                consoleOutputLines.push_back("Type TASKKILL /? for command syntax.");
                            }
                            else {
                                if (targetProcess == "helpy.ini") {
                                    auto it_helpy_check = find(runningProcesses.begin(), runningProcesses.end(), targetProcess);
                                    if (helpyKilled && it_helpy_check == runningProcesses.end())
                                        consoleOutputLines.push_back("You've already broken him...");
                                    else {
                                        if (argT || argF) {
                                            if (!helpyKilled) {
                                                helpyKilled = true;
                                                consoleOutputLines.push_back("SUCCESS: The process \"" + targetProcess + "\" has been terminated.");
                                                helpyCrackedCurrentPlayingFrame = 0;
                                                helpyCrackedAnimInternalTimer = 0;
                                                musicRestartKillTimer = 0;
                                                if (res.helpyCrackedGifImage.data)
                                                    UpdateTexture(res.helpyCrackedTexture, (char*)res.helpyCrackedGifImage.data);

                                                PlaySound(res.helpyCrackSound);
                                                if (IsMusicStreamPlaying(res.settingsMusic))
                                                    StopMusicStream(res.settingsMusic);

                                                runningProcesses.erase(it_helpy_check);
                                            }
                                        }
                                        else {
                                            consoleOutputLines.push_back("ERROR: Invalid argument/option - '" + (tokens.size() > 1 ? tokens[1] : "") + "'.");
                                            consoleOutputLines.push_back("Type \"TASKKILL /?\" for usage.");
                                        }
                                    }
                                }
                                else if (targetProcess == "fazbOS.exe") {
                                    auto it_fazbos = find(runningProcesses.begin(), runningProcesses.end(), targetProcess);
                                    if (it_fazbos == runningProcesses.end())
                                        consoleOutputLines.push_back("ERROR: The process \"" + targetProcess + "\" not found.");
                                    else {
                                        if (argF) {
                                            consoleOutputLines.push_back("SUCCESS: The process \"" + targetProcess + "\" has been terminated.");
                                            consoleOutputLines.push_back("...");
                                            consoleOutputLines.push_back("CRITICAL_PROCESS_DIED");
                                            runningProcesses.erase(it_fazbos);
                                            // TODO: EASTER EGG
                                        }
                                        else if (argT) {
                                            consoleOutputLines.push_back("ERROR: The process \"" + targetProcess + "\" could not be terminated (graceful termination not supported).");
                                            consoleOutputLines.push_back("Reason: This process can only be terminated forcefully (with /F).");
                                        }
                                        else {
                                            consoleOutputLines.push_back("ERROR: Invalid argument/option.");
                                            consoleOutputLines.push_back("Type \"TASKKILL /?\" for command syntax.");
                                        }
                                    }
                                }
                                else
                                    if (find(runningProcesses.begin(), runningProcesses.end(), targetProcess) == runningProcesses.end())
                                        consoleOutputLines.push_back("ERROR: The process \"" + targetProcess + "\" not found.");
                                    else
                                        consoleOutputLines.push_back("INFO: The process \"" + targetProcess + "\" cannot be terminated by this utility (not a recognized user process for termination).");

                            }
                            commandExecutedSuccessfully = true;

                        }
                    }
                }
                else consoleOutputLines.push_back("'" + commandNameInput + "' is not recognized as an internal or external command,\noperable program or batch file. Type HELP for a list of commands.");
            }

            memset(consoleInputBuffer, 0, consoleMaxChars + 1);
            consoleInputCursorPos = 0;

            int actualConsoleLinesToDraw = 0;
            if (tempOutputArea.height > 0)
                actualConsoleLinesToDraw = (tempOutputArea.height / 20);

            if (consoleOutputLines.size() > actualConsoleLinesToDraw)
                consoleScrollOffset = consoleOutputLines.size() - actualConsoleLinesToDraw;
            else consoleScrollOffset = 0;


        }
        else {
            consoleOutputLines.push_back(">");

            int actualConsoleLinesToDraw = 0;
            if (tempOutputArea.height > 0)
                actualConsoleLinesToDraw = (tempOutputArea.height / 20);

            if (consoleOutputLines.size() > actualConsoleLinesToDraw)
                consoleScrollOffset = consoleOutputLines.size() - actualConsoleLinesToDraw;
            else consoleScrollOffset = 0;
        }
    }

    float mouseWheelMove = GetMouseWheelMove();
    if (mouseWheelMove != 0) {
        consoleScrollOffset -= mouseWheelMove * 2;

        if (consoleScrollOffset < 0) consoleScrollOffset = 0;
        int maxPossibleScroll = 0;

        if (consoleOutputLines.size() > consoleLinesToDraw)
            maxPossibleScroll = consoleOutputLines.size() - consoleLinesToDraw;

        if (consoleScrollOffset > maxPossibleScroll)
            consoleScrollOffset = maxPossibleScroll;
    }


// --- Console Drawing ---
    DrawRectangleRec(consoleRect, Fade(DARKGRAY, 0.92));
    DrawRectangleLinesEx(consoleRect, 1, LIGHTGRAY);

    Rectangle inputBgArea = {
        consoleRect.x + 10,
        consoleRect.y + consoleRect.height - 40,
        consoleRect.width - 20,
        30
    };
    DrawRectangleRec(inputBgArea, Fade(BLACK, 0.75));

    Rectangle outputArea = {
        consoleRect.x + 10,
        consoleRect.y + 10,
        consoleRect.width - 20,
        inputBgArea.y - consoleRect.y - 18
    };

    float outputFontSize = 14;
    float lineHeight = 28; 
    int calculatedConsoleLinesToDraw = 0;
    if (outputArea.height > 0)
        calculatedConsoleLinesToDraw = (outputArea.height / lineHeight);
    
    int startLineToDraw = consoleScrollOffset;
    if (consoleOutputLines.empty() || consoleOutputLines.size() <= calculatedConsoleLinesToDraw) {
        startLineToDraw = 0;
        consoleScrollOffset = 0;
    }
    else {
        if (startLineToDraw < 0) startLineToDraw = 0;
        int maxScroll = consoleOutputLines.size() - calculatedConsoleLinesToDraw;
        if (startLineToDraw > maxScroll) startLineToDraw = maxScroll;
        consoleScrollOffset = startLineToDraw;
    }

    BeginScissorMode(outputArea.x, outputArea.y, outputArea.width, outputArea.height);
    for (int i = 0; i < consoleLinesToDraw; i++) {
        int currentLineInHistory = consoleScrollOffset + i;
        if (currentLineInHistory >= 0 && currentLineInHistory < consoleOutputLines.size())
            DrawTextEx(font, consoleOutputLines[currentLineInHistory].c_str(), { outputArea.x + 2, outputArea.y + 2 + i * lineHeight }, outputFontSize, 1, WHITE);
        else break;
    }
    EndScissorMode();

    float textInputY = inputBgArea.y + (inputBgArea.height - outputFontSize) / 2;
    DrawTextEx(font, TextFormat("> %s", consoleInputBuffer), { inputBgArea.x + 5, textInputY }, outputFontSize, 1, LIME);

    if (((int)(GetTime() * 3)) % 2 == 0) {
        char tempCursorBuffer[consoleMaxChars + 3];
        snprintf(tempCursorBuffer, sizeof(tempCursorBuffer), "> %.*s", consoleInputCursorPos, consoleInputBuffer);
        Vector2 cursorPosTextSize = MeasureTextEx(font, tempCursorBuffer, outputFontSize, 1);
        DrawRectangle(inputBgArea.x + 5 + cursorPosTextSize.x, textInputY - 1, 2, outputFontSize + 2, LIME);
    }
}

int main(void) {
    InitializeGameSettings();

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
    

    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(g_settings.screenWidth, g_settings.screenHeight, "FNaF Minigames - Loading...");
    InitAudioDevice();
    HideCursor();

    MainMenuResources mainMenuRes{};
    Font consoleFont = mainMenuRes.consolasFont;

    bool initialStaticLoadScreenReady = LoadInitialLoadingScreenResources(mainMenuRes);
    float loadingScreenMinVisibleTime = 2;
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
        Vector2 mousePos = GetMousePosition();

        if (IsKeyPressed(KEY_F11))
            ToggleFullscreen();

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
                        runMidnightMotorist(g_settings.quality, mainMenuRes.crtShader, g_settings.useCRTShader && mainMenuRes.shaderLoadedSuccessfully);
                        HideCursor();
                    }
                    else cout << "Failed to load Motorist resources. Returning to Main Menu." << endl;

                    UnloadMotoristResources(motoristRes);
                }
                else if (fadeTargetScreen == PLAYING_RAINBOW) {
                    RainbowLandGameResources rainbowRes = LoadRainbowLandResources(g_settings.quality);
                    if (CheckRainbowLandResourcesLoaded(rainbowRes)) {
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
                        if (helpyKilled) SetMusicPitch(mainMenuRes.settingsMusic, 0.4);
                        else SetMusicPitch(mainMenuRes.settingsMusic, 1);
                        
                        helpyCurrentAnimFrame = 0; 
                        helpyFrameTimer = 0;

                        if (helpyKilled && mainMenuRes.helpyCrackedGifLoaded) {
                            if (mainMenuRes.helpyCrackedAnimFrames > 0 &&
                                helpyCrackedCurrentPlayingFrame >= mainMenuRes.helpyCrackedAnimFrames - 1) {

                                int lastFrameIndex = mainMenuRes.helpyCrackedAnimFrames - 1;
                                helpyCrackedCurrentPlayingFrame = lastFrameIndex;

                                int frameDataSize = GetPixelDataSize(mainMenuRes.helpyCrackedGifImage.width, mainMenuRes.helpyCrackedGifImage.height, mainMenuRes.helpyCrackedGifImage.format);
                                char* frameDataOffset = (char*)mainMenuRes.helpyCrackedGifImage.data + (lastFrameIndex * frameDataSize);
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
                    UpdateMainMenuScreen(mainMenuRes, mousePos, currentScreen, previousScreen, fadeTargetScreen, isFadingOut, fadeAlpha, dt, mainMenuBgCurrentFrame, mainMenuBgFrameTimer, mainMenuBgFrameDelay);
                } break;
                case SETTINGS: {
                    UpdateSettingsScreen(mainMenuRes, mousePos, currentScreen, previousScreen, fadeTargetScreen, isFadingOut, fadeAlpha, dt, helpyCurrentAnimFrame, helpyFrameTimer, helpyFrameDelay);
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
            if (mainMenuRes.helpyLoadingScreenTexture.id > 0) {
                float imgDrawWidth = mainMenuRes.helpyLoadingScreenTexture.width;
                float imgDrawHeight = mainMenuRes.helpyLoadingScreenTexture.height;
                DrawTexture(mainMenuRes.helpyLoadingScreenTexture, (GetScreenWidth() - imgDrawWidth) / 2, (GetScreenHeight() - imgDrawHeight - 120) / 2, WHITE);
            }
            else DrawText("Loading...", (GetScreenWidth() - MeasureText("Loading...", 40)) / 2, (GetScreenHeight() - 40) / 2, 40, LIGHTGRAY);
        }
        else if (mainMenuRes.targetRenderTexture.id > 0) {
            BeginTextureMode(mainMenuRes.targetRenderTexture);
            ClearBackground(BLANK);

            GameScreen screenToDrawForContent = currentScreen;
            if (isFadingOut) screenToDrawForContent = previousScreen;
            switch (screenToDrawForContent) {
            case MAIN_MENU:
                DrawMainMenuScreen(mainMenuRes, mousePos, mainMenuBgCurrentFrame, currentScreen, previousScreen, fadeTargetScreen, isFadingOut, fadeAlpha);
                break;
            case SETTINGS: {
                DrawSettingsScreen(mainMenuRes, mousePos, helpyCurrentAnimFrame, currentScreen, previousScreen, fadeTargetScreen, isFadingOut, fadeAlpha);
                if (consoleActive) UpdateDrawConsole(mainMenuRes, consoleFont, mousePos, dt);
            } break;
            case PLAYING_MOTORIST:
            case PLAYING_RAINBOW:
                ClearBackground(BLACK);
                DrawText("IN GAME...", (logicalWidth - MeasureText("IN GAME...", 20)) / 2, (logicalHeight - 20) / 2, 20, WHITE);
                break;
            default: break;
            }
            EndTextureMode();

            float finalScale = fminf(GetScreenWidth() / logicalWidth, GetScreenHeight() / logicalHeight);
            float finalOffsetX = (GetScreenWidth() - (logicalWidth * finalScale)) * 0.5;
            float finalOffsetY = (GetScreenHeight() - (logicalHeight * finalScale)) * 0.5;
            Rectangle srcRect = { 0, 0, mainMenuRes.targetRenderTexture.texture.width, -mainMenuRes.targetRenderTexture.texture.height };
            Rectangle destRect = { finalOffsetX, finalOffsetY, logicalWidth * finalScale, logicalHeight * finalScale };
            DrawTexturePro(mainMenuRes.targetRenderTexture.texture, srcRect, destRect, { 0, 0 }, 0, WHITE);

        }
        else if (currentScreen != LOADING) DrawText("Error: Render Target Failed", 100, 100, 20, RED);

        if ((isFadingOut || isFadingIn) && fadeAlpha > 0)
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, fadeAlpha));

        if (mainMenuRes.cursor.id > 0 && !consoleActive && (currentScreen == MAIN_MENU || currentScreen == SETTINGS))
            DrawTexture(mainMenuRes.cursor, mousePos.x, mousePos.y, WHITE);

        //DrawFPS(10, 10);
        EndDrawing();
    }

    cout << "CLEANUP: Starting main cleanup..." << endl;

    UnloadMainMenuResources(mainMenuRes);
    CloseAudioDevice();
    CloseWindow();

    cout << "CLEANUP: Finished. Exiting application." << endl;

    return 0;
}