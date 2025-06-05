#include "FruityMazeResources.h"

FruityMazeGameResources LoadFruityMazeResources(GraphicsQuality quality) {
    cout << "Loading Fruity Maze resources..." << endl;
    FruityMazeGameResources resources = {};

    // Initialize default values
    resources.lightingShaderLoaded = false;
    resources.mapLoaded = false;
    resources.playerModelLoaded = false;
    resources.wallTextureLoaded = false;
    resources.backgroundMusicLoaded = false;
    resources.fruit1SoundLoaded = false;
    resources.fruit2SoundLoaded = false;
    resources.fruit3SoundLoaded = false;
    resources.powerUpSoundLoaded = false;
    resources.timerPingSoundLoaded = false;
    resources.boundsSoundLoaded = false;
    resources.cherryLoaded = false;
    resources.orangeLoaded = false;
    resources.grapesLoaded = false;
    resources.lightningLoaded = false;
    resources.gummybearLoaded = false;
    resources.magnetLoaded = false;
    resources.mapPixels = nullptr;
    resources.wallPixelCount = 0;
    resources.pathPixelCount = 0;
    resources.cubicMapSize = { 1, 4, 1 };

    // Load map image
    if (FileExists("resources/fm/map.png")) {
        Image originalMap = LoadImage("resources/fm/map.png");
        if (originalMap.data) {
            ImageFormat(&originalMap, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
            resources.mapImage = ImageCopy(originalMap);
            resources.mapWidth = originalMap.width;
            resources.mapHeight = originalMap.height;

            // Process map for walls and paths
            for (int y = 0; y < resources.mapImage.height; y++) {
                for (int x = 0; x < resources.mapImage.width; x++) {
                    Color pixelColor = GetImageColor(originalMap, x, y);
                    bool isWall = (pixelColor.r <= 50 &&
                        pixelColor.g >= 200 &&
                        pixelColor.b <= 50 &&
                        pixelColor.a >= 200);
                    if (isWall) {
                        ImageDrawPixel(&resources.mapImage, x, y, WHITE);
                        resources.wallPixelCount++;
                    }
                    else {
                        ImageDrawPixel(&resources.mapImage, x, y, BLACK);
                        resources.pathPixelCount++;
                    }
                }
            }

            // Generate 3D mesh from map
            if (resources.wallPixelCount > 0) {
                Mesh mesh = GenMeshCubicmap(resources.mapImage, resources.cubicMapSize);
                resources.mazeModel = LoadModelFromMesh(mesh);
            }

            // Load pixel data for collision detection
            resources.mapPixels = LoadImageColors(resources.mapImage);

            // Create minimap texture
            resources.minimapTexture = LoadTextureFromImage(resources.mapImage);

            // Set map position
            resources.mapModelPosition = {
                -resources.mapImage.width / 2.0f,
                0,
                -resources.mapImage.height / 2.0f
            };

            resources.mapLoaded = true;
            UnloadImage(originalMap);
        }
    }

    // Load wall texture
    if (FileExists("resources/fm/cube.png")) {
        Image cubeImage = LoadImage("resources/fm/cube.png");
        resources.cubeWallTexture = LoadTextureFromImage(cubeImage);
        if (resources.cubeWallTexture.id > 0) {
            resources.wallTextureLoaded = true;
            SetMaterialTexture(&resources.mazeModel.materials[0], MATERIAL_MAP_DIFFUSE, resources.cubeWallTexture);

            // Add metallic properties
            //mazeMaterial->maps[MATERIAL_MAP_METALNESS].color = { 180, 255, 180, 255 }; // Green tint
            //mazeMaterial->maps[MATERIAL_MAP_ROUGHNESS].color = { 100, 100, 100, 255 }; // Semi-rough (metallic glint)
            //mazeMaterial->maps[MATERIAL_MAP_NORMAL].color = { 128, 128, 255, 255 };    // Slight normal variation
            
            UnloadImage(cubeImage);
        }
    }

    if (FileExists("lighting.vs") && FileExists("lighting.fs")) {
        resources.lightingShader = LoadShader("lighting.vs", "lighting.fs");
        if (resources.lightingShader.id > 0) {
            resources.lightingShaderLoaded = true;

            // Get uniform locations
            resources.lightingShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(resources.lightingShader, "matModel");
            resources.lightingShader.locs[SHADER_LOC_MATRIX_NORMAL] = GetShaderLocation(resources.lightingShader, "matNormal");
        }
    }

    // Load player model
    if (FileExists("resources/fm/freddy.glb")) {
        resources.playerModel = LoadModel("resources/fm/freddy.glb");
        if (resources.playerModel.meshCount > 0) {
            resources.playerModelLoaded = true;
        }
    }

    // Load background music
    if (FileExists("resources/fm/fbttl.mp3")) {
        resources.backgroundMusic = LoadMusicStream("resources/fm/fbttl.mp3");
        if (resources.backgroundMusic.stream.buffer) {
            resources.backgroundMusicLoaded = true;
        }
    }
    //else if (FileExists("resources/fm/maze_music.ogg")) {
    //    resources.backgroundMusic = LoadMusicStream("resources/fm/maze_music.ogg");
    //    if (resources.backgroundMusic.stream.buffer) {
    //        resources.backgroundMusicLoaded = true;
    //    }
    //}
    //else if (FileExists("resources/fm/maze_music.wav")) {
    //    resources.backgroundMusic = LoadMusicStream("resources/fm/maze_music.wav");
    //    if (resources.backgroundMusic.stream.buffer) {
    //        resources.backgroundMusicLoaded = true;
    //    }
    //}

    // Load prop models
    if (FileExists("resources/fm/cherry.glb")) {
        resources.cherryModel = LoadModel("resources/fm/cherry.glb");
        if (resources.cherryModel.meshCount > 0) {
            resources.cherryLoaded = true;
        }
    }

    if (FileExists("resources/fm/orange.glb")) {
        resources.orangeModel = LoadModel("resources/fm/orange.glb");
        if (resources.orangeModel.meshCount > 0) {
            resources.orangeLoaded = true;
        }
    }

    if (FileExists("resources/fm/grapes.glb")) {
        resources.grapesModel = LoadModel("resources/fm/grapes.glb");
        if (resources.grapesModel.meshCount > 0) {
            resources.grapesLoaded = true;
        }
    }

    if (FileExists("resources/fm/lightning.glb")) {
        resources.lightningModel = LoadModel("resources/fm/lightning.glb");
        if (resources.lightningModel.meshCount > 0) {
            resources.lightningLoaded = true;
        }
    }

    if (FileExists("resources/fm/gummybear.glb")) {
        resources.gummybearModel = LoadModel("resources/fm/gummybear.glb");
        if (resources.gummybearModel.meshCount > 0) {
            resources.gummybearLoaded = true;
        }
    }

    if (FileExists("resources/fm/magnet.glb")) {
        resources.magnetModel = LoadModel("resources/fm/magnet.glb");
        if (resources.magnetModel.meshCount > 0) {
            resources.magnetLoaded = true;
        }
    }

    // Load fruit collection sounds
    if (FileExists("resources/fm/fruit1.wav")) {
        resources.fruit1Sound = LoadSound("resources/fm/fruit1.wav");
        if (resources.fruit1Sound.frameCount > 0) {
            resources.fruit1SoundLoaded = true;
        }
    }

    if (FileExists("resources/fm/fruit2.wav")) {
        resources.fruit2Sound = LoadSound("resources/fm/fruit2.wav");
        if (resources.fruit2Sound.frameCount > 0) {
            resources.fruit2SoundLoaded = true;
        }
    }

    if (FileExists("resources/fm/fruit3.wav")) {
        resources.fruit3Sound = LoadSound("resources/fm/fruit3.wav");
        if (resources.fruit3Sound.frameCount > 0) {
            resources.fruit3SoundLoaded = true;
        }
    }

    // Load power-up collection sound
    if (FileExists("resources/fm/powerUp.wav")) {
        resources.powerUpSound = LoadSound("resources/fm/powerUp.wav");
        if (resources.powerUpSound.frameCount > 0) {
            resources.powerUpSoundLoaded = true;
        }
    }

    // Load timer ping sound
    if (FileExists("resources/fm/timerPing.wav")) {
        resources.timerPingSound = LoadSound("resources/fm/timerPing.wav");
        if (resources.timerPingSound.frameCount > 0) {
            resources.timerPingSoundLoaded = true;
        }
    }

    // Load out of bounds sound
    if (FileExists("resources/fm/bounds.wav")) {
        resources.boundsSound = LoadSound("resources/fm/bounds.wav");
        if (resources.boundsSound.frameCount > 0) {
            resources.boundsSoundLoaded = true;
        }
    }

    // Load extended time sound
    if (FileExists("resources/fm/timeExt.wav")) {
        resources.timeExtendedSound = LoadSound("resources/fm/timeExt.wav");
        if (resources.timeExtendedSound.frameCount > 0) {
            resources.timeExtendedSoundLoaded = true;
        }
    }

    cout << "Fruity Maze resources loaded successfully." << endl;
    return resources;
}

void UnloadFruityMazeResources(FruityMazeGameResources& resources) {
    cout << "Unloading Fruity Maze resources..." << endl;

    if (resources.mapLoaded) {
        UnloadImage(resources.mapImage);
        if (resources.minimapTexture.id > 0) {
            UnloadTexture(resources.minimapTexture);
        }
        if (resources.mapPixels) {
            UnloadImageColors(resources.mapPixels);
        }
        if (resources.mazeModel.meshCount > 0) {
            UnloadModel(resources.mazeModel);
        }
    }

    if (resources.wallTextureLoaded && resources.cubeWallTexture.id > 0) {
        UnloadTexture(resources.cubeWallTexture);
    }

    if (resources.playerModelLoaded && resources.playerModel.meshCount > 0) {
        UnloadModel(resources.playerModel);
    }

    if (resources.backgroundMusicLoaded && resources.backgroundMusic.stream.buffer) {
        if (IsMusicStreamPlaying(resources.backgroundMusic)) {
            StopMusicStream(resources.backgroundMusic);
        }
        UnloadMusicStream(resources.backgroundMusic);
    }

    // Unload prop models
    if (resources.cherryLoaded) UnloadModel(resources.cherryModel);
    if (resources.orangeLoaded) UnloadModel(resources.orangeModel);
    if (resources.grapesLoaded) UnloadModel(resources.grapesModel);
    if (resources.lightningLoaded) UnloadModel(resources.lightningModel);
    if (resources.gummybearLoaded) UnloadModel(resources.gummybearModel);
    if (resources.magnetLoaded) UnloadModel(resources.magnetModel);

    // Unload sounds
    if (resources.fruit1SoundLoaded) UnloadSound(resources.fruit1Sound);
    if (resources.fruit2SoundLoaded) UnloadSound(resources.fruit2Sound);
    if (resources.fruit3SoundLoaded) UnloadSound(resources.fruit3Sound);
    if (resources.powerUpSoundLoaded) UnloadSound(resources.powerUpSound);
    if (resources.timerPingSoundLoaded) UnloadSound(resources.timerPingSound);
    if (resources.boundsSoundLoaded) UnloadSound(resources.boundsSound);
    if (resources.timeExtendedSoundLoaded) UnloadSound(resources.timeExtendedSound);

    // Reset all values
    resources = {};
}

bool CheckFruityMazeResourcesLoaded(FruityMazeGameResources& res) {
    vector<string> missingResources;

    if (!res.mapLoaded) missingResources.push_back("map image");
    if (!res.wallTextureLoaded) missingResources.push_back("wall texture");
    // Player model is optional - game can run without it in camera mode
    // Background music is optional - game can run without it
    // Sounds are optional - game can run without them

    if (!missingResources.empty()) {
        cout << "FRUITY MAZE: Critical error - failed to load resources:" << endl;
        for (const string& resourceName : missingResources) {
            cout << "- " << resourceName << endl;
        }
        return false;
    }

    // Print optional resource status
    if (!res.backgroundMusicLoaded) {
        cout << "FRUITY MAZE: Background music not found (optional)" << endl;
    }
    if (!res.timerPingSoundLoaded) {
        cout << "FRUITY MAZE: Timer ping sound not found (optional)" << endl;
    }
    if (!res.boundsSoundLoaded) {
        cout << "FRUITY MAZE: Bounds sound not found (optional)" << endl;
    }

    return true;
}