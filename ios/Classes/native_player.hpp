#include <map>

#ifndef NATIVEPLAYER_INCLUDED
#define NATIVEPLAYER_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    FMOD::Channel *channel;
    FMOD::Sound *sound;
    float volume;
} fplayer;

typedef struct {
    FMOD::System *system;
} sfmod;

__attribute__((used))
std::map<int, fplayer*> allPlayers;
sfmod *finst = nullptr;

void (*print)(char *);

#ifdef __cplusplus
}
#endif

#endif