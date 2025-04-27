//#include "Animation.h"
//#include "Constants.h" // Potrzebuje sta³ych animacji
//
//// Implementacja funkcji animacji
//void UpdatePlayerAnimation(PlayerData& player,
//                           const Texture2D& idleRight, const Texture2D& idleLeft,
//                           const Texture2D& walkRight, const Texture2D& walkLeft,
//                           const Texture2D& jumpRight, const Texture2D& jumpLeft)
//{
//    // Okreœl maxFrames na podstawie aktualnego stanu
//    if (!player.isGrounded)
//        player.maxFramesForCurrentAnim = JUMP_FRAMES;
//    else if (player.isMoving)
//        player.maxFramesForCurrentAnim = WALK_FRAMES;
//    else
//        player.maxFramesForCurrentAnim = IDLE_FRAMES;
//
//    // Aktualizacja klatki
//    player.animFrameCounter++;
//    if (player.animFrameCounter >= ANIM_UPDATE_RATE)
//    {
//        player.animFrameCounter = 0;
//        player.currentFrame++;
//        if (player.currentFrame >= player.maxFramesForCurrentAnim)
//            player.currentFrame = 0;
//    }
//
//    // Wybór odpowiedniej tekstury
//    if (!player.isGrounded)
//        player.currentTexture = player.facingRight ? jumpRight : jumpLeft;
//    else if (player.isMoving)
//        player.currentTexture = player.facingRight ? walkRight : walkLeft;
//    else
//        player.currentTexture = player.facingRight ? idleRight : idleLeft;
//}