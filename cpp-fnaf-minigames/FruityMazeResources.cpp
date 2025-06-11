#include "FruityMazeResources.h"

void InitializeAnimation(AnimationData& anim, const char* spritesheetPath,
    int frameCount, int frameWidth, int frameHeight,
    float frameTime, bool loop, int maxLoops = 0) {
    anim.spriteSheet = LoadTexture(spritesheetPath);
    anim.frameCount = frameCount;
    anim.frameWidth = frameWidth;
    anim.frameHeight = frameHeight;
    anim.frameTime = frameTime;
    anim.currentTime = 0;
    anim.currentFrame = 0;
    anim.loop = loop;
    anim.isPlaying = false;
    anim.loopCount = 0;
    anim.maxLoops = maxLoops;

    TraceLog(LOG_DEBUG, TextFormat("Animation loaded: %s (%dx%d, %d frames)", spritesheetPath, frameWidth, frameHeight, frameCount));
}

void UpdateAnimation(AnimationData& anim, float deltaTime) {
    if (!anim.isPlaying || anim.frameCount <= 1) return;

    anim.currentTime += deltaTime;

    if (anim.currentTime >= anim.frameTime) {
        anim.currentTime -= anim.frameTime;
        anim.currentFrame++;

        if (anim.currentFrame >= anim.frameCount) {
            bool shouldLoop = anim.loop;

            if (anim.maxLoops > 0) {
                anim.loopCount++;

                if (anim.loopCount >= anim.maxLoops) shouldLoop = false;
            }

            if (shouldLoop) anim.currentFrame = 0;
            else {
                anim.currentFrame = anim.frameCount - 1;
                anim.isPlaying = false;
            }
        }
    }
}

void StartAnimation(AnimationData& anim) {
    anim.isPlaying = true;
    anim.currentTime = 0;
    anim.currentFrame = 0;
    anim.loopCount = 0;
}

void StopAnimation(AnimationData& anim) {
    anim.isPlaying = false;
    anim.currentTime = 0;
    anim.currentFrame = 0;
}

Rectangle GetCurrentAnimationFrame(const AnimationData& anim) {
    return { 
        anim.currentFrame * anim.frameWidth, 0,
        anim.frameWidth, anim.frameHeight 
    };
}

