//#include "Level.h"
//#include "Constants.h"
//#include "Player.h" // Potrzebuje PLAYER_TEXTURE_WIDTH
//#include <cmath>    // Dla fmaxf, fminf, fmodf
//
//void InitLevel(LevelData& level, const Texture2D& tileTex, const Texture2D& bgTex)
//{
//    level.tileTexture = tileTex;
//    level.backgroundTexture = bgTex;
//    level.scrollX = 0.0f;
//    level.bgScroll = 0.0f;
//    level.groundLevelY = VIRTUAL_SCREEN_HEIGHT - (float)level.tileTexture.height;
//
//    // Oblicz maksymalny scroll, upewniaj¹c siê, ¿e nie jest ujemny
//    level.maxScrollX = fmaxf(0.0f, (float)level.backgroundTexture.width - (float)VIRTUAL_SCREEN_WIDTH);
//}
//
//void UpdateLevelScroll(LevelData& level, const PlayerData& player)
//{
//    // Pozycja X œrodka ekranu wirtualnego
//    const float playerVirtualScreenX = VIRTUAL_SCREEN_WIDTH / 2.0f - PLAYER_TEXTURE_WIDTH / 2.0f;
//
//    // Docelowy scroll, aby gracz by³ na œrodku (lub blisko)
//    float targetScrollX = player.position.x - playerVirtualScreenX;
//
//    // Ogranicz scroll do granic poziomu (0 do maxScrollX)
//    level.scrollX = fmaxf(0.0f, fminf(targetScrollX, level.maxScrollX));
//
//    // Oblicz scroll t³a z efektem paralaksy
//    level.bgScroll = level.scrollX * BG_SCROLL_FACTOR;
//}
//
//void DrawLevel(const LevelData& level)
//{
//    // Rysuj T³o z paralaks¹
//    // U¿yj DrawTextureEx dla precyzyjnego pozycjonowania float
//    DrawTextureEx(level.backgroundTexture, { -level.bgScroll, 0.0f }, 0.0f, 1.0f, WHITE);
//
//    // Rysuj Pod³o¿e (kafelki)
//    if (level.tileTexture.id > 0 && level.tileTexture.width > 0) // Sprawdzenie czy kafelek za³adowany i ma szerokoœæ
//    {
//        int tileCount = (VIRTUAL_SCREEN_WIDTH / level.tileTexture.width) + 2; // +2 dla pewnoœci pokrycia krawêdzi
//        for (int i = -1; i < tileCount; i++) // Zaczynamy od -1, aby pokryæ lew¹ krawêdŸ przy scrollowaniu
//        {
//            // Oblicz pozycjê X kafelka, uwzglêdniaj¹c scroll i zawijanie (modulo)
//            float tileX = (float)i * level.tileTexture.width - fmodf(level.scrollX, (float)level.tileTexture.width);
//            DrawTexture(level.tileTexture, (int)tileX, (int)level.groundLevelY, WHITE);
//        }
//    }
//}
//
//void UnloadLevelTextures(LevelData& level)
//{
//    // Te tekstury s¹ ³adowane w Game.cpp i przekazywane do InitLevel,
//    // wiêc powinny byæ zwolnione w Game::Shutdown
//    // Ta funkcja mo¿e byæ pusta lub usuniêta, jeœli LevelData nie "posiada" tekstur.
//    // Jeœli w przysz³oœci LevelData bêdzie ³adowaæ w³asne tekstury, tutaj je zwolnij.
//    // UnloadTexture(level.tileTexture);
//    // UnloadTexture(level.backgroundTexture);
//}