// Audio.h
#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h" // Potrzebne dla typ�w Music, Sound

// --- DEKLARACJE GLOBALNYCH ZMIENNYCH AUDIO (extern) ---
// 'extern' m�wi kompilatorowi: "Ta zmienna istnieje, ale jest zdefiniowana gdzie indziej"
// (w pliku Audio.cpp)
extern Music g_backgroundMusic;
extern Sound g_jumpSound;
extern Sound g_npcDialogueSound;

extern bool g_musicLoaded;
extern bool g_jumpSoundLoaded;
extern bool g_npcDialogueSoundLoaded;
extern bool g_audioInitialized;

// Deklaracje �cie�ek (mog� by� const char* je�li si� nie zmieniaj�)
extern const char* g_musicPath;
extern const char* g_jumpPath;
extern const char* g_npcDialoguePath;


// --- DEKLARACJE GLOBALNYCH FUNKCJI AUDIO ---
// Te prototypy informuj� inne pliki, jak wywo�ywa� te funkcje

// Inicjalizuje urz�dzenie audio i �aduje zasoby
bool InitializeAudioSystem();

// Aktualizuje strumie� muzyki (wywo�ywana w p�tli gry)
void UpdateAudioMusicStream();

// Odtwarza d�wi�k skoku
void PlayJumpSoundEffect();

// Odtwarza d�wi�k dialogu NPC i pauzuje muzyk�
void PlayDialogueSoundEffect();

// Wznawia odtwarzanie muzyki w tle
void ResumeBackgroundMusic();

// Sprawdza, czy d�wi�k dialogu jest odtwarzany
bool IsDialogueAudioPlaying();

// Zwalnia zasoby audio i zamyka urz�dzenie
void ShutdownAudioSystem();

#endif // AUDIO_H