FruityMazeGameResources LoadFruityMazeResources(GraphicsQuality quality) {
    TraceLog(LOG_DEBUG, "Loading Fruity Maze resources...");
    FruityMazeGameResources resources = {};

    // Initialize default values
    Vector3 cubicMapSize = { 1, 4, 1 };
    resources.mapPixels = nullptr;
    resources.mapWidth = 0;
    resources.mapHeight = 0;
    int wallPixelCount = 0;
    int pathPixelCount = 0;

    // Load map image
    Image mapImage = LoadImage("resources/fm/map.png");
    if (mapImage.data) {
        resources.mapWidth = mapImage.width;
        resources.mapHeight = mapImage.height;

        ImageFormat(&mapImage, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

        // Process map for walls and paths
        for (int x = 0; x < mapImage.width; x++) {
            for (int y = 0; y < mapImage.height; y++) {
                Color pixelColor = GetImageColor(mapImage, x, y);
                bool isWall = (pixelColor.r <= 50 && pixelColor.g >= 200 && pixelColor.b <= 50 && pixelColor.a >= 200);

                if (isWall) {
                    ImageDrawPixel(&mapImage, x, y, WHITE);
                    wallPixelCount++;
                }
                else {
                    ImageDrawPixel(&mapImage, x, y, BLACK);
                    pathPixelCount++;
                }
            }
        }

        // Generate 3D mesh from map
        if (wallPixelCount > 0) {
            Mesh mesh = GenMeshCubicmap(mapImage, cubicMapSize);
            resources.mazeModel = LoadModelFromMesh(mesh);
        }

        resources.mapPixels = LoadImageColors(mapImage);
        resources.mapModelPosition = { -mapImage.width / 2.0f, 0, -mapImage.height / 2.0f };

        UnloadImage(mapImage);
        TraceLog(LOG_DEBUG, TextFormat("Map loaded: %dx%d, %d walls, %d paths", resources.mapWidth, resources.mapHeight, wallPixelCount, pathPixelCount));
    }
    else return resources;

    // Load shader
    resources.lightingShader = LoadShader("lighting.vs", "lighting.fs");
    if (resources.lightingShader.id) {
        resources.lightingShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(resources.lightingShader, "matModel");
        resources.lightingShader.locs[SHADER_LOC_MATRIX_NORMAL] = GetShaderLocation(resources.lightingShader, "matNormal");
    }

    // Load minimap & wall texture
    resources.minimapTexture = LoadTexture("resources/fm/minimap.png");
    resources.cubeWallTexture = LoadTexture("resources/fm/cube.png");

    // Apply wall texture to maze
    if (resources.cubeWallTexture.id && resources.mazeModel.materials)
        SetMaterialTexture(&resources.mazeModel.materials[0], MATERIAL_MAP_DIFFUSE, resources.cubeWallTexture);

    InitializeAnimation(resources.amazingAnim, "resources/fm/amazing.png", 4, 1000, 300, 0.05, true, 9);
    InitializeAnimation(resources.timeIsUpAnim, "resources/fm/timeIsUp.png", 4, 1000, 300, 0.05, true, 9);
    InitializeAnimation(resources.timeExtendedAnim, "resources/fm/timeExtended.png", 3, 1000, 300, 0.05, true, 7);

    resources.outOfBounds = LoadTexture("resources/fm/outOfBounds.png");

    // Load 3D models
    resources.playerModel = LoadModel("resources/fm/freddy.glb");

    resources.cherryModel = LoadModel("resources/fm/cherry.glb");
    resources.orangeModel = LoadModel("resources/fm/orange.glb");
    resources.grapesModel = LoadModel("resources/fm/grapes.glb");

    resources.lightningModel = LoadModel("resources/fm/lightning.glb");
    resources.gummybearModel = LoadModel("resources/fm/gummybear.glb");
    resources.magnetModel = LoadModel("resources/fm/magnet.glb");

    // Load sounds
    resources.backgroundMusic = LoadMusicStream("resources/fm/fbttl.mp3");

    resources.fruit1Sound = LoadSound("resources/fm/fruit1.wav");
    resources.fruit2Sound = LoadSound("resources/fm/fruit2.wav");
    resources.fruit3Sound = LoadSound("resources/fm/fruit3.wav");

    resources.boundsSound = LoadSound("resources/fm/bounds.wav");
    resources.powerUpSound = LoadSound("resources/fm/powerUp.wav");
    resources.timerPingSound = LoadSound("resources/fm/timerPing.wav");
    resources.timeExtendedSound = LoadSound("resources/fm/timeExt.wav");

    TraceLog(LOG_DEBUG, "Fruity Maze resources loaded successfully.");
    return resources;
}

void UnloadFruityMazeResources(FruityMazeGameResources& resources) {
    TraceLog(LOG_DEBUG, "Unloading Fruity Maze resources...");

    // Unload textures
    if (resources.mapPixels) UnloadImageColors(resources.mapPixels);
    if (resources.cubeWallTexture.id > 0) UnloadTexture(resources.cubeWallTexture);
    if (resources.minimapTexture.id > 0) UnloadTexture(resources.minimapTexture);
    if (resources.amazingAnim.spriteSheet.id > 0) UnloadTexture(resources.amazingAnim.spriteSheet);
    if (resources.timeIsUpAnim.spriteSheet.id > 0) UnloadTexture(resources.timeIsUpAnim.spriteSheet);
    if (resources.timeExtendedAnim.spriteSheet.id > 0) UnloadTexture(resources.timeExtendedAnim.spriteSheet);
    if (resources.outOfBounds.id > 0) UnloadTexture(resources.outOfBounds);

    // Unload models
    if (resources.mazeModel.meshCount > 0) UnloadModel(resources.mazeModel);
    if (resources.playerModel.meshCount > 0) UnloadModel(resources.playerModel);
    if (resources.cherryModel.meshCount > 0) UnloadModel(resources.cherryModel);
    if (resources.orangeModel.meshCount > 0) UnloadModel(resources.orangeModel);
    if (resources.grapesModel.meshCount > 0) UnloadModel(resources.grapesModel);
    if (resources.lightningModel.meshCount > 0) UnloadModel(resources.lightningModel);
    if (resources.gummybearModel.meshCount > 0) UnloadModel(resources.gummybearModel);
    if (resources.magnetModel.meshCount > 0) UnloadModel(resources.magnetModel);

    // Unload sounds with safety checks
    if (resources.backgroundMusic.stream.buffer) UnloadMusicStream(resources.backgroundMusic);
    if (resources.fruit1Sound.frameCount > 0) UnloadSound(resources.fruit1Sound);
    if (resources.fruit2Sound.frameCount > 0) UnloadSound(resources.fruit2Sound);
    if (resources.fruit3Sound.frameCount > 0) UnloadSound(resources.fruit3Sound);
    if (resources.boundsSound.frameCount > 0) UnloadSound(resources.boundsSound);
    if (resources.powerUpSound.frameCount > 0) UnloadSound(resources.powerUpSound);
    if (resources.timerPingSound.frameCount > 0) UnloadSound(resources.timerPingSound);
    if (resources.timeExtendedSound.frameCount > 0) UnloadSound(resources.timeExtendedSound);

    // Unload shader
    if (resources.lightingShader.id > 0) UnloadShader(resources.lightingShader);

    TraceLog(LOG_DEBUG, "Fruity Maze resources unloaded successfully.");
}

bool CheckFruityMazeResourcesLoaded(FruityMazeGameResources& res) {
    vector<string> missingResources;

    if (!res.mapPixels) missingResources.push_back("mapPixels");
    if (res.mapWidth == 0 || res.mapHeight == 0) missingResources.push_back("mapDimensions");
    if (!res.cubeWallTexture.id) missingResources.push_back("cubeWallTexture");
    if (!res.minimapTexture.id) missingResources.push_back("minimapTexture");

    if (!res.amazingAnim.spriteSheet.id) missingResources.push_back("amazingAnim");
    if (!res.timeIsUpAnim.spriteSheet.id) missingResources.push_back("timeIsUpAnim");
    if (!res.timeExtendedAnim.spriteSheet.id) missingResources.push_back("timeExtendedAnim");
    if (!res.outOfBounds.id) missingResources.push_back("outOfBounds");

    if (!res.playerModel.meshes) missingResources.push_back("playerModel");
    if (!res.cherryModel.meshes) missingResources.push_back("cherryModel");
    if (!res.orangeModel.meshes) missingResources.push_back("orangeModel");
    if (!res.grapesModel.meshes) missingResources.push_back("grapesModel");
    if (!res.lightningModel.meshes) missingResources.push_back("lightningModel");
    if (!res.gummybearModel.meshes) missingResources.push_back("gummybearModel");
    if (!res.magnetModel.meshes) missingResources.push_back("magnetModel");

    if (!res.backgroundMusic.stream.buffer) missingResources.push_back("backgroundMusic");
    if (!res.fruit1Sound.frameCount) missingResources.push_back("fruit1Sound");
    if (!res.fruit2Sound.frameCount) missingResources.push_back("fruit2Sound");
    if (!res.fruit3Sound.frameCount) missingResources.push_back("fruit3Sound");
    if (!res.boundsSound.frameCount) missingResources.push_back("boundsSound");
    if (!res.powerUpSound.frameCount) missingResources.push_back("powerUpSound");
    if (!res.timerPingSound.frameCount) missingResources.push_back("timerPingSound");
    if (!res.timeExtendedSound.frameCount) missingResources.push_back("timeExtendedSound");

    if (!res.lightingShader.id) missingResources.push_back("lightingShader");

    if (!missingResources.empty()) {
        TraceLog(LOG_DEBUG, "FRUITY MAZE: Critical error - failed to load resources:");
        for (const string& resourceName : missingResources)
            TraceLog(LOG_DEBUG, "- %s", resourceName);

        return false;
    }

    return true;
}