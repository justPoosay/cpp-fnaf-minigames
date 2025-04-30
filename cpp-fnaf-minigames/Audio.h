// Audio.h
#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h" // Potrzebne dla typów Music, Sound

// --- DEKLARACJE GLOBALNYCH ZMIENNYCH AUDIO (extern) ---
// 'extern' mówi kompilatorowi: "Ta zmienna istnieje, ale jest zdefiniowana gdzie indziej"
// (w pliku Audio.cpp)
extern Music g_backgroundMusic;
extern Sound g_jumpSound;
extern Sound g_npcDialogueSound;

extern bool g_musicLoaded;
extern bool g_jumpSoundLoaded;
extern bool g_npcDialogueSoundLoaded;
extern bool g_audioInitialized;

// Deklaracje œcie¿ek (mog¹ byæ const char* jeœli siê nie zmieniaj¹)
extern const char* g_musicPath;
extern const char* g_jumpPath;
extern const char* g_npcDialoguePath;


// --- DEKLARACJE GLOBALNYCH FUNKCJI AUDIO ---
// Te prototypy informuj¹ inne pliki, jak wywo³ywaæ te funkcje

// Inicjalizuje urz¹dzenie audio i ³aduje zasoby
bool InitializeAudioSystem();

// Aktualizuje strumieñ muzyki (wywo³ywana w pêtli gry)
void UpdateAudioMusicStream();

// Odtwarza dŸwiêk skoku
void PlayJumpSoundEffect();

// Odtwarza dŸwiêk dialogu NPC i pauzuje muzykê
void PlayDialogueSoundEffect();

// Wznawia odtwarzanie muzyki w tle
void ResumeBackgroundMusic();

// Sprawdza, czy dŸwiêk dialogu jest odtwarzany
bool IsDialogueAudioPlaying();

// Zwalnia zasoby audio i zamyka urz¹dzenie
void ShutdownAudioSystem();

#endif // AUDIO_H