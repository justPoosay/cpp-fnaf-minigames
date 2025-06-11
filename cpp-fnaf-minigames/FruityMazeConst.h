#pragma once

using namespace std;

// --- SCREEN CONSTANTS ---
const int virtualScreenWidth = 1280;
const int virtualScreenHeight = 720;

// --- ADVANCED ENUMS ---
typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_NORMAL
} DifficultyLevel;

typedef enum {
    VIEW_CAMERA_FIRST_PERSON,
    VIEW_CAMERA_SECOND_PERSON,
    VIEW_CAMERA_THIRD_PERSON
} ViewCameraMode;

typedef enum {
    PROP_CHERRY,
    PROP_ORANGE,
    PROP_GRAPES,
    PROP_LIGHTNING,
    PROP_GUMMYBEAR,
    PROP_MAGNET
} PropType;

typedef enum {
    KEY_ACTION_NONE,
    KEY_ACTION_EXIT,
    KEY_ACTION_CAMERA_SWITCH,
    KEY_ACTION_DEBUG_TOGGLE,
    KEY_ACTION_SHADER_RELOAD,
    KEY_ACTION_DEBUG_LIGHTNING,
    KEY_ACTION_DEBUG_GUMMYBEAR,
    KEY_ACTION_DEBUG_MAGNET,
    KEY_ACTION_MINIMAP_EXPAND
} KeyAction;

typedef enum {
    GAME_PHASE_INITIALIZING,
    GAME_PHASE_SPAWNING,
    GAME_PHASE_PLAYING,
    GAME_PHASE_PAUSED,
    GAME_PHASE_GAME_OVER,
    GAME_PHASE_CLEANUP
} GamePhase;

typedef enum {
    PROP_CATEGORY_FRUIT,
    PROP_CATEGORY_POWERUP,
    PROP_CATEGORY_UNKNOWN
} PropCategory;

typedef enum {
    POWERUP_TYPE_LIGHTNING,
    POWERUP_TYPE_GUMMYBEAR,
    POWERUP_TYPE_MAGNET
} PowerUpType;

typedef enum {
    MOVE_DIRECTION_NONE = 0,
    MOVE_DIRECTION_FORWARD = 1 << 0,  // 1
    MOVE_DIRECTION_BACKWARD = 1 << 1,  // 2
    MOVE_DIRECTION_LEFT = 1 << 2,  // 4
    MOVE_DIRECTION_RIGHT = 1 << 3   // 8
} MovementDirection;

typedef enum {
    UI_ELEMENT_TIMER,
    UI_ELEMENT_SCORE,
    UI_ELEMENT_POWERUP_TIMERS,
    UI_ELEMENT_DEBUG_INFO,
    UI_ELEMENT_GAME_OVER,
    UI_ELEMENT_CONTROLS_HELP
} UIElement;

// --- GAME STRUCTS ---
struct PowerUpState {
    bool lightningActive;
    float lightningTimeLeft;
    bool gummybearActive;
    float gummybearTimeLeft;
    bool magnetActive;
    float magnetTimeLeft;
};

struct GameState {
    float gameTimer;
    int score;
    bool gameOver;
    float gameOverTimer;
    bool outOfBounds;
    int lastTickSecond;
    bool temporaryNoclip;
    bool gameWon;
};

struct TextState {
    bool showAmazing;
    float amazingTimer;
    bool showTimeIsUp;
    float timeIsUpTimer;
    bool showTimeExtended;
    float timeExtendedTimer;
    bool showOutOfBounds;
};

struct Prop {
    Vector3 position;
    PropType type;
    float rotationY;
    float bobOffset;
    bool collected;
    bool beingAttracted;
};

// --- ADVANCED STRUCTURES ---
struct PlayerMovement {
    Vector3 position;
    float rotationAngle;
    float currentSpeed;
    int movementFlags;
    bool isNoclipping;
    bool canMove;
};

struct CameraConfig {
    ViewCameraMode currentMode;
    float fovy;
    Vector3 position;
    Vector3 target;
    Vector3 up;
    bool mouseLookEnabled;
};

struct InputState {
    int pressedKey;
    KeyAction currentAction;
    Vector2 mouseDelta;
    bool exitRequested;
    bool debugKeysEnabled;
};

struct MinimapConfig {
    bool isVisible;
    bool isExpanded;
    float scale;
    float expandedScale;
    Vector2 position;
    Vector2 size;
    Color borderColor;
    Color playerColor;
    Color propColor;
};

struct RenderingConfig {
    bool shaderEnabled;
    bool debugMode;
    bool drawPlayer;
    bool drawFPS;
    float lightIntensity;
    Vector3 lightColor;
    float ambientStrength;
};

struct SoundConfig {
    float musicVolume;
    float sfxVolume;
    bool musicEnabled;
    bool sfxEnabled;
    bool musicLooping;
};

struct DebugConfig {
    bool enabled;
    bool showCollisionBoxes;
    bool showPlayerRay;
    bool showMagnetRange;
    bool showPositionInfo;
    bool allowCheats;
};

struct PropRenderInfo {
    PropType type;
    PropCategory category;
    Model* model;
    Color fallbackColor;
    float scale;
    bool shouldBob;
};

struct TimerConfig {
    float initialTime;
    float warningThreshold;
    float bonusInterval;
    float bonusAmount;
};

struct CollisionConfig {
    float playerHitboxWidth;
    float playerHitboxDepth;
    float propCollisionRadius;
    int wallCheckRange;         // How many cells to check around player
};

struct GameBalance {
    TimerConfig timer;
    CollisionConfig collision;
    float playerBaseSpeed;
    float lightningSpeedMultiplier;
    float magnetRange;
    float magnetSpeed;
    int scorePerFruit;
};

struct UILayout {
    float textPadding;
    float largeFontSize;
    float mediumFontSize;
    float smallFontSize;
    Vector2 timerPosition;
    Vector2 scorePosition;
    Vector2 powerupPosition;
};

// --- CONSTANTS GROUPED BY CATEGORY ---
// 
// **Cam Offsets**
const Vector3 FirstPersonOffset = { 0, 2.25, 0.1 };
const Vector3 SecondPersonOffset = { 0, 20, 0 };
const Vector3 ThirdPersonOffset = { 0, 14, -10 };

// **Player**
const float playerMoveSpeed = 17.5;
const float playerScale = 2;
const float collisionRadius = 2;
const float playerRotationSpeed = 180;
const float mouseSensitivity = 0.2;
const float playerHitboxWidth = 3.6;
const float playerHitboxDepth = 2.4;

// **UI**
const float UI_LARGE_FONT_SIZE = 48;
const float UI_MEDIUM_FONT_SIZE = 32;
const float UI_SMALL_FONT_SIZE = 24;
const float textPadding = 45;
const float minimapScale = 2;
const float gifScale = 1;

// **Power-ups**
const float lightningSpeedMultiplier = 2;
const float lightningDuration = 30;
const float gummybearDuration = 14;
const float magnetDuration = 23;
const float magnetRange = 12;
const float magnetPropSpeed = playerMoveSpeed * 1.2;

const float propScale = 1.2;
const float propRotationSpeed = 45;
const float propBobSpeed = 2;
const float propBobHeight = 0.25;
const float propCollectionRadius = 3;

// --- CONFIGURATION HELPERS ---
static inline GameBalance GetDefaultGameBalance() {
    GameBalance config;
    config.timer.initialTime = 60;
    config.timer.warningThreshold = 10;
    config.timer.bonusInterval = 75;
    config.timer.bonusAmount = 5;

    config.collision.playerHitboxWidth = playerHitboxWidth;
    config.collision.playerHitboxDepth = playerHitboxDepth;
    config.collision.propCollisionRadius = collisionRadius;
    config.collision.wallCheckRange = 2;

    config.playerBaseSpeed = playerMoveSpeed;
    config.lightningSpeedMultiplier = lightningSpeedMultiplier;
    config.magnetRange = magnetRange;
    config.magnetSpeed = magnetPropSpeed;
    config.scorePerFruit = 5;

    return config;
}

static inline MinimapConfig GetDefaultMinimapConfig() {
    MinimapConfig config;
    config.isVisible = true;
    config.isExpanded = false;
    config.scale = minimapScale;
    config.expandedScale = 4;
    config.position = { virtualScreenWidth - 200, 10 };
    config.size = { 180, 180 };
    config.borderColor = LIME;
    config.playerColor = RED;
    config.propColor = YELLOW;
    return config;
}

static inline RenderingConfig GetDefaultRenderingConfig() {
    RenderingConfig config;
    config.shaderEnabled = true;
    config.debugMode = false;
    config.drawPlayer = true;
    config.drawFPS = false;
    config.lightIntensity = 1;
    config.lightColor = { 1, 0.9, 0.8 };
    config.ambientStrength = 0.3;
    return config;
}

static inline SoundConfig GetDefaultSoundConfig() {
    SoundConfig config;
    config.musicVolume = 1;
    config.sfxVolume = 1;
    config.musicEnabled = true;
    config.sfxEnabled = true;
    config.musicLooping = true;
    return config;
}

static inline DebugConfig GetDefaultDebugConfig() {
    DebugConfig config;
    config.enabled = false;
    config.showCollisionBoxes = true;
    config.showPlayerRay = true;
    config.showMagnetRange = true;
    config.showPositionInfo = true;
    config.allowCheats = true;
    return config;
}

static inline UILayout GetDefaultUILayout() {
    UILayout config;
    config.textPadding = textPadding;
    config.largeFontSize = UI_LARGE_FONT_SIZE;
    config.mediumFontSize = UI_MEDIUM_FONT_SIZE;
    config.smallFontSize = UI_SMALL_FONT_SIZE;
    config.timerPosition = { textPadding, textPadding };
    config.scorePosition = { virtualScreenWidth - 200, virtualScreenHeight - 120 };
    config.powerupPosition = { virtualScreenWidth - 100, textPadding };
    return config;
}

// --- UTILITY FUNCTIONS ---

static inline PropCategory GetPropCategory(PropType type) {
    switch (type) {
        case PROP_CHERRY:
        case PROP_ORANGE:
        case PROP_GRAPES:
            return PROP_CATEGORY_FRUIT;

        case PROP_LIGHTNING:
        case PROP_GUMMYBEAR:
        case PROP_MAGNET:
            return PROP_CATEGORY_POWERUP;

        default:
            return PROP_CATEGORY_UNKNOWN;
    }
}

static inline PowerUpType PropTypeToPowerUpType(PropType propType) {
    switch (propType) {
        case PROP_LIGHTNING: return POWERUP_TYPE_LIGHTNING;
        case PROP_GUMMYBEAR: return POWERUP_TYPE_GUMMYBEAR;
        case PROP_MAGNET: return POWERUP_TYPE_MAGNET;
        default: return POWERUP_TYPE_LIGHTNING;
    }
}

static inline bool HasMovementFlag(int flags, MovementDirection direction) {
    return (flags & direction) != 0;
}

static inline int AddMovementFlag(int flags, MovementDirection direction) {
    return flags | direction;
}

static inline int RemoveMovementFlag(int flags, MovementDirection direction) {
    return flags & ~direction;
}

static inline KeyAction GetKeyAction(int key, bool debugEnabled) {
    switch (key) {
        case KEY_ESCAPE: return KEY_ACTION_EXIT;
        case KEY_T: return KEY_ACTION_CAMERA_SWITCH;
        case KEY_F3: return KEY_ACTION_DEBUG_TOGGLE;
        case KEY_F5: return KEY_ACTION_SHADER_RELOAD;
        case KEY_TAB: return KEY_ACTION_MINIMAP_EXPAND;

        case KEY_L: return debugEnabled ? KEY_ACTION_DEBUG_LIGHTNING : KEY_ACTION_NONE;
        case KEY_G: return debugEnabled ? KEY_ACTION_DEBUG_GUMMYBEAR : KEY_ACTION_NONE;
        case KEY_M: return debugEnabled ? KEY_ACTION_DEBUG_MAGNET : KEY_ACTION_NONE;

        default: return KEY_ACTION_NONE;
    }
}

static inline Color GetPropFallbackColor(PropType type) {
    switch (type) {
        case PROP_CHERRY: return RED;
        case PROP_ORANGE: return ORANGE;
        case PROP_GRAPES: return PURPLE;
        case PROP_LIGHTNING: return YELLOW;
        case PROP_GUMMYBEAR: return GREEN;
        case PROP_MAGNET: return GRAY;
        default: return WHITE;
    }
}

// Fruit spawn cords (179x168)
const Vector2 FRUIT_POSITIONS[] = {
    // Row 1
    /*{4, 5}, */{13, 5}, {22, 5}, {32, 5}, {41, 5}, {49, 5}, {58, 5}, {67, 5}, {76, 5}, {85, 5}, {94, 5}, {103, 5}, {112, 5}, {121, 5}, {130, 5}, {139, 5}, {148, 5}, {156, 5}, {165, 5}, {174, 5},

    // Row 2
    {4, 13}, {13, 13}, {22, 13}, {32, 13}, {41, 13}, {49, 13}, {58, 13}, {67, 13}, {76, 13}, {85, 13}, {94, 13}, {103, 13}, {112, 13}, {121, 13}, {130, 13}, {139, 13}, {148, 13}, {156, 13}, {165, 13}, {174, 13},

    // Row 3
    {4, 21}, {13, 21}, {22, 21}, {32, 21}, {41, 21}, {49, 21}, {58, 21}, {67, 21}, {76, 21}, {85, 21}, {94, 21}, {103, 21}, {112, 21}, {121, 21}, {130, 21}, {139, 21}, {148, 21}, {156, 21}, {165, 21}, {174, 21},

    // Row 4
    {4, 30}, {13, 30}, {22, 30}, {32, 30}, {41, 30}, {49, 30}, {58, 30}, {67, 30}, {76, 30}, {85, 30}, {94, 30}, {103, 30}, {112, 30}, {121, 30}, {130, 30}, {139, 30}, {148, 30}, {156, 30}, {165, 30}, {174, 30},

    // Row 5
    {4, 38}, {13, 38}, {22, 38}, {32, 38}, {41, 38}, {49, 38}, {58, 38}, {67, 38}, {76, 38}, {85, 38}, {94, 38}, {103, 38}, /*{112, 38}, */{121, 38}, {130, 38}, {139, 38}, {148, 38}, {156, 38}, {165, 38}, {174, 38},

    // Row 6
    {4, 46}, {13, 46}, {22, 46}, /*{32, 46}, */{41, 46}, {49, 46}, {58, 46}, {67, 46}, {76, 46}, {85, 46}, {94, 46}, {103, 46}, {112, 46}, {121, 46}, {130, 46}, {139, 46}, {148, 46}, /*{156, 46}, */{165, 46}, {174, 46},

    // Row 7
    {4, 55}, {13, 55}, {22, 55}, {32, 55}, {41, 55}, {49, 55}, {58, 55}, {67, 55}, {76, 55}, {85, 55}, {94, 55}, {103, 55}, {112, 55}, {121, 55}, {130, 55}, {139, 55}, {148, 55}, {156, 55}, {165, 55}, {174, 55},

    // Row 8
    {4, 63}, {13, 63}, {22, 63}, {32, 63}, {41, 63}, {49, 63}, {58, 63}, {67, 63}, {76, 63}, {85, 63}, {94, 63}, {103, 63}, {112, 63}, {121, 63}, {130, 63}, {139, 63}, {148, 63}, {156, 63}, {165, 63}, {174, 63},

    // Row 9
    {4, 71}, {13, 71}, {22, 71}, {32, 71}, {41, 71}, {49, 71}, {58, 71}, {67, 71}, {76, 71}, {85, 71}, {94, 71}, {103, 71}, {112, 71}, {121, 71}, {130, 71}, {139, 71}, {148, 71}, {156, 71}, {165, 71}, {174, 71},

    // Row 10
    {4, 79}, {13, 79}, {22, 79}, {32, 79}, {41, 79}, {49, 79}, {58, 79}, {67, 79}, {76, 79}, {85, 79}, {94, 79}, {103, 79}, {112, 79}, {121, 79}, /*{130, 79}, */{139, 79}, {148, 79}, {156, 79}, {165, 79}, {174, 79},

    // Row 11
    {4, 88}, {13, 88}, {22, 88}, {32, 88}, {41, 88}, {49, 88}, {58, 88}, {67, 88}, {76, 88}, {85, 88}, {94, 88}, {103, 88}, {112, 88}, {121, 88}, {130, 88}, {139, 88}, {148, 88}, {156, 88}, {165, 88}, {174, 88},

    // Row 12
    {4, 96}, {13, 96}, {22, 96}, {32, 96}, {41, 96}, {49, 96}, {58, 96}, {67, 96}, {76, 96}, {85, 96}, {94, 96}, {103, 96}, {112, 96}, {121, 96}, {130, 96}, {139, 96}, {148, 96}, {156, 96}, {165, 96}, {174, 96},

    // Row 13
    {4, 104}, {13, 104}, {22, 104}, {32, 104}, {41, 104}, {49, 104}, {58, 104}, {67, 104}, {76, 104}, {85, 104}, {94, 104}, {103, 104}, {112, 104}, {121, 104}, {130, 104}, {139, 104}, {148, 104}, {156, 104}, {165, 104}, {174, 104},

    // Row 14
    {4, 113}, {13, 113}, {22, 113}, {32, 113}, {41, 113}, {49, 113}, {58, 113}, {67, 113}, {76, 113}, {85, 113}, {94, 113}, {103, 113}, {112, 113}, {121, 113}, {130, 113}, {139, 113}, {148, 113}, {156, 113}, {165, 113}, {174, 113},

    // Row 15
    {4, 121}, {13, 121}, {22, 121}, {32, 121}, {41, 121}, {49, 121}, {58, 121}, {67, 121}, {76, 121}, {85, 121}, {94, 121}, {103, 121}, {112, 121}, {121, 121}, {130, 121}, {139, 121}, {148, 121}, {156, 121}, {165, 121}, {174, 121},

    // Row 16
    {4, 130}, {13, 130}, {22, 130}, {32, 130}, {41, 130}, {49, 130}, {58, 130}, {67, 130}, {76, 130}, {85, 130}, {94, 130}, {103, 130}, {112, 130}, {121, 130}, {130, 130}, {139, 130}, {148, 130}, {156, 130}, {165, 130}, {174, 130},

    // Row 17
    {4, 138}, {13, 138}, {22, 138}, {32, 138}, /*{41, 138}, */{49, 138}, {58, 138}, {67, 138}, {76, 138}, {85, 138}, {94, 138}, {103, 138}, {112, 138}, {121, 138}, {130, 138}, {139, 138}, {148, 138}, {156, 138}, {165, 138}, {174, 138},

    // Row 18
    {4, 146}, {13, 146}, {22, 146}, {32, 146}, {41, 146}, {49, 146}, {58, 146}, {67, 146}, {76, 146}, {85, 146}, {94, 146}, {103, 146}, {112, 146}, {121, 146}, {130, 146}, {139, 146}, {148, 146}, {156, 146}, {165, 146}, {174, 146},

    // Row 19
    {4, 155}, {13, 155}, {22, 155}, {32, 155}, {41, 155}, {49, 155}, {58, 155}, {67, 155}, {76, 155}, {85, 155}, {94, 155}, {103, 155}, {112, 155}, {121, 155}, {130, 155}, {139, 155}, {148, 155}, /*{156, 155}, */{165, 155}, {174, 155},

    // Row 20
    /*{4, 163}, */{13, 163}, {22, 163}, {32, 163}, {41, 163}, {49, 163}, {58, 163}, {67, 163}, {76, 163}, {85, 163}, {94, 163}, {103, 163}, {112, 163}, {121, 163}, {130, 163}, {139, 163}, {148, 163}, {156, 163}, {165, 163}, {174, 163},
};

const Vector2 POWERUP_POSITIONS[] = {
    {4, 5},
    {112, 38},
    {32, 46}, {156, 46},
    {130, 79},
    {41, 138},
    {156, 155},
    {4, 163}
};